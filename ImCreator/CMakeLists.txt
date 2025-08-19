cmake_minimum_required(VERSION 3.15)

# 平台选择提前到project()之前
#if(NOT DEFINED BUILD_PLATFORM)
    # 自动检测默认平台
#    if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
#        set(BUILD_PLATFORM "linux-arm" CACHE STRING "Target platform (linux-arm or win64)")
#    else()
#        set(BUILD_PLATFORM "win64" CACHE STRING "Target platform (linux-arm or win64)")
#    endif()
#endif()
#set_property(CACHE BUILD_PLATFORM PROPERTY STRINGS linux-arm win64)

# 根据平台加载工具链文件
if(BUILD_PLATFORM STREQUAL "linux-arm")
   set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/linux-arm-toolchain.cmake"
       CACHE FILEPATH "Linux ARM toolchain file")
elseif(BUILD_PLATFORM STREQUAL "win64")
   set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/win64-toolchain.cmake"
       CACHE FILEPATH "Windows 64-bit toolchain file")
endif()

project(ImCreator)

# imgui配置文件
add_definitions(-DIMGUI_USER_CONFIG=<ImWidgetConfig.h>)

message(STATUS "Building ImCreator for platform: ${BUILD_PLATFORM}")

#-----------------------------------------------
# 平台特定编译设置 (仅保留项目特有设置)
#-----------------------------------------------
if(BUILD_PLATFORM STREQUAL "linux-arm")
    # 通用Linux ARM设置
    add_compile_options(-fPIC -std=c++17)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --sysroot=${IMWIDGET_SYSROOT}")

elseif(BUILD_PLATFORM STREQUAL "win64")
    # Windows统一配置
    add_definitions(-DUNICODE -D_UNICODE)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    
    if(MSVC)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:wWinMainCRTStartup")
        add_compile_options(/W4 /EHsc /D_CRT_SECURE_NO_WARNINGS /permissive- /DNOMINMAX)
    else()
        add_compile_options(-Wall -Wextra -DIMWIDGET_WIN64_PLATFORM)
    endif()
endif()

#-----------------------------------------------
# 添加ImWidget依赖（跨平台关键）
#-----------------------------------------------
# 传递平台参数给子项目
set(IMWIDGET_PLATFORM ${BUILD_PLATFORM} CACHE STRING "Forced platform" FORCE)

# 添加子项目（保持二进制目录隔离）
add_subdirectory(../ImWidget ${CMAKE_CURRENT_BINARY_DIR}/ImWidget)

#-----------------------------------------------
# 可执行文件配置
#-----------------------------------------------
set(TARGET_NAME ImCreator_${BUILD_PLATFORM})

file(GLOB_RECURSE All_Headers ./Include/*.h)
file(GLOB_RECURSE All_Resource ./Src/*cpp)

add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE ${All_Resource} ${All_Headers} ../ImMain.cpp)

# 自动继承ImWidget的头文件设置
target_link_libraries(${TARGET_NAME} PRIVATE ImWidget)

# 添加公共包含目录
target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

#-----------------------------------------------
# 安装规则（跨平台兼容）
#-----------------------------------------------
install(TARGETS ${TARGET_NAME}
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION bin  # macOS支持
)

# Windows调试工作目录设置
if(BUILD_PLATFORM STREQUAL "win64" AND MSVC)
    set_target_properties(${TARGET_NAME} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>"
    )
endif()

# Linux ARM运行时配置
if(BUILD_PLATFORM STREQUAL "linux-arm")
    set_target_properties(${TARGET_NAME} PROPERTIES
        INSTALL_RPATH "${IMWIDGET_SYSROOT}/usr/lib/arm-linux-gnueabihf"
        BUILD_WITH_INSTALL_RPATH ON
    )
endif()