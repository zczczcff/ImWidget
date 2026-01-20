#include "EditorObjectBase.h"

// 路径变更方法

bool EditorObjectBase::ChangePath(const std::string& newPath)
{
    if (newPath == m_Path)
    {
        return false; // 路径未改变
    }

    // 保存当前绑定信息用于重新绑定
    auto oldBindings = m_BindingInfos;

    // 取消所有之前的监听
    UnbindAllListeners();
    m_BindingInfos.clear();

    // 更新路径
    std::string oldPath = m_Path;
    m_Path = newPath;

    // 重新绑定所有属性
    for (const auto& binding : oldBindings)
    {
        if (binding.isRelative)
        {
            // 重新绑定相对路径
            switch (binding.type)
            {
            case BindingInfo::ValueType::INT:
                BindIntValueRelative(binding.relativePath, static_cast<int*>(binding.valuePtr));
                break;
            case BindingInfo::ValueType::BOOL:
                BindBoolValueRelative(binding.relativePath, static_cast<bool*>(binding.valuePtr));
                break;
            case BindingInfo::ValueType::FLOAT:
                BindFloatValueRelative(binding.relativePath, static_cast<float*>(binding.valuePtr));
                break;
            case BindingInfo::ValueType::STRING:
                BindStringValueRelative(binding.relativePath, static_cast<std::string*>(binding.valuePtr));
                break;
            }
        }
        else
        {
            // 重新绑定全局路径
            switch (binding.type)
            {
            case BindingInfo::ValueType::INT:
                BindIntValueGlobal(binding.path, static_cast<int*>(binding.valuePtr));
                break;
            case BindingInfo::ValueType::BOOL:
                BindBoolValueGlobal(binding.path, static_cast<bool*>(binding.valuePtr));
                break;
            case BindingInfo::ValueType::FLOAT:
                BindFloatValueGlobal(binding.path, static_cast<float*>(binding.valuePtr));
                break;
            case BindingInfo::ValueType::STRING:
                BindStringValueGlobal(binding.path, static_cast<std::string*>(binding.valuePtr));
                break;
            }
        }
    }

    // 重新绑定监听器
    auto oldWatchInfos = m_WatchInfos;
    for (const auto& watchInfo : oldWatchInfos)
    {
        if (watchInfo.isRelative)
        {
            // 重新绑定相对路径监听器
            switch (watchInfo.type)
            {
            case WatchInfo::ValueType::INT:
                WatchIntRelative(watchInfo.relativePath, watchInfo.intCallback);
                break;
            case WatchInfo::ValueType::FLOAT:
                WatchFloatRelative(watchInfo.relativePath, watchInfo.floatCallback);
                break;
            case WatchInfo::ValueType::BOOL:
                WatchBoolRelative(watchInfo.relativePath, watchInfo.boolCallback);
                break;
            case WatchInfo::ValueType::STRING:
                WatchStringRelative(watchInfo.relativePath, watchInfo.stringCallback);
                break;
            case WatchInfo::ValueType::POINTER:
                WatchPointerRelative(watchInfo.relativePath, watchInfo.pointerCallback);
                break;
            }
        }
        else
        {
            // 重新绑定绝对路径监听器（路径不变）
            switch (watchInfo.type)
            {
            case WatchInfo::ValueType::INT:
                WatchIntGlobal(watchInfo.path, watchInfo.intCallback);
                break;
            case WatchInfo::ValueType::FLOAT:
                WatchFloatGlobal(watchInfo.path, watchInfo.floatCallback);
                break;
            case WatchInfo::ValueType::BOOL:
                WatchBoolGlobal(watchInfo.path, watchInfo.boolCallback);
                break;
            case WatchInfo::ValueType::STRING:
                WatchStringGlobal(watchInfo.path, watchInfo.stringCallback);
                break;
            case WatchInfo::ValueType::POINTER:
                WatchPointerGlobal(watchInfo.path, watchInfo.pointerCallback);
                break;
            }
        }
    }

    return true;
}

