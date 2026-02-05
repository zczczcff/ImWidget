#include "UI/MainUI.h"
#include "Model/Model_MainModel.h"
#include "ImWidget/ImBasicWidgetDeclaration.h"

MainUI* global_MainUI;
Model_MainModel* global_MainModel;


ImGuiWidget::ImWidget* ImInit()
{
    ImGuiWidget::RegisterBaseWidget();
    global_MainUI = new MainUI("ImDesigner_MainUI");
    global_MainModel = new Model_MainModel;

    return global_MainUI;
}

void ImTick()
{
    global_MainModel->Tick();
}
