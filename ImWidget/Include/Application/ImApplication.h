#pragma once
#include <imgui.h>
#include "CHFONT.h"
#include <map>
#include <string>
#include "ImEvent/ImEventSystem.h"
#include "ImWindows/ImWindowManager.h"
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
	//ImGuiWidget::ImEventSystem* m_EventSys;
	ImGuiWidget::ImWindowManager* m_windowManager = nullptr; // 新增窗口管理器
protected:
	static ImFont* LoadDefaultFontInternal(int Size);
public:
	ImApplication();
	void RenderTick();
	void SetRootWidget(ImGuiWidget::ImWidget* RootWidget);

	//void AddWindow(ImGuiWidget::ImWindow* window); // 新增：添加窗口

	// 获取窗口管理器
	ImGuiWidget::ImWindowManager* GetWindowManager() const { return m_windowManager; }

	// 创建默认窗口（兼容原有代码）
	ImGuiWidget::ImWindow* CreateDefaultWindow(const std::string& title = "Main Window");
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