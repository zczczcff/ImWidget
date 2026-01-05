#include "UI/UI_ProjectView.h"
#include "ImWidget/ImBasicWidgetList.h"
#include "Tools/ProjectFileManager.h"
#include "UI/IconManager.h"
#include "Application/ImApplication.h"
#include "ImWidget/ImImage.h"
#include "Model/FileUtil.h"
#include "ImGlobal.h"
#include "ImTools/DelayEventQueue.h"

UI_ProjectView::UI_ProjectView(const std::string& name)
    : ImGuiWidget::ImUserWidget(name)
{
    Init();
    InitPopUpMenu();
}

void UI_ProjectView::Init()
{
    ImScrollBox_Folder = new ImGuiWidget::ImScrollBox("ImScrollBox_Folder");
    ImVerticalBox_Folder = new ImGuiWidget::ImVerticalBox("ImVerticalBox_Folder");
    ImInputText_Rename = new ImGuiWidget::ImInputText("ImInputText_Rename");

    ImScrollBox_Folder->bHaveBorder = false;
    ImVerticalBox_Folder->bHaveBorder = false;
    ImScrollBox_Folder->SetContent(ImVerticalBox_Folder);

    // 设置根组件
    SetRootWidget(ImScrollBox_Folder);
}

void UI_ProjectView::InitPopUpMenu()
{
    //创建弹出菜单
    ImVerticalBox_FolderOperatorMenu = new ImGuiWidget::ImVerticalBox("ImVerticalBox_FolderOperatorMenu");
    // 创建弹出菜单窗口
    m_FolderOperatorMenuWindow = ImGuiWidget::GlobalApp->GetWindowManager()->CreatePopupWindow(ImVec2(0, 0), ImVec2(0, 0), ImVerticalBox_FolderOperatorMenu, false);
    m_FolderOperatorMenuWindow->Close();

    ImGuiWidget::ImButton* ImButton_CreateNewFile = CreateWidgetMenuButton(u8"新建", m_FolderOperatorMenuWindow);
    
    ImVerticalBox_FolderOperatorMenu->AddChildToVerticalBox(ImButton_CreateNewFile)->SetIfAutoSize(false);
    ImButton_CreateNewFile->OnLeftClicked.Add([this]() { OnRequestCreateFileInDir.Broadcast(CurrentOperatedDirPath); });

}

ImGuiWidget::ImButton* UI_ProjectView::CreateWidgetMenuButton(const std::string& Text, ImGuiWidget::ImWindow* BindPopupWindow)
{
    ImGuiWidget::ImButton* button = new ImGuiWidget::ImButton("UI_WidgetTreeView_WidgetMenuButton");
    ImGuiWidget::ImTextBlock* text = new ImGuiWidget::ImTextBlock("UI_WidgetTreeView_WidgetMenuButton_Text");
    text->SetText(Text);
    text->SetHorizontalAlignment(ImGuiWidget::ImTextBlock::TextAlignment_Horizontal::Left);
    button->SetContent(text);
    button->GetContentSlot()->SetPadding(2, 2, 10, 10);

    button->bHaveBorder = false;
    button->OnLeftClicked.Add([BindPopupWindow]() { BindPopupWindow->Close(); });
    return button;
}

void UI_ProjectView::SetProjectViewVBoxContent(ProjectFileManager* projectmananger, ImGuiWidget::ImVerticalBox* Vbox, const std::string& CurrentPath)
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
        SetupButtonStyle(DirButton);
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
        FileButton->OnLeftClicked.Add([this, file]() { On_UIFileButtonClicked(file.filename, file.fullPath); });
        FileButton->OnDoubleClicked.Add([this, file]() { ActivateFileRename(file.fullPath); });
        SetupButtonStyle(FileButton);
        BodyHBox->AddChildToHorizontalBox(Icon)->SetIfAutoSize(false);
        BodyHBox->AddChildToHorizontalBox(FileButton);
        BodyHBox->bHaveBorder = false;
        Vbox->AddChildToVerticalBox(BodyHBox)->SetIfAutoSize(false);
        FileFullPathToFileBodyHBox.insert(std::make_pair(file.fullPath, BodyHBox));
    }

    Vbox->bHaveBorder = false;
}

