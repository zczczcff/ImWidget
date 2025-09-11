#!/bin/bash

# 获取当前脚本所在的目录
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
IMWIDGET_DIR="$SCRIPT_DIR/ImWidget"

# 检查 Install_Dependencies.sh 是否存在
if [ ! -f "$IMWIDGET_DIR/Install_Dependencies.sh" ]; then
    echo "错误：找不到 $IMWIDGET_DIR/Install_Dependencies.sh"
    read -p "按任意键退出..." -n1 -s
    exit 1
fi

# 进入 ImWidget 目录执行 Install_Dependencies.sh
echo "正在执行 Install_Dependencies.sh..."
cd "$IMWIDGET_DIR" || exit 1
bash "./Install_Dependencies.sh"

# 返回原目录（可选，取决于是否需要恢复原路径）
cd "$SCRIPT_DIR" || exit 1

echo "执行完成。"
read -p "按任意键退出..." -n1 -s