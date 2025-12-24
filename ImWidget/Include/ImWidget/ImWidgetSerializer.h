#pragma once
#include "ImWidgetProperty.h"
#include "ImWidget.h"
#include "ImWidgetFactory.h"
#include <json.hpp>
#include <vector>
#include <functional>
#include <fstream>

namespace ImGuiWidget
{
    template<typename T>
    nlohmann::ordered_json SerializeProperties(T* obj) {
        nlohmann::ordered_json j;
        if (!obj) return j;

        // 使用对象的 GetProperties() 方法
        auto properties = obj->GetProperties();
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
            case PropertyType::Vec2: 
            {
                ImVec2 vec = *static_cast<ImVec2*>(valuePtr);
                j[prop.name] = { vec.x, vec.y };
                break;
            }
            case PropertyType::Struct: 
            {
                // 对于嵌套结构，需要递归处理
                PropertyStruct* childStruct = static_cast<PropertyStruct*>(valuePtr);
                if (childStruct) {
                    j[prop.name] = SerializeProperties(childStruct);
                }
                break;
            }
            case PropertyType::StringArray: 
            {
                std::vector<std::string>* vec = static_cast<std::vector<std::string>*>(valuePtr);
                j[prop.name] = *vec; // 直接序列化为JSON数组
                break;
            }
            case PropertyType::Enum: 
            {
                // 获取枚举选项和当前值
                std::vector<std::string>* vec = static_cast<std::vector<std::string>*>(valuePtr);
                if (vec && !vec->empty()) {
                    // 最后一个元素是当前选中的值
                    j[prop.name] = vec->back();
                }
                break;
            }
            }
        }
        return j;
    }

    // 通用反序列化函数（模板版）
    template<typename T>
    void DeserializeProperties(T* obj, const nlohmann::ordered_json& j) {
        if (!obj) return;

        // 使用对象的 GetProperties() 方法
        auto properties = obj->GetProperties();
        for (const auto& prop : properties) {
            if (!j.contains(prop.name)) continue;

            switch (prop.type) {
            case PropertyType::Color: {
                if (j[prop.name].is_array() && j[prop.name].size() == 4) {
                    int r = j[prop.name][0].template get<int>();
                    int g = j[prop.name][1].template get<int>();
                    int b = j[prop.name][2].template get<int>();
                    int a = j[prop.name][3].template get<int>();
                    ImU32 color = IM_COLOR_SET_RGBA(r, g, b, a);
                    prop.setter(&color);
                }
                break;
            }
            case PropertyType::Float:
                if (j[prop.name].is_number()) {
                    float val = j[prop.name].template get<float>();
                    prop.setter(&val);
                }
                break;
            case PropertyType::Bool:
                if (j[prop.name].is_boolean()) {
                    bool val = j[prop.name].template get<bool>();
                    prop.setter(&val);
                }
                break;
            case PropertyType::Int:
                if (j[prop.name].is_number_integer()) {
                    int val = j[prop.name].template get<int>();
                    prop.setter(&val);
                }
                break;
            case PropertyType::String:
                if (j[prop.name].is_string()) {
                    std::string val = j[prop.name].template get<std::string>();
                    prop.setter(&val);
                }
                break;
            case PropertyType::Vec2: {
                if (j[prop.name].is_array() && j[prop.name].size() == 2) {
                    ImVec2 vec(
                        j[prop.name][0].template get<float>(),
                        j[prop.name][1].template get<float>()
                    );
                    prop.setter(&vec);
                }
                break;
            }
            case PropertyType::Struct: 
            {
                // 对于嵌套结构，需要递归处理
                PropertyStruct* childStruct = static_cast<PropertyStruct*>(prop.getter());
                if (childStruct && j[prop.name].is_object()) 
                {
                    DeserializeProperties(childStruct, j[prop.name]);
                }
                break;
            }
            case PropertyType::StringArray: 
            {
                if (j[prop.name].is_array()) 
                {
                    std::vector<std::string> vec = j[prop.name].template get<std::vector<std::string>>();
                    prop.setter(&vec);
                }
                break;
            }
            case PropertyType::Enum: 
            {
                if (j[prop.name].is_string()) 
                {
                    std::string val = j[prop.name].template get<std::string>();
                    prop.setter(&val);
                }
                break;
            }
            }
        }
    }

    // 序列化控件属性
    inline nlohmann::ordered_json SerializeWidgetProperties(ImWidget* widget) {
        return SerializeProperties(widget);
    }

    // 序列化slot属性
    inline nlohmann::ordered_json SerializeSlotProperties(ImSlot* slot) {
        return SerializeProperties(slot);
    }

    // 序列化结构体属性
    inline nlohmann::ordered_json SerializeStructProperties(PropertyStruct* propertystruct) {
        return SerializeProperties(propertystruct);
    }

    // 反序列化控件属性
    inline void DeserializeWidgetProperties(ImWidget* widget, const nlohmann::ordered_json& j) {
        DeserializeProperties(widget, j);
    }

    // 反序列化slot属性
    inline void DeserializeSlotProperties(ImSlot* slot, const nlohmann::ordered_json& j) {
        DeserializeProperties(slot, j);
    }

    // 反序列化结构体属性
    inline void DeserializeStructProperties(PropertyStruct* propertystruct, const nlohmann::ordered_json& j) {
        DeserializeProperties(propertystruct, j);
    }

    // 序列化整个控件树（包括子控件和slot）
    nlohmann::ordered_json SerializeWidgetTree(ImWidget* root) {
        nlohmann::ordered_json j;
        if (!root) return j;

        // 序列化当前控件
        j["Type"] = root->GetRegisterTypeName();
        j["Name"] = root->GetWidgetName();
        j["Properties"] = SerializeWidgetProperties(root);

        // 序列化slot（如果存在）
        //if (auto slot = root->GetSlot()) {
        //    j["Slot"] = SerializeStructProperties(slot);
        //}

        // 检查是否是容器控件
        if (auto panel = dynamic_cast<ImPanelWidget*>(root)) {
            nlohmann::ordered_json childrenJson = nlohmann::ordered_json::array();

            // 使用现有方法获取slot数量
            int slotCount = panel->GetSlotNum();
            for (int i = 0; i < slotCount; i++) {
                // 使用现有方法获取slot
                ImSlot* slot = panel->GetSlot(i);
                if (!slot) continue;

                // 使用现有方法获取子控件
                ImWidget* child = slot->GetContent();
                if (!child) continue;

                nlohmann::ordered_json childJson = SerializeWidgetTree(child);

                // 序列化slot属性
                nlohmann::ordered_json slotJson = SerializeSlotProperties(slot);
                childJson["ParentSlot"] = slotJson;

                childrenJson.push_back(childJson);
            }

            if (!childrenJson.empty()) {
                j["Children"] = childrenJson;
            }
        }

        return j;
    }

    ImWidget* CreateWidgetFromJson(const nlohmann::ordered_json& j) {
        if (j.empty()) return nullptr;

        // 创建控件实例
        std::string type = j["Type"].get<std::string>();
        std::string name = j["Name"].get<std::string>();
        ImWidget* widget = ImWidgetFactory::GetInstance().CreateWidget(type, name);
        if (!widget) return nullptr;

        // 设置控件属性
        if (j.contains("Properties")) {
            DeserializeWidgetProperties(widget, j["Properties"]);
        }

        //// 设置slot属性（如果存在）
        //if (j.contains("Slot") && widget->GetSlot()) {
        //    DeserializeStructProperties(widget->GetSlot(), j["Slot"]);
        //}

        // 检查是否是容器控件
        if (auto panel = dynamic_cast<ImPanelWidget*>(widget)) {
            if (j.contains("Children")) {
                for (const auto& childJson : j["Children"]) {
                    ImWidget* child = CreateWidgetFromJson(childJson);
                    if (!child) continue;

                    // 添加到容器
                    ImSlot* slot = panel->AddChild(child);

                    // 设置slot属性（如果存在）
                    if (childJson.contains("ParentSlot") && slot) {
                        DeserializeSlotProperties(slot, childJson["ParentSlot"]);
                    }
                }
            }
        }

        return widget;
    }

    // 保存整个控件树到文件
    bool SaveWidgetTreeToFile(ImWidget* root, const std::string& filename) {
        nlohmann::ordered_json j = SerializeWidgetTree(root);
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

    ImWidget* LoadWidgetTreeFromFile(const std::string& filename)
    {
        std::ifstream file(filename);
        if (!file.is_open()) return nullptr;

        nlohmann::ordered_json j = nlohmann::ordered_json::parse(file);
        return CreateWidgetFromJson(j);
    }

    // 保存控件属性到文件
    bool SaveWidgetToFile(ImWidget* widget, const std::string& filename) {
        nlohmann::ordered_json j = SerializeWidgetProperties(widget);
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

            nlohmann::ordered_json j = nlohmann::ordered_json::parse(file);
            DeserializeWidgetProperties(widget, j);
            return true;
        }
        catch (const nlohmann::ordered_json::parse_error& e) {
            // 处理JSON解析错误
            //std::cerr << "JSON parse error: " << e.what() << std::endl;
        }
        catch (...) {
            // 处理其他异常
        }
        return false;
    }
}