// 绑定整数值（相对路径）

bool EditorObjectBase::BindIntValueRelative(const std::string& RelativePath, int* value_Ptr)
{
    std::string fullPath = m_Path.empty() ? RelativePath : m_Path + "/" + RelativePath;
    BindList.push_back(fullPath);

    // 记录绑定信息
    BindingInfo info;
    info.path = fullPath;
    info.valuePtr = value_Ptr;
    info.type = BindingInfo::ValueType::INT;
    info.isRelative = true;
    info.relativePath = RelativePath;
    m_BindingInfos.push_back(info);

    ListenerId addListener = m_StateSystem->addEventListener(fullPath, ListenGranularity::NODE, EventType::ADD,
        [value_Ptr](const PathEvent& event)
        {
            if (auto node = event.node->AsIntNode())
            {
                (*value_Ptr) = node->getValue();
            }
        });

    ListenerId updateListener = m_StateSystem->addEventListener(fullPath, ListenGranularity::NODE, EventType::UPDATE,
        [value_Ptr](const PathEvent& event)
        {
            if (auto node = event.node->AsIntNode())
            {
                (*value_Ptr) = node->getValue();
            }
        });

    // 存储监听器ID
    m_ListenerMap[fullPath].push_back(addListener);
    m_ListenerMap[fullPath].push_back(updateListener);

    // 立即获取当前值
    if (auto node = m_StateSystem->getNode(fullPath))
    {
        if (auto intNode = node->AsIntNode())
        {
            (*value_Ptr) = intNode->getValue();
        }
    }

    return true;
}

// 绑定整数值（全局路径）

bool EditorObjectBase::BindIntValueGlobal(const std::string& GlobalPath, int* value_Ptr)
{
    BindList.push_back(GlobalPath);

    // 记录绑定信息
    BindingInfo info;
    info.path = GlobalPath;
    info.valuePtr = value_Ptr;
    info.type = BindingInfo::ValueType::INT;
    info.isRelative = false;
    m_BindingInfos.push_back(info);

    ListenerId addListener = m_StateSystem->addEventListener(GlobalPath, ListenGranularity::NODE, EventType::ADD,
        [value_Ptr](const PathEvent& event)
        {
            if (auto node = event.node->AsIntNode())
            {
                (*value_Ptr) = node->getValue();
            }
        });

    ListenerId updateListener = m_StateSystem->addEventListener(GlobalPath, ListenGranularity::NODE, EventType::UPDATE,
        [value_Ptr](const PathEvent& event)
        {
            if (auto node = event.node->AsIntNode())
            {
                (*value_Ptr) = node->getValue();
            }
        });

    // 存储监听器ID
    m_ListenerMap[GlobalPath].push_back(addListener);
    m_ListenerMap[GlobalPath].push_back(updateListener);

    // 立即获取当前值
    if (auto node = m_StateSystem->getNode(GlobalPath))
    {
        if (auto intNode = node->AsIntNode())
        {
            (*value_Ptr) = intNode->getValue();
        }
    }

    return true;
}

// 绑定布尔值（相对路径）

