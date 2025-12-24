#pragma once
#include <imgui.h>
#include <string>
#include <imgui_internal.h>
#include <vector>

#include "ImWidgetProperty.h"
namespace ImGuiWidget
{
	class ImPanelWidget;
	class ImWidget
	{
	protected:
		std::string m_WidgetID;
		std::string m_WidgetName;
		ImVec2 Position = { 0, 0 };  // 相对位置
		ImVec2 Size = { 0, 0 };      // 控件尺寸
		bool Visibility = true;     // 可见性
		class ImSlot* m_Slot;
		class ImWidget* m_Parents;
		bool bSizeDirty;
		bool bVisible = true;
		//处理子控件最小尺寸发生变化的情况
		virtual void HandleChildSizeDirty(){}

		//控件最小尺寸发生变化时调用
		virtual void MarkSizeDirty()
		{
			if (m_Parents)
			{
				m_Parents->HandleChildSizeDirty();
			}
		}
	public:
		ImWidget(const std::string& WidgetName)
			:m_WidgetID(GetRegisterTypeName()),
			m_WidgetName(WidgetName),
			m_Slot(nullptr),
			m_Parents(nullptr),
			bSizeDirty(false)
		{
			m_WidgetID += std::to_string(GetConstructCounter());
		}
		// 拷贝构造函数（深拷贝自身属性）
		ImWidget(const ImWidget& other)
			: m_WidgetID(GetRegisterTypeName()), // 生成新ID（唯一标识）
			m_WidgetName(other.m_WidgetName),
			Position(other.Position),
			Size(other.Size),
			Visibility(other.Visibility),
			m_Slot(nullptr),        // 不拷贝slot指针
			m_Parents(nullptr),     // 不拷贝父项指针
			bSizeDirty(other.bSizeDirty)
		{
			m_WidgetID += std::to_string(GetConstructCounter());
		}

		// 赋值运算符（深拷贝自身属性）
		ImWidget& operator=(const ImWidget& other) 
		{
			if (this != &other) 
			{
				// 仅拷贝可复制属性
				m_WidgetName = other.m_WidgetName;
				Position = other.Position;
				Size = other.Size;
				Visibility = other.Visibility;
				bSizeDirty = other.bSizeDirty;
				m_WidgetID = GetRegisterTypeName();
				m_WidgetID+= std::to_string(GetConstructCounter());
				// 明确不拷贝的成员：
				// m_Slot 保持为nullptr (不拷贝slot关系)
				// m_Parents 保持为nullptr (不拷贝父项关系)
			}
			return *this;
		}
		static int GetConstructCounter()
		{
			static int counter = 0;
			counter++;
			return counter;
		}
		void SetParents(ImWidget* parents)
		{
			m_Parents = parents;
		}
		ImPanelWidget* GetParents()
		{
			return (ImPanelWidget*)m_Parents;
		}
		void SetSlot(ImSlot* slot)
		{
			m_Slot = slot;
		}
		ImSlot* GetSlot()
		{
			return m_Slot;
		}
		virtual ImWidget* ChildHitTest(ImVec2 Pos)
		{
			ImVec2 Min = Position;
			ImVec2 Max = Position + Size;
			//if (Size.x > WidgetHitTestPadding*2&&Size.y> WidgetHitTestPadding * 2)
			//{
			//	Min.x += WidgetHitTestPadding;
			//	Min.y += WidgetHitTestPadding;
			//	Max.x -= WidgetHitTestPadding;
			//	Max.y -= WidgetHitTestPadding;
			//}
			if (ImRect(Min, Max).Contains(Pos))
				return this;
			else
				return nullptr;
		}
		virtual void SetPosition(ImVec2 Pos) { Position = Pos; }
		virtual void SetSize(ImVec2 size) { Size = size; }
		ImVec2 GetPosition() { return Position; }
		ImVec2 GetSize() { return Size; }
		ImRect GetRect() { return ImRect(Position, Position + Size); }
		virtual void Render(){}
		virtual ImVec2 GetMinSize() { return ImVec2(-1.f, -1.f); }
		std::string GetWidgetName() { return m_WidgetName; }

		virtual std::unordered_set<PropertyInfo, PropertyInfo::Hasher> GetProperties()
		{
			std::unordered_set<PropertyInfo, PropertyInfo::Hasher> Props;
			Props.insert(
			{
				"Name",PropertyType::String,"Name",
				[this](void* v) {m_WidgetName = *(std::string*)v; },
				[this]()->void* {return &m_WidgetName; }
			});
			return Props;
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

		virtual std::string GetRegisterTypeName() { return "ImWidget"; }

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

		bool IsInTree(ImWidget* WidgetTree)
		{
			ImWidget* CurrentTreeNode = this;
			while (CurrentTreeNode != nullptr)
			{
				if (CurrentTreeNode == WidgetTree)
				{
					return true;
				}
				else
				{
					CurrentTreeNode = CurrentTreeNode->m_Parents;
				}
			}
			return false;
		}

		bool IsVisible() { return bVisible; }

		virtual ImWidget* CopyWidget()
		{
			return nullptr;
		}

		// 事件系统新增：事件处理入口
		virtual void HandleEvent(class ImEvent* event);
	protected:
		virtual void HandleEventInternal(class ImEvent* event)
		{}
	};
}