#pragma once
#include "EditorGlobal.h"
#include <unordered_set>


class EditorEventObject
{
protected:
	ActionSystem<KeyStringType>* m_ActionSystem;
	std::unordered_set<EditorActionID, EditorActionID::Hash> AllActionProcessor;
	std::unordered_set<EditorEventID, EditorEventID::Hash> AllSubscribedEvents;

	template<typename... Args>
	ActionResult ExecuteAction(const KeyStringType& actionKey, Args&&... args)
	{
		return m_ActionSystem->Execute(actionKey, std::forward<Args>(args)...);
	}

	template <typename Callable>
	EditorEventID Subscribe(const KeyStringType& eventName, Callable&& handler,
		const std::string& description = std::string(), int priority = 0)
	{
		EditorEventID id = m_ActionSystem->AddSequentialProcessor(eventName, handler, description, priority);
		AllSubscribedEvents.insert(id);
		return id;
	}

	template <typename... Args>
	ActionResult Publish(const KeyStringType& eventName, Args&&... args)
	{
		return m_ActionSystem->Execute(eventName, std::forward<Args>(args)...);
	}

	bool Unsubscribe(const EditorEventID& id)
	{
		bool success = true;
		success &= AllSubscribedEvents.erase(id);
		success &= m_ActionSystem->RemoveHandler(id);
		return success;
	}

	//添加Action顺序处理器
	template<typename Callable>
	ActionHandle<KeyStringType> AddSequentialProcessor(const KeyStringType& actionKey, Callable&& processor,
		const std::string& description = "", int priority = 0)
	{
		EditorActionID id = m_ActionSystem->AddSequentialProcessor(actionKey, processor, description, priority);
		AllActionProcessor.insert(id);
		return id;
	}

	//添加Action验证处理器
	template<typename Callable>
	ActionHandle<KeyStringType> AddValidator(const KeyStringType& actionKey, Callable&& validator,
		const std::string& description = "", int priority = 0)
	{
		EditorActionID id = m_ActionSystem->AddValidator(actionKey, validator, description, priority);
		AllActionProcessor.insert(id);
		return id;
	}

	bool RemoveProcessor(const EditorActionID& ProcessorID)
	{
		bool success = true;
		success &= AllActionProcessor.erase(ProcessorID);
		success &= m_ActionSystem->RemoveHandler(ProcessorID);
		return success;
	}

public:
	EditorEventObject()
	{
		m_ActionSystem = EditorGlobal::GetActionSystemInstance();
	}
	virtual ~EditorEventObject()
	{
		for (auto& id : AllActionProcessor)
		{
			m_ActionSystem->RemoveHandler(id);
		}

		for (auto& id : AllSubscribedEvents)
		{
			m_ActionSystem->RemoveHandler(id);
		}
	}
};