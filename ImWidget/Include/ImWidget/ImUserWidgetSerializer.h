// ImUserWidgetSerializer.h
#pragma once
#include <nlohmann/json.hpp>
#include <fstream>
#include <imgui.h>
#include <unordered_map>
#include <iostream>
#include <ctime>
#include "ImUserWidgetClass.h"
#include "ImWidget.h"
#include "ImPanelWidget.h"
#include "ImWidgetFactory.h"
#include "ImObjectFactory.h"
#include "ImWidget/ImButton.h"

namespace ImGuiWidget
{
    using json = nlohmann::ordered_json;

    class ImUserWidgetClassSerializer
    {
    public:
        // ================ 新增：四个核心公共函数 ================

        // 1. 将ImObject*转换为Json
        static json SerializeImObject(ImObject* obj)
        {
            if (!obj) return json();

            json j;
            j["Type"] = obj->GetRegisterTypeName();
            j["Properties"] = SerializeObjectPropertiesROP(obj);
            return j;
        }

        // 2. 通过Json对象创建ImObject*
        static ImObject* CreateImObjectFromJson(const json& j)
        {
            if (!j.contains("Type") || !j.contains("Properties"))
                return nullptr;

            std::string type = j["Type"].get<std::string>();
            ImObject* obj = ImObjectFactory::GetInstance().CreateObject(type);
            if (!obj) return nullptr;

            DeserializeObjectPropertiesROP(obj, j["Properties"]);
            return obj;
        }

        // 3. 将ImWidget*转换为Json
        static json SerializeImWidget(ImWidget* widget)
        {
            if (!widget) return json();

            // 序列化控件基本信息
            json j;
            j["Type"] = widget->GetRegisterTypeName();
            j["Name"] = widget->GetWidgetName();
            j["Properties"] = SerializeObjectPropertiesROP(widget);

            // 检查是否为容器控件
            if (auto panel = dynamic_cast<ImPanelWidget*>(widget))
            {
                nlohmann::ordered_json childrenJson = nlohmann::ordered_json::array();

                // 遍历所有子slot
                int slotCount = panel->GetChildNum();
                for (int i = 0; i < slotCount; i++)
                {
                    // 获取子slot
                    ImSlot* slot = panel->GetSlotAt(i);
                    if (!slot) continue;

                    // 获取slot子控件
                    ImWidget* child = slot->GetContent();
                    if (!child) continue;

                    // 递归序列化子控件
                    json childJson = SerializeImWidget(child);

                    // 序列化slot属性
                    childJson["ParentSlot"] = SerializeObjectPropertiesROP(slot);

                    childrenJson.push_back(childJson);
                }

                if (!childrenJson.empty())
                {
                    j["Children"] = childrenJson;
                }
            }

            return j;
        }

        // 4. 通过Json对象创建ImWidget*
        static ImWidget* CreateImWidgetFromJson(const json& j)
        {
            if (!j.contains("Type") || !j.contains("Name"))
                return nullptr;

            // 获取控件类型和名称
            std::string type = j["Type"].get<std::string>();
            std::string name = j["Name"].get<std::string>();

            // 使用工厂创建控件
            ImWidget* widget = ImWidgetFactory::GetInstance().CreateWidget(type, name);
            if (!widget) return nullptr;

            // 反序列化控件属性
            if (j.contains("Properties"))
            {
                DeserializeObjectPropertiesROP(widget, j["Properties"]);
            }

            // 检查是否为容器控件
            if (auto panel = dynamic_cast<ImPanelWidget*>(widget))
            {
                if (j.contains("Children"))
                {
                    for (const auto& childJson : j["Children"])
                    {
                        // 递归创建子控件
                        ImWidget* child = CreateImWidgetFromJson(childJson);
                        if (!child) continue;

                        // 添加到容器
                        ImSlot* slot = panel->AddChild(child);

                        // 反序列化slot属性
                        if (childJson.contains("ParentSlot") && slot)
                        {
                            DeserializeObjectPropertiesROP(slot, childJson["ParentSlot"]);
                        }
                    }
                }
            }

            return widget;
        }

        // ================ 原有公共函数保持不变，但内部调用新函数 ================

