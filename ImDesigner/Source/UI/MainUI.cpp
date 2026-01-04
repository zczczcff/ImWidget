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
#include "UI/UI_ProjectView.h"

void MainUI::Init2()
{
	ImButton_Project->SetFocusable(false);
	ImButton_Save->SetFocusable(false);
	ImButton_Generate->SetFocusable(false);



	Widget_ExampleWidgetButton* Example_Button = new Widget_ExampleWidgetButton("Example_Button", u8"按钮", "ImButton");
	ImVerticalBox_WidgetList->AddChildToVerticalBox(Example_Button)->SetIfAutoSize(false);


	ImPageManager_LeftPart = new ImGuiWidget::ImPageManager("ImPageManager_LeftPart");
	
	// 创建项目视图实例
	ProjectView = new UI_ProjectView("ProjectView");
	ProjectView->OnUIFileSelected.Add([this](const std::string& FileName, const std::string& FileFullPath) { OnUIFileSelected.Broadcast(FileName, FileFullPath); });
	ImBorder_LeftTab->SetContent(ImPageManager_LeftPart, false);
	ImPageManager_LeftPart->SetTabPosition(ImGuiWidget::ImPageManager::TabPosition::Bottom);
	ImPageManager_LeftPart->SetShowCloseButton(false);

	ImPageManager_LeftPart->AddPage(u8"项目目录", ProjectView);
	

	m_FolderOperatorMenuWindow = ImGuiWidget::GlobalApp->GetWindowManager()->CreatePopupWindow(ImVec2(0, 0), ImVec2(0, 0), nullptr, false);
	m_FolderOperatorMenuWindow->Close();

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

void MainUI::UpdateProjectView(ProjectFileManager* projectmananger)
{
	ProjectView->UpdateProjectView(projectmananger);
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
