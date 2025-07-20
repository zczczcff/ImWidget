#pragma once
#include "ImCanvasPanel.h"
#include "ImResizableBox.h"
#include <functional>

namespace ImGuiWidget
{
    class ImDesignPanel : public ImCanvasPanel
    {
    private:
        // 当前选中的子控件插槽
        ImCanvasPanelSlot* m_SelectedSlot = nullptr;

        // 包裹选中子控件的可调整框
        ImResizableBox* m_ResizableBox = nullptr;

        // 面板拖动状态
        bool m_IsDraggingPanel = false;
        ImVec2 m_DragStartMousePos;
        ImVec2 m_DragStartPanelPos;

        // 回调函数
        std::function<void(ImWidget*)> OnSelected;
        std::function<void()> OnUnSelected;

    public:
        ImDesignPanel(const std::string& WidgetName)
            : ImCanvasPanel(WidgetName)
        {}

        virtual ~ImDesignPanel()
        {
            if (m_ResizableBox) {
                delete m_ResizableBox;
            }
        }

        // 设置回调函数
        void SetOnSelected(std::function<void(ImWidget*)> callback) { OnSelected = callback; }
        void SetOnUnSelected(std::function<void()> callback) { OnUnSelected = callback; }

        // 获取当前选中的控件
        ImWidget* GetSelectedWidget() const
        {
            return m_SelectedSlot ? m_SelectedSlot->GetContent() : nullptr;
        }

        virtual void Render() override
        {
            ImGuiContext& g = *ImGui::GetCurrentContext();
            ImGuiWindow* window = g.CurrentWindow;
            const ImVec2 mouse_pos = ImGui::GetMousePos();
            const bool is_mouse_clicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
            const bool is_mouse_down = ImGui::IsMouseDown(ImGuiMouseButton_Left);
            const bool is_right_clicked = ImGui::IsMouseClicked(ImGuiMouseButton_Right);
            const bool is_right_down = ImGui::IsMouseDown(ImGuiMouseButton_Right);

            // 处理右键拖动面板
            if (is_right_clicked)
            {
                const ImRect panel_rect(Position, Position + Size);
                if (panel_rect.Contains(mouse_pos))
                {
                    m_IsDraggingPanel = true;
                    m_DragStartMousePos = mouse_pos;
                    m_DragStartPanelPos = Position;
                }
            }

            if (m_IsDraggingPanel && is_right_down)
            {
                const ImVec2 delta = mouse_pos - m_DragStartMousePos;
                Position = m_DragStartPanelPos + delta;
            }
            else
            {
                m_IsDraggingPanel = false;
            }

            // 渲染背景
            RenderBackGround();

            // 检查是否点击空白处取消选中
            if (is_mouse_clicked && m_SelectedSlot && m_ResizableBox)
            {
                if (GetRect().Contains(mouse_pos))
                {
                    bool clickedOnChild = false;

                    // 检查是否点击了子控件
                    for (auto& slot : m_Slots)
                    {
                        ImCanvasPanelSlot* canvasSlot = static_cast<ImCanvasPanelSlot*>(slot);
                        ImWidget* child = canvasSlot->GetContent();
                        const ImRect child_rect(
                            Position + canvasSlot->RelativePosition,
                            Position + canvasSlot->RelativePosition + canvasSlot->SlotSize
                        );

                        if (child_rect.Contains(mouse_pos))
                        {
                            clickedOnChild = true;
                            break;
                        }
                    }

                    // 点击空白处取消选中
                    if (!clickedOnChild)
                    {
                        if (m_ResizableBox)
                        {
                            delete m_ResizableBox;
                            m_ResizableBox = nullptr;
                            m_SelectedSlot = nullptr;

                            if (OnUnSelected) OnUnSelected();
                        }
                    }
                }
            }

            // 渲染所有子控件
            for (auto& slot : m_Slots)
            {
                ImCanvasPanelSlot* canvasSlot = static_cast<ImCanvasPanelSlot*>(slot);
                ImWidget* child = canvasSlot->GetContent();

                // 设置子控件位置和大小
                child->SetPosition(Position + canvasSlot->RelativePosition);
                child->SetSize(canvasSlot->SlotSize);

                // 检查是否选中子控件
                const ImRect child_rect(
                    Position + canvasSlot->RelativePosition,
                    Position + canvasSlot->RelativePosition + canvasSlot->SlotSize
                );

                if (is_mouse_clicked && child_rect.Contains(mouse_pos))
                {
                    // 选中新控件
                    if (m_SelectedSlot != canvasSlot)
                    {
                        // 清理之前的选中状态
                        if (m_ResizableBox)
                        {
                            delete m_ResizableBox;
                            m_ResizableBox = nullptr;
                        }

                        m_SelectedSlot = canvasSlot;

                        // 创建可调整框包裹选中的控件
                        m_ResizableBox = new ImResizableBox(m_WidgetName + "_ResizableBox");
                        m_ResizableBox->SetPosition(child->GetPosition());
                        m_ResizableBox->SetSize(child->GetSize());
                        m_ResizableBox->SetContent(child);

                        // 设置回调更新实际位置和大小
                        m_ResizableBox->SetOnResizing([this](ImVec2 newPos, ImVec2 newSize)
                            {
                                if (m_SelectedSlot)
                                {
                                    m_SelectedSlot->RelativePosition = newPos - Position;
                                    m_SelectedSlot->SlotSize = newSize;
                                }
                            });

                        // 触发选中回调
                        if (OnSelected) OnSelected(child);
                    }
                }

                // 仅渲染未被选中的子控件（被选中的由ResizableBox渲染）
                if (canvasSlot != m_SelectedSlot)
                {
                    child->Render();
                }
            }

            // 渲染选中控件的可调整框
            if (m_ResizableBox && m_SelectedSlot)
            {
                // 更新可调整框位置（处理面板移动后）
                if (!m_IsDraggingPanel)
                {
                    m_ResizableBox->SetPosition(
                        Position + m_SelectedSlot->RelativePosition
                    );
                }

                m_ResizableBox->SetSize(m_SelectedSlot->SlotSize);
                m_ResizableBox->Render();
            }
        }
    };
}