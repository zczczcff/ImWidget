@echo off
setlocal enabledelayedexpansion

:: 配置参数
set "IMGUI_URL=https://github.com/ocornut/imgui/archive/refs/heads/master.zip"
set "STB_URL=https://github.com/nothings/stb/archive/refs/heads/master.zip"
set "JSON_URL=https://github.com/nlohmann/json/archive/refs/heads/develop.zip"
set "ROP_URL=https://github.com/zczczcff/ROP/archive/refs/heads/master.zip"  :: 新增 ROP 库
set "IMGUI_TARGET_DIR=Extern\imgui-master"
set "STB_TARGET_DIR=Extern\stb_image"
set "JSON_TARGET_DIR=Extern\nlohmann_json"
set "ROP_TARGET_DIR=Extern\ROP"  :: 新增 ROP 目标目录
set "IMGUI_ZIP_FILE=%TEMP%\imgui_temp.zip"
set "STB_ZIP_FILE=%TEMP%\stb_temp.zip"
set "JSON_ZIP_FILE=%TEMP%\json_temp.zip"
set "ROP_ZIP_FILE=%TEMP%\rop_temp.zip"  :: 新增 ROP 临时文件

:: 创建目标目录
mkdir "Extern" 2>nul

:: 安装ImGui库（如果不存在）
if not exist "%IMGUI_TARGET_DIR%" (
    echo Downloading ImGui from GitHub...
    powershell -Command "(New-Object Net.WebClient).DownloadFile('%IMGUI_URL%', '%IMGUI_ZIP_FILE%')"
    if !errorlevel! neq 0 (
        echo Error: Failed to download ImGui
        exit /b 1
    )

    echo Extracting ImGui...
    powershell -Command "Expand-Archive -Path '%IMGUI_ZIP_FILE%' -DestinationPath 'Extern'"
    if !errorlevel! neq 0 (
        echo Error: Failed to extract ImGui files
        del "%IMGUI_ZIP_FILE%" 2>nul
        exit /b 1
    )

    :: 重命名目录
    if exist "Extern\imgui-master" (
        move "Extern\imgui-master" "%IMGUI_TARGET_DIR%" >nul
    )
    del "%IMGUI_ZIP_FILE%" 2>nul
    echo ImGui installed successfully at %IMGUI_TARGET_DIR%
) else (
    echo ImGui already exists at %IMGUI_TARGET_DIR%
)

:: 安装stb_image库（如果不存在）
if not exist "%STB_TARGET_DIR%" (
    echo Downloading stb_image from GitHub...
    powershell -Command "(New-Object Net.WebClient).DownloadFile('%STB_URL%', '%STB_ZIP_FILE%')"
    if !errorlevel! neq 0 (
        echo Error: Failed to download stb_image
        exit /b 1
    )

    echo Extracting stb_image...
    powershell -Command "Expand-Archive -Path '%STB_ZIP_FILE%' -DestinationPath 'Extern'"
    if !errorlevel! neq 0 (
        echo Error: Failed to extract stb_image files
        del "%STB_ZIP_FILE%" 2>nul
        exit /b 1
    )

    :: 重命名目录并创建目标文件夹
    if exist "Extern\stb-master" (
        ren "Extern\stb-master" "stb_image" >nul
        echo stb_image installed successfully at %STB_TARGET_DIR%
    ) else (
        echo Error: Extracted stb directory not found
        exit /b 1
    )
    del "%STB_ZIP_FILE%" 2>nul
) else (
    echo stb_image already exists at %STB_TARGET_DIR%
)

:: 安装nlohmann/json库（如果不存在）
if not exist "%JSON_TARGET_DIR%" (
    echo Downloading nlohmann/json from GitHub...
    powershell -Command "(New-Object Net.WebClient).DownloadFile('%JSON_URL%', '%JSON_ZIP_FILE%')"
    if !errorlevel! neq 0 (
        echo Error: Failed to download nlohmann/json
        exit /b 1
    )

    echo Extracting nlohmann/json...
    powershell -Command "Expand-Archive -Path '%JSON_ZIP_FILE%' -DestinationPath 'Extern'"
    if !errorlevel! neq 0 (
        echo Error: Failed to extract nlohmann/json files
        del "%JSON_ZIP_FILE%" 2>nul
        exit /b 1
    )

    :: 重命名目录
    if exist "Extern\json-develop" (
        move "Extern\json-develop" "%JSON_TARGET_DIR%" >nul
        echo nlohmann/json installed successfully at %JSON_TARGET_DIR%
    ) else (
        echo Error: Extracted json directory not found
        exit /b 1
    )
    del "%JSON_ZIP_FILE%" 2>nul
) else (
    echo nlohmann/json already exists at %JSON_TARGET_DIR%
)

:: 安装ROP库（如果不存在） - 新增部分
if not exist "%ROP_TARGET_DIR%" (
    echo Downloading ROP library from GitHub...
    powershell -Command "(New-Object Net.WebClient).DownloadFile('%ROP_URL%', '%ROP_ZIP_FILE%')"
    if !errorlevel! neq 0 (
        echo Error: Failed to download ROP library
        exit /b 1
    )

    echo Extracting ROP library...
    powershell -Command "Expand-Archive -Path '%ROP_ZIP_FILE%' -DestinationPath 'Extern'"
    if !errorlevel! neq 0 (
        echo Error: Failed to extract ROP library files
        del "%ROP_ZIP_FILE%" 2>nul
        exit /b 1
    )

    :: 重命名目录
    if exist "Extern\ROP-master" (
        move "Extern\ROP-master" "%ROP_TARGET_DIR%" >nul
        echo ROP library installed successfully at %ROP_TARGET_DIR%
    ) else (
        echo Error: Extracted ROP directory not found
        exit /b 1
    )
    del "%ROP_ZIP_FILE%" 2>nul
) else (
    echo ROP library already exists at %ROP_TARGET_DIR%
)

endlocal