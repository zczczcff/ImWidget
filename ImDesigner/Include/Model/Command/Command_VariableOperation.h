#pragma once

#include "ImDesignerCommandBase.h"
#include "ImWidget/ImUserWidgetClass.h"
// VariableOperation子类型
enum class VariableOperationSubType
{
    AddBasicVariable = 0,   // 增加基本变量
    RemoveBasicVariable,    // 删除基本变量
    AddObjectVariable,      // 增加ImObject变量
    RemoveObjectVariable,   // 删除ImObject变量
    AddWidgetVariable,      // 增加控件树变量
    RemoveWidgetVariable    // 删除控件树变量
};

class VariableOperationCommandBase : public ImUserWidgetClassCommandBase
{
protected:
    std::string m_VariableName;

public:
    VariableOperationCommandBase(ImGuiWidget::ImUserWidgetClass* target,
        VariableOperationSubType subType,
        const std::string& variableName)
        : ImUserWidgetClassCommandBase(target,
            CommandDataType(CommandCategory::VariableOperation,
                static_cast<int>(subType))),
        m_VariableName(variableName)
    {
    }

    virtual ~VariableOperationCommandBase() = default;

    // 变量操作通常不可合并（除非是快速连续添加相同类型的变量）
    virtual bool CanMergeWith(const CommandBase<CommandDataType>* other) const override
    {
        return false;
    }
};

class AddBasicVariableCommand : public VariableOperationCommandBase
{
private:
    ImGuiWidget::ImBasicVariable::BasicType m_Type;
    std::string m_Category;
    std::string m_GeneratedName;  // 存储实际生成的名称

public:
    AddBasicVariableCommand(ImGuiWidget::ImUserWidgetClass* target,
        ImGuiWidget::ImBasicVariable::BasicType type,
        const std::string& category)
        : VariableOperationCommandBase(target, VariableOperationSubType::AddBasicVariable, ""),
        m_Type(type), m_Category(category)
    {
    }

    virtual bool Execute() override
    {
        std::string outVarName;
        bool success = m_TargetClass->AddBasicVariable(m_Type, m_Category, outVarName);
        if (success)
        {
            m_VariableName = outVarName;  // 保存实际生成的名称
            m_GeneratedName = outVarName;
        }
        return success;
    }

    virtual bool Undo() override
    {
        return m_TargetClass->RemoveVariable(m_GeneratedName);
    }

    virtual std::string GetDescription() const override
    {
        return "Add basic variable: " + m_GeneratedName + " (Type: " + std::to_string(static_cast<int>(m_Type)) + ")";
    }
};

class RemoveBasicVariableCommand : public VariableOperationCommandBase
{
private:
    ImGuiWidget::ImBasicVariable* m_RemovedVariable;  // 保存被删除的变量，用于Undo
    ImGuiWidget::ImBasicVariable::BasicType m_Type;
    std::string m_Category;

public:
    RemoveBasicVariableCommand(ImGuiWidget::ImUserWidgetClass* target,
        const std::string& variableName)
        : VariableOperationCommandBase(target, VariableOperationSubType::RemoveBasicVariable, variableName),
        m_RemovedVariable(nullptr)
    {
        // 在构造函数中获取变量信息
        auto var = m_TargetClass->GetBasicVariable(variableName);
        if (var)
        {
            m_Type = var->GetBasicType();
            m_Category = var->GetName();  // 注意：这里可能需要获取category，但ImBasicVariable没有category成员
        }
    }

    virtual ~RemoveBasicVariableCommand()
    {
        if (m_RemovedVariable)
        {
            delete m_RemovedVariable;
        }
    }

    virtual bool Execute() override
    {
        m_RemovedVariable = m_TargetClass->GetBasicVariable(m_VariableName);
        if (!m_RemovedVariable) return false;

        // 创建副本用于Undo
        m_RemovedVariable = new ImGuiWidget::ImBasicVariable(*m_RemovedVariable);

        return m_TargetClass->RemoveVariable(m_VariableName);
    }

    virtual bool Undo() override
    {
        if (!m_RemovedVariable) return false;

        bool success = m_TargetClass->SetBasicVariableDirect(m_VariableName, m_RemovedVariable);
        if (success)
        {
            m_RemovedVariable = nullptr;  // 所有权转移给TargetClass
        }
        return success;
    }

    virtual std::string GetDescription() const override
    {
        return "Remove basic variable: " + m_VariableName;
    }
};

class AddObjectVariableCommand : public VariableOperationCommandBase
{
private:
    std::string m_TypeName;
    std::string m_GeneratedName;

public:
    AddObjectVariableCommand(ImGuiWidget::ImUserWidgetClass* target,
        const std::string& typeName)
        : VariableOperationCommandBase(target, VariableOperationSubType::AddObjectVariable, ""),
        m_TypeName(typeName)
    {
    }

