#pragma once
#include "EditorGlobal.h"

class EditorEventInterface
{
protected:

	template <typename... Args>
	ActionResult Publish(const KeyStringType& eventName, Args&&... args)
	{
		return EditorGlobal::GetActionSystemInstance()->Execute(eventName, std::forward<Args>(args)...);
	}
};