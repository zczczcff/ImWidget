#include "UI/MainUI.h"
#include "ImBasicWidgetList.h"
#include "UI/Widget_ExampleWidgetButton.h"
#include "UI/Widget_PageTag.h"
#include "ImComboWidget/ImPageManager.h"
#include "Tools/ProjectFileManager.h"

void MainUI::Init2()
{
	ImButton_Project->SetFocusable(false);
	ImButton_Save->SetFocusable(false);
	ImButton_Generate->SetFocusable(false);

	Widget_PageTag* testtag = new Widget_PageTag("testpagetag");
	ImHorizontalBox_PageTag->AddChildToHorizontalBox(testtag)->SetIfAutoSize(false);

	Widget_ExampleWidgetButton* Example_Button = new Widget_ExampleWidgetButton("Example_Button", u8"按钮", "ImButton");
	ImVerticalBox_WidgetList->AddChildToVerticalBox(Example_Button)->SetIfAutoSize(false);


	ImPageManager_LeftPart = new ImGuiWidget::ImPageManager("ImPageManager_LeftPart");
	
	ImScrollBox_Folder = new ImGuiWidget::ImScrollBox("ImScrollBox_Folder");
	ImVerticalBox_Folder = new ImGuiWidget::ImVerticalBox("ImVerticalBox_Folder");

	ImScrollBox_WidgetTree = new ImGuiWidget::ImScrollBox("ImScrollBox_WidgetTree");
	ImVerticalBox_WidgetTree = new ImGuiWidget::ImVerticalBox("ImVerticalBox_WidgetTree");

	ImScrollBox_Folder->bHaveBorder = false;
	ImVerticalBox_Folder->bHaveBorder = false;
	ImScrollBox_WidgetTree->bHaveBorder = false;
	ImVerticalBox_WidgetTree->bHaveBorder = false;

	ImBorder_LeftTab->SetContent(ImPageManager_LeftPart, false);
	ImPageManager_LeftPart->SetTabPosition(ImGuiWidget::ImPageManager::TabPosition::Bottom);
	ImPageManager_LeftPart->SetShowCloseButton(false);

	ImScrollBox_Folder->SetContent(ImVerticalBox_Folder);
	ImScrollBox_WidgetTree->SetContent(ImVerticalBox_WidgetTree);

	ImPageManager_LeftPart->AddPage(u8"项目目录", ImScrollBox_Folder);
	ImPageManager_LeftPart->AddPage(u8"控件树", ImScrollBox_WidgetTree);

}

void MainUI::SetProjectViewVBoxContent(ProjectFileManager* projectmananger, ImGuiWidget::ImVerticalBox* Vbox, const std::string& CurrentPath)
{
	for (auto& dir : projectmananger->getSubdirectories(CurrentPath))
	{
		ImGuiWidget::ImExpandableBox* SubDirBox = new ImGuiWidget::ImExpandableBox(dir.relativePath + "_EXBox");
		ImGuiWidget::ImTextBlock* SubDirName = new ImGuiWidget::ImTextBlock(dir.relativePath + "_Text");
		SubDirName->SetText(dir.filename);
		SubDirName->SetHorizontalAlignment(ImGuiWidget::ImTextBlock::TextAlignment_Horizontal::Left);
		ImGuiWidget::ImVerticalBox* SubDirVBox = new ImGuiWidget::ImVerticalBox(dir.relativePath + "_VBox");
		SubDirBox->SetHead(SubDirName);
		SubDirBox->SetBody(SubDirVBox);
		Vbox->AddChildToVerticalBox(SubDirBox)->SetIfAutoSize(false);
		SetProjectViewVBoxContent(projectmananger, SubDirVBox, dir.relativePath);
	}

	for (auto& file : projectmananger->getFilesInDirectory(CurrentPath))
	{
		ImGuiWidget::ImButton* FileButton = new ImGuiWidget::ImButton(file.relativePath + "_Button");
		ImGuiWidget::ImTextBlock* FileName = new ImGuiWidget::ImTextBlock(file.relativePath + "_Text");
		FileName->SetText(file.filename);
		FileName->SetHorizontalAlignment(ImGuiWidget::ImTextBlock::TextAlignment_Horizontal::Left);
		FileButton->SetContent(FileName);
		FileButton->SetOnPressed([this,file]() { On_ProjectButtonClicked(file.filename,file.fullPath); });
		Vbox->AddChildToVerticalBox(FileButton)->SetIfAutoSize(false);
	}

	Vbox->bHaveBorder = false;
}

void MainUI::UpdateProjectView(ProjectFileManager* projectmananger)
{
	ImVerticalBox_Folder->RemoveAllChild(true);
	SetProjectViewVBoxContent(projectmananger, ImVerticalBox_Folder, "");
}

void MainUI::On_ProjectButtonClicked(const std::string& FileName, const std::string& FileFullPath)
{
}
