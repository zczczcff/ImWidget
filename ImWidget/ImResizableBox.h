#pragma once
#include "ImWidget.h"
#include "ImPanelWidget.h"
#include <imgui_internal.h>

namespace ImGuiWidget
{
    class ImResizableBoxSlot :public ImSlot
    {
    public:
        ImResizableBoxSlot(ImWidget* Content) :ImSlot(Content) {}
        ImRect BoxRect;
    };

    class ImResizableBox : public ImPanelWidget
    {
    private:
        ImRect rect;
        ImVec2 min_size = ImVec2(10.f, 10.f);

        // 存储拖动状态
        int dragging_point_index = -1;     // 当前拖动的控制点索引
        ImVec2 drag_start_mouse_pos;       // 拖动开始时的鼠标位置
        ImRect original_rect;              // 拖动开始时的矩形位置

    public:
        ImResizableBox(const std::string& WidgetName) :ImPanelWidget(WidgetName), dragging_point_index(-1) {}

        ImResizableBoxSlot* SetContent(ImWidget* Child)
        {
            if (m_Slots.size() > 0)
            {
                if (m_Slots[0])
                {
                    delete m_Slots[0];
                }
                m_Slots[0] = new ImResizableBoxSlot(Child);
                return static_cast<ImResizableBoxSlot*>(m_Slots[0]);
            }
            else
            {
                ImResizableBoxSlot* slot = new ImResizableBoxSlot(Child);
                m_Slots.push_back(slot);
                return slot;
            }
        }

        virtual void Render() override
        {
            ImGuiContext& g = *ImGui::GetCurrentContext();
            ImGuiWindow* window = g.CurrentWindow;

            const ImGuiID base_id = ImGui::GetID(m_WidgetName.c_str());

            // 绘制矩形边框
            window->DrawList->AddRect(rect.Min, rect.Max, IM_COL32(255, 255, 0, 255), 0.0f, 0, 2.0f);

            bool changed = false;
            const float handle_radius = 5.0f; // 控制点半径
            const ImVec2 rect_size = rect.GetSize();

            // 定义8个控制点（4个角点 + 4个边中点）
            enum ControlPoint {
                TopLeft, TopCenter, TopRight,
                MidRight, BottomRight, BottomCenter,
                BottomLeft, MidLeft, Center, COUNT
            };

            ImVec2 points[ControlPoint::COUNT];
            points[TopLeft] = rect.Min;
            points[TopCenter] = ImVec2(rect.Min.x + rect_size.x * 0.5f, rect.Min.y);
            points[TopRight] = ImVec2(rect.Max.x, rect.Min.y);
            points[MidRight] = ImVec2(rect.Max.x, rect.Min.y + rect_size.y * 0.5f);
            points[BottomRight] = rect.Max;
            points[BottomCenter] = ImVec2(rect.Min.x + rect_size.x * 0.5f, rect.Max.y);
            points[BottomLeft] = ImVec2(rect.Min.x, rect.Max.y);
            points[MidLeft] = ImVec2(rect.Min.x, rect.Min.y + rect_size.y * 0.5f);
            points[Center] = rect.GetCenter();

            // 绘制控制点
            for (int i = 0; i < ControlPoint::COUNT; ++i) {
                window->DrawList->AddCircleFilled(points[i], handle_radius, IM_COL32(255, 0, 0, 255), 12);
            }

            // 处理控制点拖动
            for (int i = 0; i < ControlPoint::COUNT; ++i) {
                ImGui::PushID(i);

                // 创建控制点的交互区域
                const ImRect handle_bb(
                    points[i] - ImVec2(handle_radius, handle_radius),
                    points[i] + ImVec2(handle_radius, handle_radius)
                );

                // 注册控制点为可交互项
                ImGui::ItemAdd(handle_bb, base_id + i + 1);

                // 检查拖动开始
                bool hovered, held;
                bool pressed = ImGui::ButtonBehavior(
                    handle_bb, base_id + i + 1, &hovered, &held,
                    ImGuiButtonFlags_PressedOnClick | ImGuiButtonFlags_FlattenChildren
                );

                // 设置鼠标样式
                if (hovered || held)
                    ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);

                // 开始拖动时记录初始状态
                if (pressed) {
                    dragging_point_index = i;
                    drag_start_mouse_pos = ImGui::GetMousePos();
                    original_rect = rect;
                }

                if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
                { 
                    if (dragging_point_index >= 0)
                    {
                        if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
                        {
                            const ImVec2 mouse_delta = ImGui::GetMousePos() - drag_start_mouse_pos;

                            // 根据拖动点类型更新矩形
                            switch (dragging_point_index) {
                            case TopLeft:
                                rect.Min = original_rect.Min + mouse_delta;
                                break;
                            case TopCenter:
                                rect.Min.y = original_rect.Min.y + mouse_delta.y;
                                break;
                            case TopRight:
                                rect.Min.y = original_rect.Min.y + mouse_delta.y;
                                rect.Max.x = original_rect.Max.x + mouse_delta.x;
                                break;
                            case MidRight:
                                rect.Max.x = original_rect.Max.x + mouse_delta.x;
                                break;
                            case BottomRight:
                                rect.Max = original_rect.Max + mouse_delta;
                                break;
                            case BottomCenter:
                                rect.Max.y = original_rect.Max.y + mouse_delta.y;
                                break;
                            case BottomLeft:
                                rect.Min.x = original_rect.Min.x + mouse_delta.x;
                                rect.Max.y = original_rect.Max.y + mouse_delta.y;
                                break;
                            case MidLeft:
                                rect.Min.x = original_rect.Min.x + mouse_delta.x;
                                break;
                            case Center: // 整体拖动
                                rect.Min = original_rect.Min + mouse_delta;
                                rect.Max = original_rect.Max + mouse_delta;
                                break;
                            }

                            changed = true;

                            // 应用最小尺寸限制
                            ImVec2 size = rect.GetSize();
                            if (size.x < min_size.x) {
                                if (dragging_point_index == TopLeft ||
                                    dragging_point_index == MidLeft ||
                                    dragging_point_index == BottomLeft) {
                                    rect.Min.x = rect.Max.x - min_size.x;
                                }
                                else {
                                    rect.Max.x = rect.Min.x + min_size.x;
                                }
                            }
                            if (size.y < min_size.y) {
                                if (dragging_point_index == TopLeft ||
                                    dragging_point_index == TopCenter ||
                                    dragging_point_index == TopRight) {
                                    rect.Min.y = rect.Max.y - min_size.y;
                                }
                                else {
                                    rect.Max.y = rect.Min.y + min_size.y;
                                }
                            }
                        }
                        else {
                            // 鼠标释放时结束拖动
                            dragging_point_index = -1;
                        }
                    }
                }
				
                ImGui::PopID();
            }

           

            //渲染子项
            if (!m_Slots.empty() && m_Slots[0])
            {
                m_Slots[0]->GetContent()->SetPosition(rect.Min);
                m_Slots[0]->GetContent()->SetSize(rect.Max - rect.Min);
                m_Slots[0]->GetContent()->Render();
            }
        }
    };
}
