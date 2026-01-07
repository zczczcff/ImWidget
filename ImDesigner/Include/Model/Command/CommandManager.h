#pragma once
#include "CommandBase.h"
#include "ImWidget/ImWidgetProperty.h"

class EditCommandManager
{
private:
    std::vector<std::unique_ptr<EditCommand>> m_UndoStack;
    std::vector<std::unique_ptr<EditCommand>> m_RedoStack;
    size_t m_MaxStackSize = 100;
    bool m_IsMerging = false;
    EditCommand* m_CurrentMergingCommand = nullptr;

private:
    std::unique_ptr<EditCommand> CreatePropertyEditCommand(
        const ImGuiWidget::PropertyInfo& propInfo,
        const void* newValue);
    void ExecutePropertyEditImpl(const ImGuiWidget::PropertyInfo& propInfo, const void* newValue);
    void Execute(std::unique_ptr<EditCommand> command);
public:
    template<typename T>
    void ExecutePropertyEdit(const ImGuiWidget::PropertyInfo& propInfo, const T& NewValue)
    {
        ExecutePropertyEditImpl(propInfo, &NewValue);
    }
    void ExecutePropertyEdit(const ImGuiWidget::PropertyInfo& propInfo, const void* newValue)
    {
        ExecutePropertyEditImpl(propInfo, newValue);
    }
    void Undo();
    void Redo();
    void Clear();

    bool CanUndo() const { return !m_UndoStack.empty(); }
    bool CanRedo() const { return !m_RedoStack.empty(); }

};