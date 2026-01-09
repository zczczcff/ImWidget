#pragma once
#include "CommandBase.h"
#include "ImWidget/ImWidgetProperty.h"
#include "ImTools/ImDelegate.h"

namespace ImGuiWidget
{
    class ImWidget;
    class ImPanelWidget;
}

class EditCommandManager
{
private:
    std::vector<std::unique_ptr<EditCommand>> m_UndoStack;
    std::vector<std::unique_ptr<EditCommand>> m_RedoStack;
    size_t m_MaxStackSize = 100;
    bool bIsMerging = false;
    bool bIsUndoRedo = false;
    EditCommand* m_CurrentMergingCommand = nullptr;
public:
    ImMulticastDelegate<ImGuiWidget::PropertyStruct*, const std::string&> OnPropertyEditUnDoRedo;
    ImMulticastDelegate<> OnChildEditUndoRedo;
private:
    std::unique_ptr<EditCommand> CreatePropertyEditCommand(
        const ImGuiWidget::PropertyInfo& propInfo,
        const void* newValue,
        ImGuiWidget::PropertyStruct* target
    );
    void ExecutePropertyEditImpl(const ImGuiWidget::PropertyInfo& propInfo, const void* newValue, ImGuiWidget::PropertyStruct* target);
    bool Execute(std::unique_ptr<EditCommand> command);
public:
    //template<typename T>
    //void ExecutePropertyEdit(const ImGuiWidget::PropertyInfo& propInfo, const T& NewValue)
    //{
    //    ExecutePropertyEditImpl(propInfo, &NewValue);
    //}
    void ExecutePropertyEdit(const ImGuiWidget::PropertyInfo& propInfo, const void* newValue, ImGuiWidget::PropertyStruct* target)
    {
        ExecutePropertyEditImpl(propInfo, newValue, target);
    }
    bool ExecuteChildRemove(ImGuiWidget::ImWidget* Target, int index);
    bool ExecuteChildRemove(ImGuiWidget::ImWidget* Target, ImGuiWidget::ImWidget* child);
    bool ExecuteChildInsert(ImGuiWidget::ImWidget* Target, ImGuiWidget::ImWidget* child, int index);
    
    void Undo();
    void Redo();
    void Clear();

    bool CanUndo() const { return !m_UndoStack.empty(); }
    bool CanRedo() const { return !m_RedoStack.empty(); }

};