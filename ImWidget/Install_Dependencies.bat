@echo off
setlocal enabledelayedexpansion

:: 配置参数
set "IMGUI_URL=https://github.com/ocornut/imgui/archive/refs/heads/master.zip"
set "STB_URL=https://github.com/nothings/stb/archive/refs/heads/master.zip"
set "IMGUI_TARGET_DIR=Extern\imgui-master"
set "STB_TARGET_DIR=Extern\stb_image"
set "IMGUI_ZIP_FILE=%TEMP%\imgui_temp.zip"
set "STB_ZIP_FILE=%TEMP%\stb_temp.zip"

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

endlocal