        // 通用属性值序列化模板函数
        template<typename T>
        static json SerializeSinglePropertyValue(PropertyType type, const T& value)
        {
            try
            {
                switch (type)
                {
                case PropertyType::Color:
                    if constexpr (std::is_same_v<T, ImU32>)
                    {
                        return json::array({
                            IM_COLOR_GET_R(value),
                            IM_COLOR_GET_G(value),
                            IM_COLOR_GET_B(value),
                            IM_COLOR_GET_A(value)
                            });
                    }
                    break;

                case PropertyType::Float:
                    if constexpr (std::is_same_v<T, float>)
                    {
                        return json(value);
                    }
                    break;

                case PropertyType::Bool:
                    if constexpr (std::is_same_v<T, bool>)
                    {
                        return json(value);
                    }
                    break;

                case PropertyType::Int:
                    if constexpr (std::is_same_v<T, int>)
                    {
                        return json(value);
                    }
                    break;

                case PropertyType::String:
                    if constexpr (std::is_same_v<T, std::string>)
                    {
                        return json(value);
                    }
                    break;

                case PropertyType::Vec2:
                    if constexpr (std::is_same_v<T, ImVec2>)
                    {
                        return json::array({ value.x, value.y });
                    }
                    break;

                case PropertyType::Struct:
                    // 结构体类型需要递归序列化 - 使用新的SerializeImObject函数
                    if constexpr (std::is_same_v<T, ImObject*>)
                    {
                        if (value != nullptr)
                        {
                            return SerializeImObject(value);
                        }
                    }
                    break;

                case PropertyType::StringArray:
                    if constexpr (std::is_same_v<T, std::vector<std::string>>)
                    {
                        return json(value);
                    }
                    break;

                case PropertyType::Enum:
                    // 枚举类型 - 传入的是选项字符串(std::string)
                    if constexpr (std::is_same_v<T, std::string>)
                    {
                        return json(value);
                    }
                    break;

                default:
                    break;
                }
            }
            catch (const std::exception& e)
            {
                std::cerr << "Error serializing property value of type " <<
                    static_cast<int>(type) << ": " << e.what() << std::endl;
            }

            return json();
        }

        // 序列化变量
        static json SerializeVariable(ImWidgetClassVariable* var)
        {
            if (!var) return json();

            json j;
            j["Name"] = var->GetName();
            j["VariableType"] = static_cast<int>(var->GetType());

            switch (var->GetType())
            {
            case WidgetClassVariableType::Widget:
            {
                auto widgetVar = var->As<ImWidgetClassVariable_Widget>();
                if (widgetVar)
                {
                    j["DataType"] = widgetVar->GetTypeString();
                    j["Data"] = SerializeImWidget(widgetVar->GetWidget());  // 使用新函数
                }
                break;
            }
            case WidgetClassVariableType::Object:
            {
                auto objectVar = var->As<ImWidgetClassVariable_Object>();
                if (objectVar)
                {
                    j["DataType"] = objectVar->GetTypeString();
                    j["Data"] = SerializeImObject(objectVar->GetObject());  // 使用新函数
                }
                break;
            }
            case WidgetClassVariableType::Basic:
            {
                auto basicVar = var->As<ImWidgetClassVariable_Basic>();
                if (basicVar)
                {
                    j["DataType"] = basicVar->GetTypeString();
                    j["Data"] = SerializeBasicVariableValue(basicVar);
                }
                break;
            }
            }

            return j;
        }

        // 从JSON创建变量
        static ImWidgetClassVariable* CreateVariableFromJson(const json& j)
        {
            if (!j.contains("Name") || !j.contains("VariableType") || !j.contains("DataType"))
                return nullptr;

            std::string name = j["Name"].get<std::string>();
            WidgetClassVariableType varType = static_cast<WidgetClassVariableType>(
                j["VariableType"].get<int>());
            std::string dataType = j["DataType"].get<std::string>();

            switch (varType)
            {
            case WidgetClassVariableType::Widget:
            {
                if (!j.contains("Data"))
                    return nullptr;

                ImWidget* widget = CreateImWidgetFromJson(j["Data"]);  // 使用新函数
                if (!widget) return nullptr;

                widget->SetWidgetName(name);
                return new ImWidgetClassVariable_Widget(name, widget);
            }

            case WidgetClassVariableType::Object:
            {
                if (!j.contains("Data"))
                    return nullptr;

                ImObject* obj = CreateImObjectFromJson(j["Data"]);  // 使用新函数
                if (!obj) return nullptr;

                return new ImWidgetClassVariable_Object(name, obj);
            }

            case WidgetClassVariableType::Basic:
            {
                if (!j.contains("Data"))
                    return nullptr;

                PropertyType propType = StringToPropertyType(dataType);
                auto basicVar = new ImWidgetClassVariable_Basic(name, propType);

                if (!DeserializeBasicVariableValue(basicVar, j["Data"]))
                {
                    delete basicVar;
                    return nullptr;
                }

                return basicVar;
            }

            default:
                return nullptr;
            }
        }

