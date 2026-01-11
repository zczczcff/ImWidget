#pragma once
#include "CommandBase.h"
#include "ImWidget/ImWidgetProperty.h"
namespace ImGuiWidget
{
    class ImWidget;
}
// 属性编辑命令
class PropertyEditCommand : public EditCommand
{
protected:
    ImGuiWidget::PropertyInfo m_PropertyInfo;
    ImGuiWidget::ImObject* m_Target;
public:
    PropertyEditCommand(const ImGuiWidget::PropertyInfo& propInfo, ImGuiWidget::ImObject* Target)
        : m_PropertyInfo(propInfo),
        m_Target(Target)
    {
        m_Description = "Edit " + propInfo.name;
        m_type = EditCommandType::PropertyEdit;
    }

    virtual std::string GetDescription() const override
    {
        return m_Description;
    }

    // 检查是否可以合并
    virtual bool CanMergeWith(const EditCommand* other) const override
    {
        auto* otherCmd = dynamic_cast<const PropertyEditCommand*>(other);
        if (!otherCmd) return false;

        // 必须是同一属性且类型相同
        return m_PropertyInfo.name == otherCmd->m_PropertyInfo.name &&
            m_PropertyInfo.type == otherCmd->m_PropertyInfo.type &&
            IsWithinMergeWindow(other);
    }

    // 获取属性名称
    const std::string& GetPropertyName() const { return m_PropertyInfo.name; }

    // 获取属性类型
    ImGuiWidget::PropertyType GetPropertyType() const { return m_PropertyInfo.type; }

    ImGuiWidget::ImObject* GetTarget() { return m_Target; }
};

// 颜色编辑命令合并
class ColorEditCommand : public PropertyEditCommand
{
private:
    ImU32 m_OldValue;
    ImU32 m_NewValue;

public:
    ColorEditCommand(const ImGuiWidget::PropertyInfo& propInfo, ImU32 newValue, ImGuiWidget::ImObject* Target)
        : PropertyEditCommand(propInfo,Target)
        , m_OldValue(m_PropertyInfo.GetColorValue())
        , m_NewValue(newValue)
    {
    }

    virtual bool Execute() override
    {
        return m_PropertyInfo.SetColorValue(m_NewValue);
    }

    virtual bool Undo() override
    {
        return m_PropertyInfo.SetColorValue(m_OldValue);
    }

    virtual bool MergeWith(std::unique_ptr<EditCommand> other) override
    {
        auto* otherCmd = dynamic_cast<ColorEditCommand*>(other.get());
        if (!otherCmd || !CanMergeWith(otherCmd)) return false;

        // 保留原始旧值，更新新值为合并命令的新值
        m_NewValue = otherCmd->m_NewValue;
        m_Timestamp = otherCmd->m_Timestamp; // 更新时间戳
        return true;
    }
};

// 文本属性编辑命令
class StringEditCommand : public PropertyEditCommand
{
private:
    std::string m_OldValue;
    std::string m_NewValue;

public:
    StringEditCommand(const ImGuiWidget::PropertyInfo& propInfo, const std::string& newValue, ImGuiWidget::ImObject* Target)
        : PropertyEditCommand(propInfo,Target)
        , m_OldValue(m_PropertyInfo.GetStringValue())
        , m_NewValue(newValue)
    {
    }

    virtual bool Execute() override
    {
        return m_PropertyInfo.SetStringValue(m_NewValue);
    }

    virtual bool Undo() override
    {
        return m_PropertyInfo.SetStringValue(m_OldValue);
    }

    virtual bool MergeWith(std::unique_ptr<EditCommand> other) override
    {
        auto* otherCmd = dynamic_cast<StringEditCommand*>(other.get());
        if (!otherCmd || !CanMergeWith(otherCmd)) return false;

        m_NewValue = otherCmd->m_NewValue;
        m_Timestamp = otherCmd->m_Timestamp;
        return true;
    }
};

// 浮点数编辑命令合并
class FloatEditCommand : public PropertyEditCommand
{
private:
    float m_OldValue;
    float m_NewValue;

public:
    FloatEditCommand(const ImGuiWidget::PropertyInfo& propInfo, float newValue, ImGuiWidget::ImObject* Target)
        : PropertyEditCommand(propInfo,Target)
        , m_OldValue(m_PropertyInfo.GetFloatValue())
        , m_NewValue(newValue)
    {
    }

    virtual bool Execute() override
    {
        return m_PropertyInfo.SetFloatValue(m_NewValue);
    }

    virtual bool Undo() override
    {
        return m_PropertyInfo.SetFloatValue(m_OldValue);
    }

    virtual bool MergeWith(std::unique_ptr<EditCommand> other) override
    {
        auto* otherCmd = dynamic_cast<FloatEditCommand*>(other.get());
        if (!otherCmd || !CanMergeWith(otherCmd)) return false;

        m_NewValue = otherCmd->m_NewValue;
        m_Timestamp = otherCmd->m_Timestamp;
        return true;
    }
};

// 整数属性编辑命令
class IntEditCommand : public PropertyEditCommand
{
private:
    int m_OldValue;
    int m_NewValue;

public:
    IntEditCommand(const ImGuiWidget::PropertyInfo& propInfo, int newValue, ImGuiWidget::ImObject* Target)
        : PropertyEditCommand(propInfo,Target)
        , m_OldValue(m_PropertyInfo.GetIntValue())
        , m_NewValue(newValue)
    {
    }

