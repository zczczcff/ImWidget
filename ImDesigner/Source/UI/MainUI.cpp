#include "UI/MainUI.h"
#include "ImBasicWidgetList.h"
#include "UI/Widget_ExampleWidgetButton.h"
#include "UI/Widget_PageTag.h"
#include "ImComboWidget/ImPageManager.h"
#include "Tools/ProjectFileManager.h"
#include "ImWidget/ImImage.h"
#include "UI/IconManager.h"
#include "Application/ImApplication.h"
#include "UI/UI_WidgetEditor.h"
#include "UI/UI_WidgetTreeView.h"
#include "UI/UI_DetailView.h"

void MainUI::Init2()
{
	ImButton_Project->SetFocusable(false);
	ImButton_Save->SetFocusable(false);
	ImButton_Generate->SetFocusable(false);



	Widget_ExampleWidgetButton* Example_Button = new Widget_ExampleWidgetButton("Example_Button", u8"按钮", "ImButton");
	ImVerticalBox_WidgetList->AddChildToVerticalBox(Example_Button)->SetIfAutoSize(false);


	ImPageManager_LeftPart = new ImGuiWidget::ImPageManager("ImPageManager_LeftPart");
	
	ImScrollBox_Folder = new ImGuiWidget::ImScrollBox("ImScrollBox_Folder");
	ImVerticalBox_Folder = new ImGuiWidget::ImVerticalBox("ImVerticalBox_Folder");

	

	ImScrollBox_Folder->bHaveBorder = false;
	ImVerticalBox_Folder->bHaveBorder = false;
	

	ImBorder_LeftTab->SetContent(ImPageManager_LeftPart, false);
	ImPageManager_LeftPart->SetTabPosition(ImGuiWidget::ImPageManager::TabPosition::Bottom);
	ImPageManager_LeftPart->SetShowCloseButton(false);

	ImScrollBox_Folder->SetContent(ImVerticalBox_Folder);

	ImPageManager_LeftPart->AddPage(u8"项目目录", ImScrollBox_Folder);
	

	m_RightKeyFunMenuWindow = ImGuiWidget::GlobalApp->GetWindowManager()->CreatePopupWindow(ImVec2(0, 0), ImVec2(0, 0), nullptr, false);
	m_RightKeyFunMenuWindow->Close();

	//主工作界面
	ImPageManager_Main = new ImGuiWidget::ImPageManager("ImPageManager_Main");
	ImBorder_MainWorkSpace->SetContent(ImPageManager_Main);
	ImGuiWidget::ImTextBlock* testtext = new ImGuiWidget::ImTextBlock("testblock");
	testtext->SetText("test");
	ImPageManager_Main->AddPage("test", testtext, IconManager::GetInstance()->GetIcon(ImDesignerIcon::SingleWidget));
	ImPageManager_Main->OnPageClosed().Add([this](const std::string& FilePath) 
		{
			On_EditorPageClosed(FilePath);
		});
	ImPageManager_Main->OnPageSelected().Add([this](const std::string& PageID) { On_EditorPageSelected(PageID); });

	//控件树视图
	ImScrollBox_WidgetTree = new ImGuiWidget::ImScrollBox("ImScrollBox_WidgetTree");
	ImScrollBox_WidgetTree->bHaveBorder = false;
	ImPageManager_LeftPart->AddPage(u8"控件树", ImScrollBox_WidgetTree);

	//细节框
	ImScrollBox_FileDetail = new ImGuiWidget::ImScrollBox("ImScrollBox_FileDetail");
	ImScrollBox_FileDetail->bHaveBorder = false;
	ImBorder_Right->SetContent(ImScrollBox_FileDetail);
}

