#pragma once
#include "ImPanelWidget.h"


namespace ImGuiWidget
{
	class ImVerticalBoxSlot :public ImPaddingSlot
	{
	public:
		ImVerticalBoxSlot(ImWidget* Content, ImWidget* Owner):ImPaddingSlot(Content,Owner){}

		float SizeRatio = 1.f;

        virtual std::unordered_set<PropertyInfo, PropertyInfo::Hasher> GetProperties()
        {
            auto props = ImPaddingSlot::GetProperties();

            props.insert({
                "SizeRatio",
                PropertyType::Float,
                "Layout",
                [this](void* val) { this->SizeRatio = *static_cast<float*>(val); Owner->MarkLayoutDirty();},
                [this]() { return static_cast<void*>(&this->SizeRatio); }
                });

            return props;
        }

        virtual ImSlot* CopySlot() override
        {
            return new ImVerticalBoxSlot(*this);
        }
	};


	class ImVerticalBox :public ImPanelWidget
	{
    protected:
        virtual ImSlot* CreateSlot(ImWidget* Content)override
        {
            return new ImVerticalBoxSlot(Content,this);
        }
		
	public:
		ImVerticalBox(const std::string& WidgetName):ImPanelWidget(WidgetName){}
		ImVerticalBoxSlot* AddChildToVerticalBox(ImWidget* child)
		{
            return static_cast<ImVerticalBoxSlot*>(ImPanelWidget::AddChild(child));
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
			float minheight = 0.f;
			float minlength = 0.f;
            for (int i = 0; i < GetChildNum(); i++)
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

            return ImVec2(ImMax(minlength, 30.f), ImMax(minheight, 10.f)) + (bHaveBorder ? ImVec2(2 * BorderThickness, 2 * BorderThickness) : ImVec2(0, 0));
		}

		virtual void Relayout()override
		{
            float requiredHeight = 0.f; // 非自动大小控件所需的总高度
            float remainingHeight = 0.f; // 剩余高度（用于比例分配）
            float sumSizeRatio = 0.f;   // 比例总和

            // 第一步：计算非自动大小控件所需高度和比例总和
            for (int i=0;i<GetChildNum();i++)
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

            remainingHeight = Size.y - requiredHeight - (bHaveBorder? 2 * BorderThickness:0);

            ImVec2 currentPos = Position + (bHaveBorder ? ImVec2(BorderThickness, BorderThickness) : ImVec2(0.f, 0.f)); // 当前位置

            if (remainingHeight > 0.f) // 有足够空间
            {
                for (int i = 0; i < GetChildNum(); i++)
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
                        VSlot->SetSlotSize(ImVec2(Size.x-(bHaveBorder? 2 * BorderThickness : 0),minSize.y + VSlot->PaddingTop + VSlot->PaddingBottom));
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
                        VSlot->SetSlotSize(ImVec2(Size.x - (bHaveBorder ? 2 * BorderThickness : 0), ratioHeight));
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
                for (int i = 0; i < GetChildNum(); i++)
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
                        VSlot->SetSlotSize(ImVec2(Size.x - (bHaveBorder ? 2 * BorderThickness : 0), minSize.y + VSlot->PaddingTop + VSlot->PaddingBottom));
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

        virtual std::string GetRegisterTypeName()override { return "ImVerticalBox"; }

        virtual ImWidget* CopyWidget()
        {
            return new ImVerticalBox(*this);
        }
	};
}