bool EditorObjectBase::BindBoolValueRelative(const std::string& RelativePath, bool* value_Ptr)
{
    std::string fullPath = m_Path.empty() ? RelativePath : m_Path + "/" + RelativePath;
    BindList.push_back(fullPath);

    // 记录绑定信息
    BindingInfo info;
    info.path = fullPath;
    info.valuePtr = value_Ptr;
    info.type = BindingInfo::ValueType::BOOL;
    info.isRelative = true;
    info.relativePath = RelativePath;
    m_BindingInfos.push_back(info);

    ListenerId addListener = m_StateSystem->addEventListener(fullPath, ListenGranularity::NODE, EventType::ADD,
        [value_Ptr](const PathEvent& event)
        {
            if (auto node = event.node->AsBoolNode())
            {
                (*value_Ptr) = node->getValue();
            }
        });

    ListenerId updateListener = m_StateSystem->addEventListener(fullPath, ListenGranularity::NODE, EventType::UPDATE,
        [value_Ptr](const PathEvent& event)
        {
            if (auto node = event.node->AsBoolNode())
            {
                (*value_Ptr) = node->getValue();
            }
        });

    // 存储监听器ID
    m_ListenerMap[fullPath].push_back(addListener);
    m_ListenerMap[fullPath].push_back(updateListener);

    // 立即获取当前值
    if (auto node = m_StateSystem->getNode(fullPath))
    {
        if (auto boolNode = node->AsBoolNode())
        {
            (*value_Ptr) = boolNode->getValue();
        }
    }

    return true;
}

// 绑定布尔值（全局路径）

bool EditorObjectBase::BindBoolValueGlobal(const std::string& GlobalPath, bool* value_Ptr)
{
    BindList.push_back(GlobalPath);

    // 记录绑定信息
    BindingInfo info;
    info.path = GlobalPath;
    info.valuePtr = value_Ptr;
    info.type = BindingInfo::ValueType::BOOL;
    info.isRelative = false;
    m_BindingInfos.push_back(info);

    ListenerId addListener = m_StateSystem->addEventListener(GlobalPath, ListenGranularity::NODE, EventType::ADD,
        [value_Ptr](const PathEvent& event)
        {
            if (auto node = event.node->AsBoolNode())
            {
                (*value_Ptr) = node->getValue();
            }
        });

    ListenerId updateListener = m_StateSystem->addEventListener(GlobalPath, ListenGranularity::NODE, EventType::UPDATE,
        [value_Ptr](const PathEvent& event)
        {
            if (auto node = event.node->AsBoolNode())
            {
                (*value_Ptr) = node->getValue();
            }
        });

    // 存储监听器ID
    m_ListenerMap[GlobalPath].push_back(addListener);
    m_ListenerMap[GlobalPath].push_back(updateListener);

    // 立即获取当前值
    if (auto node = m_StateSystem->getNode(GlobalPath))
    {
        if (auto boolNode = node->AsBoolNode())
        {
            (*value_Ptr) = boolNode->getValue();
        }
    }

    return true;
}

// 绑定字符串（相对路径）

bool EditorObjectBase::BindStringValueRelative(const std::string& RelativePath, std::string* value_Ptr)
{
    std::string fullPath = m_Path.empty() ? RelativePath : m_Path + "/" + RelativePath;
    BindList.push_back(fullPath);

    // 记录绑定信息
    BindingInfo info;
    info.path = fullPath;
    info.valuePtr = value_Ptr;
    info.type = BindingInfo::ValueType::STRING;
    info.isRelative = true;
    info.relativePath = RelativePath;
    m_BindingInfos.push_back(info);

    ListenerId addListener = m_StateSystem->addEventListener(fullPath, ListenGranularity::NODE, EventType::ADD,
        [value_Ptr](const PathEvent& event)
        {
            if (auto node = event.node->AsStringNode())
            {
                (*value_Ptr) = node->getValue();
            }
        });

    ListenerId updateListener = m_StateSystem->addEventListener(fullPath, ListenGranularity::NODE, EventType::UPDATE,
        [value_Ptr](const PathEvent& event)
        {
            if (auto node = event.node->AsStringNode())
            {
                (*value_Ptr) = node->getValue();
            }
        });

    // 存储监听器ID
    m_ListenerMap[fullPath].push_back(addListener);
    m_ListenerMap[fullPath].push_back(updateListener);

    // 立即获取当前值
    if (auto node = m_StateSystem->getNode(fullPath))
    {
        if (auto stringNode = node->AsStringNode())
        {
            (*value_Ptr) = stringNode->getValue();
        }
    }

    return true;
}

