#pragma once
#include "ImWidgetProperty.h"
#include "ImWidget.h"
#include "ImPanelWidget.h"
#include "ImWidgetFactory.h"
#include <json.hpp>
#include <vector>
#include <functional>
#include <fstream>

namespace ImGuiWidget
{
    nlohmann::ordered_json SerializeProperties(PropertyStruct* obj);
    void DeserializeProperties(PropertyStruct* obj, const nlohmann::ordered_json& j);



    // 单个属性的序列化函数
    nlohmann::ordered_json SerializeProperty(PropertyType type, void* valuePtr)
    {
        nlohmann::ordered_json j;

        switch (type)
        {
        case PropertyType::Color: {
            ImU32 color = *static_cast<ImU32*>(valuePtr);
            j = {
                IM_COLOR_GET_R(color),
                IM_COLOR_GET_G(color),
                IM_COLOR_GET_B(color),
                IM_COLOR_GET_A(color)
            };
            break;
        }
        case PropertyType::Float:
            j = *static_cast<float*>(valuePtr);
            break;
        case PropertyType::Bool:
            j = *static_cast<bool*>(valuePtr);
            break;
        case PropertyType::Int:
            j = *static_cast<int*>(valuePtr);
            break;
        case PropertyType::String:
            j = *static_cast<std::string*>(valuePtr);
            break;
        case PropertyType::Vec2:
        {
            ImVec2 vec = *static_cast<ImVec2*>(valuePtr);
            j = { vec.x, vec.y };
            break;
        }
        case PropertyType::Struct:
        {
            // 嵌套结构体需要递归处理
            PropertyStruct* childStruct = static_cast<PropertyStruct*>(valuePtr);
            if (childStruct)
            {
                j = SerializeProperties(childStruct);
            }
            break;
        }
        case PropertyType::StringArray:
        {
            std::vector<std::string>* vec = static_cast<std::vector<std::string>*>(valuePtr);
            j = *vec;
            break;
        }
        case PropertyType::Enum:
        {
            // 获取枚举当前值
            std::vector<std::string>* vec = static_cast<std::vector<std::string>*>(valuePtr);
            if (vec && !vec->empty())
            {
                j = vec->back();
            }
            break;
        }
        }

        return j;
    }

    // 单个属性的反序列化函数
    bool DeserializeProperty(PropertyType type, void* valuePtr, const nlohmann::ordered_json& j)
    {
        switch (type)
        {
        case PropertyType::Color: {
            if (j.is_array() && j.size() == 4)
            {
                int r = j[0].template get<int>();
                int g = j[1].template get<int>();
                int b = j[2].template get<int>();
                int a = j[3].template get<int>();
                ImU32 color = IM_COLOR_SET_RGBA(r, g, b, a);
                *static_cast<ImU32*>(valuePtr) = color;
                return true;
            }
            break;
        }
        case PropertyType::Float:
            if (j.is_number())
            {
                *static_cast<float*>(valuePtr) = j.template get<float>();
                return true;
            }
            break;
        case PropertyType::Bool:
            if (j.is_boolean())
            {
                *static_cast<bool*>(valuePtr) = j.template get<bool>();
                return true;
            }
            break;
        case PropertyType::Int:
            if (j.is_number_integer())
            {
                *static_cast<int*>(valuePtr) = j.template get<int>();
                return true;
            }
            break;
        case PropertyType::String:
            if (j.is_string())
            {
                *static_cast<std::string*>(valuePtr) = j.template get<std::string>();
                return true;
            }
            break;
        case PropertyType::Vec2: {
            if (j.is_array() && j.size() == 2)
            {
                ImVec2 vec(
                    j[0].template get<float>(),
                    j[1].template get<float>()
                );
                *static_cast<ImVec2*>(valuePtr) = vec;
                return true;
            }
            break;
        }
        case PropertyType::Struct:
        {
            // 嵌套结构体需要递归处理
            PropertyStruct* childStruct = static_cast<PropertyStruct*>(valuePtr);
            if (childStruct && j.is_object())
            {
                DeserializeProperties(childStruct, j);
                return true;
            }
            break;
        }
        case PropertyType::StringArray:
        {
            if (j.is_array())
            {
                *static_cast<std::vector<std::string>*>(valuePtr) = j.template get<std::vector<std::string>>();
                return true;
            }
            break;
        }
        case PropertyType::Enum:
        {
            if (j.is_string())
            {
                std::string val = j.template get<std::string>();
                // 对于枚举类型，我们需要更新vector中的最后一个元素
                std::vector<std::string>* vec = static_cast<std::vector<std::string>*>(valuePtr);
                if (vec && !vec->empty())
                {
                    vec->back() = val;
                }
                return true;
            }
            break;
        }
        }

        return false;
    }

