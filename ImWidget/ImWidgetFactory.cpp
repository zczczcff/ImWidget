#include "ImWidgetFactory.h"

#include "ImButton.h"
#include "ImCanvasPanel.h"
#include "ImCheckBox.h"
#include "ImColorPicker.h"
#include "ImComboBox.h"
#include "ImDesignPanel.h"
#include "ImExpandableBox.h"
#include "ImHorizontalBox.h"
#include "ImHorizontalSplitter.h"
#include "ImImage.h"
#include "ImInputText.h"
#include "ImMultiLineTextBlock.h"
#include "ImResizableBox.h"
#include "ImScrollBox.h"
#include "ImScrollingTextList.h"
#include "ImSlider.h"
#include "ImTextBlock.h"
#include "ImVerticalBox.h"
#include "ImVerticalSplitter.h"

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
