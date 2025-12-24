#pragma once
#include <imgui.h>
#include "CHFONT.h"
#include <map>
#include <string>
#include "ImEvent/ImEventSystem.h"
namespace ImGuiWidget
{
	class ImWidget;
}

class ImApplication
{
protected:
	static int DefalutFontSize;
	static std::string DefaultFont;
	static std::map<int, ImFont*> DefalutFonts;
	ImGuiWidget::ImEventSystem* m_EventSys;
protected:
	static ImFont* LoadDefaultFontInternal(int Size);
public:
	void Render();
	void SetRootWidget(ImGuiWidget::ImWidget* RootWidget)
	{
		m_EventSys = new ImGuiWidget::ImEventSystem(RootWidget);
	}
	static ImFont* GetFont(int Size, const std::string& FontFile = "")
	{
		if (!(Size >= 1 && Size <= 80))
			return nullptr;
		if (FontFile.empty())
		{
			auto it = DefalutFonts.find(Size);
			if (it != DefalutFonts.end())
			{
				return it->second;
			}
			else
			{
				DefalutFonts.insert(std::make_pair(Size, LoadDefaultFontInternal(Size)));
				return GetFont(Size);
			}
		}
	}
	virtual ImTextureID LoadTextureFromFile(const char* filename, int& width, int& height) = 0;
	virtual void ReleaseTexture(ImTextureID TextureID) = 0;
	virtual bool Initialize() { return false; };
	virtual void Run() {};
};