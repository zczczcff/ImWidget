#pragma once

#include "ImWidget.h"
#include "../Application/ImApplication.h"
#include <imgui_internal.h>


namespace ImGuiWidget
{
	extern ImApplication* GlobalApp;
	class ImImage :public ImWidget
	{
	private:
		ImTextureID m_TextureID;
		int OriginalWidth, OriginalHeight;
		ImU32 TintColor;
	public:
		ImImage(const std::string& WidgetName):ImWidget(WidgetName),m_TextureID(0), OriginalWidth(0), OriginalHeight(0),TintColor(IM_COL32(255,255,255,255)){}
		ImImage(const std::string& WidgetName,const std::string& FilePath):ImWidget(WidgetName), m_TextureID(0),TintColor(IM_COL32(255, 255, 255, 255))
		{
			LoadImageFromFile(FilePath);
		}
		void SetTextureID(ImTextureID TextureID) { m_TextureID = TextureID; }
		bool LoadImageFromFile(const std::string& FilePath)
		{
			if (m_TextureID != 0)
			{
				GlobalApp->ReleaseTexture(m_TextureID);
				m_TextureID = 0;
			}
			m_TextureID = GlobalApp->LoadTextureFromFile(FilePath.c_str(), OriginalWidth, OriginalHeight);
			if (m_TextureID != 0)
			{
				return true;
			}
			else
			{
				OriginalWidth = 0;
				OriginalHeight = 0;
				return false;
			}
		}
		virtual ImVec2 GetMinSize() { return ImVec2(OriginalWidth, OriginalHeight); }
		void SetAlpha(float alpha)
		{
			ImVec4 Temp = ImGui::ColorConvertU32ToFloat4(TintColor);
			Temp.w = alpha;
			TintColor = ImGui::ColorConvertFloat4ToU32(Temp);
		}

		virtual void Render() override
		{
			ImGuiContext& g = *GImGui;
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			window->DrawList->AddImage(m_TextureID, Position, Position + Size, ImVec2(0, 0), ImVec2(1, 1), TintColor);
		}

		virtual std::string GetRegisterTypeName()override { return "ImImage"; }
	};


};