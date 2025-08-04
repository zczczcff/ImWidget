#include "ImWidgetFactory.h"

#include "ImBasicWidgetList.h"

/**
* @brief 初始化工厂，注册所有内置控件类型
*/

void ImGuiWidget::ImWidgetFactory::Initialize()
{
    // 注册所有内置控件类型
    RegisterType<ImWidget>("ImWidget");
    RegisterType<ImPanelWidget>("ImPanelWidget");
    RegisterType<ImCanvasPanel>("ImCanvasPanel");
    RegisterType<ImHorizontalBox>("ImHorizontalBox");
    RegisterType<ImVerticalBox>("ImVerticalBox");
    RegisterType<ImHorizontalSplitter>("ImHorizontalSplitter");
    RegisterType<ImVerticalSplitter>("ImVerticalSplitter");

    // 可以根据需要添加更多控件
}