void UI_ProjectView::SetupButtonStyle(ImGuiWidget::ImButton* filebutton)
{
    // 正常状态样式 - 稍深的灰色
    ImGuiWidget::ButtonStateStyle normalStyle;
    normalStyle.BackgroundColor = IM_COL32(220, 230, 245, 255);
    normalStyle.Rounding = 0.0f;
    normalStyle.HasBorder = false;
    normalStyle.BorderThickness = 1.0f;
    normalStyle.BorderColor = IM_COL32(190, 190, 190, 255);
    filebutton->SetNormalStyle(normalStyle);

    // 悬停状态样式 - 稍深的蓝色调灰色
    ImGuiWidget::ButtonStateStyle hoverStyle;
    hoverStyle.BackgroundColor = IM_COL32(205, 215, 235, 255);
    hoverStyle.Rounding = 0.0f;
    hoverStyle.HasBorder = false;
    hoverStyle.BorderThickness = 1.0f;
    hoverStyle.BorderColor = IM_COL32(170, 170, 170, 255);
    filebutton->SetHoveredStyle(hoverStyle);

    // 按下状态样式 - 更深的蓝色调灰色
    ImGuiWidget::ButtonStateStyle pressedStyle;
    pressedStyle.BackgroundColor = IM_COL32(185, 200, 225, 255);
    pressedStyle.Rounding = 0.0f;
    pressedStyle.HasBorder = false;
    pressedStyle.BorderThickness = 1.0f;
    pressedStyle.BorderColor = IM_COL32(150, 150, 150, 255);
    filebutton->SetPressedStyle(pressedStyle);

    // 选中状态样式（焦点状态）- 蓝色高亮
    ImGuiWidget::ButtonStateStyle selectedStyle;
    selectedStyle.BackgroundColor = IM_COL32(100, 149, 237, 255);
    selectedStyle.Rounding = 0.0f;
    selectedStyle.HasBorder = false;
    selectedStyle.BorderThickness = 1.0f;
    selectedStyle.BorderColor = IM_COL32(70, 130, 180, 255);
    filebutton->SetFocusedStyle(selectedStyle);
}

void UI_ProjectView::InitDirButton(ImGuiWidget::ImButton* Dirbutton, const std::string& dir)
{
    Dirbutton->OnRightClicked.Add([this, dir]()
        {
            CurrentOperatedDirPath = dir;
            PopupDirRightKeyWindow();
        });
}

void UI_ProjectView::PopupDirRightKeyWindow()
{
    m_FolderOperatorMenuWindow->SetPosition(ImGuiWidget::GetMousePos());
    m_FolderOperatorMenuWindow->SetSize(ImVerticalBox_FolderOperatorMenu->GetMinSize());
    m_FolderOperatorMenuWindow->SetActive();
    //ImGuiWidget::ImVerticalBox* ImVerticalBox_DirRightKeyMenu = new ImGuiWidget::ImVerticalBox("ImVerticalBox_DirRightKeyMenu");
    //ImGuiWidget::ImButton* OptionButton = new ImGuiWidget::ImButton(dir + "_OptionButton");
    //ImGuiWidget::ImTextBlock* Text_NewFile = new ImGuiWidget::ImTextBlock(dir + "_Text");
    //Text_NewFile->SetText(u8"在" + dir + u8"中创建UI文件");
    //OptionButton->SetContent(Text_NewFile);
    //ImVerticalBox_DirRightKeyMenu->AddChildToVerticalBox(OptionButton);
    //PopupRightKeyWindow(ImVerticalBox_DirRightKeyMenu);
}

