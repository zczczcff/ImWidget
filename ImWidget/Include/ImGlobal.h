#pragma once
#include <imgui.h>
class ImApplication;

namespace ImGuiWidget
{
	class ImWidget;
	ImApplication* GetGlobalApp();
	ImVec2 GetMousePos();
	void RequestFocus(ImWidget* TargetWidget);
}