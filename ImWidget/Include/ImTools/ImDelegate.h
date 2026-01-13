#pragma once
#include <functional>
#include <vector>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <atomic>

template<typename... Args>
class ImMulticastDelegate
{
private:
    // 委托句柄类型
    using HandleType = uint64_t;

    // 委托项结构
    struct DelegateItem
    {
        HandleType handle;
        std::function<void(Args...)> callback;

        DelegateItem(HandleType h, std::function<void(Args...)> cb)
            : handle(h), callback(std::move(cb))
        {
        }
    };

    std::vector<DelegateItem> delegates;
    std::unordered_map<HandleType, size_t> handleToIndex;
    std::atomic<HandleType> nextHandle{ 1 };

    // 获取唯一句柄
    HandleType GetNextHandle()
    {
        return nextHandle++;
    }

    // 清理被移除的委托
    void CompactDelegates()
    {
        std::vector<DelegateItem> validDelegates;
        validDelegates.reserve(delegates.size());

        for (const auto& item : delegates)
        {
            if (item.callback)
            {
                validDelegates.push_back(item);
            }
        }

        delegates.swap(validDelegates);

        // 重新构建索引映射
        handleToIndex.clear();
        for (size_t i = 0; i < delegates.size(); ++i)
        {
            handleToIndex[delegates[i].handle] = i;
        }
    }

public:
    ImMulticastDelegate() = default;

    // 禁止拷贝
    ImMulticastDelegate(const ImMulticastDelegate&) = delete;
    ImMulticastDelegate& operator=(const ImMulticastDelegate&) = delete;

    // 允许移动
    ImMulticastDelegate(ImMulticastDelegate&& other) noexcept
        : delegates(std::move(other.delegates))
        , handleToIndex(std::move(other.handleToIndex))
        , nextHandle(other.nextHandle.load())
    {
    }

    ImMulticastDelegate& operator=(ImMulticastDelegate&& other) noexcept
    {
        if (this != &other)
        {
            delegates = std::move(other.delegates);
            handleToIndex = std::move(other.handleToIndex);
            nextHandle = other.nextHandle.load();
        }
        return *this;
    }

    // 添加委托
    template<typename Callable>
    HandleType Add(Callable&& callable)
    {
        HandleType handle = GetNextHandle();
        delegates.emplace_back(handle, std::forward<Callable>(callable));
        handleToIndex[handle] = delegates.size() - 1;
        return handle;
    }

    // 移除委托
    bool Remove(HandleType handle)
    {
        auto it = handleToIndex.find(handle);
        if (it != handleToIndex.end())
        {
            size_t index = it->second;
            if (index < delegates.size())
            {
                // 标记为已移除（设置为null函数对象）
                delegates[index].callback = nullptr;
                handleToIndex.erase(it);
                return true;
            }
        }
        return false;
    }

    // 添加快速转发到另一个同类型委托
    HandleType AddForward(ImMulticastDelegate<Args...>& targetDelegate)
    {
        return Add([&targetDelegate](Args... args)
            {
                targetDelegate.Broadcast(std::forward<Args>(args)...);
            });
    }

    // 添加线程安全的快速转发
    HandleType AddForwardThreadSafe(ImMulticastDelegate<Args...>& targetDelegate)
    {
        return Add([&targetDelegate](Args... args)
            {
                targetDelegate.BroadcastThreadSafe(std::forward<Args>(args)...);
            });
    }

    // 清除所有委托
    void Clear()
    {
        delegates.clear();
        handleToIndex.clear();
    }

    // 触发所有委托
    void Broadcast(Args... args) const
    {
        // 创建副本以防止在回调中修改委托列表
        std::vector<std::function<void(Args...)>> callbacks;
        callbacks.reserve(delegates.size());

        for (const auto& item : delegates)
        {
            if (item.callback)
            {
                callbacks.push_back(item.callback);
            }
        }

        // 依次调用所有有效委托
        for (const auto& callback : callbacks)
        {
            if (callback)
            {
                callback(args...);
            }
        }
    }

    // 线程安全版本的广播（需要额外的同步机制）
    void BroadcastThreadSafe(Args... args) const
    {
        std::vector<std::function<void(Args...)>> callbacks;
        callbacks.reserve(delegates.size());

        // 复制回调函数（线程安全）
        {
            // 这里假设有外部同步或使用互斥锁
            for (const auto& item : delegates)
            {
                if (item.callback)
                {
                    callbacks.push_back(item.callback);
                }
            }
        }

        // 执行回调
        for (const auto& callback : callbacks)
        {
            if (callback)
            {
                try
                {
                    callback(args...);
                }
                catch (...)
                {
                    // 防止单个委托异常影响其他委托
                }
            }
        }
    }

    // 获取委托数量
    size_t Size() const
    {
        return std::count_if(delegates.begin(), delegates.end(),
            [](const DelegateItem& item) { return static_cast<bool>(item.callback); });
    }

    // 是否为空
    bool Empty() const
    {
        return Size() == 0;
    }

    // 优化内存使用
    void ShrinkToFit()
    {
        CompactDelegates();
        delegates.shrink_to_fit();
    }
};

template<typename... Args>
using ImMultiDelegate = ImMulticastDelegate<Args...>;