void MainUI::SetProjectViewVBoxContent(ProjectFileManager* projectmananger, ImGuiWidget::ImVerticalBox* Vbox, const std::string& CurrentPath)
{
	for (auto& dir : projectmananger->getSubdirectories(CurrentPath))
	{
		ImGuiWidget::ImExpandableBox* SubDirBox = new ImGuiWidget::ImExpandableBox(dir.relativePath + "_EXBox");
		ImGuiWidget::ImHorizontalBox* HeaderHBox = new ImGuiWidget::ImHorizontalBox(dir.relativePath + "_HBox");
		ImGuiWidget::ImTextBlock* SubDirName = new ImGuiWidget::ImTextBlock(dir.relativePath + "_Text");
		ImGuiWidget::ImButton* DirButton = new ImGuiWidget::ImButton(dir.relativePath + "_Button");
		SubDirName->SetText(dir.filename);
		SubDirName->SetHorizontalAlignment(ImGuiWidget::ImTextBlock::TextAlignment_Horizontal::Left);
		ImGuiWidget::ImVerticalBox* SubDirVBox = new ImGuiWidget::ImVerticalBox(dir.relativePath + "_VBox");
		ImGuiWidget::ImImage* Icon = IconManager::GetInstance()->CreateIconImage(ImDesignerIcon::ClosedFolder, 16, 16);
		HeaderHBox->AddChildToHorizontalBox(Icon)->SetIfAutoSize(false);
		DirButton->SetContent(SubDirName);
		InitDirButton(DirButton, dir.fullPath);
		SetupFileButton(DirButton);
		HeaderHBox->AddChildToHorizontalBox(DirButton)->SetIfAutoSize(true);
		HeaderHBox->bHaveBorder = false;
		SubDirBox->SetHead(HeaderHBox);
		SubDirBox->SetBody(SubDirVBox);
		Vbox->AddChildToVerticalBox(SubDirBox)->SetIfAutoSize(false);
		SetProjectViewVBoxContent(projectmananger, SubDirVBox, dir.relativePath);
		SubDirBox->bHaveBorder = false;
		SubDirBox->SetOnExpandedStateChanged([Icon](bool NewState) 
			{
				if (NewState)
				{
					Icon->SetTextureID(IconManager::GetInstance()->GetIcon(ImDesignerIcon::OpenedFolder));
				}
				else
				{
					Icon->SetTextureID(IconManager::GetInstance()->GetIcon(ImDesignerIcon::ClosedFolder));
				}
			});
	}

	for (auto& file : projectmananger->getFilesInDirectory(CurrentPath))
	{
		ImGuiWidget::ImButton* FileButton = new ImGuiWidget::ImButton(file.relativePath + "_Button");
		ImGuiWidget::ImHorizontalBox* BodyHBox = new ImGuiWidget::ImHorizontalBox(file.relativePath + "_HBox");
		ImGuiWidget::ImImage* Icon = IconManager::GetInstance()->CreateIconImage(ImDesignerIcon::UIFile, 16, 16);
		ImGuiWidget::ImTextBlock* FileName = new ImGuiWidget::ImTextBlock(file.relativePath + "_Text");
		FileName->SetText(file.filename);
		FileName->SetHorizontalAlignment(ImGuiWidget::ImTextBlock::TextAlignment_Horizontal::Left);
		FileButton->SetContent(FileName);
		FileButton->OnLeftClicked.Add([this,file]() { On_UIFileButtonClicked(file.filename,file.fullPath); });
		SetupFileButton(FileButton);
		BodyHBox->AddChildToHorizontalBox(Icon)->SetIfAutoSize(false);
		BodyHBox->AddChildToHorizontalBox(FileButton);
		BodyHBox->bHaveBorder = false;
		Vbox->AddChildToVerticalBox(BodyHBox)->SetIfAutoSize(false);
	}

	Vbox->bHaveBorder = false;
}

