#pragma once

#include <nlohmann/json.hpp>
#include <queue>
#include <string>

// 剪贴板对象类型枚举
enum class ClipboardObjectType
{
    BasicVariable,    // 基本变量
    ObjectVariable,    // Object变量
    WidgetVariable,    // 控件树变量
    WidgetChild        // 控件子项
};

// 编辑器剪贴板类
class EditorClipboard
{
private:
    // 剪贴板对象结构
    struct ClipboardObject
    {
        ClipboardObjectType type;
        nlohmann::json jsonData;
        std::string sourceName;  // 源变量名/控件名（用于标识）

        ClipboardObject(ClipboardObjectType t, const nlohmann::json& data, const std::string& name)
            : type(t), jsonData(data), sourceName(name)
        {
        }

        ClipboardObject() : type(ClipboardObjectType::BasicVariable)
        {
        }
    };

    // 各类型对象的队列
    std::queue<ClipboardObject> m_BasicVariableQueue;
    std::queue<ClipboardObject> m_ObjectVariableQueue;
    std::queue<ClipboardObject> m_WidgetVariableQueue;
    std::queue<ClipboardObject> m_WidgetChildQueue;

    // 队列最大长度
    size_t m_MaxQueueSize;

    // 辅助函数：向指定队列添加对象
    void AddToQueue(ClipboardObjectType type, const ClipboardObject& obj)
    {
        std::queue<ClipboardObject>* queue = nullptr;

        switch (type)
        {
        case ClipboardObjectType::BasicVariable:
            queue = &m_BasicVariableQueue;
            break;
        case ClipboardObjectType::ObjectVariable:
            queue = &m_ObjectVariableQueue;
            break;
        case ClipboardObjectType::WidgetVariable:
            queue = &m_WidgetVariableQueue;
            break;
        case ClipboardObjectType::WidgetChild:
            queue = &m_WidgetChildQueue;
            break;
        }

        if (queue)
        {
            // 如果队列已满，移除最旧的元素
            if (queue->size() >= m_MaxQueueSize)
            {
                queue->pop();
            }
            queue->push(obj);
        }
    }

    // 辅助函数：获取指定队列的引用
    std::queue<ClipboardObject>* GetQueue(ClipboardObjectType type)
    {
        switch (type)
        {
        case ClipboardObjectType::BasicVariable:
            return &m_BasicVariableQueue;
        case ClipboardObjectType::ObjectVariable:
            return &m_ObjectVariableQueue;
        case ClipboardObjectType::WidgetVariable:
            return &m_WidgetVariableQueue;
        case ClipboardObjectType::WidgetChild:
            return &m_WidgetChildQueue;
        default:
            return nullptr;
        }
    }

    const std::queue<ClipboardObject>* GetQueue(ClipboardObjectType type) const
    {
        switch (type)
        {
        case ClipboardObjectType::BasicVariable:
            return &m_BasicVariableQueue;
        case ClipboardObjectType::ObjectVariable:
            return &m_ObjectVariableQueue;
        case ClipboardObjectType::WidgetVariable:
            return &m_WidgetVariableQueue;
        case ClipboardObjectType::WidgetChild:
            return &m_WidgetChildQueue;
        default:
            return nullptr;
        }
    }

public:
    explicit EditorClipboard(size_t maxQueueSize = 10)
        : m_MaxQueueSize(maxQueueSize)
    {
    }

    // ==================== 存入接口 ====================

    // 存入基本变量
    void PushBasicVariable(const nlohmann::json& jsonData, const std::string& varName = "")
    {
        AddToQueue(ClipboardObjectType::BasicVariable, ClipboardObject(ClipboardObjectType::BasicVariable, jsonData, varName));
    }

    // 存入Object变量
    void PushObjectVariable(const nlohmann::json& jsonData, const std::string& varName = "")
    {
        AddToQueue(ClipboardObjectType::ObjectVariable, ClipboardObject(ClipboardObjectType::ObjectVariable, jsonData, varName));
    }

    // 存入控件树变量
    void PushWidgetVariable(const nlohmann::json& jsonData, const std::string& varName = "")
    {
        AddToQueue(ClipboardObjectType::WidgetVariable, ClipboardObject(ClipboardObjectType::WidgetVariable, jsonData, varName));
    }

    // 存入控件子项
    void PushWidgetChild(const nlohmann::json& jsonData, const std::string& widgetPath = "")
    {
        AddToQueue(ClipboardObjectType::WidgetChild, ClipboardObject(ClipboardObjectType::WidgetChild, jsonData, widgetPath));
    }

    // ==================== 查询接口 ====================

    // 获取最新对象（指定类型）
    bool GetLatest(ClipboardObjectType type, nlohmann::json& outJson, std::string& outSourceName) const
    {
        const std::queue<ClipboardObject>* queue = GetQueue(type);
        if (!queue || queue->empty())
        {
            return false;
        }

        const ClipboardObject& obj = queue->back();
        outJson = obj.jsonData;
        outSourceName = obj.sourceName;
        return true;
    }

    // 获取所有对象（指定类型）
    std::vector<nlohmann::json> GetAll(ClipboardObjectType type) const
    {
        std::vector<nlohmann::json> result;
        const std::queue<ClipboardObject>* queue = GetQueue(type);
        if (!queue)
        {
            return result;
        }

        std::queue<ClipboardObject> tempQueue = *queue;
        while (!tempQueue.empty())
        {
            result.push_back(tempQueue.front().jsonData);
            tempQueue.pop();
        }
        return result;
    }

    // 获取队列大小
    size_t GetQueueSize(ClipboardObjectType type) const
    {
        const std::queue<ClipboardObject>* queue = GetQueue(type);
        return queue ? queue->size() : 0;
    }

    // 检查队列是否为空
    bool IsEmpty(ClipboardObjectType type) const
    {
        return GetQueueSize(type) == 0;
    }

    // ==================== 清空接口 ====================

    // 清空指定类型的队列
    void Clear(ClipboardObjectType type)
    {
        std::queue<ClipboardObject>* queue = GetQueue(type);
        if (queue)
        {
            while (!queue->empty())
            {
                queue->pop();
            }
        }
    }

    // 清空所有队列
    void ClearAll()
    {
        Clear(ClipboardObjectType::BasicVariable);
        Clear(ClipboardObjectType::ObjectVariable);
        Clear(ClipboardObjectType::WidgetVariable);
        Clear(ClipboardObjectType::WidgetChild);
    }

    // ==================== 设置队列最大长度 ====================

    void SetMaxQueueSize(size_t maxSize)
    {
        m_MaxQueueSize = maxSize;
        // 如果当前队列超过新的大小，移除多余的元素
        TrimQueue(m_BasicVariableQueue);
        TrimQueue(m_ObjectVariableQueue);
        TrimQueue(m_WidgetVariableQueue);
        TrimQueue(m_WidgetChildQueue);
    }

    size_t GetMaxQueueSize() const
    {
        return m_MaxQueueSize;
    }

private:
    // 辅助函数：裁剪队列到最大长度
    void TrimQueue(std::queue<ClipboardObject>& queue)
    {
        while (queue.size() > m_MaxQueueSize)
        {
            queue.pop();
        }
    }
};
