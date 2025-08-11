#include <ImWidget/ImBasicWidgetList.h>
//#include "ImWidget/ImWidgetCodeGenerator.h"

//#include "ImWidget/ImWidgetSerializer.h"

#include <ImWindows/ImMenuButton.h>
#include <ImWindows/ImPageManager.h>

#include "WidgetTreeView.h"

#include "ImExampleWidget.h"
#include "ImDesignPanel.h"
#include "ImCreator_DetailList.h"
#include "FileUtil.h"
#include "ImCreator_PageManager.h"
#include "ImCreator_Projectconfig.h"
class MyApp
{
public:
    ImGuiWidget::ImVerticalBox* m_Box;
    ImGuiWidget::ImHorizontalSplitter* m_MiddleSplitter;
    ImGuiWidget::ImVerticalSplitter* m_VSplitter;
    //ImGuiWidget::ImCanvasPanel* m_Canvas;
    ImGuiWidget::ImHorizontalBox* m_MenuList;
    ImGuiWidget::ImVerticalBox* m_MainBox;
    ImGuiWidget::ImVerticalBox* m_MiddleBox;
    ImGuiWidget::ImVerticalBox* m_BottomBox;
    //ImGuiWidget::ImMenuButton* Button_Project;
    //ImGuiWidget::ImVerticalBox* m_Menu_ProjectMenu;
    ImWindows::ImMenuButton* m_MenuButton_Project;
    //ImWindows::ImMenuButton* m_MenuButton_Project_History;
    //ImWindows::ImMenuButton* m_MenuButton_Project_History1;
    //ImWindows::ImPageManager* m_CenterPageManager;
    ImCreatorUIPageManager* m_CenterPageManager;
    ImGuiWidget::ImVerticalSplitter* m_WidgetList_WidgetTreeSplitter;
    ImGuiWidget::ImVerticalBox* m_WidgetList;
    WidgetTreeView* m_WidgetTreeView;

    //DesiginPanel* m_DesiginPanel;

    DetailList* m_DetailList;
    //ImGuiWidget::ImVerticalBox* m_DetailList;

    ImGuiWidget::ImScrollingTextList* m_LogList;

    ImGuiWidget::ImInputText* m_InputTextTest;
    ImGuiWidget::ImCheckBox* m_CheckBoxTest;

    //ImGuiWidget::ImScrollBox* m_LogBox;
    ExampleWidget* m_Example_Button;
    ExampleWidget* m_Example_CheckBox;
    ExampleWidget* m_Example_ComboBox;
    ExampleWidget* m_Example_Image;
    ExampleWidget* m_Example_InputText;
    ExampleWidget* m_Example_IntInput;
    ExampleWidget* m_Example_FloatInput;
    ExampleWidget* m_Example_MultiLineTextBlock;
    ExampleWidget* m_Example_ScrollingTextList;
    ExampleWidget* m_Example_Slider;
    ExampleWidget* m_Example_TextBlock;
    
    ExampleWidget* m_Example_ImCanvasPanel;
    ExampleWidget* m_Example_HorizontalBox;
    ExampleWidget* m_Example_HorizontalSplitter;
    ExampleWidget* m_Example_VerticalBox;
    ExampleWidget* m_Example_VerticalSplitter;
    ExampleWidget* m_Example_ExpandableBox;
    ExampleWidget* m_Example_ScrollBox;

