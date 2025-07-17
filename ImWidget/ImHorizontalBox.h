#pragma once
#pragma once
#include "ImPanelWidget.h"

namespace ImGuiWidget
{
    class ImHorizontalBoxSlot : public ImSlot
    {
    public:
        ImHorizontalBoxSlot(ImWidget* Content) : ImSlot(Content) {}

        float SizeRatio = 1.f; // 宽度比例
    };

    class ImHorizontalBox : public ImPanelWidget
    {
    public:
        ImHorizontalBox(const std::string& WidgetName) : ImPanelWidget(WidgetName) {}

        ImHorizontalBoxSlot* AddChildToHorizontalBox(ImWidget* child)
        {
            return AddChild<ImHorizontalBoxSlot>(child);
        }

        virtual void Render() override
        {
            ReLayOut(); // 重新布局子控件

            // 渲染所有子控件
            for (auto& slot : m_Slots)
            {
                slot->GetContent()->Render();
            }
        }
        virtual ImVec2 GetMinSize()
        {
            float minheight = 0.f;
            float minlength = 0.f;
            for (auto& child : m_Slots)
            {
                ImVec2 childminsize = child->GetContent()->GetMinSize();
                float childminheight = child->PaddingTop + child->PaddingBottom + childminsize.y;
                minheight = ImMax(minheight, childminheight);
                minlength += child->PaddingLeft;
                minlength += child->PaddingRight;
                minlength += childminsize.x;
            }

            return ImVec2(minlength, minheight);
        }
    private:
        // 重新布局子控件（水平方向）
        void ReLayOut()
        {
            float requiredWidth = 0.f; // 自动大小控件所需的总宽度
            float remainingWidth = 0.f; // 剩余宽度（用于比例分配）
            float sumSizeRatio = 0.f;   // 比例总和

            // 第一步：计算自动大小控件所需宽度和比例总和
            for (auto& slot : m_Slots)
            {
                ImHorizontalBoxSlot* HSlot = static_cast<ImHorizontalBoxSlot*>(slot);
                if (!slot->GetIfAutoSize())
                {
                    ImVec2 minSize = slot->GetContent()->GetMinSize();
                    requiredWidth += (minSize.x + HSlot->PaddingLeft + HSlot->PaddingRight);
                }
                else
                {
                    sumSizeRatio += HSlot->SizeRatio;
                }
            }

            remainingWidth = Size.x - requiredWidth;

            ImVec2 currentPos = Position; // 当前位置

            if (remainingWidth > 0.f) // 有足够空间
            {
                for (auto& slot : m_Slots)
                {
                    ImHorizontalBoxSlot* HSlot = static_cast<ImHorizontalBoxSlot*>(slot);
                    // 设置子控件位置（考虑内边距）
                    slot->GetContent()->SetPosition(ImVec2(
                        currentPos.x + HSlot->PaddingLeft,
                        currentPos.y + HSlot->PaddingTop
                    ));

                    if (!slot->GetIfAutoSize())
                    {
                        // 自动大小控件：使用最小宽度，高度填满
                        ImVec2 minSize = slot->GetContent()->GetMinSize();
                        slot->GetContent()->SetSize(ImVec2(
                            minSize.x,
                            Size.y - HSlot->PaddingTop - HSlot->PaddingBottom
                        ));
                        // 移动到下一个位置
                        currentPos.x += (minSize.x + HSlot->PaddingLeft + HSlot->PaddingRight);
                    }
                    else
                    {
                        // 比例分配控件：计算实际宽度
                        float ratioWidth = (HSlot->SizeRatio / sumSizeRatio) * remainingWidth;
                        slot->GetContent()->SetSize(ImVec2(
                            ratioWidth - HSlot->PaddingLeft - HSlot->PaddingRight,
                            Size.y - HSlot->PaddingTop - HSlot->PaddingBottom
                        ));
                        // 移动到下一个位置
                        currentPos.x += ratioWidth;
                    }
                }
            }
            else // 空间不足
            {
                for (auto& slot : m_Slots)
                {
                    ImHorizontalBoxSlot* HSlot = static_cast<ImHorizontalBoxSlot*>(slot);
                    slot->GetContent()->SetPosition(ImVec2(
                        currentPos.x + HSlot->PaddingLeft,
                        currentPos.y + HSlot->PaddingTop
                    ));

                    if (!slot->GetIfAutoSize())
                    {
                        // 自动大小控件：使用最小宽度
                        ImVec2 minSize = slot->GetContent()->GetMinSize();
                        slot->GetContent()->SetSize(ImVec2(
                            minSize.x,
                            Size.y - HSlot->PaddingTop - HSlot->PaddingBottom
                        ));
                        currentPos.x += (minSize.x + HSlot->PaddingLeft + HSlot->PaddingRight);
                    }
                    else
                    {
                        // 比例分配控件：宽度设为0
                        slot->GetContent()->SetSize(ImVec2(0.f, 0.f));
                    }
                }
            }
        }
    };
}