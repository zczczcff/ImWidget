#pragma once
#include "ImPanelWidget.h"
#include <functional>
#include "ImEvent/ImMouseEvent.h"

namespace ImGuiWidget
{
    class ImExpandableBox : public ImPanelWidget
    {
    private:
        bool bIsExpanded;
        bool m_IsHovered = false;
        float TriangleSize = 7.f;
        float HeadPad;
        float BodyPad = 6.f;

        ImVec2 p0, p1, p2, p3, p4, p5; // TrianglePoints
        ImRect buttonrect_NotExpanded, buttonrect_Expanded;
        ImU32 TriangleColor;
        ImU32 HoveredTriangleColor = IM_COL32(100, 100, 100, 255);

        std::function<void(bool)> OnExpandedStateChanged;
        std::function<void(void)> OnHoverBegin;
        std::function<void(void)> OnHoverEnd;

    protected:
        virtual ImSlot* CreateSlot(ImWidget* Content) override
        {
            return new ImPaddingSlot(Content);
        }

        virtual void Relayout() override
        {
            float headhight = HeadPad;

            if (auto slot = GetSlotAt(0))
            {
                slot->SetSlotPosition(Position + ImVec2(HeadPad, 0.f));
                ImVec2 minsize(0.f, 0.f);
                if (auto widget = slot->GetContent())
                {
                    minsize = widget->GetMinSize();
                }
                headhight = ImMax(headhight, minsize.y);
                float headlength = ImMax(minsize.x, Size.x - HeadPad);
                slot->SetSlotSize(ImVec2(minsize.x, headhight));
                slot->ApplyLayout();
            }
            if (bIsExpanded)
            {
                if (auto slot = GetSlotAt(1))
                {
                    slot->SetSlotPosition(Position + ImVec2(BodyPad, headhight));

                    if (auto widget = slot->GetContent())
                    {
                        float bodylength = ImMax(widget->GetMinSize().x, Size.x - BodyPad);
                        slot->SetSlotSize(ImVec2(bodylength, widget->GetMinSize().y));
                    }
                    slot->ApplyLayout();
                }
            }
        }

        virtual void HandleEventInternal(ImEvent* event) override
        {
            if (event->IsHandled()) return;
            if (event->GetPhase() == ImEventPhase::Capture) return;

            switch (event->GetType())
            {
            case ImEventType::MouseEnter:
                HandleMouseEnter(event->As<ImMouseEnterEvent>());
                break;

            case ImEventType::MouseLeave:
                HandleMouseLeave(event->As<ImMouseLeaveEvent>());
                break;

            case ImEventType::MouseClick:
                HandleMouseClick(event->As<ImMouseClickEvent>());
                break;

            default:
                break;
            }
        }

        void HandleMouseEnter(ImMouseEnterEvent* event)
        {
            if (!m_IsHovered)
            {
                m_IsHovered = true;
                if (OnHoverBegin)
                {
                    OnHoverBegin();
                }
            }
            // 不停止传播，让子控件也能接收事件
        }

        void HandleMouseLeave(ImMouseLeaveEvent* event)
        {
            if (m_IsHovered)
            {
                m_IsHovered = false;
                if (OnHoverEnd)
                {
                    OnHoverEnd();
                }
            }
        }

        void HandleMouseClick(ImMouseClickEvent* event)
        {
            if (event->GetButton() == ImMouseButton::Left)
            {
                ImVec2 clickPos = event->GetLocalPosition();
                ImRect buttonRect = bIsExpanded ? buttonrect_Expanded : buttonrect_NotExpanded;

                // 检查是否点击了三角形按钮区域
                if (buttonRect.Contains(clickPos))
                {
                    bIsExpanded = !bIsExpanded;
                    MarkSizeDirty();
                    MarkLayoutDirty();
                    if (OnExpandedStateChanged)
                    {
                        OnExpandedStateChanged(bIsExpanded);
                    }
                    event->StopPropagation(); // 阻止事件继续传播
                }
            }
        }

        void RenderTriangleButton()
        {
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            ImU32 currentTriangleColor = m_IsHovered ? HoveredTriangleColor : TriangleColor;

            if (bIsExpanded)
            {
                draw_list->AddTriangleFilled(
                    p3 + Position,
                    p4 + Position,
                    p5 + Position,
                    currentTriangleColor
                );
            }
            else
            {
                draw_list->AddTriangle(
                    p0 + Position,
                    p1 + Position,
                    p2 + Position,
                    currentTriangleColor
                );
            }
        }

    public:
        ImExpandableBox(const std::string& WidgetName)
            : ImPanelWidget(WidgetName),
            bIsExpanded(false),
            TriangleColor(IM_COL32(0, 0, 0, 255))
        {
            ReCaculateTriangle();
        }

        ImPaddingSlot* SetHead(ImWidget* HeadWidget)
        {
            SetChildAt(0, HeadWidget);
            return (ImPaddingSlot*)GetSlotAt(0);
        }

        ImPaddingSlot* SetBody(ImWidget* BodyWidget)
        {
            SetChildAt(1, BodyWidget);
            return (ImPaddingSlot*)GetSlotAt(1);
        }

