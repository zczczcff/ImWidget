#pragma once
#include "ImPanelWidget.h"

namespace ImGuiWidget
{
	class ImCanvasPanelSlot :public ImSlot
	{
	public:
		ImCanvasPanelSlot(ImWidget* Content, ImWidget* Owner):ImSlot(Content, Owner){}
		ImVec2 RelativePosition;
		ImVec2 SlotSize;
		void SetSlotPosAndSize(const ImVec2& relativepos, const ImVec2& size)
		{
			RelativePosition = relativepos;
			SlotSize = size;
		}
		virtual std::unordered_set<PropertyInfo, PropertyInfo::Hasher> GetProperties() override 
		{
			auto props = ImSlot::GetProperties();
			props.insert({
				"RelativePosition",
				PropertyType::Vec2,
				"Layout",
				[this](void* v) { RelativePosition = *static_cast<ImVec2*>(v); },
				[this]() -> void* { return &RelativePosition; }
				});
			props.insert({
				"SlotSize",
				PropertyType::Vec2,
				"Layout",
				[this](void* v) { SlotSize = *static_cast<ImVec2*>(v); },
				[this]() -> void* { return &SlotSize; }
				});
			return props;
		}
		virtual ImSlot* CopySlot() override
		{
			return new ImCanvasPanelSlot(*this);
		}
		DECLARE_IMOBJECT(ImCanvasPanelSlot, ImSlot)
		registrar
			.RegisterProperty(PropertyType::Vec2, "RelativePosition",
				&ImCanvasPanelSlot::RelativePosition, u8"相对位置")
			.RegisterProperty(PropertyType::Vec2, "SlotSize",
				&ImCanvasPanelSlot::SlotSize, u8"插槽大小");
		END_DECLARE_IMOBJECT()
	};

	class ImCanvasPanel :public ImPanelWidget
	{
	private:

	protected:
		virtual void Relayout() override
		{
			for (int i = 0; i < GetChildNum(); i++)
			{
				if (auto slot = (ImCanvasPanelSlot*)GetSlotAt(i))
				{

					slot->SetSlotPosition(Position + (slot->RelativePosition)*Size);
					slot->SetSlotSize(slot->SlotSize*Size);
					slot->ApplyLayout();
				}
			}
		}
	public:
		ImCanvasPanel(const std::string& WidgetName):ImPanelWidget(WidgetName){}
		virtual ImSlot* CreateSlot(ImWidget* Content)
		{
			return new ImCanvasPanelSlot(Content, this);
		}
		ImCanvasPanelSlot* AddChildToCanvasPanel(ImWidget* Child)
		{
			ImCanvasPanelSlot* slot = static_cast<ImCanvasPanelSlot*> (ImPanelWidget::AddChild(Child));
			slot->RelativePosition = ImVec2(0, 0);
			slot->SlotSize = Child->GetMinSize();
			return slot;
		}
		virtual ImSlot* AddChild(ImWidget* Child, ImVec2 RelativePosition = ImVec2(FLT_MIN, FLT_MIN))override
		{
			ImCanvasPanelSlot* slot = AddChildToCanvasPanel(Child);
			slot->RelativePosition = RelativePosition == ImVec2(FLT_MIN, FLT_MIN) ? ImVec2(0.f, 0.f) : RelativePosition / Size;
			slot->SlotSize = ImVec2(0.1f, 0.1f);
			return  slot;
		}
		virtual ImVec2 GetMinSize()override
		{
			return Size;
		}
		virtual void Render() override
		{
			HandleLayout();
			RenderBackGround();
			RenderChild();
		}
		virtual std::string GetRegisterTypeName()override { return "ImCanvasPanel"; }

		virtual ImWidget* CopyWidget()
		{
			return new ImCanvasPanel(*this);
		}
		DECLARE_IMOBJECT(ImCanvasPanel, ImPanelWidget)
		END_DECLARE_IMOBJECT()
	};
}