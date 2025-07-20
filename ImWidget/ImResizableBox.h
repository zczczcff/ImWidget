#pragma once
#include "ImWidget.h"
#include "ImPanelWidget.h"
#include <imgui_internal.h>
#include <functional>

namespace ImGuiWidget
{
    class ImResizableBoxSlot : public ImSlot
    {
    public:
        ImResizableBoxSlot(ImWidget* Content) : ImSlot(Content) {}
    };

    class ImResizableBox : public ImPanelWidget
    {
    private:
        ImVec2 min_size = ImVec2(10.f, 10.f);

        // 存储拖动状态
        int dragging_point_index = -1;     // 当前拖动的控制点索引
        ImVec2 drag_start_mouse_pos;       // 拖动开始时的鼠标位置
        ImVec2 original_position;          // 拖动开始时的位置
        ImVec2 original_size;              // 拖动开始时的尺寸

        // 控制点样式
        float m_ControlPointRadius = 3.0f;
        ImU32 m_ControlPointColor = IM_COL32(0, 0, 0, 255);

        // 控制框样式
        ImU32 m_BoxBorderColor = IM_COL32(0, 0, 0, 255);
        float m_BoxBorderThickness = 1.0f;

    public:
        // 回调函数
        std::function<void(ImVec2, ImVec2)> OnResizing;  // 拖动过程中回调
        std::function<void(ImVec2, ImVec2)> OnResized;   // 拖动结束回调

        ImResizableBox(const std::string& WidgetName)
            : ImPanelWidget(WidgetName),
            dragging_point_index(-1)
        {}

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

        // 设置回调函数
        void SetOnResizing(std::function<void(ImVec2, ImVec2)> callback) { OnResizing = callback; }
        void SetOnResized(std::function<void(ImVec2, ImVec2)> callback) { OnResized = callback; }

        // 设置控制点样式
        void SetControlPointRadius(float radius) { m_ControlPointRadius = radius; }
        void SetControlPointColor(ImU32 color) { m_ControlPointColor = color; }

        // 设置控制框样式
        void SetBoxBorderColor(ImU32 color) { m_BoxBorderColor = color; }
        void SetBoxBorderThickness(float thickness) { m_BoxBorderThickness = thickness; }

