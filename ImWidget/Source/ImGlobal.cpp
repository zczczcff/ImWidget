#include "ImGlobal.h"
#include "Application/ImApplication.h"

namespace ImGuiWidget
{
	extern ImApplication* GlobalApp;
	ImApplication* ImGuiWidget::GetGlobalApp()
	{
		return GlobalApp;
	}
}