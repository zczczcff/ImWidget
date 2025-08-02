#pragma once
#include "ImPanelWidget.h"


namespace ImGuiWidget
{
	class ImVerticalBoxSlot :public ImPaddingSlot
	{
	public:
		ImVerticalBoxSlot(ImWidget* Content):ImPaddingSlot(Content){}

		float SizeRatio = 1.f;
	};


	class ImVerticalBox :public ImPanelWidget
	{
    protected:
        virtual ImSlot* CreateSlot(ImWidget* Content)override
        {
            return new ImVerticalBoxSlot(Content);
        }
		
	public:
		ImVerticalBox(const std::string& WidgetName):ImPanelWidget(WidgetName){}
		ImVerticalBoxSlot* AddChildToVerticalBox(ImWidget* child)
		{
			return AddChildInternal<ImVerticalBoxSlot>(child);
		}
        virtual ImSlot* AddChild(ImWidget* Child, ImVec2 RelativePosition = ImVec2(FLT_MIN, FLT_MIN)) override
        {
            // 如果没有指定相对位置，添加到末尾
            if (RelativePosition.y == FLT_MIN) {
                return AddChildInternal<ImVerticalBoxSlot>(Child);
            }

            // 计算在垂直框中的相对位置
            float relativeY = RelativePosition.y - Position.y;

            // 如果没有子项，直接添加到末尾
            if (GetSlotNum() == 0) {
                return AddChildInternal<ImVerticalBoxSlot>(Child);
            }

            // 遍历所有子项，寻找插入位置
            int insertIndex = GetSlotNum(); // 默认插入到最后

            for (int i = 0; i < GetSlotNum(); i++) {
                ImSlot* currentSlot = GetSlotAt(i);

                // 计算当前子项的中点
                float currentMid = currentSlot->GetContent()->GetPosition().y +
                    currentSlot->GetContent()->GetSize().y / 2.0f;

                // 如果位置在当前子项中点之前，插入在当前位置
                if (relativeY <= currentMid) {
                    insertIndex = i;
                    break;
                }
            }

            // 插入到找到的位置
            return InsertChildAt(insertIndex, Child);
        }

		virtual void Render() override
		{
            HandleLayout();
			
			RenderBackGround();

			//for (auto& slot : m_Slots)
			//{
			//	slot->GetContent()->Render();
			//}
            RenderChild();

		}

		virtual ImVec2 GetMinSize()
		{
			float minheight = 10.f;
			float minlength = 30.f;
            for (int i = 0; i < GetSlotNum(); i++)
            {
                ImVerticalBoxSlot* child = (ImVerticalBoxSlot*)GetSlotAt(i);
                if (child)
                {
                    ImVec2 childminsize = child->GetContent()->GetMinSize();
                    float childminwidth = child->PaddingLeft + child->PaddingRight + childminsize.x;
                    minlength = ImMax(minlength, childminwidth);
                    minheight += child->PaddingBottom;
                    minheight += child->PaddingTop;
                    minheight += childminsize.y;
                }
            }
			//for (auto& child : m_Slots)
			//{
			//	ImVec2 childminsize = child->GetContent()->GetMinSize();
			//	float childminwidth = child->PaddingLeft + child->PaddingRight+ childminsize.x;
			//	minlength = ImMax(minlength, childminwidth);
			//	minheight += child->PaddingBottom;
			//	minheight += child->PaddingTop;
			//	minheight += childminsize.y;
			//}

			return ImVec2(minlength, minheight);
		}

		virtual void Relayout()override
		{
            float requiredHeight = 0.f; // 非自动大小控件所需的总高度
            float remainingHeight = 0.f; // 剩余高度（用于比例分配）
            float sumSizeRatio = 0.f;   // 比例总和

            // 第一步：计算非自动大小控件所需高度和比例总和
            for (int i=0;i<GetSlotNum();i++)
            {
                ImVerticalBoxSlot* VSlot = static_cast<ImVerticalBoxSlot*>(GetSlotAt(i));
                if (!VSlot|| !VSlot->IsValid())continue;
                if (!VSlot->GetIfAutoSize())
                {
                    ImVec2 minSize = VSlot->GetContent()->GetMinSize();
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
                for (int i = 0; i < GetSlotNum(); i++)
                {
                    ImVerticalBoxSlot* VSlot = static_cast<ImVerticalBoxSlot*>(GetSlotAt(i));
                    if (!VSlot || !VSlot->IsValid())continue;
                    // 设置子控件位置（考虑内边距）
                    VSlot->SetSlotPosition(currentPos);
                    //VSlot->GetContent()->SetPosition(ImVec2(
                    //    currentPos.x + VSlot->PaddingLeft,
                    //    currentPos.y + VSlot->PaddingTop
                    //));

                    if (!VSlot->GetIfAutoSize())
                    {
                        // 非自动大小控件：使用最小高度，宽度填满
                        ImVec2 minSize = VSlot->GetContent()->GetMinSize();
                        VSlot->SetSlotSize(ImVec2(Size.x,minSize.y + VSlot->PaddingTop + VSlot->PaddingBottom));
                        //VSlot->GetContent()->SetSize(ImVec2(
                        //    Size.x - VSlot->PaddingLeft - VSlot->PaddingRight,
                        //    minSize.y
                        //));
                        // 移动到下一个位置（垂直方向）
                        currentPos.y += (minSize.y + VSlot->PaddingTop + VSlot->PaddingBottom);
                    }
                    else
                    {
                        // 比例分配控件：计算实际高度
                        float ratioHeight = (VSlot->SizeRatio / sumSizeRatio) * remainingHeight;
                        VSlot->SetSlotSize(ImVec2(Size.x, ratioHeight));
                        //VSlot->GetContent()->SetSize(ImVec2(
                        //    Size.x - VSlot->PaddingLeft - VSlot->PaddingRight,
                        //    ratioHeight - VSlot->PaddingTop - VSlot->PaddingBottom
                        //));
                        // 移动到下一个位置（垂直方向）
                        currentPos.y += ratioHeight;
                    }

                    VSlot->ApplyLayout();
                }
            }
            else // 空间不足
            {
                for (int i = 0; i < GetSlotNum(); i++)
                {
                    ImVerticalBoxSlot* VSlot = static_cast<ImVerticalBoxSlot*>(GetSlotAt(i));
                    if (!VSlot || !VSlot->IsValid())continue;
                    VSlot->SetSlotPosition(currentPos);
                    //VSlot->GetContent()->SetPosition(ImVec2(
                    //    currentPos.x + VSlot->PaddingLeft,
                    //    currentPos.y + VSlot->PaddingTop
                    //));

                    if (!VSlot->GetIfAutoSize())
                    {
                        // 非自动大小控件：使用最小高度
                        ImVec2 minSize = VSlot->GetContent()->GetMinSize();
                        VSlot->SetSlotSize(ImVec2(Size.x, minSize.y + VSlot->PaddingTop + VSlot->PaddingBottom));
                        //VSlot->GetContent()->SetSize(ImVec2(
                        //    Size.x - VSlot->PaddingLeft - VSlot->PaddingRight,
                        //    minSize.y
                        //));
                        currentPos.y += (minSize.y + VSlot->PaddingTop + VSlot->PaddingBottom);
                    }
                    else
                    {
                        // 比例分配控件：高度设为0
                        VSlot->SetSlotSize(ImVec2(0.f, 0.f));
                        //VSlot->GetContent()->SetSize(ImVec2(0.f, 0.f));
                    }
                    VSlot->ApplyLayout();
                }
            }
		}
	};
}