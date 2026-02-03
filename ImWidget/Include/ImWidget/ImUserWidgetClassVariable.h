// ImWidgetClassVariable.h（修改版）
#pragma once
#include <string>
#include <memory>
#include <variant>
#include <functional>
#include "ImObject.h"
#include "ImWidget.h"

namespace ImGuiWidget
{
    // 变量类型枚举
    enum class WidgetClassVariableType
    {
        Widget = 0,  // 控件树变量
        Object,      // ImObject变量
        Basic        // 基本属性变量
    };

    // 统一的变量基类
    class ImWidgetClassVariable
    {
    protected:
        std::string m_VariableName;
        WidgetClassVariableType m_Type;

    public:
        ImWidgetClassVariable(const std::string& name, WidgetClassVariableType type)
            : m_VariableName(name), m_Type(type)
        {
        }

        virtual ~ImWidgetClassVariable() = default;

        // 获取变量类型
        WidgetClassVariableType GetType() const { return m_Type; }

        // 获取变量名
        virtual std::string GetName() const { return m_VariableName; }

        // 设置变量名
        virtual void SetName(const std::string& name) { m_VariableName = name; }

        // 获取类型字符串
        virtual std::string GetTypeString() const = 0;

        // 克隆变量（深拷贝）
        virtual std::unique_ptr<ImWidgetClassVariable> Clone() const = 0;

        // 转换为具体类型（用于类型安全的访问）
        template<typename T>
        T* As() { return dynamic_cast<T*>(this); }

        template<typename T>
        const T* As() const { return dynamic_cast<const T*>(this); }
    };

    // 基本属性变量类（整合了原ImBasicVariable的功能）
    class ImWidgetClassVariable_Basic : public ImWidgetClassVariable
    {
    public:
        // 使用PropertyType作为基本类型
        using BasicType = PropertyType;  // 兼容别名

    private:
        PropertyType m_ValueType;

        // 使用std::variant存储不同类型的值
        std::variant<
            int,            // Int
            float,          // Float
            bool,           // Bool
            std::string,    // String
            ImU32,          // Color
            ImVec2          // Vec2
        > m_Value;

    public:
        ImWidgetClassVariable_Basic(const std::string& name, PropertyType type)
            : ImWidgetClassVariable(name, WidgetClassVariableType::Basic)
            , m_ValueType(type)
        {
            // 根据类型初始化默认值
            switch (type)
            {
            case PropertyType::Int:
                m_Value = 0;
                break;
            case PropertyType::Float:
                m_Value = 0.0f;
                break;
            case PropertyType::Bool:
                m_Value = false;
                break;
            case PropertyType::String:
                m_Value = std::string("");
                break;
            case PropertyType::Color:
                m_Value = IM_COL32(255, 255, 255, 255);
                break;
            case PropertyType::Vec2:
                m_Value = ImVec2(0.0f, 0.0f);
                break;
            default:
                // 对于不支持的类型，默认为Int
                m_ValueType = PropertyType::Int;
                m_Value = 0;
                break;
            }
        }

        ~ImWidgetClassVariable_Basic() = default;

        // 获取值类型
        PropertyType GetValueType() const { return m_ValueType; }

        // 获取值的指针
        void* GetValuePtr()
        {
            return std::visit([](auto& value) -> void*
                {
                    return &value;
                }, m_Value);
        }

        const void* GetValuePtr() const
        {
            return std::visit([](const auto& value) -> const void*
                {
                    return &value;
                }, m_Value);
        }

        // 类型安全的获取值
        template<typename T>
        T GetValue() const
        {
            try
            {
                return std::get<T>(m_Value);
            }
            catch (const std::bad_variant_access&)
            {
                return T{};
            }
        }

        // 类型安全的设置值
        template<typename T>
        bool SetValue(const T& value)
        {
            try
            {
                m_Value = value;
                return true;
            }
            catch (const std::bad_variant_access&)
            {
                return false;
            }
        }

        // 根据PropertyType设置值（通用接口）
        bool SetValueByType(PropertyType type, void* valuePtr)
        {
            if (!valuePtr) return false;

            switch (type)
            {
            case PropertyType::Int:
                m_Value = *static_cast<int*>(valuePtr);
                return true;
            case PropertyType::Float:
                m_Value = *static_cast<float*>(valuePtr);
                return true;
            case PropertyType::Bool:
                m_Value = *static_cast<bool*>(valuePtr);
                return true;
            case PropertyType::String:
                m_Value = *static_cast<std::string*>(valuePtr);
                return true;
            case PropertyType::Color:
                m_Value = *static_cast<ImU32*>(valuePtr);
                return true;
            case PropertyType::Vec2:
                m_Value = *static_cast<ImVec2*>(valuePtr);
                return true;
            default:
                return false;
            }
        }

        // 获取类型对应的字符串表示
        virtual std::string GetTypeString() const override
        {
            return PropertyTypeToString(m_ValueType);
        }

        // 支持旧代码的兼容性函数
        BasicType GetBasicTypeOld() const { return static_cast<BasicType>(m_ValueType); }