        // ================ 其他原有公共函数保持不变 ================

        // 序列化ImUserWidgetClass
        static json SerializeUserWidgetClass(const ImUserWidgetClass& widgetClass)
        {
            json j;

            // 基本信息
            j["ClassName"] = widgetClass.GetClassName();
            j["DefaultRoot"] = widgetClass.GetDefaultRootVariableName();
            j["Namespace"] = widgetClass.GetNamespace();
            j["BaseClass"] = widgetClass.GetBaseClass();

            // 序列化所有变量
            json variablesJson = json::array();
            auto variableNames = widgetClass.GetAllVariableNames();

            for (const auto& varName : variableNames)
            {
                ImWidgetClassVariable* var = widgetClass.GetVariable(varName);
                if (var)
                {
                    variablesJson.push_back(SerializeVariable(var));
                }
            }

            j["Variables"] = variablesJson;

            // 元数据
            j["Version"] = "2.0";
            j["Timestamp"] = std::time(nullptr);

            return j;
        }

        // 反序列化ImUserWidgetClass（新格式）
        static bool DeserializeUserWidgetClass(ImUserWidgetClass& widgetClass, const json& j)
        {
            if (!j.contains("ClassName"))
                return false;

            // 清空现有数据
            widgetClass.ClearAllVariables();

            // 设置基本信息
            widgetClass.SetClassName(j["ClassName"].get<std::string>());

            if (j.contains("Namespace"))
                widgetClass.SetNamespace(j["Namespace"].get<std::string>());

            if (j.contains("BaseClass"))
                widgetClass.SetBaseClass(j["BaseClass"].get<std::string>());

            bool success = true;

            // 反序列化变量
            if (j.contains("Variables") && j["Variables"].is_array())
            {
                // 检查版本
                std::string version = j.value("Version", "1.0");

                if (version == "2.0")
                {
                    // 新格式：统一变量存储
                    for (const auto& varJson : j["Variables"])
                    {
                        try
                        {
                            auto var = CreateVariableFromJson(varJson);
                            if (var)
                            {
                                std::string name = var->GetName();

                                // 根据类型添加到目标类
                                switch (var->GetType())
                                {
                                case WidgetClassVariableType::Widget:
                                {
                                    auto widgetVar = var->As<ImWidgetClassVariable_Widget>();
                                    if (widgetVar)
                                    {
                                        // 转移控件所有权
                                        ImWidget* widget = widgetVar->GetWidget();
                                        widgetVar->SetWidget(nullptr, false);
                                        widgetClass.SetWidgetVariableDirect(name, widget);
                                    }
                                    break;
                                }
                                case WidgetClassVariableType::Object:
                                {
                                    auto objectVar = var->As<ImWidgetClassVariable_Object>();
                                    if (objectVar)
                                    {
                                        // 转移对象所有权
                                        ImObject* obj = objectVar->GetObject();
                                        objectVar->SetObject(nullptr, false);
                                        widgetClass.SetObjectVariableDirect(name, obj);
                                    }
                                    break;
                                }
                                case WidgetClassVariableType::Basic:
                                {
                                    auto basicVar = var->As<ImWidgetClassVariable_Basic>();
                                    if (basicVar)
                                    {
                                        // 转移基本变量所有权
                                        widgetClass.SetBasicVariableDirect(name, basicVar);
                                    }
                                    break;
                                }
                                }
                            }
                            else
                            {
                                success = false;
                            }
                        }
                        catch (const std::exception& e)
                        {
                            std::cerr << "Error deserializing variable: " << e.what() << std::endl;
                            success = false;
                        }
                    }
                }
                else
                {
                    // 旧格式
                    success = false;
                }
            }

            // 设置默认根控件
            if (j.contains("DefaultRoot"))
            {
                std::string defaultRoot = j["DefaultRoot"].get<std::string>();
                if (!defaultRoot.empty())
                {
                    widgetClass.SetDefaultRootVariable(defaultRoot);
                }
            }

            return success;
        }

