#pragma once
#include "ImPanelWidget.h"
#include "ImResizableBox.h"
#include "ImCanvasPanel.h" // 包含ImCanvasPanelSlot定义
#include <functional>

namespace ImGuiWidget
{
    class ImDesignPanel : public ImPanelWidget
    {
    private:
        // 当前选中的控件和其slot
        ImWidget* m_SelectedWidget = nullptr;
        ImSlot* m_SelectedSlot = nullptr;

        // 包裹选中控件的可调整框
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
            : ImPanelWidget(WidgetName)
        {}

        virtual ~ImDesignPanel()
        {
            if (m_ResizableBox) {
                delete m_ResizableBox;
            }
        }

        //只允许唯一子控件
        virtual ImSlot* AddChild(ImWidget* child, ImVec2 RelativePosition = ImVec2(FLT_MIN, FLT_MIN))override
        {
            if (GetSlotNum() > 0)
            {
                return nullptr;
            }
            else
            {
                return AddChildInternal<ImSlot>(child);
            }
        }
        // 设置回调函数
        void SetOnSelected(std::function<void(ImWidget*)> callback) { OnSelected = callback; }
        void SetOnUnSelected(std::function<void()> callback) { OnUnSelected = callback; }

        // 获取当前选中的控件
        ImWidget* GetSelectedWidget() const
        {
            return m_SelectedWidget;
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

            // 设置子控件Slot并渲染
            if (GetSlotNum() > 0) {
                ImSlot* slot = GetSlotAt(0);
                if (slot) {
                    // 设置Slot位置和大小（填满设计面板）
                    slot->SetSlotPosition(Position);
                    slot->SetSlotSize(Size);
                    slot->ApplyLayout(); // 应用布局
                }
            }
            // 通过基类方法渲染子控件
            RenderChild();

            // 处理左键点击
            if (is_mouse_clicked)
            {
                // 通过ChildHitTest查找被点击的最上层控件
                ImWidget* hitWidget = ChildHitTest(mouse_pos);

                if (hitWidget && hitWidget != this) // 排除设计面板自身
                {
                    // 获取该控件的slot
                    ImSlot* hitSlot = hitWidget->GetSlot();

                    // 更新选中状态
                    if (m_SelectedWidget != hitWidget)
                    {
                        // 清理之前的选中状态
                        if (m_ResizableBox)
                        {
                            delete m_ResizableBox;
                            m_ResizableBox = nullptr;
                        }

                        m_SelectedWidget = hitWidget;
                        m_SelectedSlot = hitSlot;

                        // 根据slot类型决定创建调整框还是简单边框
                        if (dynamic_cast<ImCanvasPanelSlot*>(hitSlot))
                        {
                            // 创建可调整框
                            m_ResizableBox = new ImResizableBox(m_WidgetName + "_ResizableBox");
                            m_ResizableBox->SetPosition(m_SelectedWidget->GetPosition());
                            m_ResizableBox->SetSize(m_SelectedWidget->GetSize());

                            // 设置回调更新实际位置和大小
                            m_ResizableBox->SetOnResizing([this](ImVec2 newPos, ImVec2 newSize)
                                {
                                    if (m_SelectedSlot)
                                    {
                                        // 更新slot的位置和大小
                                        if (auto canvasSlot = dynamic_cast<ImCanvasPanelSlot*>(m_SelectedSlot))
                                        {
                                            // 使用相对位置（相对于设计面板）
                                            canvasSlot->RelativePosition = newPos - Position;
                                            canvasSlot->SlotSize = newSize;
                                        }
                                    }
                                });
                        }

                        // 触发选中回调
                        if (OnSelected) OnSelected(hitWidget);
                    }
                }
                else
                {
                    // 点击空白处取消选中
                    if (m_ResizableBox)
                    {
                        delete m_ResizableBox;
                        m_ResizableBox = nullptr;
                        m_SelectedWidget = nullptr;
                        m_SelectedSlot = nullptr;

                        if (OnUnSelected) OnUnSelected();
                    }
                }
            }

            // 渲染选中控件效果
            if (m_SelectedWidget)
            {
                if (m_ResizableBox)
                {
                    // 更新并渲染可调整框
                    m_ResizableBox->SetPosition(m_SelectedWidget->GetPosition());
                    m_ResizableBox->SetSize(m_SelectedWidget->GetSize());
                    m_ResizableBox->Render();
                }
                else
                {
                    // 绘制简单边框表示选中
                    ImRect selectedRect = m_SelectedWidget->GetRect();
                    window->DrawList->AddRect(
                        selectedRect.Min,
                        selectedRect.Max,
                        IM_COL32(255, 0, 0, 255), // 红色边框
                        0.0f,
                        0,
                        2.0f // 边框粗细
                    );
                }
            }
        }
    };
}