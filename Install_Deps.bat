
set "SCRIPT_DIR=%~dp0"
set "IMWIDGET_DIR=%SCRIPT_DIR%ImWidget"

cd /d "%~dp0"
if exist ".\ImWidget\Install_Dependencies.bat" (
    echo 正在执行Install_Dependencies.bat...
    pushd "%IMWIDGET_DIR%"
    call "Install_Dependencies.bat"
    popd
    echo 执行完成。
    pause
) else (
    echo 错误：找不到.\ImWidget\Install_Dependencies.bat文件
    pause
)