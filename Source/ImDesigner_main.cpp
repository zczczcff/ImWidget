#include "UI/MainUI.h"
#include "Model/Model_MainModel.h"
#include "Controller/Controller_MainController.h"
#include "ImWidget/ImBasicWidgetDeclaration.h"

MainUI* global_MainUI;
Model_MainModel* global_MainModel;
Controller_MainController* global_MainController;


ImGuiWidget::ImWidget* ImInit()
{
    ImGuiWidget::RegisterBaseWidget();
    global_MainUI = new MainUI("ImDesigner_MainUI");
    global_MainModel = new Model_MainModel;
    global_MainController = new Controller_MainController(global_MainUI, global_MainModel);
    
    return global_MainUI;
}

void ImTick()
{
    global_MainModel->Tick();
    //app->Render();
}