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
    bool m_IsUpdatingProperty = false;  // 防止Updater触发回调导致的循环

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
    // �����ַ���������ĸ�������
    ImGuiWidget::ImHorizontalBox* HandleAddStringItem(
        const ROP::Property<ImGuiWidget::PropertyType>& prop,
        std::string& SingleString,
        ImGuiWidget::ImVerticalBox* StringListBox,
        ImGuiWidget::ImObject* Target,
        ImGuiWidget::ImWidget* WidgetOwner);

    // ������������ - ����ʹ��ROP::Property
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

    // ������������ȡ���Ե���ʾ����
    std::string GetPropertyDisplayName(const ROP::Property<ImGuiWidget::PropertyType>& prop);

    // ��������������Ƿ�Ϊѡ������
    bool IsOptionalProperty(const ROP::Property<ImGuiWidget::PropertyType>& prop);

    // ������������ȡѡ�����ԵĿ�ѡֵ
    std::vector<std::string> GetOptionalPropertyOptions(const ROP::Property<ImGuiWidget::PropertyType>& prop);
};