// 绑定字符串（全局路径）

bool EditorObjectBase::BindStringValueGlobal(const std::string& GlobalPath, std::string* value_Ptr)
{
    BindList.push_back(GlobalPath);

    // 记录绑定信息
    BindingInfo info;
    info.path = GlobalPath;
    info.valuePtr = value_Ptr;
    info.type = BindingInfo::ValueType::STRING;
    info.isRelative = false;
    m_BindingInfos.push_back(info);

    ListenerId addListener = m_StateSystem->addEventListener(GlobalPath, ListenGranularity::NODE, EventType::ADD,
        [value_Ptr](const PathEvent& event)
        {
            if (auto node = event.node->AsStringNode())
            {
                (*value_Ptr) = node->getValue();
            }
        });

    ListenerId updateListener = m_StateSystem->addEventListener(GlobalPath, ListenGranularity::NODE, EventType::UPDATE,
        [value_Ptr](const PathEvent& event)
        {
            if (auto node = event.node->AsStringNode())
            {
                (*value_Ptr) = node->getValue();
            }
        });

    // 存储监听器ID
    m_ListenerMap[GlobalPath].push_back(addListener);
    m_ListenerMap[GlobalPath].push_back(updateListener);

    // 立即获取当前值
    if (auto node = m_StateSystem->getNode(GlobalPath))
    {
        if (auto stringNode = node->AsStringNode())
        {
            (*value_Ptr) = stringNode->getValue();
        }
    }

    return true;
}

// 绑定浮点数（相对路径）

bool EditorObjectBase::BindFloatValueRelative(const std::string& RelativePath, float* value_Ptr)
{
    std::string fullPath = m_Path.empty() ? RelativePath : m_Path + "/" + RelativePath;
    BindList.push_back(fullPath);

    // 记录绑定信息
    BindingInfo info;
    info.path = fullPath;
    info.valuePtr = value_Ptr;
    info.type = BindingInfo::ValueType::FLOAT;
    info.isRelative = true;
    info.relativePath = RelativePath;
    m_BindingInfos.push_back(info);

    ListenerId addListener = m_StateSystem->addEventListener(fullPath, ListenGranularity::NODE, EventType::ADD,
        [value_Ptr](const PathEvent& event)
        {
            if (auto node = event.node->AsFloatNode())
            {
                (*value_Ptr) = node->getValue();
            }
        });

    ListenerId updateListener = m_StateSystem->addEventListener(fullPath, ListenGranularity::NODE, EventType::UPDATE,
        [value_Ptr](const PathEvent& event)
        {
            if (auto node = event.node->AsFloatNode())
            {
                (*value_Ptr) = node->getValue();
            }
        });

    // 存储监听器ID
    m_ListenerMap[fullPath].push_back(addListener);
    m_ListenerMap[fullPath].push_back(updateListener);

    // 立即获取当前值
    if (auto node = m_StateSystem->getNode(fullPath))
    {
        if (auto floatNode = node->AsFloatNode())
        {
            (*value_Ptr) = floatNode->getValue();
        }
    }

    return true;
}

// 绑定浮点数（全局路径）

