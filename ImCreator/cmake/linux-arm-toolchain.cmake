# Linux ARM通用工具链配置
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# 编译器设置
set(CMAKE_C_COMPILER 
    "/home/book/tools/arm-bcm2708/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc"
    CACHE FILEPATH "C compiler for Linux ARM")

set(CMAKE_CXX_COMPILER 
    "/home/book/tools/arm-bcm2708/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-g++"
    CACHE FILEPATH "C++ compiler for Linux ARM")

# Sysroot设置
set(IMWIDGET_SYSROOT "/home/book/PI/PiRoot" CACHE PATH "Sysroot for Linux ARM")
set(CMAKE_SYSROOT ${IMWIDGET_SYSROOT})

# 编译器标志
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --sysroot=${IMWIDGET_SYSROOT}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --sysroot=${IMWIDGET_SYSROOT}")

# 查找规则
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)