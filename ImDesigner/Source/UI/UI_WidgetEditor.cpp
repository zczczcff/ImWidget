// UI_WidgetEditor.cpp
#include "UI/UI_WidgetEditor.h"
#include "EditorAction.h"

// 绘制动态虚线框函数
void UI_WidgetEditor::DrawAnimatedDashedRect(const ImVec2& min, const ImVec2& max, ImU32 color, float thickness, float dashLen, float gapLen, float& offset, float speed)
{
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    // 偏移增量实现动画效果
    offset += speed;
    if (offset >= dashLen + gapLen)
    {
        offset -= (dashLen + gapLen);
    }

    // 计算矩形四个角
    ImVec2 topLeft = min;
    ImVec2 topRight = ImVec2(max.x, min.y);
    ImVec2 bottomRight = max;
    ImVec2 bottomLeft = ImVec2(min.x, max.y);

    // 计算四条边的总长度
    float topLen = topRight.x - topLeft.x;
    float rightLen = bottomRight.y - topRight.y;
    float bottomLen = bottomRight.x - bottomLeft.x;
    float leftLen = bottomLeft.y - topLeft.y;

    // 绘制顶部虚线
    DrawDashedLine(drawList, topLeft, topRight, color, thickness, dashLen, gapLen, offset);

    // 绘制右侧虚线
    DrawDashedLine(drawList, topRight, bottomRight, color, thickness, dashLen, gapLen, offset - topLen);

    // 绘制底部虚线（从右到左）
    DrawDashedLine(drawList, bottomRight, bottomLeft, color, thickness, dashLen, gapLen, offset - topLen - rightLen);

    // 绘制左侧虚线（从下到上）
    DrawDashedLine(drawList, bottomLeft, topLeft, color, thickness, dashLen, gapLen, offset - topLen - rightLen - bottomLen);
}

// 绘制带偏移量的虚线
void UI_WidgetEditor::DrawDashedLine(ImDrawList* drawList, const ImVec2& start, const ImVec2& end, ImU32 color, float thickness, float dashLen, float gapLen, float offset)
{
    // 计算线段方向和长度
    ImVec2 dir = ImVec2(end.x - start.x, end.y - start.y);
    float length = sqrtf(dir.x * dir.x + dir.y * dir.y);
    dir.x /= length;
    dir.y /= length;

    // 计算周期长度
    float period = dashLen + gapLen;

    // 开始偏移量处理
    float currentPos = fmod(offset, period);
    if (currentPos < 0) currentPos += period;

    // 绘制线段，循环处理
    float traveled = 0.0f;
    while (traveled < length)
    {
        // 计算当前周期的剩余长度
        float remaining = length - traveled;

        // 计算当前相位的位置
        float phasePos = fmod(currentPos, period);

        if (phasePos < dashLen)
        {
            // 应该绘制虚线的部分
            float dashStart = traveled;
            float dashEnd = dashStart + fmin(dashLen - phasePos, remaining);

            if (dashEnd > dashStart)
            {
                ImVec2 p1 = ImVec2(
                    start.x + dir.x * dashStart,
                    start.y + dir.y * dashStart
                );
                ImVec2 p2 = ImVec2(
                    start.x + dir.x * dashEnd,
                    start.y + dir.y * dashEnd
                );
                drawList->AddLine(p1, p2, color, thickness);
            }

            traveled += (dashEnd - dashStart);
            currentPos += (dashEnd - dashStart);
        }
        else
        {
            // 间隙部分
            float gapRemaining = period - phasePos;
            float move = fmin(gapRemaining, remaining);
            traveled += move;
            currentPos += move;
        }
    }
}

void UI_WidgetEditor::HandleEventInternal(ImGuiWidget::ImEvent* event)
{
    ImGuiWidget::ImUserWidget::HandleEventInternal(event);
    if (!bAllowOperateChild && event->GetPhase() == ImGuiWidget::ImEventPhase::Capture)
    {
        event->StopPropagation();
    }
}

