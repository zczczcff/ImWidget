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

        // 更新Undo/Redo状态

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
    }
}

// ==================== 变量粘贴操作 ====================

void Model_ImUserWidgetClassEditor::OnPasteVariable(const nlohmann::json& serializedData, bool keepOriginalName)
{
    if (!m_TargetClass) return;

    auto command = std::make_unique<PasteVariableCommand>(
        m_TargetClass, this, serializedData, keepOriginalName);

    if (m_CommandManager->Execute(std::move(command)))
    {
        m_IsModified = true;
    }
}

void Model_ImUserWidgetClassEditor::OnPasteObjectVariable(const nlohmann::json& objectJson, const std::string& suggestedName, bool keepSuggestedName)
{
    if (!m_TargetClass) return;

    auto command = std::make_unique<PasteObjectVariableCommand>(
        m_TargetClass, this, objectJson, suggestedName, keepSuggestedName);

    if (m_CommandManager->Execute(std::move(command)))
    {
        m_IsModified = true;
    }
}

void Model_ImUserWidgetClassEditor::OnPasteWidgetVariable(const nlohmann::json& widgetJson, const std::string& suggestedName, bool keepSuggestedName)
{
    if (!m_TargetClass) return;

    auto command = std::make_unique<PasteWidgetVariableCommand>(
        m_TargetClass, this, widgetJson, suggestedName, keepSuggestedName);

    if (m_CommandManager->Execute(std::move(command)))
    {
        m_IsModified = true;
    }
}

// ==================== 通过JSON插入子控件 ====================

void Model_ImUserWidgetClassEditor::OnInsertWidgetByJson(const std::string& widgetTreeVarName, const std::string& parentPath,
    const nlohmann::json& widgetJson, int insertIndex)
{
    if (!m_TargetClass) return;

    auto command = std::make_unique<InsertChildByJsonCommand>(
        m_TargetClass, this, widgetTreeVarName, parentPath, widgetJson, insertIndex);

    if (m_CommandManager->Execute(std::move(command)))
    {
        m_IsModified = true;
    }
}

// ==================== 类字符串属性编辑 ====================

void Model_ImUserWidgetClassEditor::OnEditClassName(const std::string& newName)
{
    if (!m_TargetClass) return;

    std::string oldName = m_TargetClass->GetClassName();
    auto command = std::make_unique<EditClassNameCommand>(
        m_TargetClass, this, oldName, newName);

    if (m_CommandManager->Execute(std::move(command)))
    {
        m_IsModified = true;
    }
}

void Model_ImUserWidgetClassEditor::OnEditNamespace(const std::string& newName)
{
    if (!m_TargetClass) return;

    std::string oldName = m_TargetClass->GetNamespace();
    auto command = std::make_unique<EditNamespaceCommand>(
        m_TargetClass, this, oldName, newName);

    if (m_CommandManager->Execute(std::move(command)))
    {
        m_IsModified = true;
    }
}

void Model_ImUserWidgetClassEditor::OnEditBaseClass(const std::string& newName)
{
    if (!m_TargetClass) return;

    std::string oldName = m_TargetClass->GetBaseClass();
    auto command = std::make_unique<EditBaseClassCommand>(
        m_TargetClass, this, oldName, newName);

    if (m_CommandManager->Execute(std::move(command)))
    {
        m_IsModified = true;
    }
}

void Model_ImUserWidgetClassEditor::OnEditDefaultRoot(const std::string& newName)
{
    if (!m_TargetClass) return;

    std::string oldName = m_TargetClass->GetDefaultRootVariableName();
    auto command = std::make_unique<EditDefaultRootCommand>(
        m_TargetClass, this, oldName, newName);

    if (m_CommandManager->Execute(std::move(command)))
    {
        m_IsModified = true;
    }
}

// ==================== 复制变量 ====================

void Model_ImUserWidgetClassEditor::OnCopyVariable(const std::string& variableName)
{
    if (!m_TargetClass) return;

    //------------------暂不具体实现-----------------

    // 获取变量类型
    //auto varType = m_TargetClass->GetVariableType(variableName);
    //if (varType == ImGuiWidget::WidgetClassVariableType::Widget)
    //{
    //    // 获取控件变量
    //    ImGuiWidget::ImWidget* widget = m_TargetClass->GetWidgetVariable(variableName);
    //    if (!widget) return;

    //    // 序列化控件为JSON
    //    nlohmann::json widgetJson = ImGuiWidget::ImUserWidgetSerializer::SerializeWidget(widget);
    //    std::string jsonString = widgetJson.dump();

    //    // 设置到剪贴板
    //    ImGui::SetClipboardText(jsonString.c_str());
    //}
    //else if (varType == ImGuiWidget::WidgetClassVariableType::Object)
    //{
    //    // 获取Object变量
    //    ImGuiWidget::ImObject* obj = m_TargetClass->GetObjectVariable(variableName);
    //    if (!obj) return;

    //    // 序列化Object为JSON
    //    nlohmann::json objJson = ImGuiWidget::ImUserWidgetSerializer::SerializeObject(obj);
    //    std::string jsonString = objJson.dump();

    //    // 设置到剪贴板
    //    ImGui::SetClipboardText(jsonString.c_str());
    //}
    //else if (varType == ImGuiWidget::WidgetClassVariableType::Basic)
    //{
    //    // 获取基本变量
    //    auto* basicVar = m_TargetClass->GetVariableAs<ImGuiWidget::ImWidgetClassVariable_Basic>(variableName);
    //    if (!basicVar) return;

    //    // 序列化基本变量为JSON
    //    nlohmann::json varJson = ImGuiWidget::ImUserWidgetSerializer::SerializeVariable(basicVar);
    //    std::string jsonString = varJson.dump();

    //    // 设置到剪贴板
    //    ImGui::SetClipboardText(jsonString.c_str());
    //}
}

// ==================== 复制控件子项 ====================

void Model_ImUserWidgetClassEditor::OnCopyWidget(const std::string& widgetTreeVarName, const std::string& widgetPath)
{
    if (!m_TargetClass) return;

    //------------------暂不具体实现-----------------

    // 获取控件树根
    //ImGuiWidget::ImWidget* rootWidget = m_TargetClass->GetWidgetVariable(widgetTreeVarName);
    //if (!rootWidget) return;

    //// 找到目标控件
    //ImGuiWidget::ImWidget* targetWidget = rootWidget;
    //if (!widgetPath.empty())
    //{
    //    std::vector<std::string> pathParts;
    //    std::stringstream ss(widgetPath);
    //    std::string part;
    //    while (std::getline(ss, part, '/'))
    //    {
    //        if (!part.empty())
    //        {
    //            pathParts.push_back(part);
    //        }
    //    }

    //    for (const auto& widgetName : pathParts)
    //    {
    //        bool found = false;
    //        for (int i = 0; i < targetWidget->GetChildNum(); ++i)
    //        {
    //            ImGuiWidget::ImWidget* child = targetWidget->GetChildAt(i);
    //            if (child && child->GetWidgetName() == widgetName)
    //            {
    //                targetWidget = child;
    //                found = true;
    //                break;
    //            }
    //        }
    //        if (!found) return;
    //    }
    //}

    //// 序列化控件为JSON
    //nlohmann::json widgetJson = ImGuiWidget::ImUserWidgetSerializer::SerializeWidget(targetWidget);
    //std::string jsonString = widgetJson.dump();

    //// 设置到剪贴板
    //ImGui::SetClipboardText(jsonString.c_str());
}