    ImCreator_ProjectConfig m_Config;
    void Init()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        // 设置工具提示背景颜色（RGBA，这里设置为半透明的蓝色）
        style.Colors[ImGuiCol_PopupBg] = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);
        // 设置工具提示文本颜色
        style.Colors[ImGuiCol_Text] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);


        m_Box = new ImGuiWidget::ImVerticalBox("Box0");
        m_MiddleSplitter = new ImGuiWidget::ImHorizontalSplitter("Splitter0");
        m_VSplitter = new ImGuiWidget::ImVerticalSplitter("VSplitter0");
        m_MainBox = new ImGuiWidget::ImVerticalBox("MainBox");

        m_MiddleBox= new ImGuiWidget::ImVerticalBox("MiddleBox");
        m_MiddleBox->AddChildToVerticalBox(m_MiddleSplitter);


        m_WidgetList_WidgetTreeSplitter= new ImGuiWidget::ImVerticalSplitter("WidgetList_WidgetTreeSplitter");
        m_WidgetList = new ImGuiWidget::ImVerticalBox("WidgetList");
        m_WidgetTreeView = new WidgetTreeView("WidgetTreeView");
        m_WidgetList_WidgetTreeSplitter->AddPart(m_WidgetList);
        m_WidgetList_WidgetTreeSplitter->AddPart(m_WidgetTreeView);

        //m_DesiginPanel = new DesiginPanel("CenterPanel");
       // m_DetailList= new ImGuiWidget::ImVerticalBox("DetailList");
        m_DetailList = new DetailList("DetailList");


        //m_DesiginPanel->SetOnSelected([this](ImGuiWidget::ImWidget* selectedwidget) 
        //    {
        //        if (selectedwidget)
        //        {
        //            m_DetailList->SetCurrentWidget(selectedwidget);
        //        }
        //    });
        //m_DesiginPanel->SetOnDragWidgetOn([this](ImGuiWidget::ImWidget* NewWidget) 
        //    {
        //        if (!m_WidgetTreeView->GetTargetWidget())
        //        {
        //            m_WidgetTreeView->SetTargetWidget(NewWidget);
        //        }
        //        else
        //        {
        //            m_WidgetTreeView->Refresh();
        //        }
        //    });
        
        m_CenterPageManager = new ImCreatorUIPageManager;

        m_CenterPageManager->SetOnWidgetSelected([this](ImGuiWidget::ImWidget* selectedwidget)
            {
                if (selectedwidget)
                {
                    m_DetailList->SetCurrentWidget(selectedwidget);
                }
            });
        m_CenterPageManager->SetOnWidgetDragedOn([this](ImGuiWidget::ImWidget*) {m_WidgetTreeView->Refresh(); });

        //m_CenterPageManager = new ImWindows::ImPageManager("CenterPageManager",ImWindows::TabDockPosition::Top);
        //m_CenterPageManager->AddPage("test", m_DesiginPanel);
        //m_CenterPageManager->SetTabBarThickness(20.f);

        m_MiddleSplitter->AddPart(m_WidgetList_WidgetTreeSplitter);
        m_MiddleSplitter->AddPart(m_CenterPageManager)->Ratio = 5.0f;
        m_MiddleSplitter->AddPart(m_DetailList);
        
        m_Example_Button = new ExampleWidget("Example_Button", "Button", WidgetType::ImButton);
        m_Example_CheckBox = new ExampleWidget("Example_CheckBox", "CheckBox", WidgetType::ImCheckBox);
        m_Example_ComboBox = new ExampleWidget("Example_ComboBox", "ComboBox", WidgetType::ImComboBox);
        m_Example_Image = new ExampleWidget("Example_Image", "Image", WidgetType::ImImage);
        m_Example_InputText = new ExampleWidget("Example_InputText", "InputText", WidgetType::ImInputText);
        m_Example_IntInput = new ExampleWidget("Example_IntInput", "IntInput", WidgetType::ImIntInput);
        m_Example_FloatInput = new ExampleWidget("Example_FloatInput", "FloatInput", WidgetType::ImFloatInput);
        m_Example_MultiLineTextBlock = new ExampleWidget("Example_MultiLineTextBlock", "MultiLineTextBlock", WidgetType::ImMultiLineTextBlock);
        m_Example_ScrollingTextList = new ExampleWidget("Example_ScrollingTextList", "ScrollingTextList ", WidgetType::ImScrollingTextList);
        m_Example_Slider = new ExampleWidget("Example_Slider ", "Slider", WidgetType::ImSlider);
        m_Example_TextBlock = new ExampleWidget("Example_TextBlock", "TextBlock", WidgetType::ImTextBlock);

        m_Example_ImCanvasPanel = new ExampleWidget("Example_CanvasPanel", "CanvasPanel", WidgetType::ImCanvasPanel);
        m_Example_HorizontalBox = new ExampleWidget("Example_HorizontalBox", "HorizontalBox", WidgetType::ImHorizontalBox);
        m_Example_HorizontalSplitter = new ExampleWidget("Example_HorizontalSplitter", "HorizontalSplitter", WidgetType::ImHorizontalSplitter);
        m_Example_VerticalBox = new ExampleWidget("Example_VerticalBox", "VerticalBox", WidgetType::ImVerticalBox);
        m_Example_VerticalSplitter = new ExampleWidget("Example_VerticalSplitter", "VerticalSplitter", WidgetType::ImVerticalSplitter);
        m_Example_ExpandableBox = new ExampleWidget("Example_ExpandableBox", "ExpandableBox", WidgetType::ImExpandableBox);
        m_Example_ScrollBox = new ExampleWidget("Example_ScrollBox", "ScrollBox", WidgetType::ImScrollBox);

        //ImGuiWidget::ImButton* button_test = new ImGuiWidget::ImButton("buttontest");
        m_WidgetList->AddChildToVerticalBox(m_Example_Button)->SetIfAutoSize(false);
        m_WidgetList->AddChildToVerticalBox(m_Example_CheckBox)->SetIfAutoSize(false);
        m_WidgetList->AddChildToVerticalBox(m_Example_ComboBox)->SetIfAutoSize(false);
        m_WidgetList->AddChildToVerticalBox(m_Example_Image)->SetIfAutoSize(false);
        m_WidgetList->AddChildToVerticalBox(m_Example_InputText)->SetIfAutoSize(false);
        m_WidgetList->AddChildToVerticalBox(m_Example_IntInput)->SetIfAutoSize(false);
        m_WidgetList->AddChildToVerticalBox(m_Example_FloatInput)->SetIfAutoSize(false);
        m_WidgetList->AddChildToVerticalBox(m_Example_MultiLineTextBlock)->SetIfAutoSize(false);
        m_WidgetList->AddChildToVerticalBox(m_Example_ScrollingTextList)->SetIfAutoSize(false);
        m_WidgetList->AddChildToVerticalBox(m_Example_Slider)->SetIfAutoSize(false);
        m_WidgetList->AddChildToVerticalBox(m_Example_TextBlock)->SetIfAutoSize(false);
        
        m_WidgetList->AddChildToVerticalBox(m_Example_ImCanvasPanel)->SetIfAutoSize(false);
        m_WidgetList->AddChildToVerticalBox(m_Example_HorizontalBox)->SetIfAutoSize(false);
        m_WidgetList->AddChildToVerticalBox(m_Example_HorizontalSplitter)->SetIfAutoSize(false);
        m_WidgetList->AddChildToVerticalBox(m_Example_VerticalBox)->SetIfAutoSize(false);
        m_WidgetList->AddChildToVerticalBox(m_Example_VerticalSplitter)->SetIfAutoSize(false);
        m_WidgetList->AddChildToVerticalBox(m_Example_ExpandableBox)->SetIfAutoSize(false);
        m_WidgetList->AddChildToVerticalBox(m_Example_ScrollBox)->SetIfAutoSize(false);

        m_BottomBox=new ImGuiWidget::ImVerticalBox("BottomBox");
        m_VSplitter->AddPart(m_MiddleBox)->Ratio = 4.0f;
        m_VSplitter->AddPart(m_BottomBox);

        m_LogList = new ImGuiWidget::ImScrollingTextList("LogList");
        for (int i = 0; i < 20; i++)
        {
            m_LogList->AddItem("abcdesafsafefa tesrt skanf isf nin aifn ansifasf nia fni asfaf",IM_COL32(150,100,10*i,255));
        }
        

        m_BottomBox->AddChildToVerticalBox(m_LogList)->SetIfAutoSize(true);

        m_MenuList = new ImGuiWidget::ImHorizontalBox("m_MenuList");
        m_MenuList->SetBackGroundColor(IM_COL32(50, 50, 230, 255));
        //Button_Project = new ImGuiWidget::ImMenuButton("Button_Project");

        m_MenuButton_Project = new ImWindows::ImMenuButton("MenuButton_Project");

        m_MenuButton_Project->GetNormalStyle().BackgroundColor = IM_COL32(210, 180, 150, 120);
        m_MenuButton_Project->GetHoveredStyle().BackgroundColor = IM_COL32(230, 200, 170, 120);
        m_MenuButton_Project->GetPressedStyle().BackgroundColor = IM_COL32(250, 220, 190, 120);
        m_MenuButton_Project->SetTooltipText("Project");
        //m_MenuButton_Project_History = new ImWindows::ImMenuButton("MenuButton_Project_History");
        //m_MenuButton_Project_History->SetDockDirection(ImWindows::MenuDockDirection::Dock_Right);
        //m_MenuButton_Project_History1 = new ImWindows::ImMenuButton("MenuButton_Project_History1");
        //m_MenuButton_Project_History1->SetDockDirection(ImWindows::MenuDockDirection::Dock_Right);
        ImGuiWidget::ImTextBlock* Button_Project_Text = new ImGuiWidget::ImTextBlock("Button_Project_Text");
        Button_Project_Text->SetText("Project");
        
        m_MenuButton_Project->SetContent(Button_Project_Text);
        //ImGuiWidget::ImButton* m_MenuButton_Project;
        //auto properties = m_MenuButton_Project->GetProperties();
        //for (auto& SingleProperty : properties)
        //{
        //    if (SingleProperty.name == "TooltipText")
        //    {
        //        //设置TooltipText
        //        std::string Temp_TooltipText = "Test";
        //        SingleProperty.setter(&Temp_TooltipText);
        //    }
        //    else if (SingleProperty.name == "NormalStyle")
        //    {
        //        //此处递归展开生成代码
        //    }
        //}
        
        //ImGuiWidget::LoadWidgetFromFile(m_MenuButton_Project, "test.imui");
        
        
        ImWindows::ImMenuButton* Button_NewUI = new ImWindows::ImMenuButton("Button_NewUI");
        ImGuiWidget::ImTextBlock* NewUI_MenuText = new ImGuiWidget::ImTextBlock("NewUI_MenuText");
        NewUI_MenuText->SetText("NewUI");
        Button_NewUI->SetContent(NewUI_MenuText);
        ImGuiWidget::ImVerticalBox* NewUI_VBox = new ImGuiWidget::ImVerticalBox("NewUI_VBox");
        ImGuiWidget::ImInputText* NewUI_InputNameBox = new ImGuiWidget::ImInputText("NewUI_InputNameBox");
        ImGuiWidget::ImButton* NewUI_CreateButton = new ImGuiWidget::ImButton("NewUI_CreateButton");
        ImGuiWidget::ImTextBlock* NewUI_Text = new ImGuiWidget::ImTextBlock("NewUI_Text");
        NewUI_Text->SetText("Create");
        NewUI_CreateButton->SetContent(NewUI_Text);
        NewUI_CreateButton->SetOnPressed([this, NewUI_InputNameBox]() 
            {
                std::string NewFileName = NewUI_InputNameBox->GetText();
                if (m_CenterPageManager->CreateNewUIFile(NewFileName))return;
                m_DetailList->CreateNewFileDetail(NewFileName);
                m_DetailList->SetActiveFileDetail(NewFileName);
                m_WidgetTreeView->CreateNewTreeView(NewFileName, m_CenterPageManager->GetMainPanelByName(NewFileName));
                m_WidgetTreeView->SetActiveTreeView(NewFileName);
            });
        NewUI_VBox->AddChildToVerticalBox(NewUI_InputNameBox)->SetIfAutoSize(false);
        NewUI_VBox->AddChildToVerticalBox(NewUI_CreateButton)->SetIfAutoSize(false);
        Button_NewUI->AddMenuOption(NewUI_VBox);


        ImGuiWidget::ImButton* Button_SaveUI = new ImGuiWidget::ImButton("Button_SaveUI");
        ImGuiWidget::ImTextBlock* SaveUI_MenuText = new ImGuiWidget::ImTextBlock("SaveUI_MenuText");
        SaveUI_MenuText->SetText("Save");
        Button_SaveUI->SetContent(SaveUI_MenuText);
        Button_SaveUI->SetOnPressed([this]() 
            {
                m_CenterPageManager->SaveCurrentUIFile(m_Config.UIFolder);
            });

        ImGuiWidget::ImButton* Button_GenCode = new ImGuiWidget::ImButton("Button_GenCode");
        ImGuiWidget::ImTextBlock* GenCode_MenuText = new ImGuiWidget::ImTextBlock("GenCode_MenuText");
        GenCode_MenuText->SetText("Gen Code");
        Button_GenCode->SetContent(GenCode_MenuText);
        Button_GenCode->SetOnPressed([this]()
            {
                m_CenterPageManager->GenerateCode(m_Config.CPPFolder, m_Config.HeaderFolder);
            });

        m_MenuList->AddChildToHorizontalBox(m_MenuButton_Project)->SetIfAutoSize(false);
        m_MenuList->AddChildToHorizontalBox(Button_NewUI)->SetIfAutoSize(false);
        m_MenuList->AddChildToHorizontalBox(Button_SaveUI)->SetIfAutoSize(false);
        m_MenuList->AddChildToHorizontalBox(Button_GenCode)->SetIfAutoSize(false);

        m_MainBox->AddChildToVerticalBox(m_MenuList)->SetIfAutoSize(false);
        m_MainBox->AddChildToVerticalBox(m_VSplitter);


        auto AllUiFilesPath = FileUtil::getFilesWithExtension(m_Config.UIFolder, ".imui");

        for (auto& FullFileName : AllUiFilesPath)
        {
            std::string FileName = FileUtil::getPureFileName(FullFileName);
            ImWindows::ImMenuButton* FileChooseButton = new ImWindows::ImMenuButton("FileChooseButton");
            ImGuiWidget::ImTextBlock* FileChooseButtonText = new ImGuiWidget::ImTextBlock("FileChooseButtonText");
            FileChooseButtonText->SetText(FileName);
            FileChooseButton->SetContent(FileChooseButtonText);
            m_MenuButton_Project->AddMenuOption(FileChooseButton);
            FileChooseButton->SetOnPressed([FileName,FullFileName,this]()
                {
                    if (!m_CenterPageManager->AddEditedPage(FileName,FullFileName))return;
                    m_DetailList->CreateNewFileDetail(FileName);
                    m_DetailList->SetActiveFileDetail(FileName);
                    m_WidgetTreeView->CreateNewTreeView(FileName, m_CenterPageManager->GetMainPanelByName(FileName));
                    m_WidgetTreeView->SetActiveTreeView(FileName);
                });
        }

        m_WidgetTreeView->SetOnSelectionChanged([this](ImGuiWidget::ImWidget* SelectedWidget) 
            {
                m_CenterPageManager->GetCurrentMainPanel()->SetSelectedWidget(SelectedWidget);
            });

        m_WidgetTreeView->SetOnWidgetDeleted([this](ImGuiWidget::ImWidget* DeletedWidget) 
            {
                if (m_WidgetTreeView->GetSelectedWidget()&& m_WidgetTreeView->GetSelectedWidget() == DeletedWidget)
                {
                    m_DetailList->SetCurrentWidget(nullptr);
                }
            });

    }
    void Render()
    {
        m_MainBox->SetPosition(ImVec2(1.f, 1.f));
        m_MainBox->SetSize(ImGui::GetWindowSize()-ImVec2(2.f,2.f));
        m_MainBox->Render();

        //if (bRenderMenu_ProjectMenu)
        //{
        //    m_Menu_ProjectMenu->Render();
        //}

        //m_Canvas->Render();
    }
};

MyApp* app;
int ImInit()
{
    app = new MyApp;
    app->Init();
    return 1;
}

void ImTick()
{
    app->Render();
}