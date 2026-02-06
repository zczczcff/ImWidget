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
#include "UI/UI_DetailView.h"
#include "UI/UI_ProjectView.h"
#include "Model/FileUtil.h"
#include "Public/WidgetInfor.h"
#include "UI/IconManager.h"
#include "EditorAction.h"
#include "EditorEvents.h"
#include "UI/UI_ImUserWidgetClassOutlineView.h"
#include "ImWidget/ImUserWidgetClass.h"

void MainUI::Init2()
{
	ImButton_Project->SetFocusable(false);
	ImButton_Save->SetFocusable(false);
	ImButton_Generate->SetFocusable(false);

	for (auto& SingleWidgetInfor : BasicWidgetList::GetBasicWidgetList())
	{
		Widget_ExampleWidgetButton* Example_Button = new Widget_ExampleWidgetButton
		(
			SingleWidgetInfor.EN_DisplayName,
			SingleWidgetInfor.CN_DisplayName,
			SingleWidgetInfor.RegisterName,
			IconManager::GetInstance()->GetIcon(SingleWidgetInfor.IconID)
		);
		ImVerticalBox_WidgetList->AddChildToVerticalBox(Example_Button)->SetIfAutoSize(false);
	}




	ImPageManager_LeftPart = new ImGuiWidget::ImPageManager("ImPageManager_LeftPart");
	
	// 创建项目视图实例
	ProjectView = new UI_ProjectView("ProjectView");
	//ProjectView->OnUIFileSelected.Add([this](const std::string& FileName, const std::string& FileFullPath) 
	//	{
	//		ExecuteAction(Action::MainUI::UI_FILE_SELECTED, FileName ,FileFullPath);
	//		//OnUIFileSelected.Broadcast(FileName, FileFullPath); 
	//	});
	ImBorder_LeftTab->SetContent(ImPageManager_LeftPart, false);
	ImPageManager_LeftPart->SetTabPosition(ImGuiWidget::ImPageManager::TabPosition::Bottom);
	ImPageManager_LeftPart->SetShowCloseButton(false);

	ImPageManager_LeftPart->AddPage(u8"项目目录", ProjectView);
	

	m_FolderOperatorMenuWindow = ImGuiWidget::GetGlobalInstance()->GetGlobalApp()->GetWindowManager()->CreatePopupWindow(ImVec2(0, 0), ImVec2(0, 0), nullptr, false);
	m_FolderOperatorMenuWindow->Close();

	//主工作界面
	ImPageManager_Main = new ImGuiWidget::ImPageManager("ImPageManager_Main");
	ImBorder_MainWorkSpace->SetContent(ImPageManager_Main);
	//ImGuiWidget::ImTextBlock* testtext = new ImGuiWidget::ImTextBlock("testblock");
	//testtext->SetText("test");
	//ImPageManager_Main->AddPage("test", testtext, IconManager::GetInstance()->GetIcon(ImDesignerIcon::SingleWidget));
	ImPageManager_Main->OnPageClosed().Add([this](const std::string& FilePath) 
		{
			On_EditorPageClosed(FilePath);
		});
	ImPageManager_Main->OnPageSelected().Add([this](const std::string& PageID) 
		{
			On_EditorPageSelected(PageID); 
		});

	//控件树视图
	ImScrollBox_Outline = new ImGuiWidget::ImScrollBox("ImScrollBox_Outline");
	ImScrollBox_Outline->bHaveBorder = false;
	ImPageManager_LeftPart->AddPage(u8"变量列表", ImScrollBox_Outline);

	//细节框
	ImScrollBox_FileDetail = new ImGuiWidget::ImScrollBox("ImScrollBox_FileDetail");
	ImScrollBox_FileDetail->bHaveBorder = false;
	ImBorder_Right->SetContent(ImScrollBox_FileDetail);

	//UndoRedo

	ImButton_Save->OnLeftClicked.Add([this]()
		{
			OnSaveFile();
		});
	ImButton_Save->SetToolTipEnable(true);
	ImButton_Save->SetToolTip(u8"保存（Ctrl+S）");

	ImButton_Undo->OnLeftClicked.Add([this]() 
		{
			ExecuteAction(CurrentEditedFile + Action::_REQUEST_UNDO);
			//OnRequestUndo.Broadcast(); 
		});
	ImButton_Undo->SetToolTipEnable(true);
	ImButton_Undo->SetToolTip(u8"撤销（Ctrl+Z）");
	ImButton_Redo->OnLeftClicked.Add([this]() 
		{
			ExecuteAction(CurrentEditedFile + Action::_REQUEST_REDO);
			//OnRequestRedo.Broadcast();
		});
	ImButton_Redo->SetToolTipEnable(true);
	ImButton_Redo->SetToolTip(u8"重做");
	ImImage_Undo->SetTextureID(IconManager::GetInstance()->GetIcon(ImDesignerIcon::Undo),20,20);
	ImImage_Redo->SetTextureID(IconManager::GetInstance()->GetIcon(ImDesignerIcon::Redo),20,20);
	UpdateUndoRedoState(false, false);

}

