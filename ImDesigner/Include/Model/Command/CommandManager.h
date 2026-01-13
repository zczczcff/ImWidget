#pragma once
#include "CommandBase.h"
#include "ImWidget/ImObject.h"
#include "ImTools/ImDelegate.h"

namespace ImGuiWidget
{
    class ImWidget;
    class ImPanelWidget;
    class ImUserWidgetClass;
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
    ImMulticastDelegate<ImGuiWidget::ImObject*, const std::string&> OnPropertyEditUnDoRedo;
    ImMulticastDelegate<> OnChildEditUndoRedo;
    ImMulticastDelegate<const std::string&, const std::string&> OnVarRename;
private:
    std::unique_ptr<EditCommand> CreatePropertyEditCommand(
        const ImGuiWidget::PropertyInfo& propInfo,
        const void* newValue,
        ImGuiWidget::ImObject* target
    );
    bool ExecutePropertyEditImpl(const ImGuiWidget::PropertyInfo& propInfo, const void* newValue, ImGuiWidget::ImObject* target);
    bool Execute(std::unique_ptr<EditCommand> command);
public:
    //template<typename T>
    //void ExecutePropertyEdit(const ImGuiWidget::PropertyInfo& propInfo, const T& NewValue)
    //{
    //    ExecutePropertyEditImpl(propInfo, &NewValue);
    //}
    bool ExecutePropertyEdit(const ImGuiWidget::PropertyInfo& propInfo, const void* newValue, ImGuiWidget::ImObject* target)
    {
        return ExecutePropertyEditImpl(propInfo, newValue, target);
    }
    bool ExecuteChildRemove(ImGuiWidget::ImWidget* Target, int index);
    bool ExecuteChildRemove(ImGuiWidget::ImWidget* Target, ImGuiWidget::ImWidget* child);
    bool ExecuteChildInsert(ImGuiWidget::ImWidget* Target, ImGuiWidget::ImWidget* child, int index);
    bool ExcuteVarRename(ImGuiWidget::ImUserWidgetClass* Target, const std::string& OldName, const std::string& NewName);

    void Undo();
    void Redo();
    void Clear();

    bool CanUndo() const { return !m_UndoStack.empty(); }
    bool CanRedo() const { return !m_RedoStack.empty(); }

};