void MainUI::SetupFileButton(ImGuiWidget::ImButton* filebutton)
{
	// 正常状态样式 - 稍深的灰色
	ImGuiWidget::ButtonStateStyle normalStyle;
	normalStyle.BackgroundColor = IM_COL32(220, 230, 245, 255);  // 从 (240,245,255) 加深
	normalStyle.Rounding = 0.0f;
	normalStyle.HasBorder = false;
	normalStyle.BorderThickness = 1.0f;
	normalStyle.BorderColor = IM_COL32(190, 190, 190, 255);  // 稍微加深边框颜色
	filebutton->SetNormalStyle(normalStyle);

	// 悬停状态样式 - 稍深的蓝色调灰色
	ImGuiWidget::ButtonStateStyle hoverStyle;
	hoverStyle.BackgroundColor = IM_COL32(205, 215, 235, 255);  // 从 (225,235,255) 加深
	hoverStyle.Rounding = 0.0f;
	hoverStyle.HasBorder = false;
	hoverStyle.BorderThickness = 1.0f;
	hoverStyle.BorderColor = IM_COL32(170, 170, 170, 255);
	filebutton->SetHoveredStyle(hoverStyle);

	// 按下状态样式 - 更深的蓝色调灰色
	ImGuiWidget::ButtonStateStyle pressedStyle;
	pressedStyle.BackgroundColor = IM_COL32(185, 200, 225, 255);  // 从 (200,220,250) 加深
	pressedStyle.Rounding = 0.0f;
	pressedStyle.HasBorder = false;
	pressedStyle.BorderThickness = 1.0f;
	pressedStyle.BorderColor = IM_COL32(150, 150, 150, 255);
	filebutton->SetPressedStyle(pressedStyle);

	// 选中状态样式（焦点状态）- 蓝色高亮
	ImGuiWidget::ButtonStateStyle selectedStyle;
	selectedStyle.BackgroundColor = IM_COL32(100, 149, 237, 255); // 矢车菊蓝
	selectedStyle.Rounding = 0.0f;
	selectedStyle.HasBorder = false;
	selectedStyle.BorderThickness = 1.0f;
	selectedStyle.BorderColor = IM_COL32(70, 130, 180, 255); // 钢蓝色
	filebutton->SetFocusedStyle(selectedStyle);
}

void MainUI::InitDirButton(ImGuiWidget::ImButton* Dirbutton, const std::string& dir)
{
	Dirbutton->OnRightClicked.Add([this,dir]() 
		{
			PopupDirRightKeyWindow(dir);
		});
}

void MainUI::PopupDirRightKeyWindow(const std::string& dir)
{
	ImGuiWidget::ImVerticalBox* ImVerticalBox_DirRightKeyMenu = new ImGuiWidget::ImVerticalBox("ImVerticalBox_DirRightKeyMenu");
	ImGuiWidget::ImButton* OptionButton = new ImGuiWidget::ImButton(dir + "_OptionButton");
	ImGuiWidget::ImTextBlock* Text_NewFile = new ImGuiWidget::ImTextBlock(dir + "_Text");
	Text_NewFile->SetText(u8"在" + dir + u8"中创建UI文件");
	OptionButton->SetContent(Text_NewFile);
	ImVerticalBox_DirRightKeyMenu->AddChildToVerticalBox(OptionButton);
	PopupRightKeyWindow(ImVerticalBox_DirRightKeyMenu);
}

void MainUI::PopupRightKeyWindow(ImWidget* rootwidget)
{
	m_RightKeyFunMenuWindow->SetPosition(ImGuiWidget::GlobalApp->GetCurrentMousePos());
	m_RightKeyFunMenuWindow->SetRootWidget(rootwidget, true);
	m_RightKeyFunMenuWindow->SetSize(rootwidget->GetMinSize());
	//m_RightKeyFunMenuWindow->SetSize(ImVec2(2,2));
	m_RightKeyFunMenuWindow->SetActive();
	ImGuiWidget::GlobalApp->GetWindowManager()->SetActiveWindow(m_RightKeyFunMenuWindow);
}

void MainUI::UpdateProjectView(ProjectFileManager* projectmananger)
{
	ImVerticalBox_Folder->RemoveAllChild(true);
	SetProjectViewVBoxContent(projectmananger, ImVerticalBox_Folder, "");
}

void MainUI::On_UIFileButtonClicked(const std::string& FileName, const std::string& FileFullPath)
{
	OnUIFileSelected.Broadcast(FileName, FileFullPath);
}