void UI_WidgetEditor::OnMouseDown(ImGuiWidget::ImMouseDownEvent& e)
{
    if (!(e.GetButton() == ImGuiWidget::ImMouseButton::Left || e.GetButton() == ImGuiWidget::ImMouseButton::Right))return;

    if (!m_TargetClass) return;

    // 如果用户没有指定当前编辑的控件树则返回
    if (m_CurrentEditingWidgetVarName.empty()) return;

    ImGuiWidget::ImWidget* currentRoot = GetCurrentRootWidget();
    if (!currentRoot) return;

    ImGuiWidget::ImWidget* HitChild = currentRoot->ChildHitTest(e.GetPosition());
    if (HitChild)
    {
        // 构建控件路径：控件树变量名 + 相对路径
        std::string widgetPath = BuildWidgetPath(HitChild);

        // 发布控件选中动作，使用单参数（控件路径）
        ExecuteAction(EditFileFullPath + Action::WIDGET_SELECTED, widgetPath);
    }
    e.StopPropagation();
}

void UI_WidgetEditor::OnKeyDown(ImGuiWidget::ImKeyDownEvent& e)
{
    if (bHasFocus && e.IsCtrl_Z())
    {
        ExecuteAction(EditFileFullPath + Action::_REQUEST_UNDO);
    }
}

void UI_WidgetEditor::PostRender()
{
    // 检查选中控件是否仍然有效
    if (SelectedWidgetRef)
    {
        ImGuiWidget::ImWidget* widget = SelectedWidgetRef.GetWidget();
        if (!widget)
        {
            // 控件已失效，清除选中状态
            SelectedWidgetRef.Reset();
            SelectedWidgetVarName.clear();
            return;
        }

        // 只绘制属于当前编辑控件树的选中控件
        if (!SelectedWidgetVarName.empty() && SelectedWidgetVarName == m_CurrentEditingWidgetVarName)
        {
            // 再次检查控件是否在当前树中
            ImGuiWidget::ImWidget* currentRoot = GetCurrentRootWidget();
            if (currentRoot && widget->IsInTree(currentRoot))
            {
                ImVec2 Pos1 = widget->GetPosition();
                ImVec2 Pos2 = Pos1 + widget->GetSize();
                DrawAnimatedDashedRect(Pos1, Pos2, IM_COL32(255, 0, 0, 255), 2, 10, 10, dashOffset, 0);
            }
        }
    }
}

void UI_WidgetEditor::ActionInit()
{
    ResetAction();

    AddSequentialProcessor(Action::ProjectView::RENAME_FILE, [this](const std::string& OldFullPath, const std::string& NewFullPath)
        {
            if (EditFileFullPath == OldFullPath)
            {
                EditFileFullPath = NewFullPath;
                ResetAction();
            }
        });
}

void UI_WidgetEditor::ResetAction()
{
    for (auto& id : FileActions)
    {
        RemoveProcessor(id);
    }
    FileActions.clear();

    // 订阅控件选中事件，使用单参数（控件路径）
    FileActions.push_back(AddSequentialProcessor(EditFileFullPath + Action::WIDGET_SELECTED,
        [this](const std::string& widgetPath)
        {
            SetSelectedWidgetByPath(widgetPath);
        }));
}

