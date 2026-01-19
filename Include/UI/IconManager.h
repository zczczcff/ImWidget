#pragma once
#include "Application/ImApplication.h"

enum class ImDesignerIcon
{
	ClosedFolder,
	OpenedFolder,
	UIFile,
	ComboWidget,

	Button,
	CheckBox,
	ComboBox,
	Image,
	InputText,
	IntInput,
	FloatInput,
	MultiLineTextBlock,
	ScrollTextList,
	Slider,
	TextBlock,
	SimpleFigure,

	Border,
	CanvasPanel,
	HorizontalBox,
	HorizontalSplitter,
	VerticalBox,
	VerticalSplitter,
	ExpandableBox,
	ScrollBox,

	Undo,
	Redo,

	WidgetTree,
	SingleWidget,
	Dialog,

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