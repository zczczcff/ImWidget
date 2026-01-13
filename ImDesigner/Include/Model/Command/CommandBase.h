#pragma once
#include <memory>
#include <vector>
#include <string>
#include <chrono>

enum class EditCommandType
{
    PropertyEdit,
    ChildChange,
    RenameVar,
    Paste
};

class EditCommand
{
protected:
    std::chrono::steady_clock::time_point m_Timestamp; // 命令时间戳
    EditCommandType m_type;
    std::string m_Description;
public:
    EditCommand() : m_Timestamp(std::chrono::steady_clock::now()),m_type(EditCommandType::PropertyEdit) {}
    virtual ~EditCommand() = default;
    virtual bool Execute() = 0;
    virtual bool Undo() = 0;
    virtual std::string GetDescription() { return ""; };
    virtual bool CanMergeWith(const EditCommand* other) const { return false; }
    virtual bool MergeWith(std::unique_ptr<EditCommand> other) { return false; }

    // 获取命令时间戳
    std::chrono::steady_clock::time_point GetTimestamp() const
    {
        return m_Timestamp;
    }

    // 检查是否在合并时间窗口内（默认500ms）
    bool IsWithinMergeWindow(const EditCommand* other,
        std::chrono::milliseconds window = std::chrono::milliseconds(500)) const
    {
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(
            other->m_Timestamp - m_Timestamp);
        return diff <= window;
    }

    EditCommandType GetType() const { return m_type; }
};