        virtual std::unique_ptr<ImWidgetClassVariable> Clone() const override
        {
            auto cloned = std::make_unique<ImWidgetClassVariable_Basic>(m_VariableName, m_ValueType);

            // 复制值
            switch (m_ValueType)
            {
            case PropertyType::Int:
                cloned->SetValue<int>(std::get<int>(m_Value));
                break;
            case PropertyType::Float:
                cloned->SetValue<float>(std::get<float>(m_Value));
                break;
            case PropertyType::Bool:
                cloned->SetValue<bool>(std::get<bool>(m_Value));
                break;
            case PropertyType::String:
                cloned->SetValue<std::string>(std::get<std::string>(m_Value));
                break;
            case PropertyType::Color:
                cloned->SetValue<ImU32>(std::get<ImU32>(m_Value));
                break;
            case PropertyType::Vec2:
                cloned->SetValue<ImVec2>(std::get<ImVec2>(m_Value));
                break;
            }

            return cloned;
        }
    };

    // 控件树变量（保持不变）
    class ImWidgetClassVariable_Widget : public ImWidgetClassVariable
    {
    private:
        ImWidget* m_Widget;

    public:
        ImWidgetClassVariable_Widget(const std::string& name, ImWidget* widget)
            : ImWidgetClassVariable(name, WidgetClassVariableType::Widget)
            , m_Widget(widget)
        {
            // 确保控件名称与变量名一致
            if (m_Widget)
            {
                m_Widget->SetWidgetName(name);
            }
        }

        ~ImWidgetClassVariable_Widget()
        {
            if (m_Widget)
            {
                delete m_Widget;
                m_Widget = nullptr;
            }
        }

        // 禁止拷贝构造和赋值（使用Clone）
        ImWidgetClassVariable_Widget(const ImWidgetClassVariable_Widget&) = delete;
        ImWidgetClassVariable_Widget& operator=(const ImWidgetClassVariable_Widget&) = delete;

        // 移动构造
        ImWidgetClassVariable_Widget(ImWidgetClassVariable_Widget&& other) noexcept
            : ImWidgetClassVariable(std::move(other)), m_Widget(other.m_Widget)
        {
            other.m_Widget = nullptr;
        }

        // 获取控件
        ImWidget* GetWidget() const { return m_Widget; }

        // 设置控件（转移所有权）
        void SetWidget(ImWidget* widget, bool deleteOld = true)
        {
            if (m_Widget && deleteOld)
            {
                delete m_Widget;
            }
            m_Widget = widget;

            // 更新控件名称与变量名一致
            if (m_Widget)
            {
                m_Widget->SetWidgetName(m_VariableName);
            }
        }

        virtual std::string GetTypeString() const override
        {
            if (m_Widget)
            {
                return m_Widget->GetRegisterTypeName();
            }
            return "UnknownWidget";
        }

        virtual std::unique_ptr<ImWidgetClassVariable> Clone() const override
        {
            ImWidget* clonedWidget = nullptr;
            if (m_Widget)
            {
                clonedWidget = m_Widget->CopyWidget();
                if (clonedWidget)
                {
                    clonedWidget->SetWidgetName(m_VariableName);
                }
            }

            return std::make_unique<ImWidgetClassVariable_Widget>(m_VariableName, clonedWidget);
        }
    };

    // ImObject变量（保持不变）
    class ImWidgetClassVariable_Object : public ImWidgetClassVariable
    {
    private:
        ImObject* m_Object;

    public:
        ImWidgetClassVariable_Object(const std::string& name, ImObject* object)
            : ImWidgetClassVariable(name, WidgetClassVariableType::Object)
            , m_Object(object)
        {
        }

        ~ImWidgetClassVariable_Object()
        {
            if (m_Object)
            {
                delete m_Object;
                m_Object = nullptr;
            }
        }

        // 禁止拷贝
        ImWidgetClassVariable_Object(const ImWidgetClassVariable_Object&) = delete;
        ImWidgetClassVariable_Object& operator=(const ImWidgetClassVariable_Object&) = delete;

        // 移动构造
        ImWidgetClassVariable_Object(ImWidgetClassVariable_Object&& other) noexcept
            : ImWidgetClassVariable(std::move(other)), m_Object(other.m_Object)
        {
            other.m_Object = nullptr;
        }

        // 获取对象
        ImObject* GetObject() const { return m_Object; }

        // 设置对象（转移所有权）
        void SetObject(ImObject* object, bool deleteOld = true)
        {
            if (m_Object && deleteOld)
            {
                delete m_Object;
            }
            m_Object = object;
        }

        virtual std::string GetTypeString() const override
        {
            if (m_Object)
            {
                return m_Object->GetRegisterTypeName();
            }
            return "UnknownObject";
        }

        virtual std::unique_ptr<ImWidgetClassVariable> Clone() const override
        {
            // ImObject的克隆需要工厂支持，这里简化处理
            // 实际使用时需要根据具体需求实现
            return std::make_unique<ImWidgetClassVariable_Object>(m_VariableName, nullptr);
        }
    };
}