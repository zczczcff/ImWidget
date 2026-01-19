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
    void SetIcon(ImTextureID texture);
    Widget_ExampleWidgetButton(const std::string& name, const std::string& displayname, const std::string& WidgetRegisterTypeName, ImTextureID texture):
        ImGuiWidget::ImUserWidget(name),
        m_WidgetTypeName(WidgetRegisterTypeName)
    {
        Init();
        Init2();
        SetDisplayName(displayname);
        SetIcon(texture);
    }
protected:
    virtual void OnDragStart(ImGuiWidget::ImDragStartEvent& e) override;
protected:
//----Gen Members Begin----
    // Auto-generated widget pointers
    ImGuiWidget::ImButton* ImButton_ExampleButton;
    ImGuiWidget::ImHorizontalBox* ImHorizontalBox_0;
    ImGuiWidget::ImImage* ImImage_Icon;
    ImGuiWidget::ImTextBlock* ImTextBlock_WidgetTypeName;
//----Gen Members End----
};
