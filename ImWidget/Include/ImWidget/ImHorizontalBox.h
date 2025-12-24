#pragma once
#include "ImPanelWidget.h"

namespace ImGuiWidget
{
    class ImHorizontalBoxSlot : public ImPaddingSlot // 改为继承ImPaddingSlot
    {
    public:
        ImHorizontalBoxSlot(ImWidget* Content) : ImPaddingSlot(Content) {}

        float SizeRatio = 1.f; // 宽度比例

        virtual std::unordered_set<PropertyInfo, PropertyInfo::Hasher> GetProperties() override 
        {
            auto props = ImPaddingSlot::GetProperties();
            props.insert({
                "SizeRatio",
                PropertyType::Float,
                "Layout",
                [this](void* v) { SizeRatio = *static_cast<float*>(v); },
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
            return new ImHorizontalBoxSlot(Content);
        }
        virtual void Relayout() override // 重写布局算法
        {
            float requiredWidth = 0.f;   // 非自动大小控件所需的总宽度
            float remainingWidth = 0.f;   // 剩余宽度（用于比例分配）
            float sumSizeRatio = 0.f;     // 比例总和

            // 第一步：计算非自动大小控件所需宽度和比例总和
            for (int i = 0; i < GetSlotNum(); i++)
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

            remainingWidth = Size.x - requiredWidth;
            ImVec2 currentPos = Position;

            if (remainingWidth > 0.f) // 有足够空间
            {
                for (int i = 0; i < GetSlotNum(); i++)
                {
                    ImHorizontalBoxSlot* HSlot = static_cast<ImHorizontalBoxSlot*>(GetSlotAt(i));
                    if (!HSlot || !HSlot->IsValid()) continue;

                    HSlot->SetSlotPosition(currentPos);

                    if (!HSlot->GetIfAutoSize())
                    {
                        ImVec2 minSize = HSlot->GetContent()->GetMinSize();
                        float slotWidth = minSize.x + HSlot->PaddingLeft + HSlot->PaddingRight;
                        HSlot->SetSlotSize(ImVec2(slotWidth, Size.y));
                        currentPos.x += slotWidth;
                    }
                    else
                    {
                        float ratioWidth = (HSlot->SizeRatio / sumSizeRatio) * remainingWidth;
                        HSlot->SetSlotSize(ImVec2(ratioWidth, Size.y));
                        currentPos.x += ratioWidth;
                    }

                    HSlot->ApplyLayout(); // 应用布局（处理内边距）
                }
            }
            else // 空间不足
            {
                for (int i = 0; i < GetSlotNum(); i++)
                {
                    ImHorizontalBoxSlot* HSlot = static_cast<ImHorizontalBoxSlot*>(GetSlotAt(i));
                    if (!HSlot || !HSlot->IsValid()) continue;

                    HSlot->SetSlotPosition(currentPos);

                    if (!HSlot->GetIfAutoSize())
                    {
                        ImVec2 minSize = HSlot->GetContent()->GetMinSize();
                        float slotWidth = minSize.x + HSlot->PaddingLeft + HSlot->PaddingRight;
                        HSlot->SetSlotSize(ImVec2(slotWidth, Size.y));
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
            return AddChildInternal<ImHorizontalBoxSlot>(child);
        }
        virtual ImSlot* AddChild(ImWidget* Child, ImVec2 RelativePosition = ImVec2(FLT_MIN, FLT_MIN)) override
        {
            // 如果没有指定相对位置，添加到末尾
            if (RelativePosition.x == FLT_MIN) {
                return AddChildInternal<ImHorizontalBoxSlot>(Child);
            }

            // 计算在水平框中的相对位置
            float relativeX = RelativePosition.x - Position.x;

            // 如果没有子项，直接添加到末尾
            if (GetSlotNum() == 0) {
                return AddChildInternal<ImHorizontalBoxSlot>(Child);
            }

            // 遍历所有子项，寻找插入位置
            int insertIndex = GetSlotNum(); // 默认插入到最后

            for (int i = 0; i < GetSlotNum(); i++) {
                ImSlot* currentSlot = GetSlotAt(i);

                // 计算当前子项的中点
                float currentMid = currentSlot->GetContent()->GetPosition().x +
                    currentSlot->GetContent()->GetSize().x / 2.0f;

                // 如果位置在当前子项中点之前，插入在当前位置
                if (relativeX <= currentMid) {
                    insertIndex = i;
                    break;
                }
            }

            // 插入到找到的位置
            return InsertChildAt(insertIndex, Child);
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
            for (int i = 0; i < GetSlotNum(); i++)
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
            return ImVec2(ImMax(minWidth,10.f), ImMax(minHeight,10.f));
        }
        virtual std::string GetRegisterTypeName()override { return "ImHorizontalBox"; }

        virtual ImWidget* CopyWidget()
        {
            return new ImHorizontalBox(*this);
        }
    };
}