bool EditorObjectBase::BindFloatValueGlobal(const std::string& GlobalPath, float* value_Ptr)
{
    BindList.push_back(GlobalPath);

    // 记录绑定信息
    BindingInfo info;
    info.path = GlobalPath;
    info.valuePtr = value_Ptr;
    info.type = BindingInfo::ValueType::FLOAT;
    info.isRelative = false;
    m_BindingInfos.push_back(info);

    ListenerId addListener = m_StateSystem->addEventListener(GlobalPath, ListenGranularity::NODE, EventType::ADD,
        [value_Ptr](const PathEvent& event)
        {
            if (auto node = event.node->AsFloatNode())
            {
                (*value_Ptr) = node->getValue();
            }
        });

    ListenerId updateListener = m_StateSystem->addEventListener(GlobalPath, ListenGranularity::NODE, EventType::UPDATE,
        [value_Ptr](const PathEvent& event)
        {
            if (auto node = event.node->AsFloatNode())
            {
                (*value_Ptr) = node->getValue();
            }
        });

    // 存储监听器ID
    m_ListenerMap[GlobalPath].push_back(addListener);
    m_ListenerMap[GlobalPath].push_back(updateListener);

    // 立即获取当前值
    if (auto node = m_StateSystem->getNode(GlobalPath))
    {
        if (auto floatNode = node->AsFloatNode())
        {
            (*value_Ptr) = floatNode->getValue();
        }
    }

    return true;
}

// 取消所有绑定

void EditorObjectBase::UnbindAll()
{
    UnbindAllListeners();
    m_BindingInfos.clear();
    BindList.clear();
}

// 取消所有监听

void EditorObjectBase::UnbindAllListeners()
{
    for (auto& pair : m_ListenerMap)
    {
        for (ListenerId id : pair.second)
        {
            m_StateSystem->removeEventListener(id);
        }
    }
    m_ListenerMap.clear();
    BindList.clear();

    for (auto& pair : m_WatchListenerMap)
    {
        for (ListenerId id : pair.second)
        {
            m_StateSystem->removeEventListener(id);
        }
    }
    m_WatchListenerMap.clear();
    m_WatchInfos.clear();
}

// 监听整型值变化 - 相对路径

bool EditorObjectBase::WatchIntRelative(const std::string& relativePath, std::function<void(int)> callback)
{
    std::string fullPath = m_Path.empty() ? relativePath : m_Path + "/" + relativePath;
    return WatchIntInternal(fullPath, callback, true, relativePath);
}

// 监听整型值变化 - 绝对路径

bool EditorObjectBase::WatchIntGlobal(const std::string& globalPath, std::function<void(int)> callback)
{
    return WatchIntInternal(globalPath, callback, false, "");
}

// 监听浮点值变化 - 相对路径

bool EditorObjectBase::WatchFloatRelative(const std::string& relativePath, std::function<void(float)> callback)
{
    std::string fullPath = m_Path.empty() ? relativePath : m_Path + "/" + relativePath;
    return WatchFloatInternal(fullPath, callback, true, relativePath);
}

// 监听浮点值变化 - 绝对路径

bool EditorObjectBase::WatchFloatGlobal(const std::string& globalPath, std::function<void(float)> callback)
{
    return WatchFloatInternal(globalPath, callback, false, "");
}

// 监听布尔值变化 - 相对路径

bool EditorObjectBase::WatchBoolRelative(const std::string& relativePath, std::function<void(bool)> callback)
{
    std::string fullPath = m_Path.empty() ? relativePath : m_Path + "/" + relativePath;
    return WatchBoolInternal(fullPath, callback, true, relativePath);
}

// 监听布尔值变化 - 绝对路径

bool EditorObjectBase::WatchBoolGlobal(const std::string& globalPath, std::function<void(bool)> callback)
{
    return WatchBoolInternal(globalPath, callback, false, "");
}

// 监听字符串变化 - 相对路径

bool EditorObjectBase::WatchStringRelative(const std::string& relativePath, std::function<void(const std::string&)> callback)
{
    std::string fullPath = m_Path.empty() ? relativePath : m_Path + "/" + relativePath;
    return WatchStringInternal(fullPath, callback, true, relativePath);
}

// 监听字符串变化 - 绝对路径

bool EditorObjectBase::WatchStringGlobal(const std::string& globalPath, std::function<void(const std::string&)> callback)
{
    return WatchStringInternal(globalPath, callback, false, "");
}

// 监听指针变化 - 相对路径

