// UI_WidgetEditor.h
#pragma once
#include "ImWidget/ImUserWidget.h"
#include "ImTools/ImDelegate.h"
#include "EditorEventObject.h"
#include "ImUserWidgetClass.h"  // 引入头文件

class UI_WidgetEditor :public ImGuiWidget::ImUserWidget, public EditorEventObject
{
private:
    std::string EditFileFullPath;
    ImGuiWidget::ImUserWidgetClass* m_TargetClass = nullptr;  // 替换 EditorRootWidget

    std::string m_CurrentEditingWidgetVarName;  // 当前编辑的控件树变量名
    bool bAllowOperateChild = false;
    float dashOffset = 0.0f;

    ImGuiWidget::ImWidgetRef SelectedWidgetRef;
    std::string SelectedWidgetVarName;  // 选中控件所属的控件树变量名

    std::vector<EditorActionID> FileActions;

protected:
    // 绘制动态虚线框函数
    void DrawAnimatedDashedRect(
        const ImVec2& min,
        const ImVec2& max,
        ImU32 color,
        float thickness,
        float dashLen,
        float gapLen,
        float& offset,
        float speed = 1.0f
    );

    // 绘制虚线
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

    // 设置当前编辑的控件树
    void SetCurrentEditingWidgetTree(const std::string& widgetVarName);

    // 获取当前编辑的根控件
    ImGuiWidget::ImWidget* GetCurrentRootWidget() const;

    // 查找控件所属的控件树变量名
    std::string FindWidgetVarName(ImGuiWidget::ImWidget* widget) const;

    // 构建控件路径
    std::string BuildWidgetPath(ImGuiWidget::ImWidget* widget) const;

public:
    UI_WidgetEditor(const std::string& name,
        ImGuiWidget::ImUserWidgetClass* targetClass,
        const std::string& EditFileFullPath);

    // 设置选中的控件
    bool SetSelectedWidget(ImGuiWidget::ImWidget* widget);

    // 设置选中的控件（指定控件树变量名）
    bool SetSelectedWidget(const std::string& widgetVarName, ImGuiWidget::ImWidget* widget);

    // 通过控件路径设置选中控件
    bool SetSelectedWidgetByPath(const std::string& widgetPath);

    // 设置当前编辑的控件树
    void SetEditingWidgetTree(const std::string& widgetVarName);

    // 获取当前编辑的控件树变量名
    std::string GetCurrentEditingWidgetVarName() const { return m_CurrentEditingWidgetVarName; }

    // 获取目标类
    ImGuiWidget::ImUserWidgetClass* GetTargetClass() const { return m_TargetClass; }

    // 设置目标类
    void SetTargetClass(ImGuiWidget::ImUserWidgetClass* targetClass);
};
