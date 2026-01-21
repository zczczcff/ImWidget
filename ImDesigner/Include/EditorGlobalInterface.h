#pragma once
#include "EditorGlobal.h"
#include <unordered_set>


class EditorGlobalInterface
{
protected:
	EventBus<KeyStringType>* m_EditorEventbus;
	ActionSystem<KeyStringType>* m_ActionSystem;
	std::unordered_set<ActionID, ActionID::Hash> AllActionProcessor;
	std::unordered_set<EventID, EventID::Hash> AllSubscribedEvents;

	template<typename... Args>
	ActionResult ExecuteAction(const KeyStringType& actionKey, Args... args)
	{
		return m_ActionSystem->Execute(actionKey, args...);
	}

	template <typename Callable>
	EventID Subscribe(const KeyStringType& eventName, Callable&& handler,
		const std::string& description = std::string(), bool once = false)
	{
		EventID id = m_EditorEventbus->Subscribe(eventName, handler, description, once);
		AllSubscribedEvents.insert(id);
		return id;
	}

	template <typename... Args>
	PublishResult Publish(const KeyStringType& eventName, Args... args)
	{
		return m_EditorEventbus->Publish(eventName, args...);
	}

	template<typename Callable>
	ActionHandle<KeyStringType> AddSequentialProcessor(const KeyStringType& actionKey, Callable&& processor,
		const std::string& description = "", int priority = 0)
	{
		ActionID id = m_ActionSystem->AddSequentialProcessor(actionKey, processor, description, priority);
		AllActionProcessor.insert(id);
		return id;
	}

	template<typename Callable>
	ActionHandle<KeyStringType> AddValidator(const KeyStringType& actionKey, Callable&& validator,
		const std::string& description = "", int priority = 0)
	{
		ActionID id = m_ActionSystem->AddValidator(actionKey, validator, description, priority);
		AllActionProcessor.insert(id);
		return id;
	}

public:
	EditorGlobalInterface()
	{
		m_EditorEventbus = EditorGlobal::GetEventBusInstance();
		m_ActionSystem = EditorGlobal::GetActionSystemInstance();
	}
	virtual ~EditorGlobalInterface()
	{
		for (auto& id : AllActionProcessor)
		{
			m_ActionSystem->RemoveHandler(id);
		}

		for (auto& id : AllSubscribedEvents)
		{
			m_EditorEventbus->Unsubscribe(id);
		}
	}
};