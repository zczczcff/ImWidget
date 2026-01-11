#include "ImGlobal.h"
#include "Application/ImApplication.h"
#include "ImEvent/ImEventSystem.h"
#include "ImGlobalInstance.h"

namespace ImGuiWidget
{
	ImApplication* GetGlobalApp()
	{
		return ImGuiWidget::GetGlobalInstance()->GetGlobalApp();
	}
	ImVec2 GetMousePos()
	{
		return ImGui::GetMousePos();
	}
	void RequestFocus(ImWidget* TargetWidget)
	{
		ImGuiWidget::GetGlobalInstance()->GetGlobalApp()->GetWindowManager()->GetEventSystem()->SetFocus(TargetWidget);
	}
}