        // 保存到JSON文件
        static bool SaveToFile(const ImUserWidgetClass& widgetClass, const std::string& filePath)
        {
            try
            {
                json j = SerializeUserWidgetClass(widgetClass);

                std::ofstream file(filePath);
                if (!file.is_open())
                {
                    std::cerr << "无法打开文件: " << filePath << std::endl;
                    return false;
                }

                file << j.dump(4);
                file.close();

                return true;
            }
            catch (const std::exception& e)
            {
                std::cerr << "保存文件失败: " << e.what() << std::endl;
                return false;
            }
        }

        // 从JSON文件加载
        static bool LoadFromFile(ImUserWidgetClass& widgetClass, const std::string& filePath)
        {
            try
            {
                std::ifstream file(filePath);
                if (!file.is_open())
                {
                    std::cerr << "无法打开文件: " << filePath << std::endl;
                    return false;
                }

                json j;
                file >> j;
                file.close();

                return DeserializeUserWidgetClass(widgetClass, j);
            }
            catch (const std::exception& e)
            {
                std::cerr << "加载文件失败: " << e.what() << std::endl;
                return false;
            }
        }

    private:
        // ================ 私有辅助函数保持不变 ================

        // 统计属性名出现次数，用于处理重名属性
        static std::unordered_map<std::string, int> CountPropertyNames(ImObject* obj)
        {
            std::unordered_map<std::string, int> nameCount;

            if (!obj) return nameCount;

            // 通过ROP属性系统获取所有属性
            auto properties = obj->GetAllPropertiesOrdered();
            for (const auto& prop : properties)
            {
                nameCount[prop.GetName()]++;
            }

            return nameCount;
        }

        // 生成属性键名：对重名属性使用 "ClassName::PropertyName" 格式
        static std::string GeneratePropertyKey(const ROP::Property<PropertyType>& prop,
            const std::unordered_map<std::string, int>& nameCount)
        {
            std::string propName = prop.GetName();

            // 如果属性名唯一，直接使用属性名
            auto it = nameCount.find(propName);
            if (it != nameCount.end() && it->second == 1)
            {
                return propName;
            }

            // 重名属性：使用 "ClassName::PropertyName" 格式
            return prop.GetClassName() + "::" + propName;
        }

        // 从键名解析类名和属性名
        static std::pair<std::string, std::string> ParsePropertyKey(const std::string& key)
        {
            size_t pos = key.find("::");
            if (pos != std::string::npos)
            {
                return { key.substr(0, pos), key.substr(pos + 2) };
            }
            return { "", key };
        }

        // 序列化对象的所有属性（使用ROP属性系统）
        static json SerializeObjectPropertiesROP(ImObject* obj)
        {
            json j;
            if (!obj) return j;

            // 统计属性名出现次数
            auto nameCount = CountPropertyNames(obj);

            // 获取所有属性（按顺序）
            auto allProps = obj->GetAllPropertiesOrdered();

            for (auto& prop : allProps)
            {
                if (!prop.IsValid()) continue;

                // 生成属性键名
                std::string key = GeneratePropertyKey(prop, nameCount);

                // 序列化属性值
                j[key] = SerializeROPPropertyValue(prop);
            }

            return j;
        }

        // 反序列化对象的所有属性（使用ROP属性系统）
        static void DeserializeObjectPropertiesROP(ImObject* obj, const json& j)
        {
            if (!obj) return;

            for (auto it = j.begin(); it != j.end(); ++it)
            {
                std::string key = it.key();
                auto [className, propName] = ParsePropertyKey(key);

                ROP::Property<PropertyType> prop;

                if (!className.empty())
                {
                    // 使用类名+属性名精确查找
                    prop = obj->GetProperty(propName, className);
                }
                else
                {
                    // 只使用属性名查找（会返回第一个匹配的）
                    prop = obj->GetProperty(propName);
                }

                if (prop.IsValid())
                {
                    DeserializeROPPropertyValue(prop, it.value());
                }
                else
                {
                    std::cerr << "Warning: Property not found: " << key << std::endl;
                }
            }
        }

