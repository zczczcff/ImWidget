#include "ImWidgetFactory.h"

#include "ImBasicWidgetList.h"
#include "ImBasicWidgetDeclaration.h"

/**
* @brief 初始化工厂，注册所有内置控件类型
*/

namespace ImGuiWidget
{
    void RegisterBaseWidget()
    {
        REGISTER_WIDGET_TYPE(ImGuiWidget::ImCanvasPanel);
        REGISTER_WIDGET_TYPE(ImGuiWidget::ImHorizontalBox);
        REGISTER_WIDGET_TYPE(ImGuiWidget::ImVerticalBox);
        REGISTER_WIDGET_TYPE(ImGuiWidget::ImHorizontalSplitter);
        REGISTER_WIDGET_TYPE(ImGuiWidget::ImVerticalSplitter);
        REGISTER_WIDGET_TYPE(ImGuiWidget::ImButton);
        REGISTER_WIDGET_TYPE(ImGuiWidget::ImCheckBox);
        REGISTER_WIDGET_TYPE(ImGuiWidget::ImColorPicker);
        REGISTER_WIDGET_TYPE(ImGuiWidget::ImComboBox);
        REGISTER_WIDGET_TYPE(ImGuiWidget::ImDesignPanel);
        REGISTER_WIDGET_TYPE(ImGuiWidget::ImExpandableBox);
        REGISTER_WIDGET_TYPE(ImGuiWidget::ImImage);
        REGISTER_WIDGET_TYPE(ImGuiWidget::ImInputText);
        REGISTER_WIDGET_TYPE(ImGuiWidget::ImIntInput);
        REGISTER_WIDGET_TYPE(ImGuiWidget::ImFloatInput);
        REGISTER_WIDGET_TYPE(ImGuiWidget::ImMultiLineTextBlock);
        REGISTER_WIDGET_TYPE(ImGuiWidget::ImResizableBox);
        REGISTER_WIDGET_TYPE(ImGuiWidget::ImScrollBox);
        REGISTER_WIDGET_TYPE(ImGuiWidget::ImScrollingTextList);
        REGISTER_WIDGET_TYPE(ImGuiWidget::ImSlider);
        REGISTER_WIDGET_TYPE(ImGuiWidget::ImTextBlock);
        REGISTER_WIDGET_TYPE(ImGuiWidget::ImSimpleFigure);
        REGISTER_WIDGET_TYPE(ImGuiWidget::ImBorder);
        REGISTER_WIDGET_TYPE(ImGuiWidget::ImCurve);
    }

}