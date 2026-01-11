#pragma once
#include "ImWidget/ImGlobalInstance.h"
#include "ImWidget/ImWidgetFactory.h"
#include "ImWidget/ImObjectFactory.h"

namespace ImGuiWidget
{
	ImGlobalInstance::ImGlobalInstance()
		:m_WidgetFactory(new ImWidgetFactory),
		m_ObjectFactory(new ImObjectFactory)
	{

	}
}