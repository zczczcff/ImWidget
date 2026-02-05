#pragma once
#include "ImDesignerCommandBase.h"
#include "ImWidget/ImUserWidgetClass.h"
#include "ImWidget/ImUserWidgetSerializer.h"
#include <nlohmann/json.hpp>

// 控件子节点操作子类型
enum class ChildWidgetOperationSubType
{
    InsertChildByPath = 0,    // 通过路径插入子控件
    InsertChildByJson = 1,    // 通过Json插入子控件
    RemoveChildByPath = 2,    // 通过路径移除子控件
};

class InsertChildByPathCommand : public ImUserWidgetClassCommandBase
{
private:
    std::string m_WidgetTreeVarName;
    std::string m_ParentWidgetPath;
    std::string m_InsertWidgetRegisterName;
    int m_InsertIndex;

    // 执行后实际生成的子控件信息
    std::string m_ActualChildName;    // 实际生成的名称（可能被重命名）
    std::string m_ActualChildPath;    // 实际生成的子路径
    nlohmann::json m_ActualWidgetJson; // 实际生成的控件JSON

public:
    InsertChildByPathCommand(ImGuiWidget::ImUserWidgetClass* target,
        Model_ImUserWidgetClassEditor* Model,
        const std::string& widgetTreeVarName,
        const std::string& parentWidgetPath,
        const std::string& insertWidgetRegisterName,
        int insertIndex)
        : ImUserWidgetClassCommandBase(target,Model,
            CommandDataType(CommandCategory::ChildWidgetOperation,
                static_cast<int>(ChildWidgetOperationSubType::InsertChildByPath),
                "InsertChildByPath")),
        m_WidgetTreeVarName(widgetTreeVarName),
        m_ParentWidgetPath(parentWidgetPath),
        m_InsertWidgetRegisterName(insertWidgetRegisterName),
        m_InsertIndex(insertIndex)
    {
    }

    virtual ~InsertChildByPathCommand() = default;

    virtual bool Execute() override
    {
        // 执行插入操作，父路径为空时表示根控件
        std::string actualParentPath = m_ParentWidgetPath.empty() ? "." : m_ParentWidgetPath;

        auto insertedWidget = m_TargetClass->InsertChildWidgetByPath(
            m_WidgetTreeVarName,
            actualParentPath,
            m_InsertWidgetRegisterName,
            m_InsertIndex);

        if (!insertedWidget) return false;

        // 记录实际生成的控件信息
        m_ActualChildName = insertedWidget->GetWidgetName();

        if (m_ParentWidgetPath.empty())
        {
            m_ActualChildPath = m_ActualChildName;
        }
        else
        {
            m_ActualChildPath = m_ParentWidgetPath + "/" + m_ActualChildName;
        }

        // 序列化实际生成的控件，用于撤销
        m_ActualWidgetJson = ImGuiWidget::ImUserWidgetClassSerializer::SerializeImWidget(insertedWidget);

        // 发布控件添加事件
        if (m_Model) {
            std::string filePath = m_Model->GetEditedFileFullPath();
            Publish(filePath + Events::OutlineView::WIDGET_CHILD_ADDED,
                m_WidgetTreeVarName, m_ParentWidgetPath, m_InsertWidgetRegisterName);
        }

        return true;
    }

    virtual bool Undo() override
    {
        if (m_ActualChildPath.empty()) return false;

        // 移除插入的子控件，使用实际路径
        bool success = m_TargetClass->RemoveChildWidgetByPath(
            m_WidgetTreeVarName,
            m_ActualChildPath);

        if (success && m_Model) {
            std::string filePath = m_Model->GetEditedFileFullPath();
            Publish(filePath + Events::OutlineView::WIDGET_CHILD_REMOVED,
                m_WidgetTreeVarName, m_ActualChildPath);
        }

        return success;
    }

    virtual std::string GetDescription() const override
    {
        std::string expectedName = m_InsertWidgetRegisterName;
        // 移除命名空间前缀
        size_t pos = expectedName.find_last_of("::");
        if (pos != std::string::npos)
            expectedName = expectedName.substr(pos + 1);

        std::string actualNameInfo = m_ActualChildName.empty() ?
            expectedName :
            expectedName + " (重命名为 " + m_ActualChildName + ")";

        return "插入子控件: " + actualNameInfo + " 在 " +
            m_WidgetTreeVarName + "/" +
            (m_ParentWidgetPath.empty() ? "[根]" : m_ParentWidgetPath) +
            " 索引 " + std::to_string(m_InsertIndex);
    }

    virtual bool CanMergeWith(const CommandBase<CommandDataType>* other) const override
    {
        return false; // 插入操作通常不能合并
    }

    virtual bool MergeWith(std::unique_ptr<CommandBase<CommandDataType>> other) override
    {
        return false;
    }
};