bool EditorObjectBase::WatchPointerRelative(const std::string& relativePath, std::function<void(void*)> callback)
{
    std::string fullPath = m_Path.empty() ? relativePath : m_Path + "/" + relativePath;
    return WatchPointerInternal(fullPath, callback, true, relativePath);
}

// 监听指针变化 - 绝对路径

bool EditorObjectBase::WatchPointerGlobal(const std::string& globalPath, std::function<void(void*)> callback)
{
    return WatchPointerInternal(globalPath, callback, false, "");
}

// 移除特定路径的所有监听器

void EditorObjectBase::UnwatchPath(const std::string& path)
{
    auto it = m_WatchListenerMap.find(path);
    if (it != m_WatchListenerMap.end())
    {
        for (ListenerId id : it->second)
        {
            m_StateSystem->removeEventListener(id);
        }
        m_WatchListenerMap.erase(it);
    }

    // 同时从回调信息列表中移除
    m_WatchInfos.erase(
        std::remove_if(m_WatchInfos.begin(), m_WatchInfos.end(),
            [&path](const WatchInfo& info) { return info.path == path; }),
        m_WatchInfos.end());
}

// 内部监听实现方法

bool EditorObjectBase::WatchIntInternal(const std::string& fullPath, std::function<void(int)> callback, bool isRelative, const std::string& relativePath)
{
    // 存储监听信息
    WatchInfo info;
    info.path = fullPath;
    info.isRelative = isRelative;
    info.relativePath = relativePath;
    info.type = WatchInfo::ValueType::INT;
    info.intCallback = callback;
    m_WatchInfos.push_back(info);

    // 添加事件监听器
    ListenerId addListener = m_StateSystem->addEventListener(fullPath,
        ListenGranularity::NODE, EventType::ADD,
        [callback](const PathEvent& event)
        {
            if (auto node = event.node->AsIntNode())
            {
                callback(node->getValue());
            }
        });

    ListenerId updateListener = m_StateSystem->addEventListener(fullPath,
        ListenGranularity::NODE, EventType::UPDATE,
        [callback](const PathEvent& event)
        {
            if (auto node = event.node->AsIntNode())
            {
                callback(node->getValue());
            }
        });

    // 存储监听器ID
    m_WatchListenerMap[fullPath].push_back(addListener);
    m_WatchListenerMap[fullPath].push_back(updateListener);

    // 立即获取当前值并触发回调
    if (auto node = m_StateSystem->getNode(fullPath))
    {
        if (auto intNode = node->AsIntNode())
        {
            callback(intNode->getValue());
        }
    }

    return true;
}

bool EditorObjectBase::WatchFloatInternal(const std::string& fullPath, std::function<void(float)> callback, bool isRelative, const std::string& relativePath)
{
    WatchInfo info;
    info.path = fullPath;
    info.isRelative = isRelative;
    info.relativePath = relativePath;
    info.type = WatchInfo::ValueType::FLOAT;
    info.floatCallback = callback;
    m_WatchInfos.push_back(info);

    ListenerId addListener = m_StateSystem->addEventListener(fullPath,
        ListenGranularity::NODE, EventType::ADD,
        [callback](const PathEvent& event)
        {
            if (auto node = event.node->AsFloatNode())
            {
                callback(node->getValue());
            }
        });

    ListenerId updateListener = m_StateSystem->addEventListener(fullPath,
        ListenGranularity::NODE, EventType::UPDATE,
        [callback](const PathEvent& event)
        {
            if (auto node = event.node->AsFloatNode())
            {
                callback(node->getValue());
            }
        });

    m_WatchListenerMap[fullPath].push_back(addListener);
    m_WatchListenerMap[fullPath].push_back(updateListener);

    if (auto node = m_StateSystem->getNode(fullPath))
    {
        if (auto floatNode = node->AsFloatNode())
        {
            callback(floatNode->getValue());
        }
    }

    return true;
}

