#include "UI/UI_WidgetEditor.h"
#include "EditorAction.h"

// 动态虚线框绘制函数
void UI_WidgetEditor::DrawAnimatedDashedRect(const ImVec2& min, const ImVec2& max, ImU32 color, float thickness, float dashLen, float gapLen, float& offset, float speed)
{
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    // 更新偏移量，实现动画效果
    offset += speed;
    if (offset >= dashLen + gapLen)
    {
        offset -= (dashLen + gapLen);
    }

    // 计算方框的四条边
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

    // 绘制底部虚线（方向从右到左）
    DrawDashedLine(drawList, bottomRight, bottomLeft, color, thickness, dashLen, gapLen, offset - topLen - rightLen);

    // 绘制左侧虚线（方向从下到上）
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

    // 初始偏移处理
    float currentPos = fmod(offset, period);
    if (currentPos < 0) currentPos += period;

    // 遍历线段，绘制虚线
    float traveled = 0.0f;
    while (traveled < length)
    {
        // 计算当前段的剩余长度
        float remaining = length - traveled;

        // 计算当前周期位置
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
            // 间隔部分
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
	if (EditorRootWidget)
	{
		ImGuiWidget::ImWidget* HitChild = EditorRootWidget->ChildHitTest(e.GetPosition());
		if (HitChild && HitChild != SelectedWidgetRef.GetWidget())
		{
            ExecuteAction(Action::WIDGET_SELECTED, HitChild);
			//OnWidgetSelected.Broadcast(HitChild);
            SelectedWidgetRef = HitChild->GetWidgetRef();
		}
	}
	e.StopPropagation();
}

void UI_WidgetEditor::OnKeyDown(ImGuiWidget::ImKeyDownEvent& e)
{
    if (bHasFocus && e.IsCtrl_Z())
    {
        OnRequestUndo.Broadcast();
    }
}

void UI_WidgetEditor::PostRender()
{
    if (SelectedWidgetRef)
    {
        ImVec2 Pos1 = SelectedWidgetRef->GetPosition();
        ImVec2 Pos2 = Pos1 + SelectedWidgetRef->GetSize();
        DrawAnimatedDashedRect(Pos1, Pos2, IM_COL32(255, 0, 0, 255), 2, 10, 10, dashOffset,0);
    }
}

void UI_WidgetEditor::ActionInit()
{
    AddSequentialProcessor(Action::WIDGET_SELECTED, [this](ImGuiWidget::ImWidget* SelectedWidget)
        {
            SetSelectedWidget(SelectedWidget);
        });
}

UI_WidgetEditor::UI_WidgetEditor(const std::string& name, ImGuiWidget::ImWidget* EditorRootWidget) :
	ImUserWidget(name),
	EditorRootWidget(EditorRootWidget)
{
	SetFocusable(true);
	SetRootWidget(EditorRootWidget, false);
    ActionInit();
}

bool UI_WidgetEditor::SetSelectedWidget(ImGuiWidget::ImWidget* widget)
{
    if (widget == SelectedWidgetRef.GetWidget())return true;
    if (widget->IsInTree(EditorRootWidget))
    {
        SelectedWidgetRef = widget->GetWidgetRef();
        return true;
    }
    return false;
}
