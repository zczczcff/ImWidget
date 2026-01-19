#!/bin/bash

# 设置下载链接和目录路径
repo_url="https://github.com/zczczcff/EditorKit.git"
target_dir="./ThirdParty/EditorKit"

# 检查是否安装Git
if ! command -v git &> /dev/null; then
    echo "Git未安装，请先安装Git"
    exit 1
fi

# 创建目标目录
mkdir -p "$target_dir"

# 克隆或更新仓库
if [ -d "$target_dir/.git" ]; then
    echo "目录已存在，正在更新..."
    git -C "$target_dir" pull origin master
else
    git clone "$repo_url" "$target_dir"
fi

echo "操作完成！源码位置: $target_dir"