bool EditorObjectBase::WatchBoolInternal(const std::string& fullPath, std::function<void(bool)> callback, bool isRelative, const std::string& relativePath)
{
    WatchInfo info;
    info.path = fullPath;
    info.isRelative = isRelative;
    info.relativePath = relativePath;
    info.type = WatchInfo::ValueType::BOOL;
    info.boolCallback = callback;
    m_WatchInfos.push_back(info);

    ListenerId addListener = m_StateSystem->addEventListener(fullPath,
        ListenGranularity::NODE, EventType::ADD,
        [callback](const PathEvent& event)
        {
            if (auto node = event.node->AsBoolNode())
            {
                callback(node->getValue());
            }
        });

    ListenerId updateListener = m_StateSystem->addEventListener(fullPath,
        ListenGranularity::NODE, EventType::UPDATE,
        [callback](const PathEvent& event)
        {
            if (auto node = event.node->AsBoolNode())
            {
                callback(node->getValue());
            }
        });

    m_WatchListenerMap[fullPath].push_back(addListener);
    m_WatchListenerMap[fullPath].push_back(updateListener);

    if (auto node = m_StateSystem->getNode(fullPath))
    {
        if (auto boolNode = node->AsBoolNode())
        {
            callback(boolNode->getValue());
        }
    }

    return true;
}

bool EditorObjectBase::WatchStringInternal(const std::string& fullPath, std::function<void(const std::string&)> callback, bool isRelative, const std::string& relativePath)
{
    WatchInfo info;
    info.path = fullPath;
    info.isRelative = isRelative;
    info.relativePath = relativePath;
    info.type = WatchInfo::ValueType::STRING;
    info.stringCallback = callback;
    m_WatchInfos.push_back(info);

    ListenerId addListener = m_StateSystem->addEventListener(fullPath,
        ListenGranularity::NODE, EventType::ADD,
        [callback](const PathEvent& event)
        {
            if (auto node = event.node->AsStringNode())
            {
                callback(node->getValue());
            }
        });

    ListenerId updateListener = m_StateSystem->addEventListener(fullPath,
        ListenGranularity::NODE, EventType::UPDATE,
        [callback](const PathEvent& event)
        {
            if (auto node = event.node->AsStringNode())
            {
                callback(node->getValue());
            }
        });

    m_WatchListenerMap[fullPath].push_back(addListener);
    m_WatchListenerMap[fullPath].push_back(updateListener);

    if (auto node = m_StateSystem->getNode(fullPath))
    {
        if (auto stringNode = node->AsStringNode())
        {
            callback(stringNode->getValue());
        }
    }

    return true;
}

bool EditorObjectBase::WatchPointerInternal(const std::string& fullPath, std::function<void(void*)> callback, bool isRelative, const std::string& relativePath)
{
    WatchInfo info;
    info.path = fullPath;
    info.isRelative = isRelative;
    info.relativePath = relativePath;
    info.type = WatchInfo::ValueType::POINTER;
    info.pointerCallback = callback;
    m_WatchInfos.push_back(info);

    ListenerId addListener = m_StateSystem->addEventListener(fullPath,
        ListenGranularity::NODE, EventType::ADD,
        [callback](const PathEvent& event)
        {
            if (auto node = event.node->AsPointerNode())
            {
                callback(node->getValue());
            }
        });

    ListenerId updateListener = m_StateSystem->addEventListener(fullPath,
        ListenGranularity::NODE, EventType::UPDATE,
        [callback](const PathEvent& event)
        {
            if (auto node = event.node->AsPointerNode())
            {
                callback(node->getValue());
            }
        });

    m_WatchListenerMap[fullPath].push_back(addListener);
    m_WatchListenerMap[fullPath].push_back(updateListener);

    if (auto node = m_StateSystem->getNode(fullPath))
    {
        if (auto pointerNode = node->AsPointerNode())
        {
            callback(pointerNode->getValue());
        }
    }

    return true;
}
