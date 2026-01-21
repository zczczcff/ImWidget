#pragma once
#include <EditorKit/KEventBus.h>
#include <EditorKit/ActionSystem.h>
#include <EditorKit/StaticString.h>

using KeyStringType = StaticString;
using ActionID = ActionHandle<KeyStringType>;

class EditorGlobal
{
private:
	EventBus<KeyStringType>* m_eventbus;
	ActionSystem<KeyStringType>* m_ActionSystem;
	EditorGlobal():
		m_eventbus(new EventBus<KeyStringType>()),
		m_ActionSystem(new ActionSystem<KeyStringType>)
	{}
	static EditorGlobal* GetInstance()
	{
		static EditorGlobal instance;
		return &instance;
	}

public:
	static EventBus<KeyStringType>* GetEventBusInstance()
	{
		return GetInstance()->m_eventbus;
	}
	static ActionSystem<KeyStringType>* GetActionSystemInstance()
	{
		return GetInstance()->m_ActionSystem;
	}
};