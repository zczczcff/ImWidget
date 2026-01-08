#include "Model/Command/CommandManager.h"
#include "Model/Command/Command_PropertyEdit.h"

std::unique_ptr<EditCommand> EditCommandManager::CreatePropertyEditCommand(const ImGuiWidget::PropertyInfo& propInfo, const void* newValue, ImGuiWidget::PropertyStruct* target)
{
    switch (propInfo.type)
    {
    case ImGuiWidget::PropertyType::Color:
        return std::make_unique<ColorEditCommand>(propInfo, *static_cast<const ImU32*>(newValue), target);

    case ImGuiWidget::PropertyType::String:
        return std::make_unique<StringEditCommand>(propInfo, *static_cast<const std::string*>(newValue), target);

    case ImGuiWidget::PropertyType::Float:
        return std::make_unique<FloatEditCommand>(propInfo, *static_cast<const float*>(newValue), target);

    case ImGuiWidget::PropertyType::Int:
        return std::make_unique<IntEditCommand>(propInfo, *static_cast<const int*>(newValue), target);

    case ImGuiWidget::PropertyType::Bool:
        return std::make_unique<BoolEditCommand>(propInfo, *static_cast<const bool*>(newValue), target);

    case ImGuiWidget::PropertyType::Vec2:
        return std::make_unique<Vec2EditCommand>(propInfo, *static_cast<const ImVec2*>(newValue), target);

    case ImGuiWidget::PropertyType::Enum:
        return std::make_unique<EnumEditCommand>(propInfo, *static_cast<const std::string*>(newValue), target);

    case ImGuiWidget::PropertyType::StringArray:
        return std::make_unique<StringArrayEditCommand>(propInfo, *static_cast<const std::vector<std::string>*>(newValue), target);

    case ImGuiWidget::PropertyType::Struct:
        return std::make_unique<StructEditCommand>(propInfo, *static_cast<const ImGuiWidget::PropertyStruct*>(newValue), target);

    default:
        return nullptr;
    }
}

void EditCommandManager::ExecutePropertyEditImpl(const ImGuiWidget::PropertyInfo& propInfo, const void* newValue, ImGuiWidget::PropertyStruct* target)
{
    auto command = CreatePropertyEditCommand(propInfo, newValue, target);
    if (!command) return;
    Execute(std::move(command));
}

void EditCommandManager::Execute(std::unique_ptr<EditCommand> command)
{
    if (bIsUndoRedo)//撤销重做中，直接执行并退出
    {
        command->Execute();
        return;
    }
    // 检查是否可以与上一个命令合并
    if (!m_UndoStack.empty() && m_UndoStack.back()->CanMergeWith(command.get()))
    {
        // 尝试合并命令
        if (m_UndoStack.back()->MergeWith(std::move(command)))
        {
            // 合并成功，重新执行合并后的命令以确保状态正确
            m_UndoStack.back()->Execute();
            return;
        }
    }

    // 无法合并，正常执行命令
    command->Execute();
    m_UndoStack.push_back(std::move(command));

    // 限制撤销栈大小
    if (m_UndoStack.size() > m_MaxStackSize)
    {
        m_UndoStack.erase(m_UndoStack.begin());
    }

    m_RedoStack.clear();
}

void EditCommandManager::Undo()
{
    if (m_UndoStack.empty()) return;
    bIsUndoRedo = true;
    auto& command = m_UndoStack.back();

    command->Undo();
    PropertyEditCommand* p = static_cast<PropertyEditCommand*>(command.get());
    if (command->GetType() == EditCommandType::PropertyEdit)
    {
        OnPropertyEditUnDoRedo.Broadcast(p->GetTarget(), p->GetPropertyName());
    }

    // 移动到重做栈
    m_RedoStack.push_back(std::move(command));
    m_UndoStack.pop_back();
    bIsUndoRedo = false;
}

void EditCommandManager::Redo()
{
    if (m_RedoStack.empty()) return;
    bIsUndoRedo = true;
    auto& command = m_RedoStack.back();
    command->Execute();

    if (command->GetType() == EditCommandType::PropertyEdit)
    {
        PropertyEditCommand* p = static_cast<PropertyEditCommand*>(command.get());
        OnPropertyEditUnDoRedo.Broadcast(p->GetTarget(), p->GetPropertyName());
    }

    // 移回撤销栈
    m_UndoStack.push_back(std::move(command));
    m_RedoStack.pop_back();
    bIsUndoRedo = false;
}

void EditCommandManager::Clear()
{
    m_UndoStack.clear();
    m_RedoStack.clear();
}
