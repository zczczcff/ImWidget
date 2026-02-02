#pragma once
#include <nlohmann/json.hpp>
#include <fstream>
#include <imgui.h>
#include "ImUserWidgetClass.h"
#include "ImWidget.h"
#include "ImPanelWidget.h"
#include "ImWidgetFactory.h"
#include "ImObjectFactory.h"
#include <unordered_set>
#include "ImWidget/ImButton.h"
namespace ImGuiWidget
{
    using json = nlohmann::ordered_json;

    class ImUserWidgetClassSerializer
    {
    private:
        // ================ 新的ROP属性序列化辅助函数 ================

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
            return { "", key }; // 无类名前缀
        }

        // 序列化单个属性值
        static json SerializePropertyValue(ROP::Property<PropertyType>& prop)
        {
            json j;

            try
            {
                switch (prop.GetType())
                {
                case PropertyType::Color: {
                    ImU32 color = prop.GetValue<ImU32>();
                    j = {
                        IM_COLOR_GET_R(color),
                        IM_COLOR_GET_G(color),
                        IM_COLOR_GET_B(color),
                        IM_COLOR_GET_A(color)
                    };
                    break;
                }
                case PropertyType::Float:
                    j = prop.GetValue<float>();
                    break;
                case PropertyType::Bool:
                    j = prop.GetValue<bool>();
                    break;
                case PropertyType::Int:
                    j = prop.GetValue<int>();
                    break;
                case PropertyType::String:
                    j = prop.GetValue<std::string>();
                    break;
                case PropertyType::Vec2:
                {
                    ImVec2 vec = prop.GetValue<ImVec2>();
                    j = { vec.x, vec.y };
                    break;
                }
                case PropertyType::Struct:
                {
                    // 递归序列化子结构
                    ImObject* childStruct = prop.GetPointer<ImObject>();
                    if (childStruct)
                    {
                        j = SerializeObjectPropertiesROP(childStruct);
                    }
                    break;
                }
                case PropertyType::StringArray:
                {
                    std::vector<std::string> vec = prop.GetValue<std::vector<std::string>>();
                    j = vec;
                    break;
                }
                case PropertyType::Enum:
                {
                    // 处理枚举属性
                    auto optionalProp = prop.GetObject()->ToOptionalProperty(prop);
                    if (optionalProp.IsValid())
                    {
                        // 保存当前选择的选项字符串
                        j = optionalProp.GetOptionString();
                    }
                    break;
                }
                }
            }
            catch (const std::exception& e)
            {
                std::cerr << "Error serializing property " << prop.GetName() << ": " << e.what() << std::endl;
            }

            return j;
        }

        // 反序列化单个属性值
        static bool DeserializePropertyValue(ROP::Property<PropertyType>& prop, const json& j)
        {
            try
            {
                switch (prop.GetType())
                {
                case PropertyType::Color: {
                    if (j.is_array() && j.size() == 4)
                    {
                        int r = j[0].template get<int>();
                        int g = j[1].template get<int>();
                        int b = j[2].template get<int>();
                        int a = j[3].template get<int>();
                        ImU32 color = IM_COLOR_SET_RGBA(r, g, b, a);
                        prop.SetValue<ImU32>(color);
                        return true;
                    }
                    break;
                }
                case PropertyType::Float:
                    if (j.is_number())
                    {
                        prop.SetValue<float>(j.template get<float>());
                        return true;
                    }
                    break;
                case PropertyType::Bool:
                    if (j.is_boolean())
                    {
                        prop.SetValue<bool>(j.template get<bool>());
                        return true;
                    }
                    break;
                case PropertyType::Int:
                    if (j.is_number_integer())
                    {
                        prop.SetValue<int>(j.template get<int>());
                        return true;
                    }
                    break;
                case PropertyType::String:
                    if (j.is_string())
                    {
                        prop.SetValue<std::string>(j.template get<std::string>());
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
                        prop.SetValue<ImVec2>(vec);
                        return true;
                    }
                    break;
                }
                case PropertyType::Struct:
                {
                    // 递归反序列化子结构
                    ImObject* childStruct = prop.GetPointer<ImObject>();
                    if (childStruct && j.is_object())
                    {
                        DeserializeObjectPropertiesROP(childStruct, j);
                        return true;
                    }
                    break;
                }
                case PropertyType::StringArray:
                {
                    if (j.is_array())
                    {
                        prop.SetValue<std::vector<std::string>>(j.template get<std::vector<std::string>>());
                        return true;
                    }
                    break;
                }
                case PropertyType::Enum:
                {
                    if (j.is_string())
                    {
                        // 通过OptionalProperty设置枚举值
                        auto optionalProp = prop.GetObject()->ToOptionalProperty(prop);
                        if (optionalProp.IsValid())
                        {
                            return optionalProp.SetOptionByString(j.template get<std::string>());
                        }
                    }
                    break;
                }
                }
            }
            catch (const std::exception& e)
            {
                std::cerr << "Error deserializing property " << prop.GetName() << ": " << e.what() << std::endl;
            }