        void ReCaculateTriangle()
        {
            HeadPad = 2 * TriangleSize;
            p0 = ImVec2
            (
                (2 * TriangleSize - TriangleSize / 1.414f) / 2,
                (HeadPad - 1.414f * TriangleSize) / 2.f
            );
            p1 = ImVec2
            (
                (2 * TriangleSize + TriangleSize / 1.414f) / 2,
                HeadPad / 2.f
            );
            p2 = ImVec2
            (
                (2 * TriangleSize - TriangleSize / 1.414f) / 2,
                (HeadPad + 1.414f * TriangleSize) / 2.f
            );

            buttonrect_NotExpanded = ImRect
            (
                p0.x - 1.f,
                p0.y - 1.f,
                p1.x + 1.f,
                p2.y + 1.f
            );

            p3 = ImVec2
            (
                (HeadPad - 1.414f * TriangleSize) / 2.f,
                (HeadPad - TriangleSize / 1.414f) / 2.f
            );

            p4 = ImVec2
            (
                HeadPad / 2.f,
                (HeadPad + TriangleSize / 1.414f) / 2.f
            );

            p5 = ImVec2
            (
                (HeadPad + 1.414f * TriangleSize) / 2.f,
                (HeadPad - TriangleSize / 1.414f) / 2.f
            );

            buttonrect_Expanded = ImRect
            (
                p3.x - 1.f,
                p3.y - 1.f,
                p5.x + 1.f,
                p4.y + 1.f
            );
        }

        virtual ImVec2 GetMinSize() override
        {
            ImVec2 minSize(HeadPad, HeadPad);

            if (ImSlot* headSlot = GetSlot(0))
            {
                if (ImWidget* content = headSlot->GetContent())
                {
                    const ImVec2 headMin = content->GetMinSize();
                    minSize.x += headMin.x;
                    minSize.y = ImMax(minSize.y, headMin.y);
                }
            }
            if (bIsExpanded)
            {
                if (ImSlot* bodySlot = GetSlot(1))
                {
                    if (ImWidget* content = bodySlot->GetContent())
                    {
                        const ImVec2 bodyMin = content->GetMinSize();
                        minSize.y += bodyMin.y;
                        minSize.x = ImMax(minSize.x, bodyMin.x);
                    }
                }
            }
            return minSize;
        }

        virtual void Render() override
        {
            RenderBackGround();
            RenderTriangleButton();
            HandleLayout();

            if (auto head = GetChildAt(0))
            {
                head->Render();
            }

            if (bIsExpanded)
            {
                if (auto body = GetChildAt(1))
                {
                    body->Render();
                }
            }
        }

        bool GetIfExpanded() { return bIsExpanded; }

        void SetExpandedState(bool newstate)
        {
            bIsExpanded = newstate;
            MarkSizeDirty();
            MarkLayoutDirty();
            if (OnExpandedStateChanged)
            {
                OnExpandedStateChanged(bIsExpanded);
            }
        }

        void SetOnExpandedStateChanged(std::function<void(bool)> callback) { OnExpandedStateChanged = callback; }
        void SetOnHoverBegin(std::function<void(void)> callback) { OnHoverBegin = callback; }
        void SetOnHoverEnd(std::function<void(void)> callback) { OnHoverEnd = callback; }

        void SetHoveredTriangleColor(ImU32 color) { HoveredTriangleColor = color; }
        bool IsHovered() const { return m_IsHovered; }

        virtual std::unordered_set<PropertyInfo, PropertyInfo::Hasher> GetProperties() override
        {
            auto baseProps = ImPanelWidget::GetProperties();

            baseProps.insert(
                { "IsExpanded", PropertyType::Bool, "State",
                  [this](void* v) { bIsExpanded = *static_cast<bool*>(v); },
                  [this]() -> void* { return &bIsExpanded; } }
            );

            baseProps.insert(
                { "TriangleSize", PropertyType::Float, "Appearance",
                  [this](void* v)
 {
TriangleSize = *static_cast<float*>(v);
ReCaculateTriangle();
},
[this]() -> void* { return &TriangleSize; } }
            );

            baseProps.insert(
                { "HeadPad", PropertyType::Float, "Layout",
                  [this](void* v) { HeadPad = *static_cast<float*>(v); },
                  [this]() -> void* { return &HeadPad; } }
            );

            baseProps.insert(
                { "BodyPad", PropertyType::Float, "Layout",
                  [this](void* v) { BodyPad = *static_cast<float*>(v); },
                  [this]() -> void* { return &BodyPad; } }
            );

            baseProps.insert(
                { "TriangleColor", PropertyType::Color, "Appearance",
                  [this](void* v) { TriangleColor = *static_cast<ImU32*>(v); },
                  [this]() -> void* { return &TriangleColor; } }
            );

            baseProps.insert(
                { "HoveredTriangleColor", PropertyType::Color, "Appearance",
                  [this](void* v) { HoveredTriangleColor = *static_cast<ImU32*>(v); },
                  [this]() -> void* { return &HoveredTriangleColor; } }
            );

            return baseProps;
        }

        virtual std::string GetRegisterTypeName() override { return "ImExpandableBox"; }

        virtual ImWidget* CopyWidget() override
        {
            ImExpandableBox* copy = new ImExpandableBox(*this);
            copy->bIsExpanded = bIsExpanded;
            copy->m_IsHovered = false;
            copy->TriangleSize = TriangleSize;
            copy->HeadPad = HeadPad;
            copy->BodyPad = BodyPad;
            copy->TriangleColor = TriangleColor;
            copy->HoveredTriangleColor = HoveredTriangleColor;
            copy->ReCaculateTriangle(); // 重新计算三角形位置
            return copy;
        }
    };
}