    // 通用的属性序列化函数
    nlohmann::ordered_json SerializeProperties(PropertyStruct* obj)
    {
        nlohmann::ordered_json j;
        if (!obj) return j;

        auto properties = obj->GetProperties();
        for (const auto& prop : properties)
        {
            void* valuePtr = prop.getter();
            if (!valuePtr) continue;

            j[prop.name] = SerializeProperty(prop.type, valuePtr);
        }

        return j;
    }

    // 通用的属性反序列化函数
    void DeserializeProperties(PropertyStruct* obj, const nlohmann::ordered_json& j)
    {
        if (!obj) return;

        auto properties = obj->GetProperties();
        for (const auto& prop : properties)
        {
            if (!j.contains(prop.name)) continue;

            void* valuePtr = prop.getter();
            if (!valuePtr) continue;

            DeserializeProperty(prop.type, valuePtr, j[prop.name]);
        }
    }

    // 序列化控件树
    nlohmann::ordered_json SerializeWidgetTree(ImWidget* root)
    {
        nlohmann::ordered_json j;
        if (!root) return j;

        // 序列化当前控件
        j["Type"] = root->GetRegisterTypeName();
        j["Name"] = root->GetWidgetName();
        j["Properties"] = SerializeProperties(root);

        // 判断是否为容器控件
        if (auto panel = dynamic_cast<ImPanelWidget*>(root))
        {
            nlohmann::ordered_json childrenJson = nlohmann::ordered_json::array();

            // 使用索引获取子slot
            int slotCount = panel->GetChildNum();
            for (int i = 0; i < slotCount; i++)
            {
                // 使用索引获取slot
                ImSlot* slot = panel->GetSlotAt(i);
                if (!slot) continue;

                // 使用slot获取子控件
                ImWidget* child = slot->GetContent();
                if (!child) continue;

                nlohmann::ordered_json childJson = SerializeWidgetTree(child);

                // 序列化slot
                nlohmann::ordered_json slotJson = SerializeProperties(slot);
                childJson["ParentSlot"] = slotJson;

                childrenJson.push_back(childJson);
            }

            if (!childrenJson.empty())
            {
                j["Children"] = childrenJson;
            }
        }

        return j;
    }

    ImWidget* CreateWidgetFromJson(const nlohmann::ordered_json& j)
    {
        if (j.empty()) return nullptr;

        // 创建控件实例
        std::string type = j["Type"].get<std::string>();
        std::string name = j["Name"].get<std::string>();
        ImWidget* widget = ImWidgetFactory::GetInstance().CreateWidget(type, name);
        if (!widget) return nullptr;

        // 反序列化控件属性
        if (j.contains("Properties"))
        {
            DeserializeProperties(widget, j["Properties"]);
        }

        // 判断是否为容器控件
        if (auto panel = dynamic_cast<ImPanelWidget*>(widget))
        {
            if (j.contains("Children"))
            {
                for (const auto& childJson : j["Children"])
                {
                    ImWidget* child = CreateWidgetFromJson(childJson);
                    if (!child) continue;

                    // 添加到父控件
                    ImSlot* slot = panel->AddChild(child);

                    // 反序列化slot属性
                    if (childJson.contains("ParentSlot") && slot)
                    {
                        DeserializeProperties(slot, childJson["ParentSlot"]);
                    }
                }
            }
        }

        return widget;
    }

    // 保存控件树到文件
    bool SaveWidgetTreeToFile(ImWidget* root, const std::string& filename)
    {
        nlohmann::ordered_json j = SerializeWidgetTree(root);
        if (j.empty()) return false;

        try
        {
            std::ofstream file(filename);
            if (file.is_open())
            {
                file << j.dump(4); // 使用4空格缩进
                return true;
            }
        }
        catch (...)
        {
            // 异常处理
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

}