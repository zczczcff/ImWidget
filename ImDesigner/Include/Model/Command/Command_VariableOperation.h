// Command_VariableOperation.h - 修改后的六个子命令实现
#pragma once

#include "ImDesignerCommandBase.h"
#include "ImWidget/ImUserWidgetClass.h"
#include "ImUserWidgetSerializer.h"
#include <nlohmann/json.hpp>

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
    std::string m_VariableName;  // 执行后记录的变量名

public:
    VariableOperationCommandBase(ImGuiWidget::ImUserWidgetClass* target,
        VariableOperationSubType subType)
        : ImUserWidgetClassCommandBase(target,
            CommandDataType(CommandCategory::VariableOperation,
                static_cast<int>(subType)))
    {
    }

    virtual ~VariableOperationCommandBase() = default;

    // 变量操作通常不可合并（除非是快速连续添加相同类型的变量）
    virtual bool CanMergeWith(const CommandBase<CommandDataType>* other) const override
    {
        return false;
    }

    // 获取执行后生成的变量名
    std::string GetVariableName() const { return m_VariableName; }
};

// ============================================================================
// 增加基本变量命令
// ============================================================================
class AddBasicVariableCommand : public VariableOperationCommandBase
{
private:
    ImGuiWidget::PropertyType m_Type;

public:
    AddBasicVariableCommand(ImGuiWidget::ImUserWidgetClass* target,
        ImGuiWidget::PropertyType type)
        : VariableOperationCommandBase(target,
            VariableOperationSubType::AddBasicVariable)
        , m_Type(type)
        
    {
    }

    virtual bool Execute() override
    {
        if (!m_TargetClass)
            return false;

        // 执行添加操作，变量名由ImUserWidgetClass生成
        return m_TargetClass->AddBasicVariable(m_Type, m_VariableName);
    }

    virtual bool Undo() override
    {
        if (!m_TargetClass)
            return false;

        return m_TargetClass->RemoveVariable(m_VariableName);
    }

    virtual std::string GetDescription() const override
    {
        return "添加基本变量: " + m_VariableName + " (类型: " +
            ImGuiWidget::PropertyTypeToString(m_Type) + ")";
    }

};

// ============================================================================
// 删除基本变量命令
// ============================================================================
class RemoveBasicVariableCommand : public VariableOperationCommandBase
{
private:
    ImGuiWidget::PropertyType m_Type;
    nlohmann::json m_SerializedData;  // 序列化后的变量数据（用于恢复）

public:
    RemoveBasicVariableCommand(ImGuiWidget::ImUserWidgetClass* target,
        const std::string& variableName)
        : VariableOperationCommandBase(target,
            VariableOperationSubType::RemoveBasicVariable)
    {
        m_VariableName = variableName;
    }

    virtual bool Execute() override
    {
        if (!m_TargetClass)
            return false;

        // 在删除前序列化变量数据
        ImGuiWidget::ImBasicVariable* var = m_TargetClass->GetBasicVariable(m_VariableName);
        if (!var)
            return false;

        m_Type = var->GetBasicType();
        m_SerializedData = ImGuiWidget::ImUserWidgetClassSerializer::TransBasicVariableToJson(var);

        // 执行删除
        return m_TargetClass->RemoveVariable(m_VariableName);
    }

    virtual bool Undo() override
    {
        if (!m_TargetClass)
            return false;

        // 从序列化数据重建变量
        ImGuiWidget::ImBasicVariable* var =
            ImGuiWidget::ImUserWidgetClassSerializer::CreateBasicVariableFromJson(m_SerializedData);

        if (!var)
            return false;

        // 添加到目标类
        return m_TargetClass->SetBasicVariableDirect(m_VariableName, var);
    }

    virtual std::string GetDescription() const override
    {
        return "删除基本变量: " + m_VariableName;
    }
};

// ============================================================================
// 增加ImObject变量命令
// ============================================================================
class AddObjectVariableCommand : public VariableOperationCommandBase
{
private:
    std::string m_ObjectType;

public:
    AddObjectVariableCommand(ImGuiWidget::ImUserWidgetClass* target,
        const std::string& objectType)
        : VariableOperationCommandBase(target,
            VariableOperationSubType::AddObjectVariable)
        , m_ObjectType(objectType)
    {
    }

    virtual bool Execute() override
    {
        if (!m_TargetClass)
            return false;

        // 执行添加操作，变量名由ImUserWidgetClass生成
        return  m_TargetClass->AddObjectVariable(m_ObjectType, m_VariableName);
    }

    virtual bool Undo() override
    {
        if (!m_TargetClass)
            return false;

        return m_TargetClass->RemoveVariable(m_VariableName);
    }