UI_WidgetEditor::UI_WidgetEditor(const std::string& name,
    ImGuiWidget::ImUserWidgetClass* targetClass,
    const std::string& EditFileFullPath) :
    ImUserWidget(name),
    m_TargetClass(targetClass),
    EditFileFullPath(EditFileFullPath)
{
    SetFocusable(true);

    if (m_TargetClass)
    {
        // 获取所有控件树变量
        auto widgetVarNames = m_TargetClass->GetWidgetVariableNames();

        // 如果用户没有指定当前编辑的控件树，则使用默认的第一个
        if (!widgetVarNames.empty())
        {
            // 优先使用默认根控件
            std::string defaultRootVarName = m_TargetClass->GetDefaultRootVariableName();
            if (!defaultRootVarName.empty() &&
                std::find(widgetVarNames.begin(), widgetVarNames.end(), defaultRootVarName) != widgetVarNames.end())
            {
                SetCurrentEditingWidgetTree(defaultRootVarName);
            }
            else
            {
                SetCurrentEditingWidgetTree(widgetVarNames[0]);
            }
        }
    }

    ActionInit();
}

bool UI_WidgetEditor::SetSelectedWidget(ImGuiWidget::ImWidget* widget)
{
    if (!widget) return false;

    // 查找控件所属的控件树变量名
    std::string widgetVarName = FindWidgetVarName(widget);
    if (widgetVarName.empty()) return false;

    return SetSelectedWidget(widgetVarName, widget);
}

bool UI_WidgetEditor::SetSelectedWidget(const std::string& widgetVarName, ImGuiWidget::ImWidget* widget)
{
    if (!widget || !m_TargetClass) return false;

    // 验证控件是否属于指定的控件树
    ImGuiWidget::ImWidget* rootWidget = m_TargetClass->GetWidgetVariable(widgetVarName);
    if (!rootWidget || !widget->IsInTree(rootWidget)) return false;

    SelectedWidgetRef = widget->GetWidgetRef();
    SelectedWidgetVarName = widgetVarName;

    // 如果选中的控件不在当前编辑的控件树中，自动切换到该控件树
    if (widgetVarName != m_CurrentEditingWidgetVarName)
    {
        SetEditingWidgetTree(widgetVarName);
    }

    return true;
}

bool UI_WidgetEditor::SetSelectedWidgetByPath(const std::string& widgetPath)
{
    if (!m_TargetClass) return false;
    if (widgetPath.empty()) return false;

    // 解析控件路径：格式为 "WidgetTreeVarName/RelativePath" 或 "WidgetTreeVarName"
    size_t slashPos = widgetPath.find('/');
    std::string widgetVarName;
    std::string relativePath;

    if (slashPos == std::string::npos)
    {
        // 没有斜杠，说明是根控件
        widgetVarName = widgetPath;
        relativePath = ".";
    }
    else
    {
        widgetVarName = widgetPath.substr(0, slashPos);
        relativePath = widgetPath.substr(slashPos + 1);
        if (relativePath.empty()) relativePath = ".";
    }

    // 获取控件树根控件
    ImGuiWidget::ImWidget* rootWidget = m_TargetClass->GetWidgetVariable(widgetVarName);
    if (!rootWidget) return false;

    // 通过相对路径查找控件
    ImGuiWidget::ImWidget* targetWidget = nullptr;
    if (relativePath == ".")
    {
        targetWidget = rootWidget;
    }
    else
    {
        targetWidget = rootWidget->FindChildByPath(relativePath);
    }

    if (!targetWidget) return false;

    // 设置选中状态
    SelectedWidgetRef = targetWidget->GetWidgetRef();
    SelectedWidgetVarName = widgetVarName;

    // 如果选中的控件不在当前编辑的控件树中，自动切换到该控件树
    if (widgetVarName != m_CurrentEditingWidgetVarName)
    {
        SetEditingWidgetTree(widgetVarName);
    }

    return true;
}

std::string UI_WidgetEditor::BuildWidgetPath(ImGuiWidget::ImWidget* widget) const
{
    if (!widget || !m_TargetClass) return "";

    // 查找控件所属的控件树
    std::string widgetVarName = FindWidgetVarName(widget);
    if (widgetVarName.empty()) return "";

    ImGuiWidget::ImWidget* rootWidget = m_TargetClass->GetWidgetVariable(widgetVarName);
    if (!rootWidget) return "";

    // 构建相对路径
    std::string relativePath = rootWidget->BuildPathTo(widget);

    // 组合完整路径：控件树变量名 + 相对路径
    if (relativePath == "." || relativePath.empty())
    {
        return widgetVarName;
    }

    // 移除相对路径前面的 "./"
    if (relativePath.size() >= 2 && relativePath[0] == '.' && relativePath[1] == '/')
    {
        relativePath = relativePath.substr(2);
    }

    return widgetVarName + "/" + relativePath;
}

