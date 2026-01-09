#pragma once
#include "ImPanelWidget.h"

namespace ImGuiWidget
{
    class ImHorizontalBoxSlot : public ImPaddingSlot // 改为继承ImPaddingSlot
    {
    public:
        ImHorizontalBoxSlot(ImWidget* Content, ImWidget* Slot) : ImPaddingSlot(Content, Slot) {}

        float SizeRatio = 1.f; // 宽度比例

        virtual std::unordered_set<PropertyInfo, PropertyInfo::Hasher> GetProperties() override 
        {
            auto props = ImPaddingSlot::GetProperties();
            props.insert({
                "SizeRatio",
                PropertyType::Float,
                "Layout",
                [this](void* v) { SizeRatio = *static_cast<float*>(v); Owner->MarkLayoutDirty(); },
                [this]() -> void* { return &SizeRatio; }
                });
            return props;
        }

        virtual ImSlot* CopySlot() override
        {
            return new ImHorizontalBoxSlot(*this);
        }
    };

    class ImHorizontalBox : public ImPanelWidget
    {
    protected:
        virtual ImSlot* CreateSlot(ImWidget* Content)override
        {
            return new ImHorizontalBoxSlot(Content, this);
        }
        virtual void Relayout() override // 重写布局算法
        {
            float requiredWidth = 0.f;   // 非自动大小控件所需的总宽度
            float remainingWidth = 0.f;   // 剩余宽度（用于比例分配）
            float sumSizeRatio = 0.f;     // 比例总和

            // 第一步：计算非自动大小控件所需宽度和比例总和
            for (int i = 0; i < GetChildNum(); i++)
            {
                ImHorizontalBoxSlot* HSlot = static_cast<ImHorizontalBoxSlot*>(GetSlotAt(i));
                if (!HSlot || !HSlot->IsValid()) continue;

                if (!HSlot->GetIfAutoSize())
                {
                    ImVec2 minSize = HSlot->GetContent()->GetMinSize();
                    requiredWidth += (minSize.x + HSlot->PaddingLeft + HSlot->PaddingRight);
                }
                else
                {
                    sumSizeRatio += HSlot->SizeRatio;
                }
            }

            remainingWidth = Size.x - requiredWidth - (bHaveBorder ? 2 * BorderThickness : 0);
            ImVec2 currentPos = Position + (bHaveBorder ? ImVec2(BorderThickness, BorderThickness) : ImVec2(0.f, 0.f));

            if (remainingWidth > 0.f) // 有足够空间
            {
                for (int i = 0; i < GetChildNum(); i++)
                {
                    ImHorizontalBoxSlot* HSlot = static_cast<ImHorizontalBoxSlot*>(GetSlotAt(i));
                    if (!HSlot || !HSlot->IsValid()) continue;

                    HSlot->SetSlotPosition(currentPos);

                    if (!HSlot->GetIfAutoSize())
                    {
                        ImVec2 minSize = HSlot->GetContent()->GetMinSize();
                        float slotWidth = minSize.x + HSlot->PaddingLeft + HSlot->PaddingRight;
                        HSlot->SetSlotSize(ImVec2(slotWidth, Size.y - (bHaveBorder ? 2*BorderThickness : 0)));
                        currentPos.x += slotWidth;
                    }
                    else
                    {
                        float ratioWidth = (HSlot->SizeRatio / sumSizeRatio) * remainingWidth;
                        HSlot->SetSlotSize(ImVec2(ratioWidth, Size.y - (bHaveBorder ? 2 * BorderThickness : 0)));
                        currentPos.x += ratioWidth;
                    }

                    HSlot->ApplyLayout(); // 应用布局（处理内边距）
                }
            }
            else // 空间不足
            {
                for (int i = 0; i < GetChildNum(); i++)
                {
                    ImHorizontalBoxSlot* HSlot = static_cast<ImHorizontalBoxSlot*>(GetSlotAt(i));
                    if (!HSlot || !HSlot->IsValid()) continue;

                    HSlot->SetSlotPosition(currentPos);

                    if (!HSlot->GetIfAutoSize())
                    {
                        ImVec2 minSize = HSlot->GetContent()->GetMinSize();
                        float slotWidth = minSize.x + HSlot->PaddingLeft + HSlot->PaddingRight;
                        HSlot->SetSlotSize(ImVec2(slotWidth, Size.y - (bHaveBorder ? 2 * BorderThickness : 0)));
                        currentPos.x += slotWidth;
                    }
                    else
                    {
                        HSlot->SetSlotSize(ImVec2(0.f, Size.y));
                    }

                    HSlot->ApplyLayout(); // 应用布局（处理内边距）
                }
            }
        }
    public:
        ImHorizontalBox(const std::string& WidgetName) : ImPanelWidget(WidgetName) {}

        ImHorizontalBoxSlot* AddChildToHorizontalBox(ImWidget* child)
        {
            return static_cast<ImHorizontalBoxSlot*>(ImPanelWidget::AddChild(child));
        }

        virtual void Render() override
        {
            HandleLayout(); // 添加布局处理调用

            RenderBackGround();
            RenderChild();  // 使用统一的子控件渲染
        }

        virtual ImVec2 GetMinSize() override
        {
            float minWidth = 0.f;
            float minHeight = 0.f;
            for (int i = 0; i < GetChildNum(); i++)
            {
                ImHorizontalBoxSlot* slot = static_cast<ImHorizontalBoxSlot*>(GetSlotAt(i));
                if (slot && slot->IsValid())
                {
                    ImVec2 childMinSize = slot->GetContent()->GetMinSize();
                    float slotWidth = slot->PaddingLeft + slot->PaddingRight + childMinSize.x;
                    float slotHeight = slot->PaddingTop + slot->PaddingBottom + childMinSize.y;

                    minWidth += slotWidth;
                    minHeight = ImMax(minHeight, slotHeight);
                }
            }
            return ImVec2(ImMax(minWidth,10.f), ImMax(minHeight,10.f)) + (bHaveBorder ? ImVec2(2 * BorderThickness, 2 * BorderThickness) : ImVec2(0, 0));
        }
        virtual std::string GetRegisterTypeName()override { return "ImHorizontalBox"; }

        virtual ImWidget* CopyWidget()
        {
            return new ImHorizontalBox(*this);
        }
    };
}