void MainUI::EventInit()
{
	Subscribe(Events::MainUI::UI_FILE_OPENED, [this](ImGuiWidget::ImUserWidgetClass* UerWidgetClassFile, std::string FileName, std::string FileFullPath)
		{
			CreateNewWidgetEditorPage(UerWidgetClassFile, FileName, FileFullPath);
			CreateNewOutlineView(FileFullPath, UerWidgetClassFile);
			CreateNewDetailView(FileFullPath);
			SwitchCurrentEditFile(FileFullPath);
		});
	
	Subscribe(Events::REGISTER_LOG_UPDATE_FUN, [this](std::function<void(std::vector<std::string>&&)>& OnLogUpdate)
		{
			OnLogUpdate = [this](std::vector<std::string>&& logs) { UpdateLog(std::move(logs)); };
		}, "", true);

	Subscribe(Events::MainUI::SET_UNDOREDO_STATE, [this](const std::string& FileFullPath, bool CanUndo, bool CanRedo) 
		{
			if (FileFullPath != CurrentEditedFile) return;
			UpdateUndoRedoState(CanUndo, CanRedo); 
		});
}

void MainUI::ActionInit()
{
	AddSequentialProcessor(Action::ProjectView::RENAME_FILE, [this](const std::string& OldFullPath, const std::string& NewFullPath)
		{
			HandleRenameFile(OldFullPath, NewFullPath);
		});

}

//void MainUI::UpdateProjectView(ProjectFileManager* projectmananger)
//{
//	ProjectView->UpdateProjectView(projectmananger);
//}

