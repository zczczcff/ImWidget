#pragma once
#include <imgui.h>
#include <string>

namespace ImGuiWidget
{
	class ImWidget;
	class ImSlot
	{
	protected:
		ImWidget* Content;
		bool bAutoSize = true;
	public:
		float PaddingTop = 0.f;
		float PaddingBottom = 0.f;
		float PaddingLeft = 0.f;
		float PaddingRight = 0.f;
		ImSlot():Content(nullptr) {}
		ImSlot(ImWidget* Content):Content(Content)
		{
			if (Content)
			{
				Content->SetSlot(this);
			}
		}
		ImWidget* GetContent() { return Content; }
		virtual void ApplyLayout(const ImVec2& containerPos, const ImVec2& containerSize){}
		bool GetIfAutoSize() { return bAutoSize; }
		void SetIfAutoSize(bool Value) { bAutoSize = Value; }
	};
}