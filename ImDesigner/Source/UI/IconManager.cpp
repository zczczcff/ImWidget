#include "UI/IconManager.h"
#include "Resource/ClosedFolder.h"
#include "Resource/OpenedFolder.h"
#include "Resource/UIFile.h"
#include "Resource/ComboWidget.h"
#include "ImWidget/ImImage.h"

#include "Resource/Button.h"
#include "Resource/CheckBox.h"
#include "Resource/ComboBox.h"
#include "Resource/Image.h"
#include "Resource/InputText.h"
#include "Resource/IntInput.h"
#include "Resource/FloatInput.h"
#include "Resource/MultiLineTextBlock.h"
#include "Resource/ScrollTextList.h"
#include "Resource/Slider.h"
#include "Resource/TextBlock.h"
#include "Resource/SimpleFigure.h"

#include "Resource/Border.h"
#include "Resource/CanvasPanel.h"
#include "Resource/HorizontalBox.h"
#include "Resource/HorizontalSplitter.h"
#include "Resource/VerticalBox.h"
#include "Resource/VerticalSplitter.h"
#include "Resource/ExpandableBox.h"
#include "Resource/ScrollBox.h"


#include "Resource/BitIcon_icons.h"

void IconManager::LoadIcons()
{
	int w, h;
	TextureIDArray[(int)ImDesignerIcon::ClosedFolder] =	ImGuiWidget::GlobalApp->LoadImageFromMemory(ClosedFolder_data, ClosedFolder_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::OpenedFolder] = ImGuiWidget::GlobalApp->LoadImageFromMemory(OpenedFolder_data, OpenedFolder_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::UIFile] = ImGuiWidget::GlobalApp->LoadImageFromMemory(UIFile_data, UIFile_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::ComboWidget] = ImGuiWidget::GlobalApp->LoadImageFromMemory(ComboWidget_data, ComboWidget_size, w, h);

	TextureIDArray[(int)ImDesignerIcon::Button] = ImGuiWidget::GlobalApp->LoadImageFromMemory(Button_data, Button_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::CheckBox] = ImGuiWidget::GlobalApp->LoadImageFromMemory(CheckBox_data, CheckBox_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::ComboBox] = ImGuiWidget::GlobalApp->LoadImageFromMemory(ComboBox_data, ComboBox_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::Image] = ImGuiWidget::GlobalApp->LoadImageFromMemory(Image_data, Image_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::InputText] = ImGuiWidget::GlobalApp->LoadImageFromMemory(InputText_data, InputText_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::IntInput] = ImGuiWidget::GlobalApp->LoadImageFromMemory(IntInput_data, IntInput_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::FloatInput] = ImGuiWidget::GlobalApp->LoadImageFromMemory(FloatInput_data, FloatInput_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::MultiLineTextBlock] = ImGuiWidget::GlobalApp->LoadImageFromMemory(MultiLineTextBlock_data, MultiLineTextBlock_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::ScrollTextList] = ImGuiWidget::GlobalApp->LoadImageFromMemory(ScrollTextList_data, ScrollTextList_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::Slider] = ImGuiWidget::GlobalApp->LoadImageFromMemory(Slider_data, Slider_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::TextBlock] = ImGuiWidget::GlobalApp->LoadImageFromMemory(TextBlock_data, TextBlock_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::SimpleFigure] = ImGuiWidget::GlobalApp->LoadImageFromMemory(SimpleFigure_data, SimpleFigure_size, w, h);

	TextureIDArray[(int)ImDesignerIcon::Border] = ImGuiWidget::GlobalApp->LoadImageFromMemory(Border_data, Border_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::CanvasPanel] = ImGuiWidget::GlobalApp->LoadImageFromMemory(CanvasPanel_data, CanvasPanel_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::HorizontalBox] = ImGuiWidget::GlobalApp->LoadImageFromMemory(HorizontalBox_data, HorizontalBox_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::HorizontalSplitter] = ImGuiWidget::GlobalApp->LoadImageFromMemory(HorizontalSplitter_data, HorizontalSplitter_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::VerticalBox] = ImGuiWidget::GlobalApp->LoadImageFromMemory(VerticalBox_data, VerticalBox_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::VerticalSplitter] = ImGuiWidget::GlobalApp->LoadImageFromMemory(VerticalSplitter_data, VerticalSplitter_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::ExpandableBox] = ImGuiWidget::GlobalApp->LoadImageFromMemory(ExpandableBox_data, ExpandableBox_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::ScrollBox] = ImGuiWidget::GlobalApp->LoadImageFromMemory(ScrollBox_data, ScrollBox_size, w, h);

	TextureIDArray[(int)ImDesignerIcon::Undo] = ImGuiWidget::GlobalApp->LoadTextureFromRGBAData
	(g_BitIcon_icons[BITICON_UNDO]->ToRGBA(), g_BitIcon_icons[BITICON_UNDO]->width, g_BitIcon_icons[BITICON_UNDO]->height);
	TextureIDArray[(int)ImDesignerIcon::Redo] = ImGuiWidget::GlobalApp->LoadTextureFromRGBAData
	(g_BitIcon_icons[BITICON_REDO]->ToRGBA(), g_BitIcon_icons[BITICON_REDO]->width, g_BitIcon_icons[BITICON_REDO]->height);
	TextureIDArray[(int)ImDesignerIcon::WidgetTree] = ImGuiWidget::GlobalApp->LoadTextureFromRGBAData
	(g_BitIcon_icons[BITICON_WIDGET_TREE]->ToRGBA(), g_BitIcon_icons[BITICON_WIDGET_TREE]->width, g_BitIcon_icons[BITICON_WIDGET_TREE]->height);
	TextureIDArray[(int)ImDesignerIcon::SingleWidget] = ImGuiWidget::GlobalApp->LoadTextureFromRGBAData
	(g_BitIcon_icons[BITICON_SINGLE_WIDGET]->ToRGBA(), g_BitIcon_icons[BITICON_SINGLE_WIDGET]->width, g_BitIcon_icons[BITICON_SINGLE_WIDGET]->height);
	TextureIDArray[(int)ImDesignerIcon::Dialog] = ImGuiWidget::GlobalApp->LoadTextureFromRGBAData
	(g_BitIcon_icons[BITICON_DIALOG]->ToRGBA(), g_BitIcon_icons[BITICON_DIALOG]->width, g_BitIcon_icons[BITICON_DIALOG]->height);

}

ImTextureID IconManager::GetIcon(ImDesignerIcon IconIndex)
{
	return TextureIDArray[(int)IconIndex];
}

ImGuiWidget::ImImage* IconManager::CreateIconImage(ImDesignerIcon IconIndex, int width, int height, const std::string& WidgetName)
{
	ImGuiWidget::ImImage* ret = new ImGuiWidget::ImImage(WidgetName, GetIcon(IconIndex), width, height);
	return ret;
}