void MainUI::CreateNewWidgetEditorPage(ImGuiWidget::ImUserWidgetClass* UserWidgetClassFile, const std::string& FileName, const std::string& FileFullPath)
{
	if (ImPageManager_Main->HasPage(FileFullPath))
	{
		ImPageManager_Main->SwitchToPage(FileFullPath);
		return;
	}
	UI_WidgetEditor* NewWidget_UIEditor = new UI_WidgetEditor(FileName + "_Editor", UserWidgetClassFile, FileFullPath);
	ImPageManager_Main->AddPage(FileFullPath, NewWidget_UIEditor, IconManager::GetInstance()->GetIcon(ImDesignerIcon::UIFile), FileName);
	ImPageManager_Main->SwitchToPage(FileFullPath);
	ImPageManager_Main->SetPageToolTip(FileFullPath, FileFullPath);
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

bool MainUI::ShowWidgetEditorByName(const std::string& Name)
{
	ImPageManager_Main->SwitchToPage(Name);
	return false;
}

bool MainUI::RenameWidgetEditorPage(const std::string& OldFullPath, const std::string& NewFullPath)
{
	if (!ImPageManager_Main->ResetPageID(OldFullPath, NewFullPath))return false;
	std::string NewFileName = FileUtil::getFileNameWithExtension(NewFullPath);
	ImPageManager_Main->SetPageDisplayName(NewFullPath, NewFileName);
	ImPageManager_Main->SetPageToolTip(NewFullPath, NewFullPath);
	return true;
}

void MainUI::On_EditorPageClosed(const std::string& FilePath)
{
	if (ExecuteAction(Action::MainUI::EDITOR_PAGE_CLOSED, FilePath).success)
	{
		HandleCloseFile(FilePath);
	}
	//OnEditorPageClosed.Broadcast(FilePath);
}

void MainUI::On_EditorPageSelected(const std::string& PageID)
{
	ExecuteAction(Action::MainUI::EDITOR_PAGE_SELECTED, PageID);
	ShowOutlineViewByName(PageID);
	ShowDetailViewByName(PageID);
	ShowWidgetEditorByName(PageID);
	//OnEditorPageSelected.Broadcast(PageID);
}

bool MainUI::CreateNewOutlineView(const std::string& Name, ImGuiWidget::ImUserWidgetClass* TargetWidget)
{
	if (AllOutlineViews.find(Name) != AllOutlineViews.end()) return false;
	UI_ImUserWidgetClassOutlineView* New_UI_OutlineView = new UI_ImUserWidgetClassOutlineView("UI_OutlineView", TargetWidget, Name);
	AllOutlineViews.insert(std::make_pair(Name, New_UI_OutlineView));
	return true;
}

UI_ImUserWidgetClassOutlineView* MainUI::GetOutlineViewByName(const std::string& Name)
{
	auto it = AllOutlineViews.find(Name);
	if (it != AllOutlineViews.end())
	{
		return it->second;
	}
	return nullptr;
}

bool MainUI::ShowOutlineViewByName(const std::string& Name)
{
	if (CurrentOutlineView == Name) return true;
	if (Name == "")
	{
		ImScrollBox_Outline->SetContent(nullptr, false);
		CurrentOutlineView = Name;
		return true;
	}
	auto it = AllOutlineViews.find(Name);
	if (it != AllOutlineViews.end())
	{
		ImScrollBox_Outline->SetContent(it->second, false);
		CurrentOutlineView = Name;
		return true;
	}
	return false;
}

bool MainUI::RemoveOutlineViewByName(const std::string& Name)
{
	auto it = AllOutlineViews.find(Name);
	if (it != AllOutlineViews.end())
	{
		if (CurrentOutlineView == Name)
		{
			ShowOutlineViewByName("");
		}
		delete it->second;
		AllOutlineViews.erase(it);
		return true;
	}
	return false;
}

bool MainUI::RenameOutlineView(const std::string& OldName, const std::string& NewName)
{
	auto it = AllOutlineViews.find(OldName);
	if (it != AllOutlineViews.end())
	{
		UI_ImUserWidgetClassOutlineView* v = it->second;
		AllOutlineViews.erase(it);
		AllOutlineViews.insert(std::make_pair(NewName, v));
		ShowOutlineViewByName(NewName);
		return true;
	}
	else
	{
		return false;
	}
}

bool MainUI::CreateNewDetailView(const std::string& Name)
{
	if (AllFileDetails.find(Name) != AllFileDetails.end()) return false;
	UI_DetailView* New_UI_DetailView = new UI_DetailView("UI_DetailView", Name);
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

bool MainUI::RenameDetailView(const std::string& OldName, const std::string& NewName)
{
	auto it = AllFileDetails.find(OldName);
	if (it != AllFileDetails.end())
	{
		UI_DetailView* v = it->second;
		AllFileDetails.erase(it);
		AllFileDetails.insert(std::make_pair(NewName, v));
		ShowDetailViewByName(NewName);
		return true;
	}
	else
	{
		return false;
	}
}



bool MainUI::HandleRenameFile(const std::string& OldFullPath, const std::string& NewFullPath)
{
	bool success = true;
	success &= RenameWidgetEditorPage(OldFullPath, NewFullPath);
	success &= RenameOutlineView(OldFullPath, NewFullPath);
	success &= RenameDetailView(OldFullPath, NewFullPath);
	if (CurrentEditedFile == OldFullPath)
	{
		CurrentEditedFile = NewFullPath;
	}
	if (!success)
	{
		//报错
	}
	return success;
}

bool MainUI::HandleCloseFile(const std::string& FileFullPath)
{
	bool success = true;

	success &= RemoveOutlineViewByName(FileFullPath);
	success &= RemoveDetailViewByName(FileFullPath);

	if (!success)
	{

	}
	return success;
}

void MainUI::SwitchCurrentEditFile(const std::string& FileFullPath)
{
	ShowOutlineViewByName(FileFullPath);
	ShowDetailViewByName(FileFullPath);
	ShowWidgetEditorByName(FileFullPath);
	CurrentEditedFile = FileFullPath;
	ExecuteAction(FileFullPath + Action::MainUI::_REQUEST_UPDATE_UNDOREDO_STATE);
}

void MainUI::UpdateLog(std::vector<std::string>&& Logs)
{
	for(auto& singlelog:Logs)
	{
		ImScrollingTextList_LogList->AddItem(singlelog);
	}
}

void MainUI::UpdateUndoRedoState(bool CanUndo, bool CanRedo)
{
	if (CanUndo)
	{
		ImImage_Undo->SetTintcolor(IM_COL32(0, 120, 212, 255));
	}
	else
	{
		ImImage_Undo->SetTintcolor(IM_COL32(158, 158, 158, 255));
	}

	if (CanRedo)
	{
		ImImage_Redo->SetTintcolor(IM_COL32(0, 120, 212, 255));
	}
	else
	{
		ImImage_Redo->SetTintcolor(IM_COL32(158, 158, 158, 255));
	}
}

void MainUI::OnSaveFile()
{
	if (CurrentEditedFile.empty())
	{
		AddLogLine(u8"错误：当前没有打开的文件");
		return;
	}
	ExecuteAction(CurrentEditedFile + Action::MainUI::SAVE_FILE);
}
