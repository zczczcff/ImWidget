#include "UI/MainUI.h"


MainUI* m_MainUI;



ImGuiWidget::ImWidget* ImInit()
{
    m_MainUI = new MainUI("ImDesigner_MainUI");
    return m_MainUI;
}

void ImTick()
{
    //app->Render();
}