//void UI_ProjectView::PopupRightKeyWindow(ImGuiWidget::ImWidget* rootwidget)
//{
//    m_FolderOperatorMenuWindow->SetPosition(ImGuiWidget::GlobalApp->GetCurrentMousePos());
//    m_FolderOperatorMenuWindow->SetRootWidget(rootwidget, true);
//    m_FolderOperatorMenuWindow->SetSize(rootwidget->GetMinSize());
//    m_FolderOperatorMenuWindow->SetActive();
//    ImGuiWidget::GlobalApp->GetWindowManager()->SetActiveWindow(m_FolderOperatorMenuWindow);
//}

void UI_ProjectView::On_UIFileButtonClicked(const std::string& FileName, const std::string& FileFullPath)
{
    OnUIFileSelected.Broadcast(FileName, FileFullPath);
}

void UI_ProjectView::ExpandToFile(const std::string& FileFullPath)
{
    auto it = FileFullPathToFileBodyHBox.find(FileFullPath);
    if (it != FileFullPathToFileBodyHBox.end())
    {
        ImGuiWidget::ImWidget* currentwidget = it->second;
        while (currentwidget != ImVerticalBox_Folder && currentwidget)//递归展开路径上的所有展开框
        {
            if (ImGuiWidget::ImExpandableBox* exbox = dynamic_cast<ImGuiWidget::ImExpandableBox*>(currentwidget))
            {
                exbox->SetExpandedState(true);
            }
            currentwidget = currentwidget->GetParents();
        }
    }
}

//void UI_ProjectView::On_CommitFileRename(const std::string& OldFullPath, const std::string& NewFullPath)
//{
//    OnFileRenamed.Broadcast(OldFullPath, NewFullPath);
//}

void UI_ProjectView::ActivateFileRename(const std::string& FileFullPath, bool ScrollToTarget)
{
    
    auto it = FileFullPathToFileBodyHBox.find(FileFullPath);
    if (it != FileFullPathToFileBodyHBox.end())
    {
        ExpandToFile(FileFullPath);
        ImGuiWidget::ImButton* OldButton = (ImGuiWidget::ImButton*)it->second->ExtractChildAt(1);
        ImGuiWidget::ImTextBlock* OldText = (ImGuiWidget::ImTextBlock*)OldButton->GetContent();
        it->second->RemoveChildAt(1, false);
        
        it->second->AddChildToHorizontalBox(ImInputText_Rename);
        if (ScrollToTarget)
        {
            ImGuiWidget::GetGlobalApp()->GetEventQueue()->AddDelayedFrameEvent([this]()
                {
                    ImScrollBox_Folder->ScrollToWidget(ImInputText_Rename);
                },1);
            ImScrollBox_Folder->ScrollToWidget(it->second);
        }
        
        std::string FileName = FileUtil::getFileNameWithExtension(FileFullPath);
        std::string Dir = FileUtil::getParentDirectory(FileFullPath);
        ImInputText_Rename->SetText(FileName);
        ImInputText_Rename->RequestFocus();
        ImInputText_Rename->OnTextCommit.Add([this,Dir,FileFullPath,it, OldButton, OldText](const std::string& NewFileName)
            {
                OnFileRenamed.Broadcast(FileFullPath, Dir + "/" + NewFileName);
                //if (OnFileRenamed && OnFileRenamed(FileFullPath, Dir + "/" + NewFileName))//重命名成功
                //{
                //    OldText->SetText(NewFileName);
                //}
                //else//重命名失败
                //{

                //}
                //it->second->RemoveChild(ImInputText_Rename);
                //it->second->AddChildToHorizontalBox(OldButton);
            });
    }
}

void UI_ProjectView::UpdateProjectView(ProjectFileManager* projectmananger)
{
    ImVerticalBox_Folder->RemoveAllChild(true);
    FileFullPathToFileBodyHBox.clear();
    SetProjectViewVBoxContent(projectmananger, ImVerticalBox_Folder, "");
}