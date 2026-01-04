#include "ImGlobal.h"
#include "Application/ImApplication.h"
#include "ImEvent/ImEventSystem.h"
namespace ImGuiWidget
{
	extern ImApplication* GlobalApp;
	ImApplication* GetGlobalApp()
	{
		return GlobalApp;
	}
	ImVec2 GetMousePos()
	{
		return ImGui::GetMousePos();
	}
	void RequestFocus(ImWidget* TargetWidget)
	{
		GlobalApp->GetWindowManager()->GetEventSystem()->SetFocus(TargetWidget);
	}
}