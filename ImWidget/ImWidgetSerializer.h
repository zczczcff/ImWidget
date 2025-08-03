#pragma once
#include "ImWidgetProperty.h"
#include "ImWidget.h"
#include <json.hpp>
#include <vector>
#include <functional>
#include <fstream>

namespace ImGuiWidget
{
    // 自定义颜色分量提取宏（替代缺失的IM_COL32_R等）
#define IM_COLOR_GET_R(col) ((col) & 0xFF)
#define IM_COLOR_GET_G(col) (((col) >> 8) & 0xFF)
#define IM_COLOR_GET_B(col) (((col) >> 16) & 0xFF)
#define IM_COLOR_GET_A(col) (((col) >> 24) & 0xFF)
#define IM_COLOR_SET_RGBA(r, g, b, a) ((r) | ((g) << 8) | ((b) << 16) | ((a) << 24))

// 序列化属性结构体到JSON
    nlohmann::json SerializeStructProperties(PropertyStruct* propStruct) {
        nlohmann::json j;
        if (!propStruct) return j;

        auto properties = propStruct->GetProperties();
        for (const auto& prop : properties) {
            void* valuePtr = prop.getter();
            if (!valuePtr) continue;

            switch (prop.type) {
            case PropertyType::Color: {
                ImU32 color = *static_cast<ImU32*>(valuePtr);
                j[prop.name] = {
                    IM_COLOR_GET_R(color),
                    IM_COLOR_GET_G(color),
                    IM_COLOR_GET_B(color),
                    IM_COLOR_GET_A(color)
                };
                break;
            }
            case PropertyType::Float:
                j[prop.name] = *static_cast<float*>(valuePtr);
                break;
            case PropertyType::Bool:
                j[prop.name] = *static_cast<bool*>(valuePtr);
                break;
            case PropertyType::Int:
                j[prop.name] = *static_cast<int*>(valuePtr);
                break;
            case PropertyType::String:
                j[prop.name] = *static_cast<std::string*>(valuePtr);
                break;
            case PropertyType::Vec2: {
                ImVec2 vec = *static_cast<ImVec2*>(valuePtr);
                j[prop.name] = { vec.x, vec.y };
                break;
            }
            case PropertyType::Struct: {
                PropertyStruct* childStruct = static_cast<PropertyStruct*>(valuePtr);
                if (childStruct) {
                    j[prop.name] = SerializeStructProperties(childStruct);
                }
                break;
            }
            }
        }
        return j;
    }

    // 序列化控件属性到JSON
    nlohmann::json SerializeWidgetProperties(ImWidget* widget) {
        nlohmann::json j;
        if (!widget) return j;

        auto properties = widget->GetProperties();
        for (const auto& prop : properties) {
            void* valuePtr = prop.getter();
            if (!valuePtr) continue;

            switch (prop.type) {
            case PropertyType::Color: {
                ImU32 color = *static_cast<ImU32*>(valuePtr);
                j[prop.name] = {
                    IM_COLOR_GET_R(color),
                    IM_COLOR_GET_G(color),
                    IM_COLOR_GET_B(color),
                    IM_COLOR_GET_A(color)
                };
                break;
            }
            case PropertyType::Float:
                j[prop.name] = *static_cast<float*>(valuePtr);
                break;
            case PropertyType::Bool:
                j[prop.name] = *static_cast<bool*>(valuePtr);
                break;
            case PropertyType::Int:
                j[prop.name] = *static_cast<int*>(valuePtr);
                break;
            case PropertyType::String:
                j[prop.name] = *static_cast<std::string*>(valuePtr);
                break;
            case PropertyType::Vec2: {
                ImVec2 vec = *static_cast<ImVec2*>(valuePtr);
                j[prop.name] = { vec.x, vec.y };
                break;
            }
            case PropertyType::Struct: {
                PropertyStruct* childStruct = static_cast<PropertyStruct*>(valuePtr);
                if (childStruct) {
                    j[prop.name] = SerializeStructProperties(childStruct);
                }
                break;
            }
            }
        }
        return j;
    }

