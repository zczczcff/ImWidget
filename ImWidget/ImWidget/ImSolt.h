#pragma once
#include <imgui.h>
#include <string>

namespace ImGuiWidget
{
	class ImWidget;
	class ImSlot
	{
	protected:
		ImWidget* Content;
		bool bAutoSize = true;
		//用于缓存布局信息
		ImVec2 SlotPosition;
		ImVec2 SlotSize;
	public:
		ImSlot():Content(nullptr) {}
		ImSlot(ImWidget* Content):Content(Content)
		{
			if (Content)
			{
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
		bool GetIfAutoSize() { return bAutoSize; }
		void SetIfAutoSize(bool Value) { bAutoSize = Value; }
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
				[this](void* v) { bAutoSize = *static_cast<bool*>(v); },
				[this]() -> void* { return &bAutoSize; }
				});
			return props;
		}
		
		template<typename T>
		T* GetPropertyPtr(const std::string& name)
		{
			auto properties = GetProperties();
			PropertyInfo temp;
			temp.name = name;

			auto it = properties.find(temp);
			if (it != properties.end())
			{
				return ((T*)it->getter());
			}
		}
		bool SetProperty(const std::string& name, void* value)
		{
			auto properties = GetProperties();
			PropertyInfo temp;
			temp.name = name;

			auto it = properties.find(temp);
			if (it != properties.end())
			{
				it->setter(value);
				return true;
			}
			return false;
		}

		template<typename T>
		bool SetPropertyValue(const std::string& name, const T& value)
		{
			T copy = value;
			return SetProperty(name, &copy);
		}

		virtual ~ImSlot()
		{
			//delete Content;
		}
	};

	class ImPaddingSlot :public ImSlot
	{
	public:
		float PaddingTop = 0.f;
		float PaddingBottom = 0.f;
		float PaddingLeft = 0.f;
		float PaddingRight = 0.f;

		ImPaddingSlot(){}
		ImPaddingSlot(ImWidget* Content):ImSlot(Content){}

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

		virtual std::unordered_set<PropertyInfo, PropertyInfo::Hasher> GetProperties() override 
		{
			auto props = ImSlot::GetProperties();
			props.insert
			({
				"PaddingTop",
				PropertyType::Float,
				"Padding",
				[this](void* v) { PaddingTop = *static_cast<float*>(v); },
				[this]() -> void* { return &PaddingTop; }
				});

			props.insert
			({
				"PaddingBottom",
				PropertyType::Float,
				"Padding",
				[this](void* v) { PaddingBottom = *static_cast<float*>(v); },
				[this]() -> void* { return &PaddingBottom; }
				});
			props.insert
			({
				"PaddingLeft",
				PropertyType::Float,
				"Padding",
				[this](void* v) { PaddingLeft = *static_cast<float*>(v); },
				[this]() -> void* { return &PaddingLeft; }
				});
			props.insert
			({
				"PaddingRight",
				PropertyType::Float,
				"Padding",
				[this](void* v) { PaddingRight = *static_cast<float*>(v); },
				[this]() -> void* { return &PaddingRight; }
				});
			return props;
		}
	};


}