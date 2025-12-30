#pragma once
#include "Application/ImApplication.h"

enum class ImDesignerIcon
{
	ClosedFolder,
	OpenedFolder,
	UIFile,
	SingleWidget,
	ComboWidget,

	IconNum
};

namespace ImGuiWidget
{
	class ImImage;
}

class IconManager
{
private:
	ImTextureID  TextureIDArray[(int)ImDesignerIcon::IconNum];
	IconManager() :TextureIDArray{0} 
	{
		LoadIcons();
	}
	void LoadIcons();
public:

	static IconManager* GetInstance()
	{
		static IconManager IconManagerInstance;
		return &IconManagerInstance;
	}
	ImTextureID GetIcon(ImDesignerIcon IconIndex);
	class ImGuiWidget::ImImage* CreateIconImage(ImDesignerIcon IconIndex,int width, int height,const std::string& WidgetName="Icon");
};