    virtual bool Execute() override
    {
        return m_PropertyInfo.SetIntValue(m_NewValue);
    }

    virtual bool Undo() override
    {
        return m_PropertyInfo.SetIntValue(m_OldValue);
    }

    virtual bool MergeWith(std::unique_ptr<EditCommand> other) override
    {
        auto* otherCmd = dynamic_cast<IntEditCommand*>(other.get());
        if (!otherCmd || !CanMergeWith(otherCmd)) return false;

        m_NewValue = otherCmd->m_NewValue;
        m_Timestamp = otherCmd->m_Timestamp;
        return true;
    }
};

// 布尔属性编辑命令
class BoolEditCommand : public PropertyEditCommand
{
private:
    bool m_OldValue;
    bool m_NewValue;

public:
    BoolEditCommand(const ImGuiWidget::PropertyInfo& propInfo, bool newValue, ImGuiWidget::ImObject* Target)
        : PropertyEditCommand(propInfo,Target)
        , m_OldValue(m_PropertyInfo.GetBoolValue())
        , m_NewValue(newValue)
    {
    }

    virtual bool Execute() override
    {
        return m_PropertyInfo.SetBoolValue(m_NewValue);
    }

    virtual bool Undo() override
    {
        return m_PropertyInfo.SetBoolValue(m_OldValue);
    }

    virtual bool CanMergeWith(const EditCommand* other) const { return false; }
};

// Vec2属性编辑命令
class Vec2EditCommand : public PropertyEditCommand
{
private:
    ImVec2 m_OldValue;
    ImVec2 m_NewValue;

public:
    Vec2EditCommand(const ImGuiWidget::PropertyInfo& propInfo, const ImVec2& newValue, ImGuiWidget::ImObject* Target)
        : PropertyEditCommand(propInfo,Target)
        , m_OldValue(m_PropertyInfo.GetVec2Value())
        , m_NewValue(newValue)
    {
    }

    virtual bool Execute() override
    {
        return m_PropertyInfo.SetVec2Value(m_NewValue);
    }

    virtual bool Undo() override
    {
        return m_PropertyInfo.SetVec2Value(m_OldValue);
    }

    virtual bool MergeWith(std::unique_ptr<EditCommand> other) override
    {
        auto* otherCmd = dynamic_cast<Vec2EditCommand*>(other.get());
        if (!otherCmd || !CanMergeWith(otherCmd)) return false;

        m_NewValue = otherCmd->m_NewValue;
        m_Timestamp = otherCmd->m_Timestamp;
        return true;
    }
};

// 枚举属性编辑命令
class EnumEditCommand : public PropertyEditCommand
{
private:
    std::string m_OldValue;
    std::string m_NewValue;

public:
    EnumEditCommand(const ImGuiWidget::PropertyInfo& propInfo, const std::string& newValue, ImGuiWidget::ImObject* Target)
        : PropertyEditCommand(propInfo,Target)
        , m_OldValue(m_PropertyInfo.GetEnumCurrentValue())
        , m_NewValue(newValue)
    {
    }

    virtual bool Execute() override
    {
        return m_PropertyInfo.SetEnumValue(m_NewValue);
    }

    virtual bool Undo() override
    {
        return m_PropertyInfo.SetEnumValue(m_OldValue);
    }

    virtual bool MergeWith(std::unique_ptr<EditCommand> other) override
    {
        auto* otherCmd = dynamic_cast<EnumEditCommand*>(other.get());
        if (!otherCmd || !CanMergeWith(otherCmd)) return false;

        m_NewValue = otherCmd->m_NewValue;
        m_Timestamp = otherCmd->m_Timestamp;
        return true;
    }
};

// 字符串数组属性编辑命令
class StringArrayEditCommand : public PropertyEditCommand
{
private:
    std::vector<std::string> m_OldValue;
    std::vector<std::string> m_NewValue;

public:
    StringArrayEditCommand(const ImGuiWidget::PropertyInfo& propInfo, const std::vector<std::string>& newValue, ImGuiWidget::ImObject* Target)
        : PropertyEditCommand(propInfo,Target)
        , m_OldValue(m_PropertyInfo.GetStringArrayValue())
        , m_NewValue(newValue)
    {
    }

    virtual bool Execute() override
    {
        return m_PropertyInfo.SetStringArrayValue(m_NewValue);
    }

    virtual bool Undo() override
    {
        return m_PropertyInfo.SetStringArrayValue(m_OldValue);
    }

    virtual bool CanMergeWith(const EditCommand* other) const { return false; }
};

// 结构体属性编辑命令
class StructEditCommand : public PropertyEditCommand
{
private:
    ImGuiWidget::ImObject m_OldValue;
    ImGuiWidget::ImObject m_NewValue;

public:
    StructEditCommand(const ImGuiWidget::PropertyInfo& propInfo, ImGuiWidget::ImObject newValue, ImGuiWidget::ImObject* Target)
        : PropertyEditCommand(propInfo,Target)
        , m_OldValue(*m_PropertyInfo.GetStructValue())
        , m_NewValue(newValue)
    {
    }

    virtual bool Execute() override
    {
        return m_PropertyInfo.SetStructValue(&m_NewValue);
    }

    virtual bool Undo() override
    {
        return m_PropertyInfo.SetStructValue(&m_OldValue);
    }

    virtual bool CanMergeWith(const EditCommand* other) const { return false; }
};