    virtual bool Execute() override
    {
        std::string outVarName;
        bool success = m_TargetClass->AddObjectVariable(m_TypeName, outVarName);
        if (success)
        {
            m_VariableName = outVarName;
            m_GeneratedName = outVarName;
        }
        return success;
    }

    virtual bool Undo() override
    {
        return m_TargetClass->RemoveVariable(m_GeneratedName);
    }

    virtual std::string GetDescription() const override
    {
        return "Add object variable: " + m_GeneratedName + " (Type: " + m_TypeName + ")";
    }
};

class RemoveObjectVariableCommand : public VariableOperationCommandBase
{
private:
    ImGuiWidget::ImObject* m_RemovedObject;
    std::vector<uint8_t> m_SerializedData;  // 序列化数据用于Undo

public:
    RemoveObjectVariableCommand(ImGuiWidget::ImUserWidgetClass* target,
        const std::string& variableName)
        : VariableOperationCommandBase(target, VariableOperationSubType::RemoveObjectVariable, variableName),
        m_RemovedObject(nullptr)
    {
    }

    virtual ~RemoveObjectVariableCommand()
    {
        if (m_RemovedObject)
        {
            delete m_RemovedObject;
        }
    }

    virtual bool Execute() override
    {
        auto obj = m_TargetClass->GetObjectVariable(m_VariableName);
        if (!obj) return false;

        // 序列化对象用于Undo
        m_SerializedData = obj->Serialize();
        m_RemovedObject = obj;

        return m_TargetClass->RemoveVariable(m_VariableName);
    }

    virtual bool Undo() override
    {
        if (m_SerializedData.empty()) return false;

        // 反序列化对象
        ImGuiWidget::ImObject* newObj = ImGuiWidget::ImObjectFactory::GetInstance().CreateObject(m_RemovedObject->GetRegisterTypeName());
        if (!newObj) return false;

        if (newObj->Deserialize(m_SerializedData))
        {
            bool success = m_TargetClass->SetObjectVariableDirect(m_VariableName, newObj);
            if (success)
            {
                m_RemovedObject = nullptr;  // 所有权转移
                m_SerializedData.clear();
                return true;
            }
        }

        delete newObj;
        return false;
    }

    virtual std::string GetDescription() const override
    {
        return "Remove object variable: " + m_VariableName;
    }
};

class AddWidgetVariableCommand : public VariableOperationCommandBase
{
private:
    std::string m_TypeName;
    std::string m_GeneratedName;

public:
    AddWidgetVariableCommand(ImGuiWidget::ImUserWidgetClass* target,
        const std::string& typeName)
        : VariableOperationCommandBase(target, VariableOperationSubType::AddWidgetVariable, ""),
        m_TypeName(typeName)
    {
    }

    virtual bool Execute() override
    {
        std::string outVarName;
        bool success = m_TargetClass->AddWidgetVariable(m_TypeName, outVarName);
        if (success)
        {
            m_VariableName = outVarName;
            m_GeneratedName = outVarName;
        }
        return success;
    }

    virtual bool Undo() override
    {
        return m_TargetClass->RemoveVariable(m_GeneratedName);
    }

    virtual std::string GetDescription() const override
    {
        return "Add widget variable: " + m_GeneratedName + " (Type: " + m_TypeName + ")";
    }
};

class RemoveWidgetVariableCommand : public VariableOperationCommandBase
{
private:
    ImGuiWidget::ImWidget* m_RemovedWidget;

public:
    RemoveWidgetVariableCommand(ImGuiWidget::ImUserWidgetClass* target,
        const std::string& variableName)
        : VariableOperationCommandBase(target, VariableOperationSubType::RemoveWidgetVariable, variableName),
        m_RemovedWidget(nullptr)
    {
    }

    virtual ~RemoveWidgetVariableCommand()
    {
        if (m_RemovedWidget)
        {
            delete m_RemovedWidget;
        }
    }

    virtual bool Execute() override
    {
        m_RemovedWidget = m_TargetClass->GetWidgetVariable(m_VariableName);
        if (!m_RemovedWidget) return false;

        // 创建深拷贝用于Undo
        m_RemovedWidget = m_RemovedWidget->CopyWidget();

        return m_TargetClass->RemoveVariable(m_VariableName);
    }

    virtual bool Undo() override
    {
        if (!m_RemovedWidget) return false;

        bool success = m_TargetClass->SetWidgetVariableDirect(m_VariableName, m_RemovedWidget);
        if (success)
        {
            m_RemovedWidget = nullptr;  // 所有权转移
        }
        return success;
    }

    virtual std::string GetDescription() const override
    {
        return "Remove widget variable: " + m_VariableName;
    }
};