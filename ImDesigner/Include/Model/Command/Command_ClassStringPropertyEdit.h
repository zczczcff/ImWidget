#pragma once

#include "ImDesignerCommandBase.h"
#include "ImWidget/ImUserWidgetClass.h"

// ClassProperty子类型
enum class ClassStringPropertyType
{
    ClassName = 0,          // 修改类名
    Namespace,              // 修改命名空间
    BaseClass,              // 修改基类
    DefaultRoot             // 修改默认根控件
};

// =========================== 1. 类基础属性命令基类 ===========================
class ClassPropertyCommandBase : public ImUserWidgetClassCommandBase
{
protected:
    std::string m_OldValue;
    std::string m_NewValue;

public:
    ClassPropertyCommandBase(ImGuiWidget::ImUserWidgetClass* target,
        ClassStringPropertyType subType,
        const std::string& oldValue,
        const std::string& newValue)
        : ImUserWidgetClassCommandBase(target,
            CommandDataType(CommandCategory::ClassProperty,
                static_cast<int>(subType))),
        m_OldValue(oldValue), m_NewValue(newValue)
    {
    }

    virtual ~ClassPropertyCommandBase() = default;

    virtual bool Execute() override
    {
        return ApplyChange(m_NewValue);
    }

    virtual bool Undo() override
    {
        return ApplyChange(m_OldValue);
    }

    virtual bool CanMergeWith(const CommandBase<CommandDataType>* other) const override
    {
        // 同一类型、同一目标的属性编辑命令可以合并
        if (!ImUserWidgetClassCommandBase::IsWithinMergeWindow(other))
            return false;

        const auto* otherCmd = dynamic_cast<const ClassPropertyCommandBase*>(other);
        if (!otherCmd) return false;

        return (m_TargetClass == otherCmd->m_TargetClass) &&
            (GetData().category == otherCmd->GetData().category) &&
            (GetData().subType == otherCmd->GetData().subType);
    }

    virtual bool MergeWith(std::unique_ptr<CommandBase<CommandDataType>> other) override
    {
        if (!CanMergeWith(other.get())) return false;

        auto* otherCmd = static_cast<ClassPropertyCommandBase*>(other.get());
        m_NewValue = otherCmd->m_NewValue;
        return true;
    }

protected:
    virtual bool ApplyChange(const std::string& value) = 0;
};