            return false;
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
                j[key] = SerializePropertyValue(prop);
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
                    DeserializePropertyValue(prop, it.value());
                }
                else
                {
                    std::cerr << "Warning: Property not found: " << key << std::endl;
                }
            }
        }

        // ================ 基本变量序列化辅助函数 ================

        // 序列化基本变量类型到字符串
        static std::string BasicTypeToString(ImBasicVariable::BasicType type)
        {
            switch (type)
            {
            case ImBasicVariable::BasicType::Int: return "Int";
            case ImBasicVariable::BasicType::Float: return "Float";
            case ImBasicVariable::BasicType::Bool: return "Bool";
            case ImBasicVariable::BasicType::String: return "String";
            case ImBasicVariable::BasicType::Color: return "Color";
            default: return "Unknown";
            }
        }

        // 从字符串反序列化基本变量类型
        static ImBasicVariable::BasicType StringToBasicType(const std::string& str)
        {
            if (str == "Int") return ImBasicVariable::BasicType::Int;
            if (str == "Float") return ImBasicVariable::BasicType::Float;
            if (str == "Bool") return ImBasicVariable::BasicType::Bool;
            if (str == "String") return ImBasicVariable::BasicType::String;
            if (str == "Color") return ImBasicVariable::BasicType::Color;
            return ImBasicVariable::BasicType::Int;
        }

        // 序列化基本变量值（直接使用GetValuePtr）
        static json SerializeBasicVariableValue(ImBasicVariable* var)
        {
            if (!var) return json();

            void* valuePtr = var->GetValuePtr();
            if (!valuePtr) return json();

            json j;

            switch (var->GetBasicType())
            {
            case ImBasicVariable::BasicType::Int:
                j = *static_cast<int*>(valuePtr);
                break;
            case ImBasicVariable::BasicType::Float:
                j = *static_cast<float*>(valuePtr);
                break;
            case ImBasicVariable::BasicType::Bool:
                j = *static_cast<bool*>(valuePtr);
                break;
            case ImBasicVariable::BasicType::String:
                j = *static_cast<std::string*>(valuePtr);
                break;
            case ImBasicVariable::BasicType::Color:
            {
                ImU32 color = *static_cast<ImU32*>(valuePtr);
                j = {
                    IM_COLOR_GET_R(color),
                    IM_COLOR_GET_G(color),
                    IM_COLOR_GET_B(color),
                    IM_COLOR_GET_A(color)
                };
                break;
            }
            default:
                break;
            }

            return j;
        }

        // 反序列化基本变量值（直接使用GetValuePtr）
        static bool DeserializeBasicVariableValue(ImBasicVariable* var, const json& j)
        {
            if (!var) return false;

            void* valuePtr = var->GetValuePtr();
            if (!valuePtr) return false;

            try
            {
                switch (var->GetBasicType())
                {
                case ImBasicVariable::BasicType::Int:
                    if (j.is_number_integer())
                    {
                        *static_cast<int*>(valuePtr) = j.template get<int>();
                        return true;
                    }
                    break;
                case ImBasicVariable::BasicType::Float:
                    if (j.is_number())
                    {
                        *static_cast<float*>(valuePtr) = j.template get<float>();
                        return true;
                    }
                    break;
                case ImBasicVariable::BasicType::Bool:
                    if (j.is_boolean())
                    {
                        *static_cast<bool*>(valuePtr) = j.template get<bool>();
                        return true;
                    }
                    break;
                case ImBasicVariable::BasicType::String:
                    if (j.is_string())
                    {
                        *static_cast<std::string*>(valuePtr) = j.template get<std::string>();
                        return true;
                    }
                    break;
                case ImBasicVariable::BasicType::Color:
                    if (j.is_array() && j.size() == 4)
                    {
                        int r = j[0].template get<int>();
                        int g = j[1].template get<int>();
                        int b = j[2].template get<int>();
                        int a = j[3].template get<int>();
                        *static_cast<ImU32*>(valuePtr) = IM_COLOR_SET_RGBA(r, g, b, a);
                        return true;
                    }
                    break;
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

        // 序列化基本变量（直接使用GetValuePtr）
        static json SerializeBasicVariable(ImBasicVariable* var)
        {
            json j;
            if (!var) return j;

            j["Name"] = var->GetName();
            j["Type"] = BasicTypeToString(var->GetBasicType());
            j["Value"] = SerializeBasicVariableValue(var);

            return j;
        }

        // 反序列化基本变量（直接使用GetValuePtr）
        static ImBasicVariable* DeserializeBasicVariable(const json& j)
        {
            if (!j.contains("Name") || !j.contains("Type") || !j.contains("Value"))
                return nullptr;

            std::string name = j["Name"].get<std::string>();
            ImBasicVariable::BasicType type = StringToBasicType(j["Type"].get<std::string>());

            // 创建基本变量
            ImBasicVariable* var = new ImBasicVariable(name, type);

            // 设置值
            if (!DeserializeBasicVariableValue(var, j["Value"]))
            {
                delete var;
                return nullptr;
            }

            return var;
        }
    public:
        // ================ 控件树序列化辅助函数 ================

        // 序列化控件树（使用ROP属性系统）
        static json SerializeWidgetTree(ImWidget* root)
        {
            json j;
            if (!root) return j;

            // 序列化根控件信息
            j["Type"] = root->GetRegisterTypeName();
            j["Name"] = root->GetWidgetName();
            j["Properties"] = SerializeObjectPropertiesROP(root);

            // 检查是否为容器控件
            if (auto panel = dynamic_cast<ImPanelWidget*>(root))
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
                    json childJson = SerializeWidgetTree(child);

                    // 序列化slot属性（使用ROP属性系统）
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

        // 从JSON创建控件树（使用ROP属性系统）
        static ImWidget* CreateWidgetFromJson(const json& j)
        {
            if (j.empty()) return nullptr;

            // 获取控件类型
            std::string type = j["Type"].get<std::string>();
            std::string name = j["Name"].get<std::string>();

            // 使用工厂创建控件
            ImWidget* widget = ImWidgetFactory::GetInstance().CreateWidget(type, name);
            if (!widget) return nullptr;

            // 反序列化控件属性（使用ROP属性系统）
            if (j.contains("Properties"))
            {
                DeserializeObjectPropertiesROP(widget, j["Properties"]);
            }

            // 检查是否为容器控件
            if (auto panel = dynamic_cast<ImPanelWidget*>(widget))//-----------待改进----------
            {
                if (j.contains("Children"))
                {
                    for (const auto& childJson : j["Children"])
                    {
                        // 递归创建子控件
                        ImWidget* child = CreateWidgetFromJson(childJson);
                        if (!child) continue;

                        // 添加到容器
                        ImSlot* slot = panel->AddChild(child);

                        // 反序列化slot属性（使用ROP属性系统）
                        if (childJson.contains("ParentSlot") && slot)
                        {
                            DeserializeObjectPropertiesROP(slot, childJson["ParentSlot"]);
                        }
                    }
                }
            }

            return widget;
        }

        // 序列化控件树变量
        static json SerializeWidgetVariable(ImWidget* widget)
        {
            if (!widget) return json();

            json j;
            j["Name"] = widget->GetWidgetName();
            j["Type"] = widget->GetRegisterTypeName();
            j["Data"] = SerializeWidgetTree(widget);

            return j;
        }

        // 反序列化控件树变量
        static ImWidget* DeserializeWidgetVariable(const json& j)
        {
            if (!j.contains("Name") || !j.contains("Type") || !j.contains("Data"))
                return nullptr;

            std::string type = j["Type"].get<std::string>();
            std::string name = j["Name"].get<std::string>();

            // 使用已有的CreateWidgetFromJson函数（已使用ROP属性系统）
            const json& data = j["Data"];
            ImWidget* createdWidget = CreateWidgetFromJson(data);

            return createdWidget;
        }

        // 序列化ImObject变量（使用ROP属性系统）
        static json SerializeObjectVariable(ImObject* obj, const std::string& name)
        {
            if (!obj) return json();

            json j;
            j["Name"] = name;
            j["Type"] = obj->GetRegisterTypeName();
            j["Properties"] = SerializeObjectPropertiesROP(obj);

            return j;
        }

        // 反序列化ImObject变量（使用ROP属性系统）
        static ImObject* DeserializeObjectVariable(const json& j)
        {
            if (!j.contains("Name") || !j.contains("Type") || !j.contains("Properties"))
                return nullptr;

            std::string type = j["Type"].get<std::string>();
            std::string name = j["Name"].get<std::string>();

            // 创建对象
            ImObject* obj = ImObjectFactory::GetInstance().CreateObject(type);
            if (!obj) return nullptr;

            // 设置属性（使用ROP属性系统）
            DeserializeObjectPropertiesROP(obj, j["Properties"]);

            return obj;
        }

        // ================ 主要序列化/反序列化接口 ================

        // 序列化ImUserWidgetClass
        static json SerializeUserWidgetClass(const ImUserWidgetClass& widgetClass)
        {
            json j;

            // 基本信息
            j["ClassName"] = widgetClass.GetClassName();
            j["DefaultRoot"] = widgetClass.GetDefaultRootVariableName();

            // 控件树变量
            json widgetVarsJson = json::array();
            auto widgetVarNames = widgetClass.GetWidgetVariableNames();
            for (const auto& varName : widgetVarNames)
            {
                ImWidget* widget = widgetClass.GetWidgetVariable(varName);
                if (widget)
                {
                    widgetVarsJson.push_back(SerializeWidgetVariable(widget));
                }
            }
            j["WidgetVariables"] = widgetVarsJson;

            // ImObject变量
            json objectVarsJson = json::array();
            auto objectVarNames = widgetClass.GetObjectVariableNames();
            for (const auto& varName : objectVarNames)
            {
                ImObject* obj = widgetClass.GetObjectVariable(varName);
                if (obj)
                {
                    objectVarsJson.push_back(SerializeObjectVariable(obj, varName));
                }
            }
            j["ObjectVariables"] = objectVarsJson;

            // 基本变量
            json basicVarsJson = json::array();
            auto basicVarNames = widgetClass.GetBasicVariableNames();
            for (const auto& varName : basicVarNames)
            {
                ImBasicVariable* var = widgetClass.GetBasicVariable(varName);
                if (var)
                {
                    basicVarsJson.push_back(SerializeBasicVariable(var));
                }
            }
            j["BasicVariables"] = basicVarsJson;

            // 元数据
            j["Version"] = "1.0";
            j["Timestamp"] = std::time(nullptr);

            return j;
        }

        // 反序列化ImUserWidgetClass
        static bool DeserializeUserWidgetClass(ImUserWidgetClass& widgetClass, const json& j)
        {
            if (!j.contains("ClassName"))
                return false;

            // 清空现有数据
            widgetClass.ClearAllVariables();

            // 设置类名
            widgetClass.SetClassName(j["ClassName"].get<std::string>());

            bool success = true;

            // 反序列化控件树变量
            if (j.contains("WidgetVariables") && j["WidgetVariables"].is_array())
            {
                for (const auto& widgetJson : j["WidgetVariables"])
                {
                    try
                    {
                        ImWidget* widget = DeserializeWidgetVariable(widgetJson);
                        if (widget)
                        {
                            std::string name = widgetJson["Name"].get<std::string>();
                            if (!widgetClass.SetWidgetVariableDirect(name, widget))
                            {
                                delete widget;
                                success = false;
                            }
                        }
                        else
                        {
                            success = false;
                        }
                    }
                    catch (...)
                    {
                        success = false;
                    }
                }
            }

            // 反序列化ImObject变量
            if (j.contains("ObjectVariables") && j["ObjectVariables"].is_array())
            {
                for (const auto& objectJson : j["ObjectVariables"])
                {
                    try
                    {
                        ImObject* obj = DeserializeObjectVariable(objectJson);
                        if (obj)
                        {
                            std::string name = objectJson["Name"].get<std::string>();
                            if (!widgetClass.SetObjectVariableDirect(name, obj))
                            {
                                delete obj;
                                success = false;
                            }
                        }
                        else
                        {
                            success = false;
                        }
                    }
                    catch (...)
                    {
                        success = false;
                    }
                }
            }

            // 反序列化基本变量
            if (j.contains("BasicVariables") && j["BasicVariables"].is_array())
            {
                for (const auto& basicJson : j["BasicVariables"])
                {
                    try
                    {
                        ImBasicVariable* var = DeserializeBasicVariable(basicJson);
                        if (var)
                        {
                            std::string name = basicJson["Name"].get<std::string>();
                            if (!widgetClass.SetBasicVariableDirect(name, var))
                            {
                                delete var;
                                success = false;
                            }
                        }
                        else
                        {
                            success = false;
                        }
                    }
                    catch (...)
                    {
                        success = false;
                    }
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
    };
}