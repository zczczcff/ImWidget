#pragma once
#include "ImWidget/ImUserWidget.h"
#include "ImWidget/ImBasicWidgetDeclaration.h"
#include "ImTools/ImDelegate.h"
#include <map>
#include "EditorEventObject.h"

class UI_DetailView : public ImGuiWidget::ImUserWidget, public EditorEventObject
{
private:
    struct PropertyInfor
    {
        ImGuiWidget::ImWidget* WidgetOwner;
        std::unordered_map<std::string, std::function<void()>> Updaters;
    };

private:
    std::map<ImGuiWidget::ImWidget*, ImGuiWidget::ImVerticalBox*> CachedDetails;
    std::unordered_map<ImGuiWidget::ImObject*, PropertyInfor*> CachedPropertyInfors;
    ImGuiWidget::ImWidget* CurrentWidget;
    std::string EditedFileFullPath;
    std::vector<EditorActionID> FileActions;
    std::vector<EditorEventID> FileEvents;

public:
    UI_DetailView(const std::string& widgetname, const std::string& EditedFileFullPath) :
        ImGuiWidget::ImUserWidget(widgetname),
        CurrentWidget(nullptr),
        EditedFileFullPath(EditedFileFullPath)
    {
        SetFocusable(true);
        ActionInit();
        EventInit();
    }

private:
    virtual void OnKeyDown(ImGuiWidget::ImKeyDownEvent& e) override;
    void ActionInit();
    void EventInit();
    void ResetEvent();
    void ResetFileAction();

public:
    // 处理字符串数组项的辅助函数
    ImGuiWidget::ImHorizontalBox* HandleAddStringItem(
        const ROP::Property<ImGuiWidget::PropertyType>& prop,
        std::string& SingleString,
        ImGuiWidget::ImVerticalBox* StringListBox,
        ImGuiWidget::ImObject* Target,
        ImGuiWidget::ImWidget* WidgetOwner);

    // 处理单个属性 - 现在使用ROP::Property
    void HandleSingleProperty(
        ROP::Property<ImGuiWidget::PropertyType>& prop,
        ImGuiWidget::ImVerticalBox* CurrentVerticalBox,
        ImGuiWidget::ImObject* Target,
        ImGuiWidget::ImWidget* WidgetOwner,
        std::unordered_map<std::string, std::function<void()>>& Updaters);

    void SetCurrentWidget(ImGuiWidget::ImWidget* widget);

    void ExecutePropertyEditAction(const ROP::Property<ImGuiWidget::PropertyType>& prop,
        const void* NewValue, ImGuiWidget::ImObject* Target);

    void UpdatePropertyDisplay(ImGuiWidget::ImObject* Target, const std::string& PropertyName);

    // 辅助函数：获取属性的显示名称
    std::string GetPropertyDisplayName(const ROP::Property<ImGuiWidget::PropertyType>& prop);

    // 辅助函数：检查是否为选项属性
    bool IsOptionalProperty(const ROP::Property<ImGuiWidget::PropertyType>& prop);

    // 辅助函数：获取选项属性的可选值
    std::vector<std::string> GetOptionalPropertyOptions(const ROP::Property<ImGuiWidget::PropertyType>& prop);
};