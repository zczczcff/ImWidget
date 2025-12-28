#pragma once
#include "ImUserWidget.h"

#include "ImBasicWidgetDeclaration.h"

class Widget_ExampleWidgetButton : public ImGuiWidget::ImUserWidget
{
private:
    std::string m_WidgetTypeName;
    std::string m_DisplayTypeName;
public:
    void Init();
    void Init2();
    void SetDisplayName(const std::string& displayname);
    Widget_ExampleWidgetButton(const std::string& name, const std::string& displayname, const std::string& WidgetTypeName): 
        ImGuiWidget::ImUserWidget(name),
        m_WidgetTypeName(WidgetTypeName)
    {
        Init();
        Init2();
        SetDisplayName(displayname);
    }
protected:
    virtual void OnDragStart(ImGuiWidget::ImDragStartEvent& e) override;
protected:
//----Gen Members Begin----
    // Auto-generated widget pointers
    ImGuiWidget::ImButton* ImButton_ExampleButton;
    ImGuiWidget::ImTextBlock* ImTextBlock_WidgetTypeName;
//----Gen Members End----
};