    // 反序列化JSON到属性结构体
    void DeserializeStructProperties(PropertyStruct* propStruct, const nlohmann::json& j) {
        if (!propStruct) return;

        auto properties = propStruct->GetProperties();
        for (const auto& prop : properties) {
            if (!j.contains(prop.name)) continue;

            switch (prop.type) {
            case PropertyType::Color: {
                if (j[prop.name].is_array() && j[prop.name].size() == 4) {
                    int r = j[prop.name][0].get<int>();
                    int g = j[prop.name][1].get<int>();
                    int b = j[prop.name][2].get<int>();
                    int a = j[prop.name][3].get<int>();
                    ImU32 color = IM_COLOR_SET_RGBA(r, g, b, a);
                    prop.setter(&color);
                }
                break;
            }
            case PropertyType::Float:
                if (j[prop.name].is_number()) {
                    float val = j[prop.name].get<float>();
                    prop.setter(&val);
                }
                break;
            case PropertyType::Bool:
                if (j[prop.name].is_boolean()) {
                    bool val = j[prop.name].get<bool>();
                    prop.setter(&val);
                }
                break;
            case PropertyType::Int:
                if (j[prop.name].is_number_integer()) {
                    int val = j[prop.name].get<int>();
                    prop.setter(&val);
                }
                break;
            case PropertyType::String:
                if (j[prop.name].is_string()) {
                    std::string val = j[prop.name].get<std::string>();
                    prop.setter(&val);
                }
                break;
            case PropertyType::Vec2: {
                if (j[prop.name].is_array() && j[prop.name].size() == 2) {
                    ImVec2 vec(
                        j[prop.name][0].get<float>(),
                        j[prop.name][1].get<float>()
                    );
                    prop.setter(&vec);
                }
                break;
            }
            case PropertyType::Struct: {
                PropertyStruct* childStruct = static_cast<PropertyStruct*>(prop.getter());
                if (childStruct && j[prop.name].is_object()) {
                    DeserializeStructProperties(childStruct, j[prop.name]);
                }
                break;
            }
            }
        }
    }

    // 反序列化JSON到控件属性
    void DeserializeWidgetProperties(ImWidget* widget, const nlohmann::json& j) {
        if (!widget) return;

        auto properties = widget->GetProperties();
        for (const auto& prop : properties) {
            if (!j.contains(prop.name)) continue;

            switch (prop.type) {
            case PropertyType::Color: {
                if (j[prop.name].is_array() && j[prop.name].size() == 4) {
                    int r = j[prop.name][0].get<int>();
                    int g = j[prop.name][1].get<int>();
                    int b = j[prop.name][2].get<int>();
                    int a = j[prop.name][3].get<int>();
                    ImU32 color = IM_COLOR_SET_RGBA(r, g, b, a);
                    prop.setter(&color);
                }
                break;
            }
            case PropertyType::Float:
                if (j[prop.name].is_number()) {
                    float val = j[prop.name].get<float>();
                    prop.setter(&val);
                }
                break;
            case PropertyType::Bool:
                if (j[prop.name].is_boolean()) {
                    bool val = j[prop.name].get<bool>();
                    prop.setter(&val);
                }
                break;
            case PropertyType::Int:
                if (j[prop.name].is_number_integer()) {
                    int val = j[prop.name].get<int>();
                    prop.setter(&val);
                }
                break;
            case PropertyType::String:
                if (j[prop.name].is_string()) {
                    std::string val = j[prop.name].get<std::string>();
                    prop.setter(&val);
                }
                break;
            case PropertyType::Vec2: {
                if (j[prop.name].is_array() && j[prop.name].size() == 2) {
                    ImVec2 vec(
                        j[prop.name][0].get<float>(),
                        j[prop.name][1].get<float>()
                    );
                    prop.setter(&vec);
                }
                break;
            }
            case PropertyType::Struct: {
                PropertyStruct* childStruct = static_cast<PropertyStruct*>(prop.getter());
                if (childStruct && j[prop.name].is_object()) {
                    DeserializeStructProperties(childStruct, j[prop.name]);
                }
                break;
            }
            }
        }
    }

    // 保存控件属性到文件
    bool SaveWidgetToFile(ImWidget* widget, const std::string& filename) {
        nlohmann::json j = SerializeWidgetProperties(widget);
        if (j.empty()) return false;

        try {
            std::ofstream file(filename);
            if (file.is_open()) {
                file << j.dump(4); // 使用4空格缩进
                return true;
            }
        }
        catch (...) {
            // 处理异常
        }
        return false;
    }

    // 从文件加载控件属性
    bool LoadWidgetFromFile(ImWidget* widget, const std::string& filename) {
        try {
            std::ifstream file(filename);
            if (!file.is_open()) return false;

            nlohmann::json j = nlohmann::json::parse(file);
            DeserializeWidgetProperties(widget, j);
            return true;
        }
        catch (const nlohmann::json::parse_error& e) {
            // 处理JSON解析错误
            //std::cerr << "JSON parse error: " << e.what() << std::endl;
        }
        catch (...) {
            // 处理其他异常
        }
        return false;
    }
}