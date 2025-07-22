#pragma once
#include "ImPanelWidget.h"


namespace ImGuiWidget
{
	class ImVerticalBoxSlot :public ImSlot
	{
	public:
		ImVerticalBoxSlot(ImWidget* Content):ImSlot(Content){}

		float SizeRatio = 1.f;
	};


	class ImVerticalBox :public ImPanelWidget
	{
	private:
		
	public:
		ImVerticalBox(const std::string& WidgetName):ImPanelWidget(WidgetName){}
		ImVerticalBoxSlot* AddChildToVerticalBox(ImWidget* child)
		{
			return AddChild<ImVerticalBoxSlot>(child);
		}

		virtual void Render() override
		{

			ReLayOut();

			RenderBackGround();

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
				float childminwidth = child->PaddingLeft + child->PaddingRight+ childminsize.x;
				minlength = ImMax(minlength, childminwidth);
				minheight += child->PaddingBottom;
				minheight += child->PaddingTop;
				minheight += childminsize.y;
			}

			return ImVec2(minlength, minheight);
		}



		void ReLayOut()
		{
            float requiredHeight = 0.f; // 非自动大小控件所需的总高度
            float remainingHeight = 0.f; // 剩余高度（用于比例分配）
            float sumSizeRatio = 0.f;   // 比例总和

            // 第一步：计算非自动大小控件所需高度和比例总和
            for (auto& slot : m_Slots)
            {
                ImVerticalBoxSlot* VSlot = static_cast<ImVerticalBoxSlot*>(slot);
                if (!slot->GetIfAutoSize())
                {
                    ImVec2 minSize = slot->GetContent()->GetMinSize();
                    requiredHeight += (minSize.y + VSlot->PaddingTop + VSlot->PaddingBottom);
                }
                else
                {
                    sumSizeRatio += VSlot->SizeRatio;
                }
            }

            remainingHeight = Size.y - requiredHeight;

            ImVec2 currentPos = Position; // 当前位置

            if (remainingHeight > 0.f) // 有足够空间
            {
                for (auto& slot : m_Slots)
                {
                    ImVerticalBoxSlot* VSlot = static_cast<ImVerticalBoxSlot*>(slot);
                    // 设置子控件位置（考虑内边距）
                    slot->GetContent()->SetPosition(ImVec2(
                        currentPos.x + VSlot->PaddingLeft,
                        currentPos.y + VSlot->PaddingTop
                    ));

                    if (!slot->GetIfAutoSize())
                    {
                        // 非自动大小控件：使用最小高度，宽度填满
                        ImVec2 minSize = slot->GetContent()->GetMinSize();
                        slot->GetContent()->SetSize(ImVec2(
                            Size.x - VSlot->PaddingLeft - VSlot->PaddingRight,
                            minSize.y
                        ));
                        // 移动到下一个位置（垂直方向）
                        currentPos.y += (minSize.y + VSlot->PaddingTop + VSlot->PaddingBottom);
                    }
                    else
                    {
                        // 比例分配控件：计算实际高度
                        float ratioHeight = (VSlot->SizeRatio / sumSizeRatio) * remainingHeight;
                        slot->GetContent()->SetSize(ImVec2(
                            Size.x - VSlot->PaddingLeft - VSlot->PaddingRight,
                            ratioHeight - VSlot->PaddingTop - VSlot->PaddingBottom
                        ));
                        // 移动到下一个位置（垂直方向）
                        currentPos.y += ratioHeight;
                    }
                }
            }
            else // 空间不足
            {
                for (auto& slot : m_Slots)
                {
                    ImVerticalBoxSlot* VSlot = static_cast<ImVerticalBoxSlot*>(slot);
                    slot->GetContent()->SetPosition(ImVec2(
                        currentPos.x + VSlot->PaddingLeft,
                        currentPos.y + VSlot->PaddingTop
                    ));

                    if (!slot->GetIfAutoSize())
                    {
                        // 非自动大小控件：使用最小高度
                        ImVec2 minSize = slot->GetContent()->GetMinSize();
                        slot->GetContent()->SetSize(ImVec2(
                            Size.x - VSlot->PaddingLeft - VSlot->PaddingRight,
                            minSize.y
                        ));
                        currentPos.y += (minSize.y + VSlot->PaddingTop + VSlot->PaddingBottom);
                    }
                    else
                    {
                        // 比例分配控件：高度设为0
                        slot->GetContent()->SetSize(ImVec2(0.f, 0.f));
                    }
                }
            }
		}
	};
}