#!/bin/bash

# 配置参数
IMGUI_URL="https://github.com/ocornut/imgui/archive/refs/heads/master.zip"
STB_URL="https://github.com/nothings/stb/archive/refs/heads/master.zip"
JSON_URL="https://github.com/nlohmann/json/archive/refs/heads/develop.zip"
IMGUI_TARGET_DIR="Extern/imgui-master"
STB_TARGET_DIR="Extern/stb_image"
JSON_TARGET_DIR="Extern/nlohmann_json"
IMGUI_ZIP_FILE="/tmp/imgui_temp.zip"
STB_ZIP_FILE="/tmp/stb_temp.zip"
JSON_ZIP_FILE="/tmp/json_temp.zip"

# 创建目标目录
mkdir -p "Extern"

# 安装ImGui库（如果不存在）
if [ ! -d "$IMGUI_TARGET_DIR" ]; then
    echo "Downloading ImGui from GitHub..."
    wget -q "$IMGUI_URL" -O "$IMGUI_ZIP_FILE"
    if [ $? -ne 0 ]; then
        echo "Error: Failed to download ImGui"
        exit 1
    fi

    echo "Extracting ImGui..."
    unzip -q "$IMGUI_ZIP_FILE" -d "Extern"
    if [ $? -ne 0 ]; then
        echo "Error: Failed to extract ImGui files"
        rm -f "$IMGUI_ZIP_FILE"
        exit 1
    fi

    # 重命名目录
    if [ -d "Extern/imgui-master" ]; then
        mv "Extern/imgui-master" "$IMGUI_TARGET_DIR"
    else
        echo "Error: Extracted imgui directory not found"
        rm -f "$IMGUI_ZIP_FILE"
        exit 1
    fi
    
    rm -f "$IMGUI_ZIP_FILE"
    echo "ImGui installed successfully at $IMGUI_TARGET_DIR"
else
    echo "ImGui already exists at $IMGUI_TARGET_DIR"
fi

# 安装stb_image库（如果不存在）
if [ ! -d "$STB_TARGET_DIR" ]; then
    echo "Downloading stb_image from GitHub..."
    wget -q "$STB_URL" -O "$STB_ZIP_FILE"
    if [ $? -ne 0 ]; then
        echo "Error: Failed to download stb_image"
        exit 1
    fi

    echo "Extracting stb_image..."
    unzip -q "$STB_ZIP_FILE" -d "Extern"
    if [ $? -ne 0 ]; then
        echo "Error: Failed to extract stb_image files"
        rm -f "$STB_ZIP_FILE"
        exit 1
    fi

    # 重命名目录
    if [ -d "Extern/stb-master" ]; then
        mv "Extern/stb-master" "$STB_TARGET_DIR"
        echo "stb_image installed successfully at $STB_TARGET_DIR"
    else
        echo "Error: Extracted stb directory not found"
        rm -f "$STB_ZIP_FILE"
        exit 1
    fi
    
    rm -f "$STB_ZIP_FILE"
else
    echo "stb_image already exists at $STB_TARGET_DIR"
fi

# 安装nlohmann/json库（如果不存在）
if [ ! -d "$JSON_TARGET_DIR" ]; then
    echo "Downloading nlohmann/json from GitHub..."
    wget -q "$JSON_URL" -O "$JSON_ZIP_FILE"
    if [ $? -ne 0 ]; then
        echo "Error: Failed to download nlohmann/json"
        exit 1
    fi

    echo "Extracting nlohmann/json..."
    unzip -q "$JSON_ZIP_FILE" -d "Extern"
    if [ $? -ne 0 ]; then
        echo "Error: Failed to extract nlohmann/json files"
        rm -f "$JSON_ZIP_FILE"
        exit 1
    fi

    # 重命名目录
    if [ -d "Extern/json-develop" ]; then
        mv "Extern/json-develop" "$JSON_TARGET_DIR"
        echo "nlohmann/json installed successfully at $JSON_TARGET_DIR"
    else
        echo "Error: Extracted json directory not found"
        rm -f "$JSON_ZIP_FILE"
        exit 1
    fi
    
    rm -f "$JSON_ZIP_FILE"
else
    echo "nlohmann/json already exists at $JSON_TARGET_DIR"
fi