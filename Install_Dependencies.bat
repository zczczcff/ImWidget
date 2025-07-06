@echo off
setlocal enabledelayedexpansion

:: 配置参数
set "IMGUI_URL=https://github.com/ocornut/imgui/archive/refs/heads/master.zip"
set "TARGET_DIR=Extern\imgui-master"
set "ZIP_FILE=%TEMP%\imgui_temp.zip"

:: 检查目标目录是否已存在
if exist "%TARGET_DIR%" (
    echo ImGui is already installed at %TARGET_DIR%
    exit /b 0
)

:: 创建目标目录
mkdir "Extern" 2>nul

:: 下载ImGui库
echo Downloading ImGui from GitHub...
powershell -Command "(New-Object Net.WebClient).DownloadFile('%IMGUI_URL%', '%ZIP_FILE%')"
if %errorlevel% neq 0 (
    echo Error: Failed to download ImGui
    exit /b 1
)

:: 解压文件
echo Extracting files...
powershell -Command "Expand-Archive -Path '%ZIP_FILE%' -DestinationPath 'Extern'"
if %errorlevel% neq 0 (
    echo Error: Failed to extract files
    del "%ZIP_FILE%" 2>nul
    exit /b 1
)

:: 重命名目录
move "Extern\imgui-master" "Extern\imgui-temp" >nul
move "Extern\imgui-temp" "%TARGET_DIR%" >nul

:: 清理临时文件
del "%ZIP_FILE%" 2>nul

echo ImGui installed successfully at %TARGET_DIR%
endlocal