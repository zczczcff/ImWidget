#pragma once
#include <memory>
#include <vector>
#include <string>
#include <chrono>

class EditCommand
{
protected:
    std::chrono::steady_clock::time_point m_Timestamp; // 命令时间戳

public:
    EditCommand() : m_Timestamp(std::chrono::steady_clock::now()) {}
    virtual ~EditCommand() = default;
    virtual void Execute() = 0;
    virtual void Undo() = 0;
    virtual std::string GetDescription() const = 0;
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
};