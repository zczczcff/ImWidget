#pragma once
#include <imgui.h>
#include <string>
#include "ImObject.h"
#include "ImWidget/ImWidget.h"

namespace ImGuiWidget
{
	class ImWidget;
	class ImSlot :public ImObject
	{
	protected:
		ImWidget* Content;
		ImWidget* Owner;
		bool bAutoSize = true;
		//用于缓存布局信息
		ImVec2 SlotPosition = ImVec2(0.f, 0.f);
		ImVec2 SlotSize = ImVec2(0.f, 0.f);
	public:
		ImSlot():Content(nullptr), Owner(nullptr){}
		ImSlot(ImWidget* Content, ImWidget* Owner):Content(Content),Owner(Owner)
		{
			if (Content)
			{
				Content->SetSlot(this);
			}
		}
		ImSlot(const ImSlot& other)
			:Content(nullptr), 
			Owner(nullptr),
			bAutoSize(other.bAutoSize),
			SlotPosition(other.SlotPosition),
			SlotSize(other.SlotSize)
		{

		}
		void SetContent(ImWidget* ContentToSet)
		{
			if (ContentToSet)
			{
				Content = ContentToSet;
				Content->SetSlot(this);
			}
		}
		ImWidget* GetContent() { return Content; }
		virtual void ApplyLayout()
		{
			if (!Content)return;
			Content->SetPosition(SlotPosition);
			Content->SetSize(SlotSize);
		}
		bool& GetIfAutoSize() { return bAutoSize; }
		void SetIfAutoSize(bool& Value) 
		{
			if (bAutoSize != Value)
			{
				bAutoSize = Value;
				Owner->MarkLayoutDirty();
			}
		}
		void SetIfAutoSize(const bool& Value)
		{
			if (bAutoSize != Value)
			{
				bAutoSize = Value;
				Owner->MarkLayoutDirty();
			}
		}
		void Render()
		{
			if (Content)
			{
				Content->Render();
			}
		}
		bool IsValid()
		{
			return Content;
		}
		void SetSlotPosition(ImVec2 Position)
		{
			SlotPosition = Position;
		}
		void SetSlotSize(ImVec2 Size)
		{
			SlotSize = Size;
		}
		virtual std::unordered_set<PropertyInfo, PropertyInfo::Hasher> GetProperties()
		{
			std::unordered_set<PropertyInfo, PropertyInfo::Hasher> props;
			props.insert
			(
				{
				"bAutoSize",
				PropertyType::Bool,
				"Layout",
				[this](void* v) { SetIfAutoSize(*static_cast<bool*>(v)); },
				[this]() -> void* { return &bAutoSize; }
				});
			return props;
		}

		virtual ~ImSlot()
		{
			//delete Content;
		}

		virtual ImSlot* CopySlot()
		{
			return new ImSlot(*this);
		}

		DECLARE_IMOBJECT(ImSlot, ImObject)
		registrar
			.RegisterProperty(PropertyType::Bool, "bAutoSize",
				&ImSlot::SetIfAutoSize, &ImSlot::GetIfAutoSize, u8"是否自动调整大小");
		END_DECLARE_IMOBJECT()
	};

	class ImPaddingSlot :public ImSlot
	{
	public:
		float PaddingTop = 0.f;
		float PaddingBottom = 0.f;
		float PaddingLeft = 0.f;
		float PaddingRight = 0.f;

		ImPaddingSlot(){}
		ImPaddingSlot(ImWidget* Content,ImWidget* Owner):ImSlot(Content,Owner){}

		virtual void ApplyLayout()
		{
			if (!Content)return;
			ImVec2 Rect_Min(SlotPosition.x + PaddingLeft, SlotPosition.y + PaddingTop);
			ImVec2 Rect_Max(SlotPosition.x + SlotSize.x - PaddingRight, SlotPosition.y + SlotSize.y - PaddingBottom);
			ImVec2 WidgetSize = Rect_Max - Rect_Min;
			WidgetSize.x = ImMax(0.f, WidgetSize.x);
			WidgetSize.y = ImMax(0.f, WidgetSize.y);
			Content->SetPosition(Rect_Min);
			Content->SetSize(WidgetSize);
		}
		void SetPadding(float SetPaddingTop, float SetPaddingBottom, float SetPaddingLeft, float SetPaddingRight)
		{
			PaddingTop = SetPaddingTop;
			PaddingBottom = SetPaddingBottom;
			PaddingLeft = SetPaddingLeft;
			PaddingRight = SetPaddingRight;
		}
		void SetPaddingTop(float& value) { PaddingTop = value; if (Owner) Owner->MarkLayoutDirty(); }
		float& GetPaddingTop() { return PaddingTop; }

		void SetPaddingBottom(float& value) { PaddingBottom = value; if (Owner) Owner->MarkLayoutDirty(); }
		float& GetPaddingBottom() { return PaddingBottom; }

		void SetPaddingLeft(float& value) { PaddingLeft = value; if (Owner) Owner->MarkLayoutDirty(); }
		float& GetPaddingLeft() { return PaddingLeft; }

		void SetPaddingRight(float& value) { PaddingRight = value; if (Owner) Owner->MarkLayoutDirty(); }
		float& GetPaddingRight() { return PaddingRight; }
		virtual std::unordered_set<PropertyInfo, PropertyInfo::Hasher> GetProperties() override 
		{
			auto props = ImSlot::GetProperties();
			props.insert
			({
				"PaddingTop",
				PropertyType::Float,
				"Padding",
				[this](void* v) { PaddingTop = *static_cast<float*>(v); Owner->MarkLayoutDirty(); },
				[this]() -> void* { return &PaddingTop; }
				});

			props.insert
			({
				"PaddingBottom",
				PropertyType::Float,
				"Padding",
				[this](void* v) { PaddingBottom = *static_cast<float*>(v); Owner->MarkLayoutDirty();},
				[this]() -> void* { return &PaddingBottom; }
				});
			props.insert
			({
				"PaddingLeft",
				PropertyType::Float,
				"Padding",
				[this](void* v) { PaddingLeft = *static_cast<float*>(v); Owner->MarkLayoutDirty();},
				[this]() -> void* { return &PaddingLeft; }
				});
			props.insert
			({
				"PaddingRight",
				PropertyType::Float,
				"Padding",
				[this](void* v) { PaddingRight = *static_cast<float*>(v); Owner->MarkLayoutDirty();},
				[this]() -> void* { return &PaddingRight; }
				});
			return props;
		}

		virtual ImSlot* CopySlot() override
		{
			return new ImPaddingSlot(*this);
		}

		DECLARE_IMOBJECT(ImPaddingSlot, ImSlot)
		registrar
			.RegisterProperty(PropertyType::Float, "PaddingTop",
				&ImPaddingSlot::SetPaddingTop, &ImPaddingSlot::GetPaddingTop, u8"上边距")
			.RegisterProperty(PropertyType::Float, "PaddingBottom",
				&ImPaddingSlot::SetPaddingBottom, &ImPaddingSlot::GetPaddingBottom, u8"下边距")
			.RegisterProperty(PropertyType::Float, "PaddingLeft",
				&ImPaddingSlot::SetPaddingLeft, &ImPaddingSlot::GetPaddingLeft, u8"左边距")
			.RegisterProperty(PropertyType::Float, "PaddingRight",
				&ImPaddingSlot::SetPaddingRight, &ImPaddingSlot::GetPaddingRight, u8"右边距");
		END_DECLARE_IMOBJECT()
	};


}