void MainUI::CreateNewWidgetEditorPage(ImGuiWidget::ImWidget* FileRootWidget, const std::string& FileName, const std::string& FileFullPath)
{
	if (ImPageManager_Main->HasPage(FileFullPath))
	{
		ImPageManager_Main->SwitchToPage(FileFullPath);
		return;
	}
	UI_WidgetEditor* NewWidget_UIEditor = new UI_WidgetEditor(FileName + "_Editor", FileRootWidget);
	ImPageManager_Main->AddPage(FileFullPath, NewWidget_UIEditor, IconManager::GetInstance()->GetIcon(ImDesignerIcon::UIFile), FileName);
	ImPageManager_Main->SwitchToPage(FileFullPath);
}

UI_WidgetEditor* MainUI::GetWidgetEditorByName(const std::string& Name)
{
	if (UI_WidgetEditor* finded = dynamic_cast<UI_WidgetEditor*>(ImPageManager_Main->GetPageContentByID(Name)))
	{
		return finded;
	}
	else
	{
		return nullptr;
	}
}

void MainUI::On_EditorPageClosed(const std::string& FilePath)
{
	OnEditorPageClosed.Broadcast(FilePath);
}

void MainUI::On_EditorPageSelected(const std::string& PageID)
{
	OnEditorPageSelected.Broadcast(PageID);
}

bool MainUI::CreateNewWidgetTreeView(const std::string& Name, ImGuiWidget::ImWidget* TargetWidget)
{
	if (AllTreeViews.find(Name) != AllTreeViews.end()) return false;
	UI_WidgetTreeView* New_UI_WidgetTreeView = new UI_WidgetTreeView("UI_WidgetTreeView");
	New_UI_WidgetTreeView->SetTargetWidget(TargetWidget);
	AllTreeViews.insert(std::make_pair(Name, New_UI_WidgetTreeView));
	return true;
}

UI_WidgetTreeView* MainUI::GetWidgetTreeViewByName(const std::string& Name)
{
	auto it = AllTreeViews.find(Name);
	if (it != AllTreeViews.end())
	{
		return it->second;
	}
	return nullptr;
}

bool MainUI::ShowWidgetTreeViewByName(const std::string& Name)
{
	if (CurrentTreeView == Name) return true;
	if (Name == "")
	{
		ImScrollBox_WidgetTree->SetContent(nullptr, false);
		return true;
	}
	auto it = AllTreeViews.find(Name);
	if (it != AllTreeViews.end())
	{
		ImScrollBox_WidgetTree->SetContent(it->second, false);
		CurrentTreeView = Name;
		return true;
	}
	return false;
}

bool MainUI::RemoveWidgetTreeViewByName(const std::string& Name)
{
	auto it = AllTreeViews.find(Name);
	if (it != AllTreeViews.end())
	{
		if (CurrentTreeView == Name)
		{
			ShowWidgetTreeViewByName("");
		}
		delete it->second;
		AllTreeViews.erase(it);
		return true;
	}
	return false;
}

bool MainUI::CreateNewDetailView(const std::string& Name)
{
	if (AllFileDetails.find(Name) != AllFileDetails.end()) return false;
	UI_DetailView* New_UI_DetailView = new UI_DetailView("UI_DetailView");
	AllFileDetails.insert(std::make_pair(Name, New_UI_DetailView));
	return true;
}

UI_DetailView* MainUI::GetDetailViewByName(const std::string& Name)
{
	auto it = AllFileDetails.find(Name);
	if (it != AllFileDetails.end())
	{
		return it->second;
	}
	return nullptr;
}

bool MainUI::ShowDetailViewByName(const std::string& Name)
{
	if (CurrentFileDetail == Name) return true;
	if (Name == "")
	{
		ImScrollBox_FileDetail->SetContent(nullptr, false);
		CurrentFileDetail = Name;
		return true;
	}
	auto it = AllFileDetails.find(Name);
	if (it != AllFileDetails.end())
	{
		ImScrollBox_FileDetail->SetContent(it->second, false);
		CurrentFileDetail = Name;
		return true;
	}
	return false;
}

bool MainUI::RemoveDetailViewByName(const std::string& Name)
{
	auto it = AllFileDetails.find(Name);
	if (it != AllFileDetails.end())
	{
		if (CurrentFileDetail == Name)
		{
			ShowDetailViewByName("");
		}
		delete it->second;
		AllFileDetails.erase(it);
		return true;
	}
	return false;
}
