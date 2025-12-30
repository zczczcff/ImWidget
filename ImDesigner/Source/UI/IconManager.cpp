#include "UI/IconManager.h"
#include "Resource/ClosedFolder.h"
#include "Resource/OpenedFolder.h"
#include "Resource/UIFile.h"
#include "Resource/SingleWidget.h"
#include "Resource/ComboWidget.h"
#include "ImWidget/ImImage.h"

void IconManager::LoadIcons()
{
	int w, h;
	TextureIDArray[(int)ImDesignerIcon::ClosedFolder] =	ImGuiWidget::GlobalApp->LoadTextureFromMemory(ClosedFolder_data, ClosedFolder_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::OpenedFolder] = ImGuiWidget::GlobalApp->LoadTextureFromMemory(OpenedFolder_data, OpenedFolder_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::UIFile] = ImGuiWidget::GlobalApp->LoadTextureFromMemory(UIFile_data, UIFile_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::SingleWidget] = ImGuiWidget::GlobalApp->LoadTextureFromMemory(SingleWidget_data, SingleWidget_size, w, h);
	TextureIDArray[(int)ImDesignerIcon::ComboWidget] = ImGuiWidget::GlobalApp->LoadTextureFromMemory(ComboWidget_data, ComboWidget_size, w, h);
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

