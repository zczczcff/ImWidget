#include "UI/IconManager.h"
#include "Resource/ClosedFolder.h"
#include "Resource/OpenedFolder.h"
#include "Resource/UIFile.h"
#include "Resource/SingleWidget.h"
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

#include "Resource/Redo.h"
#include "Resource/Undo.h"

void IconManager::LoadIcons()
{
	int w, h;
	TextureIDArray[(int)ImDesignerIcon::ClosedFolder] =	ImGuiWidget::GlobalApp->LoadTextureFromMemory(ClosedFolder_data, ClosedFolder_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::OpenedFolder] = ImGuiWidget::GlobalApp->LoadTextureFromMemory(OpenedFolder_data, OpenedFolder_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::UIFile] = ImGuiWidget::GlobalApp->LoadTextureFromMemory(UIFile_data, UIFile_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::SingleWidget] = ImGuiWidget::GlobalApp->LoadTextureFromMemory(SingleWidget_data, SingleWidget_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::ComboWidget] = ImGuiWidget::GlobalApp->LoadTextureFromMemory(ComboWidget_data, ComboWidget_size, w, h);

	TextureIDArray[(int)ImDesignerIcon::Button] = ImGuiWidget::GlobalApp->LoadTextureFromMemory(Button_data, Button_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::CheckBox] = ImGuiWidget::GlobalApp->LoadTextureFromMemory(CheckBox_data, CheckBox_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::ComboBox] = ImGuiWidget::GlobalApp->LoadTextureFromMemory(ComboBox_data, ComboBox_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::Image] = ImGuiWidget::GlobalApp->LoadTextureFromMemory(Image_data, Image_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::InputText] = ImGuiWidget::GlobalApp->LoadTextureFromMemory(InputText_data, InputText_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::IntInput] = ImGuiWidget::GlobalApp->LoadTextureFromMemory(IntInput_data, IntInput_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::FloatInput] = ImGuiWidget::GlobalApp->LoadTextureFromMemory(FloatInput_data, FloatInput_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::MultiLineTextBlock] = ImGuiWidget::GlobalApp->LoadTextureFromMemory(MultiLineTextBlock_data, MultiLineTextBlock_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::ScrollTextList] = ImGuiWidget::GlobalApp->LoadTextureFromMemory(ScrollTextList_data, ScrollTextList_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::Slider] = ImGuiWidget::GlobalApp->LoadTextureFromMemory(Slider_data, Slider_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::TextBlock] = ImGuiWidget::GlobalApp->LoadTextureFromMemory(TextBlock_data, TextBlock_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::SimpleFigure] = ImGuiWidget::GlobalApp->LoadTextureFromMemory(SimpleFigure_data, SimpleFigure_size, w, h);

	TextureIDArray[(int)ImDesignerIcon::Border] = ImGuiWidget::GlobalApp->LoadTextureFromMemory(Border_data, Border_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::CanvasPanel] = ImGuiWidget::GlobalApp->LoadTextureFromMemory(CanvasPanel_data, CanvasPanel_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::HorizontalBox] = ImGuiWidget::GlobalApp->LoadTextureFromMemory(HorizontalBox_data, HorizontalBox_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::HorizontalSplitter] = ImGuiWidget::GlobalApp->LoadTextureFromMemory(HorizontalSplitter_data, HorizontalSplitter_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::VerticalBox] = ImGuiWidget::GlobalApp->LoadTextureFromMemory(VerticalBox_data, VerticalBox_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::VerticalSplitter] = ImGuiWidget::GlobalApp->LoadTextureFromMemory(VerticalSplitter_data, VerticalSplitter_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::ExpandableBox] = ImGuiWidget::GlobalApp->LoadTextureFromMemory(ExpandableBox_data, ExpandableBox_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::ScrollBox] = ImGuiWidget::GlobalApp->LoadTextureFromMemory(ScrollBox_data, ScrollBox_size, w, h);

	TextureIDArray[(int)ImDesignerIcon::Undo] = ImGuiWidget::GlobalApp->LoadTextureFromMemory(Undo_data, Undo_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::Redo] = ImGuiWidget::GlobalApp->LoadTextureFromMemory(Undo_data, Undo_size, w, h);
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