        // 序列化单个ROP属性值
        static json SerializeROPPropertyValue(ROP::Property<PropertyType>& prop)
        {
            json j;

            try
            {
                switch (prop.GetType())
                {
                case PropertyType::Color:
                {
                    ImU32 color = prop.GetValue<ImU32>();
                    j = SerializeSinglePropertyValue<ImU32>(prop.GetType(), color);
                    break;
                }
                case PropertyType::Float:
                {
                    float value = prop.GetValue<float>();
                    j = SerializeSinglePropertyValue<float>(prop.GetType(), value);
                    break;
                }
                case PropertyType::Bool:
                {
                    bool value = prop.GetValue<bool>();
                    j = SerializeSinglePropertyValue<bool>(prop.GetType(), value);
                    break;
                }
                case PropertyType::Int:
                {
                    int value = prop.GetValue<int>();
                    j = SerializeSinglePropertyValue<int>(prop.GetType(), value);
                    break;
                }
                case PropertyType::String:
                {
                    std::string value = prop.GetValue<std::string>();
                    j = SerializeSinglePropertyValue<std::string>(prop.GetType(), value);
                    break;
                }
                case PropertyType::Vec2:
                {
                    ImVec2 vec = prop.GetValue<ImVec2>();
                    j = SerializeSinglePropertyValue<ImVec2>(prop.GetType(), vec);
                    break;
                }
                case PropertyType::Struct:
                {
                    // 递归序列化子结构 - 使用新的SerializeImObject函数
                    ImObject* childStruct = prop.GetPointer<ImObject>();
                    if (childStruct)
                    {
                        j = SerializeImObject(childStruct);
                    }
                    break;
                }
                case PropertyType::StringArray:
                {
                    std::vector<std::string> vec = prop.GetValue<std::vector<std::string>>();
                    j = SerializeSinglePropertyValue<std::vector<std::string>>(prop.GetType(), vec);
                    break;
                }
                case PropertyType::Enum:
                {
                    // 处理枚举属性
                    auto optionalProp = prop.GetObject()->ToOptionalProperty(prop);
                    if (optionalProp.IsValid())
                    {
                        std::string currentOption = optionalProp.GetOptionString();
                        j = SerializeSinglePropertyValue<std::string>(prop.GetType(), currentOption);
                    }
                    break;
                }
                default:
                    break;
                }
            }
            catch (const std::exception& e)
            {
                std::cerr << "Error serializing property " << prop.GetName() << ": " << e.what() << std::endl;
            }

            return j;
        }

        // 反序列化单个ROP属性值
        static bool DeserializeROPPropertyValue(ROP::Property<PropertyType>& prop, const json& j)
        {
            try
            {
                switch (prop.GetType())
                {
                case PropertyType::Color:
                {
                    ImU32 color;
                    if (DeserializeSinglePropertyValue<ImU32>(prop.GetType(), j, color))
                    {
                        prop.SetValue<ImU32>(color);
                        return true;
                    }
                    break;
                }
                case PropertyType::Float:
                {
                    float value;
                    if (DeserializeSinglePropertyValue<float>(prop.GetType(), j, value))
                    {
                        prop.SetValue<float>(value);
                        return true;
                    }
                    break;
                }
                case PropertyType::Bool:
                {
                    bool value;
                    if (DeserializeSinglePropertyValue<bool>(prop.GetType(), j, value))
                    {
                        prop.SetValue<bool>(value);
                        return true;
                    }
                    break;
                }
                case PropertyType::Int:
                {
                    int value;
                    if (DeserializeSinglePropertyValue<int>(prop.GetType(), j, value))
                    {
                        prop.SetValue<int>(value);
                        return true;
                    }
                    break;
                }
                case PropertyType::String:
                {
                    std::string value;
                    if (DeserializeSinglePropertyValue<std::string>(prop.GetType(), j, value))
                    {
                        prop.SetValue<std::string>(value);
                        return true;
                    }
                    break;
                }
                case PropertyType::Vec2:
                {
                    ImVec2 vec;
                    if (DeserializeSinglePropertyValue<ImVec2>(prop.GetType(), j, vec))
                    {
                        prop.SetValue<ImVec2>(vec);
                        return true;
                    }
                    break;
                }
                case PropertyType::Struct:
                {
                    // 递归反序列化子结构 - 使用新的CreateImObjectFromJson函数
                    ImObject* childStruct = CreateImObjectFromJson(j);
                    if (childStruct)
                    {
                        prop.SetValue<ImObject*>(childStruct);
                        return true;
                    }
                    break;
                }
                case PropertyType::StringArray:
                {
                    std::vector<std::string> vec;
                    if (DeserializeSinglePropertyValue<std::vector<std::string>>(prop.GetType(), j, vec))
                    {
                        prop.SetValue<std::vector<std::string>>(vec);
                        return true;
                    }
                    break;
                }
                case PropertyType::Enum:
                {
                    std::string optionStr;
                    if (DeserializeSinglePropertyValue<std::string>(prop.GetType(), j, optionStr))
                    {
                        auto optionalProp = prop.GetObject()->ToOptionalProperty(prop);
                        if (optionalProp.IsValid())
                        {
                            return optionalProp.SetOptionByString(optionStr);
                        }
                    }
                    break;
                }
                default:
                    break;
                }
            }
            catch (const std::exception& e)
            {
                std::cerr << "Error deserializing property " << prop.GetName() << ": " << e.what() << std::endl;
            }

            return false;
        }

