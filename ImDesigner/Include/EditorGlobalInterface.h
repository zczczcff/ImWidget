#pragma once
#include "EditorGlobal.h"

class EditorGlobalInterface
{
protected:
	EventBus<>* m_EditorEventbus;
	ActionSystem<std::string>* m_ActionSystem;
public:
	EditorGlobalInterface()
	{
		m_EditorEventbus = EditorGlobal::GetEventBusInstance();
		m_ActionSystem = EditorGlobal::GetActionSystemInstance();
	}
};