    virtual std::string GetDescription() const override
    {
        return "添加ImObject变量: " + m_VariableName + " (类型: " + m_ObjectType + ")";
    }

};

// ============================================================================
// 删除ImObject变量命令
// ============================================================================
class RemoveObjectVariableCommand : public VariableOperationCommandBase
{
private:
    std::string m_ObjectType;
    nlohmann::json m_SerializedData;  // 序列化后的对象数据

public:
    RemoveObjectVariableCommand(ImGuiWidget::ImUserWidgetClass* target,
        const std::string& variableName)
        : VariableOperationCommandBase(target,
            VariableOperationSubType::RemoveObjectVariable)
    {
        m_VariableName = variableName;
    }

    virtual bool Execute() override
    {
        if (!m_TargetClass)
            return false;

        // 在删除前序列化对象数据
        ImGuiWidget::ImObject* obj = m_TargetClass->GetObjectVariable(m_VariableName);
        if (!obj)
            return false;

        m_ObjectType = obj->GetRegisterTypeName();
        m_SerializedData = ImGuiWidget::ImUserWidgetClassSerializer::TransObjectVariableToJson(obj, m_VariableName);

        // 执行删除
        return m_TargetClass->RemoveVariable(m_VariableName);
    }

    virtual bool Undo() override
    {
        if (!m_TargetClass)
            return false;

        // 从序列化数据重建对象
        ImGuiWidget::ImObject* obj =
            ImGuiWidget::ImUserWidgetClassSerializer::CreateObjectVariableFromJson(m_SerializedData);

        if (!obj)
            return false;

        // 添加到目标类
        return m_TargetClass->SetObjectVariableDirect(m_VariableName, obj);
    }

    virtual std::string GetDescription() const override
    {
        return "删除ImObject变量: " + m_VariableName;
    }
};

// ============================================================================
// 增加控件树变量命令
// ============================================================================
class AddWidgetVariableCommand : public VariableOperationCommandBase
{
private:
    std::string m_WidgetType;
    nlohmann::json m_SerializedData;  // 序列化后的控件树数据

public:
    AddWidgetVariableCommand(ImGuiWidget::ImUserWidgetClass* target,
        const std::string& widgetType)
        : VariableOperationCommandBase(target,
            VariableOperationSubType::AddWidgetVariable)
        , m_WidgetType(widgetType)
    {
    }

    virtual bool Execute() override
    {
        if (!m_TargetClass)
            return false;

        // 执行添加操作，变量名由ImUserWidgetClass生成
        return m_TargetClass->AddWidgetVariable(m_WidgetType, m_VariableName);
    }

    virtual bool Undo() override
    {
        if (!m_TargetClass)
            return false;

        return m_TargetClass->RemoveVariable(m_VariableName);
    }

    virtual std::string GetDescription() const override
    {
        return "添加控件树变量: " + m_VariableName + " (类型: " + m_WidgetType + ")";
    }
};

// ============================================================================
// 删除控件树变量命令
// ============================================================================
class RemoveWidgetVariableCommand : public VariableOperationCommandBase
{
private:
    std::string m_WidgetType;
    nlohmann::json m_SerializedData;  // 序列化后的控件树数据

public:
    RemoveWidgetVariableCommand(ImGuiWidget::ImUserWidgetClass* target,
        const std::string& variableName)
        : VariableOperationCommandBase(target,
            VariableOperationSubType::RemoveWidgetVariable)
    {
        m_VariableName = variableName;
    }

    virtual bool Execute() override
    {
        if (!m_TargetClass)
            return false;

        // 在删除前序列化控件树数据
        ImGuiWidget::ImWidget* widget = m_TargetClass->GetWidgetVariable(m_VariableName);
        if (!widget)
            return false;

        m_WidgetType = widget->GetRegisterTypeName();
        m_SerializedData = ImGuiWidget::ImUserWidgetClassSerializer::TransWidgetVariableToJson(widget);

        // 执行删除
        return m_TargetClass->RemoveVariable(m_VariableName);
    }

    virtual bool Undo() override
    {
        if (!m_TargetClass)
            return false;

        // 从序列化数据重建控件树
        ImGuiWidget::ImWidget* widget =
            ImGuiWidget::ImUserWidgetClassSerializer::CreateWidgetFromJson(m_SerializedData);

        if (!widget)
            return false;

        // 添加到目标类
        return m_TargetClass->SetWidgetVariableDirect(m_VariableName, widget);
    }

    virtual std::string GetDescription() const override
    {
        return "删除控件树变量: " + m_VariableName;
    }
};