        // 基本变量序列化辅助函数
        static json SerializeBasicVariableValue(ImWidgetClassVariable_Basic* basicVar)
        {
            if (!basicVar) return json();

            json j;
            PropertyType type = basicVar->GetValueType();

            switch (type)
            {
            case PropertyType::Int:
            {
                int value = basicVar->GetValue<int>();
                j = SerializeSinglePropertyValue<int>(type, value);
                break;
            }
            case PropertyType::Float:
            {
                float value = basicVar->GetValue<float>();
                j = SerializeSinglePropertyValue<float>(type, value);
                break;
            }
            case PropertyType::Bool:
            {
                bool value = basicVar->GetValue<bool>();
                j = SerializeSinglePropertyValue<bool>(type, value);
                break;
            }
            case PropertyType::String:
            {
                std::string value = basicVar->GetValue<std::string>();
                j = SerializeSinglePropertyValue<std::string>(type, value);
                break;
            }
            case PropertyType::Color:
            {
                ImU32 color = basicVar->GetValue<ImU32>();
                j = SerializeSinglePropertyValue<ImU32>(type, color);
                break;
            }
            case PropertyType::Vec2:
            {
                ImVec2 vec = basicVar->GetValue<ImVec2>();
                j = SerializeSinglePropertyValue<ImVec2>(type, vec);
                break;
            }
            default:
                break;
            }

            return j;
        }

        // 反序列化基本变量值
        static bool DeserializeBasicVariableValue(ImWidgetClassVariable_Basic* basicVar, const json& j)
        {
            if (!basicVar) return false;

            PropertyType type = basicVar->GetValueType();

            try
            {
                switch (type)
                {
                case PropertyType::Int:
                {
                    int value;
                    if (DeserializeSinglePropertyValue<int>(type, j, value))
                    {
                        return basicVar->SetValue<int>(value);
                    }
                    break;
                }
                case PropertyType::Float:
                {
                    float value;
                    if (DeserializeSinglePropertyValue<float>(type, j, value))
                    {
                        return basicVar->SetValue<float>(value);
                    }
                    break;
                }
                case PropertyType::Bool:
                {
                    bool value;
                    if (DeserializeSinglePropertyValue<bool>(type, j, value))
                    {
                        return basicVar->SetValue<bool>(value);
                    }
                    break;
                }
                case PropertyType::String:
                {
                    std::string value;
                    if (DeserializeSinglePropertyValue<std::string>(type, j, value))
                    {
                        return basicVar->SetValue<std::string>(value);
                    }
                    break;
                }
                case PropertyType::Color:
                {
                    ImU32 color;
                    if (DeserializeSinglePropertyValue<ImU32>(type, j, color))
                    {
                        return basicVar->SetValue<ImU32>(color);
                    }
                    break;
                }
                case PropertyType::Vec2:
                {
                    ImVec2 vec;
                    if (DeserializeSinglePropertyValue<ImVec2>(type, j, vec))
                    {
                        return basicVar->SetValue<ImVec2>(vec);
                    }
                    break;
                }
                default:
                    break;
                }
            }
            catch (const std::exception& e)
            {
                std::cerr << "Error deserializing basic variable: " << e.what() << std::endl;
            }

            return false;
        }

