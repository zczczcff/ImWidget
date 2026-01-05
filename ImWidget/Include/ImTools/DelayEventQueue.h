#include <functional>
#include <queue>
#include <vector>
#include <chrono>
#include <memory>
#include <cstdint>

class DelayedEventQueue
{
public:
    // 事件ID类型
    using EventID = uint64_t;

private:
    // 事件结构
    struct Event
    {
        enum class Type
        {
            Immediate,      // 立即执行
            DelayFrames,    // 延迟帧
            DelayTime       // 延迟时间
        };

        EventID id;                 // 事件ID
        Type type;                  // 事件类型
        std::function<void()> func; // 事件函数
        int64_t triggerFrame;       // 触发帧数（用于延迟帧）
        std::chrono::steady_clock::time_point triggerTime; // 触发时间点

        // 比较函数，用于优先级队列（触发时间早的优先级高）
        bool operator>(const Event& other) const
        {
            if (type == Type::DelayTime && other.type == Type::DelayTime)
            {
                return triggerTime > other.triggerTime;
            }
            else if (type == Type::DelayFrames && other.type == Type::DelayFrames)
            {
                return triggerFrame > other.triggerFrame;
            }
            else if (type == Type::Immediate)
            {
                return false; // 立即事件优先级最高
            }
            else if (other.type == Type::Immediate)
            {
                return true;
            }
            else
            {
                // 混合类型比较，将时间转换为帧数估算
                auto now = std::chrono::steady_clock::now();
                auto frameEstimate1 = type == Type::DelayFrames ?
                    triggerFrame : FrameFromTime(triggerTime, now);
                auto frameEstimate2 = other.type == Type::DelayFrames ?
                    other.triggerFrame : FrameFromTime(other.triggerTime, now);
                return frameEstimate1 > frameEstimate2;
            }
        }

    private:
        static int64_t FrameFromTime(
            std::chrono::steady_clock::time_point trigger,
            std::chrono::steady_clock::time_point now)
        {
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                trigger - now).count();
            return ms > 0 ? (ms + 15) / 16 : 0; // 假设60FPS，每帧约16.67ms
        }
    };

    // 优先级队列，小顶堆，触发时间早的在前
    std::priority_queue<Event, std::vector<Event>, std::greater<Event>> eventQueue_;

    // 当前帧数
    int64_t currentFrame_ = 0;

    // 下一事件ID
    EventID nextEventId_ = 1;

    // 待执行的立即事件
    std::vector<std::function<void()>> immediateEvents_;

    // 记录事件ID是否有效
    struct EventInfo
    {
        bool valid = true;
    };

public:
    // 每帧调用一次
    void Tick()
    {
        auto now = std::chrono::steady_clock::now();

        // 首先执行所有立即事件
        for (auto& event : immediateEvents_)
        {
            if (event)
            {
                event();
            }
        }
        immediateEvents_.clear();

        // 处理延迟事件
        while (!eventQueue_.empty())
        {
            const auto& topEvent = eventQueue_.top();

            bool shouldExecute = false;

            switch (topEvent.type)
            {
            case Event::Type::Immediate:
                shouldExecute = true;
                break;

            case Event::Type::DelayFrames:
                if (currentFrame_ >= topEvent.triggerFrame)
                {
                    shouldExecute = true;
                }
                break;

            case Event::Type::DelayTime:
                if (now >= topEvent.triggerTime)
                {
                    shouldExecute = true;
                }
                break;
            }

            if (!shouldExecute)
            {
                break; // 后续事件都未到触发时间
            }

            // 执行事件
            auto event = eventQueue_.top();
            eventQueue_.pop();

            if (event.func)
            {
                event.func();
            }
        }

        currentFrame_++;
    }

    // 添加立即事件（本帧结束执行）
    EventID AddImmediateEvent(std::function<void()> func)
    {
        EventID id = nextEventId_++;
        immediateEvents_.emplace_back(std::move(func));
        return id;
    }

    // 添加延迟帧事件
    EventID AddDelayedFrameEvent(std::function<void()> func, int delayFrames)
    {
        EventID id = nextEventId_++;

        Event event;
        event.id = id;
        event.type = Event::Type::DelayFrames;
        event.func = std::move(func);
        event.triggerFrame = currentFrame_ + delayFrames;

        eventQueue_.push(std::move(event));
        return id;
    }

    // 添加延迟时间事件
    EventID AddDelayedTimeEvent(std::function<void()> func, int64_t delayMillis)
    {
        EventID id = nextEventId_++;

        Event event;
        event.id = id;
        event.type = Event::Type::DelayTime;
        event.func = std::move(func);
        event.triggerTime = std::chrono::steady_clock::now() +
            std::chrono::milliseconds(delayMillis);

        eventQueue_.push(std::move(event));
        return id;
    }

    // 添加延迟事件（自动判断单位）
    EventID AddDelayedEvent(std::function<void()> func, int delay, bool isMilliseconds = false)
    {
        if (isMilliseconds)
        {
            return AddDelayedTimeEvent(std::move(func), delay);
        }
        else
        {
            return AddDelayedFrameEvent(std::move(func), delay);
        }
    }

    // 获取当前帧数
    int64_t GetCurrentFrame() const { return currentFrame_; }

    // 清除所有未执行事件
    void Clear()
    {
        // 清空优先级队列
        while (!eventQueue_.empty())
        {
            eventQueue_.pop();
        }
        immediateEvents_.clear();
    }

    // 获取队列大小
    size_t GetPendingEventCount() const
    {
        return eventQueue_.size() + immediateEvents_.size();
    }
};