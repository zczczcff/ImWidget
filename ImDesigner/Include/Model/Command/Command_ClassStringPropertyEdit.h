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
        Model_ImUserWidgetClassEditor* Model,
        ClassStringPropertyType subType,
        const std::string& oldValue,
        const std::string& newValue)
        : ImUserWidgetClassCommandBase(target,Model,
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

// =========================== 1.1 修改类名命令 ===========================
class EditClassNameCommand : public ClassPropertyCommandBase
{
public:
    EditClassNameCommand(ImGuiWidget::ImUserWidgetClass* target,
        Model_ImUserWidgetClassEditor* Model,
        const std::string& oldName,
        const std::string& newName)
        : ClassPropertyCommandBase(target, Model, ClassStringPropertyType::ClassName, oldName, newName)
    {
    }

    virtual ~EditClassNameCommand() = default;

protected:
    virtual bool ApplyChange(const std::string& value) override
    {
        if (!m_TargetClass) return false;
        m_TargetClass->SetClassName(value);
        return true;
    }

public:
    virtual std::string GetDescription() const override
    {
        return "修改类名: " + m_OldValue + " -> " + m_NewValue;
    }
};

// =========================== 1.2 修改命名空间命令 ===========================
class EditNamespaceCommand : public ClassPropertyCommandBase
{
public:
    EditNamespaceCommand(ImGuiWidget::ImUserWidgetClass* target,
        Model_ImUserWidgetClassEditor* Model,
        const std::string& oldNamespace,
        const std::string& newNamespace)
        : ClassPropertyCommandBase(target, Model, ClassStringPropertyType::Namespace, oldNamespace, newNamespace)
    {
    }

    virtual ~EditNamespaceCommand() = default;

protected:
    virtual bool ApplyChange(const std::string& value) override
    {
        if (!m_TargetClass) return false;
        m_TargetClass->SetNamespace(value);
        return true;
    }

public:
    virtual std::string GetDescription() const override
    {
        return "修改命名空间: " + m_OldValue + " -> " + m_NewValue;
    }
};

// =========================== 1.3 修改基类命令 ===========================
class EditBaseClassCommand : public ClassPropertyCommandBase
{
public:
    EditBaseClassCommand(ImGuiWidget::ImUserWidgetClass* target,
        Model_ImUserWidgetClassEditor* Model,
        const std::string& oldBaseClass,
        const std::string& newBaseClass)
        : ClassPropertyCommandBase(target, Model, ClassStringPropertyType::BaseClass, oldBaseClass, newBaseClass)
    {
    }

    virtual ~EditBaseClassCommand() = default;

protected:
    virtual bool ApplyChange(const std::string& value) override
    {
        if (!m_TargetClass) return false;
        m_TargetClass->SetBaseClass(value);
        return true;
    }

public:
    virtual std::string GetDescription() const override
    {
        return "修改基类: " + m_OldValue + " -> " + m_NewValue;
    }
};

// =========================== 1.4 修改默认根控件命令 ===========================
class EditDefaultRootCommand : public ClassPropertyCommandBase
{
public:
    EditDefaultRootCommand(ImGuiWidget::ImUserWidgetClass* target,
        Model_ImUserWidgetClassEditor* Model,
        const std::string& oldDefaultRoot,
        const std::string& newDefaultRoot)
        : ClassPropertyCommandBase(target, Model, ClassStringPropertyType::DefaultRoot, oldDefaultRoot, newDefaultRoot)
    {
    }

    virtual ~EditDefaultRootCommand() = default;

protected:
    virtual bool ApplyChange(const std::string& value) override
    {
        if (!m_TargetClass) return false;
        m_TargetClass->SetDefaultRootVariable(value);
        return true;
    }

public:
    virtual std::string GetDescription() const override
    {
        return "修改默认根控件: " + m_OldValue + " -> " + m_NewValue;
    }
};