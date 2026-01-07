#include "Model/Command/CommandManager.h"

void CommandManager::Execute(std::unique_ptr<ICommand> command)
{
    // 正常执行命令
    command->Execute();

    // 添加到撤销栈
    m_UndoStack.push_back(std::move(command));

    // 清空重做栈
    m_RedoStack.clear();
}

void CommandManager::Undo()
{
    if (m_UndoStack.empty()) return;

    auto& command = m_UndoStack.back();
    command->Undo();

    // 移动到重做栈
    m_RedoStack.push_back(std::move(command));
    m_UndoStack.pop_back();
}

void CommandManager::Redo()
{
    if (m_RedoStack.empty()) return;

    auto& command = m_RedoStack.back();
    command->Execute();

    // 移回撤销栈
    m_UndoStack.push_back(std::move(command));
    m_RedoStack.pop_back();
}

void CommandManager::Clear()
{
    m_UndoStack.clear();
    m_RedoStack.clear();
}
