#pragma once
#include "CommandBase.h"

class CommandManager
{
private:
    std::vector<std::unique_ptr<ICommand>> m_UndoStack;
    std::vector<std::unique_ptr<ICommand>> m_RedoStack;
    size_t m_MaxStackSize = 100;
    bool m_IsMerging = false;
    ICommand* m_CurrentMergingCommand = nullptr;

public:
    void Execute(std::unique_ptr<ICommand> command);
    void Undo();
    void Redo();
    void Clear();

    bool CanUndo() const { return !m_UndoStack.empty(); }
    bool CanRedo() const { return !m_RedoStack.empty(); }

};