#pragma once
#include <EditorKit/KEventBus.h>
#include <EditorKit/ActionSystem.h>

class EditorGlobal
{
private:
	EventBus<>* m_eventbus;
	ActionSystem<std::string>* m_ActionSystem;
	EditorGlobal():
		m_eventbus(new EventBus<>()),
		m_ActionSystem(new ActionSystem<std::string>)
	{}
	static EditorGlobal* GetInstance()
	{
		static EditorGlobal instance;
		return &instance;
	}

public:
	static EventBus<>* GetEventBusInstance()
	{
		return GetInstance()->m_eventbus;
	}
	static ActionSystem<std::string>* GetActionSystemInstance()
	{
		return GetInstance()->m_ActionSystem;
	}
};