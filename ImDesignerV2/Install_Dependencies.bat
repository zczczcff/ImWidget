@echo off
setlocal enabledelayedexpansion

REM 设置下载链接和目录路径
set "repo_url=https://github.com/zczczcff/EditorKit.git"
set "target_dir=.\ThirdParty\EditorKit"

REM 检查是否安装Git
where git >nul 2>nul
if %errorlevel% neq 0 (
    echo Git未安装，请先安装Git: https://git-scm.com/
    pause
    exit /b 1
)

REM 创建目标目录
if not exist "%target_dir%" (
    mkdir "%target_dir%"
) else (
    echo 目录已存在，正在更新...
)

REM 克隆或更新仓库
cd "%target_dir%"
if exist .git (
    git pull origin master
) else (
    git clone %repo_url% .
)
cd ..

echo 操作完成！源码位置: %target_dir%
endlocal