#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <memory>
#include <algorithm>
#include <typeindex>

// 事件总线类模板
template <typename... Args>
class EventBus
{
public:
	using Handler = std::function<void(Args...)>;
	using HandlerInfo = std::pair<std::string, Handler>;
	using HandlerList = std::vector<HandlerInfo>;
	using HandlerMap = std::unordered_map<std::string, HandlerList>;

	// 订阅事件
	std::string subscribe(const std::string& eventName, Handler handler)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		std::string token = generateToken(eventName);
		handlers_[eventName].emplace_back(token, handler);
		return token;
	}

	// 取消订阅
	bool unsubscribe(const std::string& token)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		for (auto& pair : handlers_)
		{
			auto& handlers = pair.second;
			auto it = std::find_if(handlers.begin(), handlers.end(),
				[&](const HandlerInfo& info) { return info.first == token; });

			if (it != handlers.end())
			{
				handlers.erase(it);
				return true;
			}
		}
		return false;
	}

	// 发布事件
	void publish(const std::string& eventName, Args... args)
	{
		HandlerList handlers;
		{
			std::lock_guard<std::mutex> lock(mutex_);
			auto it = handlers_.find(eventName);
			if (it != handlers_.end())
			{
				handlers = it->second;
			}
		}

		for (auto& handlerInfo : handlers)
		{
			handlerInfo.second(args...);
		}
	}

private:
	// 生成唯一令牌
	std::string generateToken(const std::string& eventName)
	{
		static size_t counter = 0;
		return eventName + "_" + std::to_string(++counter);
	}

	HandlerMap handlers_;
	std::mutex mutex_;
};

// 全局事件总线管理器
class GlobalEventBus
{
public:
	template <typename... Args>
	static EventBus<Args...>& instance()
	{
		static EventBus<Args...> bus;
		return bus;
	}
};

template <typename... Args>
std::string SubscribeEvent(const std::string& eventName, std::function<void(Args...)> handler)
{
	return GlobalEventBus::instance<Args...>().subscribe(eventName, handler);
}

template <typename... Args>
void PublishEvent(const std::string& eventName, Args... args)
{
	GlobalEventBus::instance<Args...>().publish(eventName, args...);
}

template <typename... Args>
bool UnsubscribeEvent(const std::string& token)
{
	return GlobalEventBus::instance<Args...>().unsubscribe(token);
}