// 5.2 通过Json插入子控件命令 - 一般用于复制粘贴
class InsertChildByJsonCommand : public ImUserWidgetClassCommandBase
{
private:
    std::string m_WidgetTreeVarName;
    std::string m_ParentWidgetPath;
    nlohmann::json m_WidgetJson;
    int m_InsertIndex;

    // 执行后实际生成的子控件信息
    std::string m_ExpectedName;      // JSON中的期望名称
    std::string m_ActualChildName;   // 实际生成的名称（可能被重命名）
    std::string m_ActualChildPath;   // 实际生成的子路径

public:
    InsertChildByJsonCommand(ImGuiWidget::ImUserWidgetClass* target,
        Model_ImUserWidgetClassEditor* Model,
        const std::string& widgetTreeVarName,
        const std::string& parentWidgetPath,
        const nlohmann::json& widgetJson,
        int insertIndex)
        : ImUserWidgetClassCommandBase(target,Model,
            CommandDataType(CommandCategory::ChildWidgetOperation,
                static_cast<int>(ChildWidgetOperationSubType::InsertChildByJson),
                "InsertChildByJson")),
        m_WidgetTreeVarName(widgetTreeVarName),
        m_ParentWidgetPath(parentWidgetPath),
        m_WidgetJson(widgetJson),
        m_InsertIndex(insertIndex)
    {
        // 从JSON获取期望名称
        if (m_WidgetJson.contains("Name"))
        {
            m_ExpectedName = m_WidgetJson["Name"].get<std::string>();
        }
        else if (m_WidgetJson.contains("Type"))
        {
            // 如果没有名称，使用类型作为基础名称
            m_ExpectedName = m_WidgetJson["Type"].get<std::string>();
        }
    }

    virtual ~InsertChildByJsonCommand() = default;

    virtual bool Execute() override
    {
        // 执行插入操作
        auto insertedWidget = m_TargetClass->InsertChildWidget(
            m_WidgetTreeVarName,
            m_ParentWidgetPath,
            m_WidgetJson,
            m_InsertIndex);

        if (!insertedWidget) return false;

        // 记录实际生成的控件信息
        m_ActualChildName = insertedWidget->GetWidgetName();
        m_ActualChildPath = BuildWidgetPath(m_ParentWidgetPath, m_ActualChildName);

        // 发布控件添加事件
        if (m_Model) {
            std::string filePath = m_Model->GetEditedFileFullPath();
            Publish(filePath + Events::OutlineView::WIDGET_CHILD_ADDED,
                m_WidgetTreeVarName, m_ParentWidgetPath, m_ExpectedName);
        }

        return true;
    }

    virtual bool Undo() override
    {
        if (m_ActualChildPath.empty()) return false;

        // 移除插入的子控件，使用实际路径
        bool success = m_TargetClass->RemoveChildWidgetByPath(
            m_WidgetTreeVarName,
            m_ActualChildPath);

        if (success && m_Model) {
            std::string filePath = m_Model->GetEditedFileFullPath();
            Publish(filePath + Events::OutlineView::WIDGET_CHILD_REMOVED,
                m_WidgetTreeVarName, m_ActualChildPath);
        }

        return success;
    }

    virtual std::string GetDescription() const override
    {
        std::string nameInfo = m_ActualChildName.empty() ?
            m_ExpectedName :
            (m_ExpectedName == m_ActualChildName ? m_ExpectedName :
                m_ExpectedName + " (重命名为 " + m_ActualChildName + ")");

        return "从JSON插入子控件: " + nameInfo + " 在 " +
            m_WidgetTreeVarName + "/" +
            (m_ParentWidgetPath.empty() ? "[根]" : m_ParentWidgetPath) +
            " 索引 " + std::to_string(m_InsertIndex);
    }

    virtual bool CanMergeWith(const CommandBase<CommandDataType>* other) const override
    {
        return false; // 插入操作通常不能合并
    }

    virtual bool MergeWith(std::unique_ptr<CommandBase<CommandDataType>> other) override
    {
        return false;
    }

private:
    std::string BuildWidgetPath(const std::string& parentPath, const std::string& widgetName)
    {
        if (parentPath.empty() || parentPath == ".")
            return widgetName;
        return parentPath + "/" + widgetName;
    }
};

// 5.3 通过路径移除子控件命令
class RemoveChildByPathCommand : public ImUserWidgetClassCommandBase
{
private:
    std::string m_WidgetTreeVarName;
    std::string m_ChildWidgetPath;

    // 移除前控件信息
    std::string m_ParentWidgetPath;
    std::string m_ChildWidgetName;
    int m_ChildIndex;

