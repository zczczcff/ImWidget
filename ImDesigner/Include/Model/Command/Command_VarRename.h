#pragma once
#include "ImDesignerCommandBase.h"
#include "ImWidget/ImUserWidgetClass.h"

// 重命名操作子类型
enum class RenameOperationSubType
{
    RenameVariable = 0,       // 重命名变量
    RenameWidgetByPath = 1,   // 通过路径重命名控件
};


// 6.1 重命名变量命令
class RenameVariableCommand : public ImUserWidgetClassCommandBase
{
private:
    std::string m_OldName;
    std::string m_NewName;
    std::string m_VariableType; // "Widget", "Object", "Basic"
    bool m_IsDefaultRoot;

public:
    RenameVariableCommand(ImGuiWidget::ImUserWidgetClass* target,
        Model_ImUserWidgetClassEditor* Model,
        const std::string& oldName,
        const std::string& newName)
        : ImUserWidgetClassCommandBase(target,Model,
            CommandDataType(CommandCategory::RenameOperation,
                static_cast<int>(RenameOperationSubType::RenameVariable),
                "RenameVariable")),
        m_OldName(oldName),
        m_NewName(newName),
        m_IsDefaultRoot(false)
    {
        // 确定变量类型
        if (m_TargetClass->GetWidgetVariable(oldName))
        {
            m_VariableType = "Widget";
            m_IsDefaultRoot = (m_TargetClass->GetDefaultRootVariableName() == oldName);
        }
        else if (m_TargetClass->GetObjectVariable(oldName))
        {
            m_VariableType = "Object";
        }
        else if (m_TargetClass->GetBasicVariable(oldName))
        {
            m_VariableType = "Basic";
        }
    }

    virtual ~RenameVariableCommand() = default;

    virtual bool Execute() override
    {
        bool success = m_TargetClass->RenameVariable(m_OldName, m_NewName);
        if (success && m_Model) {
            std::string filePath = m_Model->GetEditedFileFullPath();
            Publish(filePath + Events::OutlineView::VARIABLE_RENAMED, m_OldName, m_NewName);
        }
        return success;
    }

    virtual bool Undo() override
    {
        bool success = m_TargetClass->RenameVariable(m_NewName, m_OldName);
        if (success && m_Model) {
            std::string filePath = m_Model->GetEditedFileFullPath();
            Publish(filePath + Events::OutlineView::VARIABLE_RENAMED, m_NewName, m_OldName);
        }
        return success;
    }

    virtual std::string GetDescription() const override
    {
        return "Rename " + m_VariableType + " Variable: " + m_OldName + " -> " + m_NewName +
            (m_IsDefaultRoot ? " (Default Root)" : "");
    }

    virtual bool CanMergeWith(const CommandBase<CommandDataType>* other) const override
    {
        if (!ImUserWidgetClassCommandBase::IsWithinMergeWindow(other))
            return false;

        if (!(m_Data == other->GetData())) return false;

        const RenameVariableCommand* otherCommand = static_cast<const RenameVariableCommand*>(other);

        return (m_TargetClass == otherCommand->m_TargetClass) &&
            (m_NewName == otherCommand->m_OldName) && // 连续重命名同一变量
            (m_VariableType == otherCommand->m_VariableType)
            ;
    }

    virtual bool MergeWith(std::unique_ptr<CommandBase<CommandDataType>> other) override
    {
        if (!CanMergeWith(other.get())) return false;

        auto* otherCmd = static_cast<RenameVariableCommand*>(other.get());
        m_NewName = otherCmd->m_NewName;
        return true;
    }
};

// 6.2 通过路径重命名控件命令
class RenameWidgetByPathCommand : public ImUserWidgetClassCommandBase
{
private:
    std::string m_WidgetTreeVarName;
    std::string m_WidgetPath;
    std::string m_OldName;
    std::string m_NewName;
    std::string m_ParentPath;

public:
    RenameWidgetByPathCommand(ImGuiWidget::ImUserWidgetClass* target,
        Model_ImUserWidgetClassEditor* Model,
        const std::string& widgetTreeVarName,
        const std::string& widgetPath,
        const std::string& newName)
        : ImUserWidgetClassCommandBase(target,Model,
            CommandDataType(CommandCategory::RenameOperation,
                static_cast<int>(RenameOperationSubType::RenameWidgetByPath),
                "RenameWidgetByPath")),
        m_WidgetTreeVarName(widgetTreeVarName),
        m_WidgetPath(widgetPath),
        m_NewName(newName)
    {
        // 从路径提取控件名称和父路径
        size_t lastSlash = widgetPath.find_last_of('/');
        if (lastSlash == std::string::npos)
        {
            m_ParentPath = ""; // 根控件路径
            m_OldName = widgetPath;
        }
        else
        {
            m_ParentPath = widgetPath.substr(0, lastSlash);
            m_OldName = widgetPath.substr(lastSlash + 1);
        }
    }

    virtual ~RenameWidgetByPathCommand() = default;

    virtual bool Execute() override
    {
        bool success = m_TargetClass->RenameWidgetByPath(
            m_WidgetTreeVarName,
            m_WidgetPath,
            m_NewName);
        if (success && m_Model) {
            std::string filePath = m_Model->GetEditedFileFullPath();
            Publish(filePath + Events::OutlineView::WIDGET_CHILD_RENAMED,
                m_WidgetTreeVarName, m_OldName, m_NewName);
        }
        return success;
    }

    virtual bool Undo() override
    {
        // 构建新的路径用于撤销
        std::string newPath;
        if (m_ParentPath.empty())
        {
            newPath = m_NewName;
        }
        else
        {
            newPath = m_ParentPath + "/" + m_NewName;
        }

        // 使用新的路径进行撤销操作
        bool success = m_TargetClass->RenameWidgetByPath(
            m_WidgetTreeVarName,
            newPath,
            m_OldName);
        if (success && m_Model) {
            std::string filePath = m_Model->GetEditedFileFullPath();
            Publish(filePath + Events::OutlineView::WIDGET_CHILD_RENAMED,
                m_WidgetTreeVarName, m_NewName, m_OldName);
        }
        return success;
    }

    virtual std::string GetDescription() const override
    {
        return "Rename Widget: " + m_WidgetTreeVarName + "/" +
            (m_ParentPath.empty() ? "" : m_ParentPath + "/") +
            m_OldName + " -> " + m_NewName;
    }

    virtual bool CanMergeWith(const CommandBase<CommandDataType>* other) const override
    {
        if (!ImUserWidgetClassCommandBase::IsWithinMergeWindow(other))
            return false;

        if (!(m_Data == other->GetData())) return false;

        const auto* otherCmd = static_cast<const RenameWidgetByPathCommand*>(other);

        // 检查是否为同一控件和父路径下的连续重命名
        bool sameParentPath = (m_ParentPath == otherCmd->m_ParentPath);
        bool sameWidgetTree = (m_WidgetTreeVarName == otherCmd->m_WidgetTreeVarName);
        bool isConsecutiveRename = (m_NewName == otherCmd->m_OldName);

        return sameWidgetTree && sameParentPath && isConsecutiveRename;
    }

    virtual bool MergeWith(std::unique_ptr<CommandBase<CommandDataType>> other) override
    {
        if (!CanMergeWith(other.get())) return false;

        auto* otherCmd = static_cast<RenameWidgetByPathCommand*>(other.get());
        m_NewName = otherCmd->m_NewName;
        return true;
    }
};