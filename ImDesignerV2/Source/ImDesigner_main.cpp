#include "UI/MainUI.h"

#include "ImWidget/ImBasicWidgetDeclaration.h"

MainUI* global_MainUI;



ImGuiWidget::ImWidget* ImInit()
{
    ImGuiWidget::RegisterBaseWidget();
    global_MainUI = new MainUI("ImDesigner_MainUI");
    
    return global_MainUI;
}

void ImTick()
{

}