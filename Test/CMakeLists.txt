# 测试项目的独立CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

# 平台选择提前到project()之前
if(NOT DEFINED BUILD_PLATFORM)
    #自动检测默认平台
   if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
       set(BUILD_PLATFORM "linux-arm" CACHE STRING "Target platform (linux-arm or win64)")
   else()
       set(BUILD_PLATFORM "win64" CACHE STRING "Target platform (linux-arm or win64)")
   endif()
endif()
set_property(CACHE BUILD_PLATFORM PROPERTY STRINGS linux-arm win64)

# 根据平台加载工具链文件
if(BUILD_PLATFORM STREQUAL "linux-arm")
   set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/linux-arm-toolchain.cmake"
       CACHE FILEPATH "Linux ARM toolchain file")
elseif(BUILD_PLATFORM STREQUAL "win64")
   set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/win64-toolchain.cmake"
       CACHE FILEPATH "Windows 64-bit toolchain file")
endif()





set(TARGET_NAME ImWidgetTest)
project(${TARGET_NAME} LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)


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
        add_compile_options(/W4 /EHsc /D_CRT_SECURE_NO_WARNINGS /permissive- /DNOMINMAX /bigobj)
    else()
        add_compile_options(-Wall -Wextra -DIMWIDGET_WIN64_PLATFORM)
    endif()
endif()

# imgui配置文件
add_definitions(-DIMGUI_USER_CONFIG=<ImWidgetConfig.h>)

# 通过FetchContent获取Catch2
# include(FetchContent)
# FetchContent_Declare(
#   Catch2
#   GIT_REPOSITORY https://github.com/catchorg/Catch2.git
#   GIT_TAG        v3.4.0
# )
# FetchContent_MakeAvailable(Catch2)

# 通过add_subdirectory添加EditorKit依赖
# 使用硬编码的路径指向EditorKit目录
message(STATUS "添加EditorKit作为子目录: ${CMAKE_CURRENT_SOURCE_DIR}/../ImWidget")
add_subdirectory(../ImWidget ${CMAKE_CURRENT_BINARY_DIR}/EditorKit)

# 查找所有测试源文件
file(GLOB_RECURSE TEST_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cxx"
)

        list(APPEND TEST_SOURCES
    "../ImMain.cpp")

# 打印找到的测试文件
message(STATUS "测试源文件: ${TEST_SOURCES}")

# 创建测试可执行文件
add_executable(${TARGET_NAME} ${TEST_SOURCES})

# 链接ImWidget库和Catch2
target_link_libraries(${TARGET_NAME} PRIVATE ImWidget)

# 添加包含目录
target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})




# 设置目标属性
set_target_properties(${TARGET_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 注册测试
# include(CTest)
# include(Catch)
# catch_discover_tests(${TARGET_NAME})