        virtual void Render() override
        {
            ImGuiContext& g = *ImGui::GetCurrentContext();
            ImGuiWindow* window = g.CurrentWindow;

            const ImGuiID base_id = ImGui::GetID(m_WidgetName.c_str());

            // 计算矩形边界
            ImVec2 rectMin = Position;
            ImVec2 rectMax = Position + Size;

            // 绘制矩形边框
            window->DrawList->AddRect(
                rectMin,
                rectMax,
                m_BoxBorderColor,
                0.0f,
                0,
                m_BoxBorderThickness
            );

            bool changed = false;
            const ImVec2 rect_size = Size;

            // 定义8个控制点（4个角点 + 4个边中点）+ 中心点
            enum ControlPoint {
                TopLeft, TopCenter, TopRight,
                MidRight, BottomRight, BottomCenter,
                BottomLeft, MidLeft, Center, COUNT
            };

            ImVec2 points[ControlPoint::COUNT];
            points[TopLeft] = rectMin;
            points[TopCenter] = ImVec2(rectMin.x + rect_size.x * 0.5f, rectMin.y);
            points[TopRight] = ImVec2(rectMax.x, rectMin.y);
            points[MidRight] = ImVec2(rectMax.x, rectMin.y + rect_size.y * 0.5f);
            points[BottomRight] = rectMax;
            points[BottomCenter] = ImVec2(rectMin.x + rect_size.x * 0.5f, rectMax.y);
            points[BottomLeft] = ImVec2(rectMin.x, rectMax.y);
            points[MidLeft] = ImVec2(rectMin.x, rectMin.y + rect_size.y * 0.5f);
            points[Center] = ImVec2(rectMin.x + rect_size.x * 0.5f, rectMin.y + rect_size.y * 0.5f);

            // 处理控制点拖动
            for (int i = 0; i < ControlPoint::COUNT; ++i) {
                ImGui::PushID(i);

                // 创建控制点的交互区域
                const ImRect handle_bb(
                    points[i] - ImVec2(m_ControlPointRadius, m_ControlPointRadius),
                    points[i] + ImVec2(m_ControlPointRadius, m_ControlPointRadius)
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
                if (hovered || held) {
                    switch (i) {
                    case TopLeft:
                    case BottomRight:
                        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNWSE);
                        break;
                    case TopRight:
                    case BottomLeft:
                        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNESW);
                        break;
                    case TopCenter:
                    case BottomCenter:
                        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
                        break;
                    case MidRight:
                    case MidLeft:
                        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
                        break;
                    case Center:
                        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
                        break;
                    }
                }

                // 开始拖动时记录初始状态
                if (pressed) {
                    dragging_point_index = i;
                    drag_start_mouse_pos = ImGui::GetMousePos();
                    original_position = Position;
                    original_size = Size;
                }

                // 处理拖动中
                if (held && dragging_point_index == i) {
                    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                        const ImVec2 mouse_delta = ImGui::GetMousePos() - drag_start_mouse_pos;

                        // 根据拖动点类型更新位置和尺寸
                        switch (dragging_point_index) {
                        case TopLeft:
                            Position = original_position + mouse_delta;
                            Size = original_size - mouse_delta;
                            break;
                        case TopCenter:
                            Position.y = original_position.y + mouse_delta.y;
                            Size.y = original_size.y - mouse_delta.y;
                            break;
                        case TopRight:
                            Position.y = original_position.y + mouse_delta.y;
                            Size = ImVec2(
                                original_size.x + mouse_delta.x,
                                original_size.y - mouse_delta.y
                            );
                            break;
                        case MidRight:
                            Size.x = original_size.x + mouse_delta.x;
                            break;
                        case BottomRight:
                            Size = original_size + mouse_delta;
                            break;
                        case BottomCenter:
                            Size.y = original_size.y + mouse_delta.y;
                            break;
                        case BottomLeft:
                            Position.x = original_position.x + mouse_delta.x;
                            Size = ImVec2(
                                original_size.x - mouse_delta.x,
                                original_size.y + mouse_delta.y
                            );
                            break;
                        case MidLeft:
                            Position.x = original_position.x + mouse_delta.x;
                            Size.x = original_size.x - mouse_delta.x;
                            break;
                        case Center: // 整体拖动
                            Position = original_position + mouse_delta;
                            break;
                        }

                        changed = true;

                        // 应用最小尺寸限制
                        if (Size.x < min_size.x) {
                            if (dragging_point_index == TopLeft ||
                                dragging_point_index == MidLeft ||
                                dragging_point_index == BottomLeft) {
                                Position.x = Position.x + Size.x - min_size.x;
                            }
                            Size.x = min_size.x;
                        }
                        if (Size.y < min_size.y) {
                            if (dragging_point_index == TopLeft ||
                                dragging_point_index == TopCenter ||
                                dragging_point_index == TopRight) {
                                Position.y = Position.y + Size.y - min_size.y;
                            }
                            Size.y = min_size.y;
                        }

                        // 调用拖动中回调
                        if (OnResizing) {
                            OnResizing(Position, Size);
                        }
                    }
                }

                ImGui::PopID();
            }

            // 处理拖动结束
            if (dragging_point_index != -1 && !ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                if (OnResized) {
                    OnResized(Position, Size);
                }
                dragging_point_index = -1;
            }

            // 绘制控制点
            for (int i = 0; i < ControlPoint::COUNT; ++i) {
                window->DrawList->AddCircleFilled(
                    points[i],
                    m_ControlPointRadius,
                    m_ControlPointColor,
                    12
                );
            }

            // 渲染子项
            if (!m_Slots.empty() && m_Slots[0])
            {
                m_Slots[0]->GetContent()->SetPosition(Position);
                m_Slots[0]->GetContent()->SetSize(Size);
                m_Slots[0]->GetContent()->Render();
            }
        }
    };
}