void UI_WidgetEditor::SetEditingWidgetTree(const std::string& widgetVarName)
{
    if (widgetVarName == m_CurrentEditingWidgetVarName) return;

    SetCurrentEditingWidgetTree(widgetVarName);

    // 清除选中状态，如果选中的控件不在当前编辑的控件树中
    if (!SelectedWidgetVarName.empty() && SelectedWidgetVarName != m_CurrentEditingWidgetVarName)
    {
        SelectedWidgetRef.Reset();
        SelectedWidgetVarName.clear();
    }
}

void UI_WidgetEditor::SetCurrentEditingWidgetTree(const std::string& widgetVarName)
{
    if (!m_TargetClass) return;

    ImGuiWidget::ImWidget* rootWidget = m_TargetClass->GetWidgetVariable(widgetVarName);
    if (!rootWidget) return;

    m_CurrentEditingWidgetVarName = widgetVarName;

    // 设置根控件
    SetRootWidget(rootWidget, false);
}

ImGuiWidget::ImWidget* UI_WidgetEditor::GetCurrentRootWidget() const
{
    if (!m_TargetClass) return nullptr;
    return m_TargetClass->GetWidgetVariable(m_CurrentEditingWidgetVarName);
}

std::string UI_WidgetEditor::FindWidgetVarName(ImGuiWidget::ImWidget* widget) const
{
    if (!widget || !m_TargetClass) return "";

    // 遍历所有控件树变量，查找控件所属的控件树变量名
    auto widgetVarNames = m_TargetClass->GetWidgetVariableNames();
    for (const auto& varName : widgetVarNames)
    {
        ImGuiWidget::ImWidget* rootWidget = m_TargetClass->GetWidgetVariable(varName);
        if (rootWidget && widget->IsInTree(rootWidget))
        {
            return varName;
        }
    }

    return "";
}

void UI_WidgetEditor::SetTargetClass(ImGuiWidget::ImUserWidgetClass* targetClass)
{
    if (m_TargetClass == targetClass) return;

    m_TargetClass = targetClass;

    if (m_TargetClass)
    {
        auto widgetVarNames = m_TargetClass->GetWidgetVariableNames();

        // 设置当前编辑的控件树
        if (!widgetVarNames.empty())
        {
            // 优先保留原有的控件树
            if (!m_CurrentEditingWidgetVarName.empty() &&
                std::find(widgetVarNames.begin(), widgetVarNames.end(), m_CurrentEditingWidgetVarName) != widgetVarNames.end())
            {
                // 使用原有的控件树
                SetCurrentEditingWidgetTree(m_CurrentEditingWidgetVarName);
            }
            else
            {
                // 使用默认根控件
                std::string defaultRootVarName = m_TargetClass->GetDefaultRootVariableName();
                if (!defaultRootVarName.empty() &&
                    std::find(widgetVarNames.begin(), widgetVarNames.end(), defaultRootVarName) != widgetVarNames.end())
                {
                    SetCurrentEditingWidgetTree(defaultRootVarName);
                }
                else
                {
                    // 使用第一个控件树
                    SetCurrentEditingWidgetTree(widgetVarNames[0]);
                }
            }
        }
        else
        {
            m_CurrentEditingWidgetVarName.clear();
            SetRootWidget(nullptr, false);
        }
    }

    // 清除选中状态
    SelectedWidgetRef.Reset();
    SelectedWidgetVarName.clear();
}
