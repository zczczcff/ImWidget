#pragma once
#include <nlohmann/json.hpp>
#include <fstream>
#include "ImUserWidgetClass.h"
#include "ImWidgetSerializer.h"

namespace ImGuiWidget
{
    using json = nlohmann::ordered_json;

    // 序列化基本变量类型到字符串
    inline std::string BasicTypeToString(ImBasicVariable::BasicType type)
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
    inline ImBasicVariable::BasicType StringToBasicType(const std::string& str)
    {
        if (str == "Int") return ImBasicVariable::BasicType::Int;
        if (str == "Float") return ImBasicVariable::BasicType::Float;
        if (str == "Bool") return ImBasicVariable::BasicType::Bool;
        if (str == "String") return ImBasicVariable::BasicType::String;
        if (str == "Color") return ImBasicVariable::BasicType::Color;
        return ImBasicVariable::BasicType::Int;
    }

    // 序列化基本变量
    inline json SerializeBasicVariable(ImBasicVariable* var)
    {
        json j;
        if (!var) return j;

        j["Name"] = var->GetName();
        j["Type"] = BasicTypeToString(var->GetBasicType());

        // 获取属性信息
        auto properties = var->GetProperties();
        if (!properties.empty())
        {
            auto prop = *properties.begin(); // 基本变量只有一个属性
            j["Value"] = SerializeProperty(prop.type, var->GetValuePtr());
        }

        return j;
    }

    // 反序列化基本变量
    inline ImBasicVariable* DeserializeBasicVariable(const json& j)
    {
        if (!j.contains("Name") || !j.contains("Type") || !j.contains("Value"))
            return nullptr;

        std::string name = j["Name"].get<std::string>();
        ImBasicVariable::BasicType type = StringToBasicType(j["Type"].get<std::string>());

        // 创建基本变量
        ImBasicVariable* var = new ImBasicVariable(name, type);

        // 设置值
        auto properties = var->GetProperties();
        if (!properties.empty())
        {
            auto prop = *properties.begin();
            void* valuePtr = var->GetValuePtr();
            if (valuePtr)
            {
                DeserializeProperty(prop.type, valuePtr, j["Value"]);
            }
        }

        return var;
    }

    // 序列化控件树变量
    inline json SerializeWidgetVariable(ImWidget* widget)
    {
        if (!widget) return json();

        json j;
        j["Name"] = widget->GetWidgetName();
        j["Type"] = widget->GetRegisterTypeName();
        j["Data"] = SerializeWidgetTree(widget);

        return j;
    }

    // 反序列化控件树变量
    inline ImWidget* DeserializeWidgetVariable(const json& j)
    {
        if (!j.contains("Name") || !j.contains("Type") || !j.contains("Data"))
            return nullptr;

        std::string type = j["Type"].get<std::string>();
        std::string name = j["Name"].get<std::string>();

        // 创建控件
        ImWidget* widget = ImWidgetFactory::GetInstance().CreateWidget(type, name);
        if (!widget) return nullptr;

        // 使用已有的CreateWidgetFromJson函数
        // 注意：这里需要重新解析数据，因为CreateWidgetFromJson期望完整的控件树JSON
        const json& data = j["Data"];
        ImWidget* createdWidget = CreateWidgetFromJson(data);

        if (createdWidget)
        {
            delete widget; // 删除临时创建的控件
            return createdWidget;
        }

        delete widget;
        return nullptr;
    }

    // 序列化ImObject变量
    inline json SerializeObjectVariable(ImObject* obj, const std::string& name)
    {
        if (!obj) return json();

        json j;
        j["Name"] = name;
        j["Type"] = obj->GetRegisterTypeName();
        j["Properties"] = SerializeProperties(const_cast<ImObject*>(obj));

        return j;
    }

    // 反序列化ImObject变量
    inline ImObject* DeserializeObjectVariable(const json& j)
    {
        if (!j.contains("Name") || !j.contains("Type") || !j.contains("Properties"))
            return nullptr;

        std::string type = j["Type"].get<std::string>();
        std::string name = j["Name"].get<std::string>();

        // 创建对象
        ImObject* obj = ImObjectFactory::GetInstance().CreateObject(type);
        if (!obj) return nullptr;

        // 设置属性
        DeserializeProperties(obj, j["Properties"]);

        return obj;
    }

    // 序列化ImUserWidgetClass
    inline json SerializeUserWidgetClass(const ImUserWidgetClass& widgetClass)
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
    inline bool DeserializeUserWidgetClass(ImUserWidgetClass& widgetClass, const json& j)
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
}