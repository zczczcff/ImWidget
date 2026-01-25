#pragma once
#include "ImWidget/ImUserWidget.h"
#include "ImTools/ImDelegate.h"
#include "EditorGlobalInterface.h"

class UI_WidgetEditor :public ImGuiWidget::ImUserWidget,public EditorGlobalInterface
{
private:
    std::string EditFileFullPath;
	ImGuiWidget::ImWidget* EditorRootWidget;
	bool bAllowOperateChild = false;
	float dashOffset = 0.0f;
    ImGuiWidget::ImWidgetRef SelectedWidgetRef;
    std::vector<EditorActionID> FileActions;
public:
    //ImMulticastDelegate<ImGuiWidget::ImWidget*> OnWidgetSelected;
    //ImMulticastDelegate<> OnRequestUndo;
protected:
    // 动态虚线框绘制函数
    void DrawAnimatedDashedRect(
        const ImVec2& min,              // 方框左上角
        const ImVec2& max,              // 方框右下角
        ImU32 color,                    // 虚线颜色
        float thickness,               // 虚线粗细
        float dashLen,                 // 每段虚线长度
        float gapLen,                  // 每段间隔长度
        float& offset,                 // 动画偏移量（需要外部维护）
        float speed = 1.0f             // 移动速度
    );

    // 绘制带偏移量的虚线
    void DrawDashedLine(
        ImDrawList* drawList,
        const ImVec2& start,
        const ImVec2& end,
        ImU32 color,
        float thickness,
        float dashLen,
        float gapLen,
        float offset
    );

	virtual void HandleEventInternal(ImGuiWidget::ImEvent* event) override;
	virtual void OnMouseDown(ImGuiWidget::ImMouseDownEvent& e)override;
    virtual void OnKeyDown(ImGuiWidget::ImKeyDownEvent& e) override;
    virtual void PostRender() override;

    void ActionInit();
    void ResetAction();
public:
	UI_WidgetEditor(const std::string& name, ImGuiWidget::ImWidget* EditorRootWidget,const std::string& EditFileFullPath);
    bool SetSelectedWidget(ImGuiWidget::ImWidget* widget);
};