cmake_minimum_required(VERSION 3.15)
project(ImWidget
    VERSION 1.2.0
    DESCRIPTION "Advanced ImGui Widget Framework"
    HOMEPAGE_URL "https://github.com/yourorg/imwidget"
    LANGUAGES CXX
)

#-----------------------------------------------
# 配置选项
#-----------------------------------------------
# 平台配置（可被上层覆盖）
if(NOT DEFINED IMWIDGET_PLATFORM AND NOT DEFINED BUILD_PLATFORM)
    # 自动检测默认平台
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
        set(IMWIDGET_PLATFORM "linux-arm" CACHE STRING "Target platform (linux-arm or win64)")
    else()
        set(IMWIDGET_PLATFORM "win64" CACHE STRING "Target platform (linux-arm or win64)")
    endif()
elseif(DEFINED BUILD_PLATFORM)
    set(IMWIDGET_PLATFORM ${BUILD_PLATFORM} CACHE STRING "" FORCE)
endif()

set_property(CACHE IMWIDGET_PLATFORM PROPERTY STRINGS linux-arm win64)
message(STATUS "Building for platform: ${IMWIDGET_PLATFORM}")

# 公共配置选项
option(IMWIDGET_BUILD_EXAMPLES "Build example applications" ON)
option(IMWIDGET_BUILD_TESTS "Build unit tests" OFF)
option(IMWIDGET_INSTALL "Generate install rules" ON)
option(IMWIDGET_EXPORT_API "Export library symbols" ON)

#-----------------------------------------------
# 平台特定配置
#-----------------------------------------------
if(IMWIDGET_PLATFORM STREQUAL "linux-arm")
    # 使用Opengl es2.0
    add_definitions(-DIMGUI_IMPL_OPENGL_ES2)

    # 设置sysroot
    if(NOT DEFINED IMWIDGET_SYSROOT)
        set(IMWIDGET_SYSROOT "/home/book/PI/PiRoot" CACHE PATH "Sysroot for Linux ARM")
    endif()
    
    # 后端设置
    set(BACKEND_SRCS
        Extern/imgui-master/backends/imgui_impl_glfw.cpp
        Extern/imgui-master/backends/imgui_impl_opengl3.cpp
    )
    
    # 包含目录
    include_directories(
        ${IMWIDGET_SYSROOT}/usr/include/GLES2
        ${IMWIDGET_SYSROOT}/usr/include/EGL
        ${IMWIDGET_SYSROOT}/usr/include/GLFW
        ${IMWIDGET_SYSROOT}/usr/include
    )
    
    # 链接库
    set(IMWIDGET_LIBS glfw GLESv2 EGL m dl pthread stdc++fs)
    
elseif(IMWIDGET_PLATFORM STREQUAL "win64")
    # 设置Unicode字符集
    add_definitions(-DUNICODE -D_UNICODE)
    add_definitions(-DTesting)
    
    # 设置C++17标准
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    
    if(MSVC)
        # 设置入口点和字符集
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:wWinMainCRTStartup")
        
        # 编译器选项
        add_compile_options(/W4 /EHsc /D_CRT_SECURE_NO_WARNINGS /permissive-)
        
        # 导出API符号
        if(IMWIDGET_EXPORT_API)
            add_compile_definitions(IMGUI_API=__declspec\(dllexport\))
        endif()
    else()
        add_compile_options(-Wall -Wextra -fPIC -DIMWIDGET_WIN64_PLATFORM -std=c++17)
    endif()
    
    # 原生Win32后端
    set(BACKEND_SRCS
        Extern/imgui-master/backends/imgui_impl_win32.cpp
        Extern/imgui-master/backends/imgui_impl_dx11.cpp
    )
    
    # 解决Windows.h和min/max冲突
    if(MSVC)
        add_compile_definitions(NOMINMAX)
    endif()
    
    # 链接库
    set(IMWIDGET_LIBS d3d11.lib d3dcompiler.lib dxgi.lib)
endif()




#-----------------------------------------------
# 公共配置（所有平台）
#-----------------------------------------------
# 完整包含目录设置
add_definitions(-DIMGUI_USER_CONFIG=<ImWidgetConfig.h>)


