#pragma once

#include "ImDesignerCommandBase.h"

// =========================== 4. Widget Ù–‘±‡º≠√¸¡Óª˘¿‡ ===========================
template<typename T>
class WidgetPropertyEditCommandBase : public ImUserWidgetClassCommandBase
{
protected:
    std::string m_WidgetVarName;
    std::string m_WidgetPath;
    std::string m_PropertyPath;
    T m_OldValue;
    T m_NewValue;

public:
    WidgetPropertyEditCommandBase(ImGuiWidget::ImUserWidgetClass* target,
        const std::string& widgetVarName,
        const std::string& widgetPath,
        const std::string& propertyPath,
        const T& oldValue,
        const T& newValue)
        : ImUserWidgetClassCommandBase(target,
            CommandDataType(CommandCategory::WidgetPropertyEdit,
                static_cast<int>(WidgetPropertyEditSubType::SetWidgetProperty))),
        m_WidgetVarName(widgetVarName),
        m_WidgetPath(widgetPath),
        m_PropertyPath(propertyPath),
        m_OldValue(oldValue),
        m_NewValue(newValue)
    {
    }

    virtual ~WidgetPropertyEditCommandBase() = default;

    virtual bool Execute() override
    {
        return m_TargetClass->SetWidgetPropertyByPath<T>(m_WidgetVarName, m_WidgetPath, m_PropertyPath, m_NewValue);
    }

    virtual bool Undo() override
    {
        return m_TargetClass->SetWidgetPropertyByPath<T>(m_WidgetVarName, m_WidgetPath, m_PropertyPath, m_OldValue);
    }

    virtual std::string GetDescription() const override
    {
        return "Edit Widget Property: " + m_WidgetVarName + "/" + m_WidgetPath + " -> " + m_PropertyPath;
    }

    virtual bool CanMergeWith(const CommandBase<CommandDataType>* other) const override
    {
        if (!ImUserWidgetClassCommandBase::IsWithinMergeWindow(other))
            return false;

        const auto* otherCmd = dynamic_cast<const WidgetPropertyEditCommandBase<T>*>(other);
        if (!otherCmd) return false;

        return (m_TargetClass == otherCmd->m_TargetClass) &&
            (m_WidgetVarName == otherCmd->m_WidgetVarName) &&
            (m_WidgetPath == otherCmd->m_WidgetPath) &&
            (m_PropertyPath == otherCmd->m_PropertyPath);
    }

    virtual bool MergeWith(std::unique_ptr<CommandBase<CommandDataType>> other) override
    {
        if (!CanMergeWith(other.get())) return false;

        auto* otherCmd = static_cast<WidgetPropertyEditCommandBase<T>*>(other.get());
        m_NewValue = otherCmd->m_NewValue;
        return true;
    }
};