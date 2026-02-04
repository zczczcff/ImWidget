#include "Model/Model_UserWidgetClassEditor.h"

#include "Model/Command/Command_VariableOperation.h"
#include "Model/Command/Command_VarRename.h"
#include "Model/Command/Command_ChildWidgetOperation.h"
#include "Model/Command/Command_ClassStringPropertyEdit.h"
#include "Model/Command/Command_ObjectVarPropertyEdit.h"
#include "Model/Command/Command_WidgetVarPropertyEdit.h"
#include "Model/Command/Command_WidgetSlotPropertyEdit.h"

void Model_ImUserWidgetClassEditor::OnCreateBasicVariable(ImGuiWidget::PropertyType type)
{
    if (!m_TargetClass) return;

    // 创建命令并执行
    std::string typeName = ImGuiWidget::PropertyTypeToString(type);
    auto command = std::make_unique<CreateNewVariableCommand>(
        m_TargetClass,this,
        ImGuiWidget::WidgetClassVariableType::Basic,
        typeName);

    if (m_CommandManager->Execute(std::move(command)))
    {
        m_IsModified = true;

        // 发布更新事件
        Publish(m_EditedFileFullPath + Events::OutlineView::UPDATE_BASIC_VARIABLE_SECTION);

        // 更新Undo/Redo状态
        UpdateUndoRedoState();

    }
}

void Model_ImUserWidgetClassEditor::OnCreateObjectVariable(const std::string& objectRegisterName)
{
    if (!m_TargetClass) return;

    auto command = std::make_unique<CreateNewVariableCommand>(
        m_TargetClass, this,
        ImGuiWidget::WidgetClassVariableType::Object,
        objectRegisterName);

    if (m_CommandManager->Execute(std::move(command)))
    {
        m_IsModified = true;

        Publish(m_EditedFileFullPath + Events::OutlineView::UPDATE_OBJECT_VARIABLE_SECTION);
        UpdateUndoRedoState();
    }
}

void Model_ImUserWidgetClassEditor::OnCreateWidgetVariable(const std::string& widgetRegisterName)
{
    if (!m_TargetClass) return;

    auto command = std::make_unique<CreateNewVariableCommand>(
        m_TargetClass, this,
        ImGuiWidget::WidgetClassVariableType::Widget,
        widgetRegisterName);

    if (m_CommandManager->Execute(std::move(command)))
    {
        m_IsModified = true;

        Publish(m_EditedFileFullPath + Events::OutlineView::UPDATE_WIDGET_VARIABLE_SECTION);
        UpdateUndoRedoState();

    }
}

void Model_ImUserWidgetClassEditor::OnDeleteVariable(const std::string& variableName)
{
    if (!m_TargetClass) return;

    // 获取变量类型以便恢复选择
    auto varType = m_TargetClass->GetVariableType(variableName);

    auto command = std::make_unique<RemoveVariableCommand>(m_TargetClass, this, variableName);

    if (m_CommandManager->Execute(std::move(command)))
    {
        m_IsModified = true;

        // 根据删除的变量类型发布相应的事件
        switch (varType)
        {
        case ImGuiWidget::WidgetClassVariableType::Widget:
            Publish(m_EditedFileFullPath + Events::OutlineView::UPDATE_WIDGET_VARIABLE_SECTION);
            break;
        case ImGuiWidget::WidgetClassVariableType::Object:
            Publish(m_EditedFileFullPath + Events::OutlineView::UPDATE_OBJECT_VARIABLE_SECTION);
            break;
        case ImGuiWidget::WidgetClassVariableType::Basic:
            Publish(m_EditedFileFullPath + Events::OutlineView::UPDATE_BASIC_VARIABLE_SECTION);
            break;
        }

        UpdateUndoRedoState();

        // 清除当前选择
        if (m_CurrentSelectedVariableName == variableName)
        {
            m_CurrentSelectedVariableName.clear();
            m_CurrentSelectedVariableType = ImGuiWidget::WidgetClassVariableType::Widget;
            m_CurrentSelectedWidget = nullptr;
        }
    }
}

// 重命名变量（从事件处理）

void Model_ImUserWidgetClassEditor::OnVariableRenamed(const std::string& oldName, const std::string& newName)
{
    if (!m_TargetClass) return;

    auto command = std::make_unique<RenameVariableCommand>(m_TargetClass, this, oldName, newName);

    if (m_CommandManager->Execute(std::move(command)))
    {
        m_IsModified = true;

        // 更新当前选择
        if (m_CurrentSelectedVariableName == oldName)
        {
            m_CurrentSelectedVariableName = newName;
        }

        // 发布变量重命名事件
        Publish(m_EditedFileFullPath + Events::OutlineView::VARIABLE_RENAMED, oldName, newName);
        UpdateUndoRedoState();
    }
}

// 重命名控件（通过路径）

void Model_ImUserWidgetClassEditor::OnRenameWidgetByPath(const std::string& widgetTreeVarName, const std::string& widgetPath, const std::string& newName)
{
    if (!m_TargetClass) return;

    // 首先获取旧名称
    ImGuiWidget::ImWidget* rootWidget = m_TargetClass->GetWidgetVariable(widgetTreeVarName);
    if (!rootWidget) return;

    // 从路径解析出控件
    // 这里需要路径解析函数，暂时简化处理
    std::string oldName = ExtractWidgetNameFromPath(widgetPath);

    auto command = std::make_unique<RenameWidgetByPathCommand>(
        m_TargetClass, this, widgetTreeVarName, widgetPath, newName);

    if (m_CommandManager->Execute(std::move(command)))
    {
        m_IsModified = true;

        // 更新当前选择
        if (m_CurrentSelectedWidget && m_CurrentSelectedWidget->GetWidgetName() == oldName)
        {
            m_CurrentSelectedWidget->SetWidgetName(newName);
        }

        // 发布控件重命名事件
        Publish(m_EditedFileFullPath + Events::OutlineView::WIDGET_CHILD_RENAMED,
            widgetTreeVarName, oldName, newName);
        UpdateUndoRedoState();
    }
}

void Model_ImUserWidgetClassEditor::OnInsertWidget(const std::string& widgetTreeVarName, const std::string& parentPath, const std::string& insertWidgetRegisterName, int insertIndex)
{
    if (!m_TargetClass) return;

    auto command = std::make_unique<InsertChildByPathCommand>(
        m_TargetClass, this, widgetTreeVarName, parentPath, insertWidgetRegisterName, insertIndex);

    if (m_CommandManager->Execute(std::move(command)))
    {
        m_IsModified = true;

        // 发布控件添加事件
        Publish(m_EditedFileFullPath + Events::OutlineView::WIDGET_CHILD_ADDED,
            widgetTreeVarName, parentPath, insertWidgetRegisterName);

        UpdateUndoRedoState();
    }
}

void Model_ImUserWidgetClassEditor::OnDeleteWidget(const std::string& widgetTreeVarName, const std::string& widgetPath)
{
    if (!m_TargetClass) return;

    auto command = std::make_unique<RemoveChildByPathCommand>(
        m_TargetClass, this, widgetTreeVarName, widgetPath);

    if (m_CommandManager->Execute(std::move(command)))
    {
        m_IsModified = true;

        // 发布控件删除事件
        Publish(m_EditedFileFullPath + Events::OutlineView::WIDGET_CHILD_REMOVED,
            widgetTreeVarName, widgetPath);

        UpdateUndoRedoState();
    }
}