    // 移除的控件JSON数据
    nlohmann::json m_RemovedWidgetJson;

public:
    RemoveChildByPathCommand(ImGuiWidget::ImUserWidgetClass* target,
        Model_ImUserWidgetClassEditor* Model,
        const std::string& widgetTreeVarName,
        const std::string& childWidgetPath)
        : ImUserWidgetClassCommandBase(target,Model,
            CommandDataType(CommandCategory::ChildWidgetOperation,
                static_cast<int>(ChildWidgetOperationSubType::RemoveChildByPath),
                "RemoveChildByPath")),
        m_WidgetTreeVarName(widgetTreeVarName),
        m_ChildWidgetPath(childWidgetPath),
        m_ChildIndex(-1)
    {
        // 解析路径
        ParseWidgetPath(childWidgetPath, m_ParentWidgetPath, m_ChildWidgetName);
    }

    virtual ~RemoveChildByPathCommand() = default;

    virtual bool Execute() override
    {
        // 获取根控件
        auto rootWidget = m_TargetClass->GetWidgetVariable(m_WidgetTreeVarName);
        if (!rootWidget) return false;

        // 通过路径找到要删除的控件
        ImGuiWidget::ImWidget* targetWidget = rootWidget->FindChildByPath(m_ChildWidgetPath);
        if (!targetWidget)
        {
            // 路径查找失败，尝试通过名称查找（备用方案）
            if (m_ParentWidgetPath.empty())
            {
                // 在根控件下查找
                targetWidget = FindChildByName(rootWidget, m_ChildWidgetName, m_ChildIndex);
            }
            else
            {
                // 在父控件下查找
                ImGuiWidget::ImWidget* parentWidget = rootWidget->FindChildByPath(m_ParentWidgetPath);
                if (parentWidget)
                {
                    targetWidget = FindChildByName(parentWidget, m_ChildWidgetName, m_ChildIndex);
                }
            }
        }

        if (!targetWidget) return false;

        // 序列化要移除的控件（在删除之前）
        m_RemovedWidgetJson = ImGuiWidget::ImUserWidgetClassSerializer::SerializeImWidget(targetWidget);

        // 获取父控件（用于记录索引）
        ImGuiWidget::ImWidget* parentWidget = targetWidget->GetParents();
        if (parentWidget)
        {
            // 记录子控件的索引位置
            for (int i = 0; i < parentWidget->GetChildNum(); i++)
            {
                if (parentWidget->GetChildAt(i) == targetWidget)
                {
                    m_ChildIndex = i;
                    break;
                }
            }
        }

        // 执行移除操作
        bool success = m_TargetClass->RemoveChildWidgetByPath(
            m_WidgetTreeVarName,
            m_ChildWidgetPath);

        if (success && m_Model) {
            std::string filePath = m_Model->GetEditedFileFullPath();
            Publish(filePath + Events::OutlineView::WIDGET_CHILD_REMOVED,
                m_WidgetTreeVarName, m_ChildWidgetPath);
        }

        return success;
    }

    virtual bool Undo() override
    {
        if (m_ChildIndex < 0 || m_RemovedWidgetJson.empty()) return false;

        // 修改JSON中的名称为实际名称，因为插入时可能会被重命名
        // 但这里我们保留原始名称，让InsertChildWidget自动处理重命名
        auto insertedWidget = m_TargetClass->InsertChildWidget(
            m_WidgetTreeVarName,
            m_ParentWidgetPath,
            m_RemovedWidgetJson,
            m_ChildIndex);

        bool success = (insertedWidget != nullptr);

        if (success && m_Model) {
            std::string filePath = m_Model->GetEditedFileFullPath();
            Publish(filePath + Events::OutlineView::WIDGET_CHILD_ADDED,
                m_WidgetTreeVarName, m_ParentWidgetPath, m_ChildWidgetName);
        }

        return success;
    }

    virtual std::string GetDescription() const override
    {
        return "移除子控件: " + m_WidgetTreeVarName + "/" + m_ChildWidgetPath;
    }

    virtual bool CanMergeWith(const CommandBase<CommandDataType>* other) const override
    {
        return false; // 移除操作通常不能合并
    }

    virtual bool MergeWith(std::unique_ptr<CommandBase<CommandDataType>> other) override
    {
        return false;
    }

private:
    void ParseWidgetPath(const std::string& path, std::string& parentPath, std::string& widgetName)
    {
        size_t lastSlash = path.find_last_of('/');
        if (lastSlash == std::string::npos)
        {
            parentPath = "";
            widgetName = path;
        }
        else
        {
            parentPath = path.substr(0, lastSlash);
            widgetName = path.substr(lastSlash + 1);
        }
    }

    ImGuiWidget::ImWidget* FindChildByName(ImGuiWidget::ImWidget* parent,
        const std::string& name,
        int& outIndex)
    {
        if (!parent) return nullptr;

        for (int i = 0; i < parent->GetChildNum(); i++)
        {
            auto child = parent->GetChildAt(i);
            if (child && child->GetWidgetName() == name)
            {
                outIndex = i;
                return child;
            }
        }

        return nullptr;
    }
};