file(GLOB_RECURSE LIB_HEADERS *.h)
file(GLOB_RECURSE LIB_SOURCES ./Source/*cpp)

# 然后添加额外的文件
list(APPEND LIB_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/Extern/imgui-master/imgui.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Extern/imgui-master/imgui_draw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Extern/imgui-master/imgui_tables.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Extern/imgui-master/imgui_widgets.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/${BACKEND_SRCS}"
)

#-----------------------------------------------
# 构建静态库
#-----------------------------------------------
add_library(ImWidget STATIC ${LIB_SOURCES} ${LIB_HEADERS})
# 创建源文件和头文件过滤器
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}
        PREFIX "Source" 
        FILES ${LIB_SOURCES}
    )
    
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} 
        PREFIX "Include" 
        FILES ${LIB_HEADERS}
    )

    

# 优化配置（按构建类型区分）
if(MSVC)
    target_compile_options(ImWidget PRIVATE
        "$<$<CONFIG:Debug>:/Od>"      # Debug禁用优化
        "$<$<CONFIG:Release>:/O2>"    # Release启用O2优化
        "$<$<CONFIG:RelWithDebInfo>:/O2 /Zi>"
    )
else()
    target_compile_options(ImWidget PRIVATE
        "$<$<CONFIG:Debug>:-O0>"      # Debug禁用优化
        "$<$<CONFIG:Release>:-O2>"    # Release启用O2优化
    )
endif()

# 设置公共包含目录 - 修复版本
target_include_directories(ImWidget PUBLIC
    # 使用生成器表达式安全地设置包含路径
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include/ImWidget>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include/ImWindows>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include/Application>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Extern/imgui-master>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Extern/imgui-master/backends>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Extern/nlohmann_json/single_include/nlohmann>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Extern/stb_image>
    
    # 安装接口使用相对路径
    $<INSTALL_INTERFACE:include>
)

# 设置库属性
set_target_properties(ImWidget PROPERTIES
    PUBLIC_HEADER  ""
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
)

# 平台特定链接选项
if(IMWIDGET_PLATFORM STREQUAL "pi")
    set_target_properties(ImWidget PROPERTIES 
        LINK_FLAGS "--sysroot=${IMWIDGET_SYSROOT} \
                   -Wl,-rpath-link=${IMWIDGET_SYSROOT}/lib \
                   -Wl,-rpath-link=${IMWIDGET_SYSROOT}/usr/lib \
                   -Wl,-rpath-link=${IMWIDGET_SYSROOT}/usr/lib/arm-linux-gnueabihf \
                   -Wl,--dynamic-linker=/lib/ld-linux-armhf.so.3"
    )
endif()

# 链接库
target_link_libraries(ImWidget PUBLIC ${IMWIDGET_LIBS})

#-----------------------------------------------
# 安装和导出（修复版本）
#-----------------------------------------------
if(IMWIDGET_INSTALL)
    # 安装头文件（所有必要目录）
    set(INSTALL_INCLUDE_DIRS
        ImWidget
        ImWindows
        Application
        Extern/imgui-master
        Extern/stb_image
    )
    
    foreach(dir ${INSTALL_INCLUDE_DIRS})
        install(DIRECTORY ${dir}/
            DESTINATION include/${dir}
            FILES_MATCHING 
            PATTERN "*.h"
            PATTERN "*.hpp"
            PATTERN "*.inl"
        )
    endforeach()
    
    # 安装库文件
    install(TARGETS ImWidget
        EXPORT ImWidgetTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
    )
    
    # 导出配置
    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/ImWidgetConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )
    
    # 创建配置文件
    configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ImWidgetConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/ImWidgetConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ImWidget
    )
    
    # 安装目标导出文件
    install(EXPORT ImWidgetTargets
        FILE ImWidgetTargets.cmake
        NAMESPACE ImWidget::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ImWidget
    )
    
    # 安装配置文件
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/ImWidgetConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/ImWidgetConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ImWidget
    )
endif()