        // 通用属性值反序列化模板函数
        template<typename T>
        static bool DeserializeSinglePropertyValue(PropertyType type, const json& j, T& outValue)
        {
            try
            {
                switch (type)
                {
                case PropertyType::Color:
                    if constexpr (std::is_same_v<T, ImU32>)
                    {
                        if (j.is_array() && j.size() == 4)
                        {
                            int r = j[0].template get<int>();
                            int g = j[1].template get<int>();
                            int b = j[2].template get<int>();
                            int a = j[3].template get<int>();
                            outValue = IM_COLOR_SET_RGBA(r, g, b, a);
                            return true;
                        }
                    }
                    break;

                case PropertyType::Float:
                    if constexpr (std::is_same_v<T, float>)
                    {
                        if (j.is_number())
                        {
                            outValue = j.template get<float>();
                            return true;
                        }
                    }
                    break;

                case PropertyType::Bool:
                    if constexpr (std::is_same_v<T, bool>)
                    {
                        if (j.is_boolean())
                        {
                            outValue = j.template get<bool>();
                            return true;
                        }
                    }
                    break;

                case PropertyType::Int:
                    if constexpr (std::is_same_v<T, int>)
                    {
                        if (j.is_number_integer())
                        {
                            outValue = j.template get<int>();
                            return true;
                        }
                    }
                    break;

                case PropertyType::String:
                    if constexpr (std::is_same_v<T, std::string>)
                    {
                        if (j.is_string())
                        {
                            outValue = j.template get<std::string>();
                            return true;
                        }
                    }
                    break;

                case PropertyType::Vec2:
                    if constexpr (std::is_same_v<T, ImVec2>)
                    {
                        if (j.is_array() && j.size() == 2)
                        {
                            outValue = ImVec2(
                                j[0].template get<float>(),
                                j[1].template get<float>()
                            );
                            return true;
                        }
                    }
                    break;

                case PropertyType::Struct:
                    // 结构体类型 - 使用新的CreateImObjectFromJson函数
                    if constexpr (std::is_same_v<T, ImObject*>)
                    {
                        outValue = CreateImObjectFromJson(j);
                        return outValue != nullptr;
                    }
                    break;

                case PropertyType::StringArray:
                    if constexpr (std::is_same_v<T, std::vector<std::string>>)
                    {
                        if (j.is_array())
                        {
                            outValue = j.template get<std::vector<std::string>>();
                            return true;
                        }
                    }
                    break;

                case PropertyType::Enum:
                    // 枚举类型 - 反序列化为字符串
                    if constexpr (std::is_same_v<T, std::string>)
                    {
                        if (j.is_string())
                        {
                            outValue = j.template get<std::string>();
                            return true;
                        }
                    }
                    break;

                default:
                    break;
                }
            }
            catch (const std::exception& e)
            {
                std::cerr << "Error deserializing property value of type " <<
                    static_cast<int>(type) << ": " << e.what() << std::endl;
            }

            return false;
        }

    public:

        // ================ 调试和验证函数 ================

        // 打印序列化结果（用于调试）
        static void PrintSerializedData(const ImUserWidgetClass& widgetClass)
        {
            json j = SerializeUserWidgetClass(widgetClass);
            std::cout << "Serialized UserWidgetClass:" << std::endl;
            std::cout << j.dump(2) << std::endl;
        }

        // 验证序列化/反序列化完整性
        static bool ValidateSerialization(const ImUserWidgetClass& widgetClass)
        {
            try
            {
                // 序列化
                json j = SerializeUserWidgetClass(widgetClass);

                // 创建新的对象
                ImUserWidgetClass testClass(widgetClass.GetClassName());
                testClass.SetNamespace(widgetClass.GetNamespace());
                testClass.SetBaseClass(widgetClass.GetBaseClass());

                // 反序列化
                if (!DeserializeUserWidgetClass(testClass, j))
                {
                    std::cerr << "反序列化失败" << std::endl;
                    return false;
                }

                // 验证基本信息
                if (testClass.GetClassName() != widgetClass.GetClassName() ||
                    testClass.GetDefaultRootVariableName() != widgetClass.GetDefaultRootVariableName())
                {
                    std::cerr << "基本信息不匹配" << std::endl;
                    return false;
                }

                // 验证变量数量
                if (testClass.GetAllVariableNames().size() != widgetClass.GetAllVariableNames().size())
                {
                    std::cerr << "变量数量不匹配" << std::endl;
                    return false;
                }

                std::cout << "序列化/反序列化验证通过" << std::endl;
                return true;
            }
            catch (const std::exception& e)
            {
                std::cerr << "验证失败: " << e.what() << std::endl;
                return false;
            }
        }
    };
}