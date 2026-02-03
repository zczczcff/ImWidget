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
    public:
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
                    // 结构体类型需要递归序列化
                    if constexpr (std::is_same_v<T, ImObject*>)
                    {
                        if (value != nullptr)
                        {
                            return SerializeObjectPropertiesROP(value);
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

                    // 可以添加更多属性类型的处理
                default:
                    // 未知或不支持的类型
                    break;
                }
            }
            catch (const std::exception& e)
            {
                std::cerr << "Error serializing property value of type " <<
                    static_cast<int>(type) << ": " << e.what() << std::endl;
            }

            // 类型不匹配或处理失败时返回空json
            return json();
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
                    // 结构体类型 - 传入ImObject引用，递归反序列化
                    if constexpr (std::is_same_v<T, ImObject&> || std::is_same_v<T, ImObject>)
                    {
                        if (j.is_object())
                        {
                            // 递归反序列化子结构
                            DeserializeObjectPropertiesROP(&outValue, j);
                            return true;
                        }
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

        // 序列化单个ROP属性值（统一使用SerializeSinglePropertyValue）
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
                    j = SerializeSinglePropertyValue<std::vector<std::string>>(prop.GetType(), vec);
                    break;
                }
                case PropertyType::Enum:
                {
                    // 处理枚举属性
                    auto optionalProp = prop.GetObject()->ToOptionalProperty(prop);
                    if (optionalProp.IsValid())
                    {
                        // 保存当前选择的选项字符串
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

        // 反序列化单个ROP属性值（统一使用DeserializeSinglePropertyValue）
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
                        // 通过OptionalProperty设置枚举值
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

        // ================ 基本变量序列化辅助函数 ================

        // 序列化基本变量类型到字符串
        static std::string BasicTypeToString(ImBasicVariable::BasicType type)
        {
            return PropertyTypeToString(static_cast<PropertyType>(type));
        }

        // 从字符串反序列化基本变量类型
        static ImBasicVariable::BasicType StringToBasicType(const std::string& str)
        {
            return static_cast<ImBasicVariable::BasicType>(StringToPropertyType(str));
        }

    public:

        // 序列化基本变量值（统一使用SerializeSinglePropertyValue）
        static json SerializeBasicVariableValue(ImBasicVariable* var)
        {
            if (!var) return json();

            json j;
            PropertyType type = static_cast<PropertyType>(var->GetBasicType());

            switch (type)
            {
            case PropertyType::Int:
            {
                int value = var->GetValue<int>();
                j = SerializeSinglePropertyValue<int>(type, value);
                break;
            }
            case PropertyType::Float:
            {
                float value = var->GetValue<float>();
                j = SerializeSinglePropertyValue<float>(type, value);
                break;
            }
            case PropertyType::Bool:
            {
                bool value = var->GetValue<bool>();
                j = SerializeSinglePropertyValue<bool>(type, value);
                break;
            }
            case PropertyType::String:
            {
                std::string value = var->GetValue<std::string>();
                j = SerializeSinglePropertyValue<std::string>(type, value);
                break;
            }
            case PropertyType::Color:
            {
                ImU32 color = var->GetValue<ImU32>();
                j = SerializeSinglePropertyValue<ImU32>(type, color);
                break;
            }
            case PropertyType::Vec2:
            {
                ImVec2 vec = var->GetValue<ImVec2>();
                j = SerializeSinglePropertyValue<ImVec2>(type, vec);
                break;
            }
            default:
                break;
            }

            return j;
        }

        // 反序列化基本变量值（统一使用DeserializeSinglePropertyValue）
        static bool DeserializeBasicVariableValue(ImBasicVariable* var, const json& j)
        {
            if (!var) return false;

            PropertyType type = static_cast<PropertyType>(var->GetBasicType());

            try
            {
                switch (type)
                {
                case PropertyType::Int:
                {
                    int value;
                    if (DeserializeSinglePropertyValue<int>(type, j, value))
                    {
                        return var->SetValue<int>(value);
                    }
                    break;
                }
                case PropertyType::Float:
                {
                    float value;
                    if (DeserializeSinglePropertyValue<float>(type, j, value))
                    {
                        return var->SetValue<float>(value);
                    }
                    break;
                }
                case PropertyType::Bool:
                {
                    bool value;
                    if (DeserializeSinglePropertyValue<bool>(type, j, value))
                    {
                        return var->SetValue<bool>(value);
                    }
                    break;
                }
                case PropertyType::String:
                {
                    std::string value;
                    if (DeserializeSinglePropertyValue<std::string>(type, j, value))
                    {
                        return var->SetValue<std::string>(value);
                    }
                    break;
                }
                case PropertyType::Color:
                {
                    ImU32 color;
                    if (DeserializeSinglePropertyValue<ImU32>(type, j, color))
                    {
                        return var->SetValue<ImU32>(color);
                    }
                    break;
                }
                case PropertyType::Vec2:
                {
                    ImVec2 vec;
                    if (DeserializeSinglePropertyValue<ImVec2>(type, j, vec))
                    {
                        return var->SetValue<ImVec2>(vec);
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

        // 序列化控件树为JSON（使用ROP属性系统）
        static json TransWidgetToJson(ImWidget* root)
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
                    json childJson = TransWidgetToJson(child);

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

        // ================ ImUserWidgetClass变量<== 转换 ==>JSON对象 ================
        
        // 序列化基本变量为JSON
        static json TransBasicVariableToJson(ImBasicVariable* var)
        {
            json j;
            if (!var) return j;

            j["Name"] = var->GetName();
            j["Type"] = BasicTypeToString(var->GetBasicType());
            j["Value"] = SerializeBasicVariableValue(var);

            return j;
        }

        // 从JSON创建基本变量
        static ImBasicVariable* CreateBasicVariableFromJson(const json& j)
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

        // 序列化控件树变量为JSON
        static json TransWidgetVariableToJson(ImWidget* widget)
        {
            if (!widget) return json();

            json j;
            j["Name"] = widget->GetWidgetName();
            j["Type"] = widget->GetRegisterTypeName();
            j["Data"] = TransWidgetToJson(widget);

            return j;
        }

        // 从JSON创建控件树变量
        static ImWidget* CreateWidgetVariableFromJson(const json& j)
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

        // 序列化ImObject变量为Json（使用ROP属性系统）
        static json TransObjectVariableToJson(ImObject* obj, const std::string& name)
        {
            if (!obj) return json();

            json j;
            j["Name"] = name;
            j["Type"] = obj->GetRegisterTypeName();
            j["Properties"] = SerializeObjectPropertiesROP(obj);

            return j;
        }

        // 从Json创建ImObject变量（使用ROP属性系统）
        static ImObject* CreateObjectVariableFromJson(const json& j)
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

        // ================ ImUserWidgetClass序列化/反序列化接口 ================

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
                    widgetVarsJson.push_back(TransWidgetVariableToJson(widget));
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
                    objectVarsJson.push_back(TransObjectVariableToJson(obj, varName));
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
                    basicVarsJson.push_back(TransBasicVariableToJson(var));
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
                        ImWidget* widget = CreateWidgetVariableFromJson(widgetJson);
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
                        ImObject* obj = CreateObjectVariableFromJson(objectJson);
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
                        ImBasicVariable* var = CreateBasicVariableFromJson(basicJson);
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