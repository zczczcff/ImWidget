#pragma once
#include <imgui.h>
#include <string>
#include <imgui_internal.h>
#include <vector>

#include "ImWidgetProperty.h"
#include "ImTools/ImWidgetRef.h"
namespace ImGuiWidget
{
	class ImPanelWidget;
	class ImWidget :public PropertyStruct
	{
	private:
		ImWidgetRef m_selfRef;
	protected:
		std::string m_WidgetID;
		std::string m_WidgetName;
		ImVec2 Position = { 0, 0 };  // 相对位置
		ImVec2 Size = { 0, 0 };      // 控件尺寸
		class ImSlot* m_Slot;
		class ImWidget* m_Parents;

		float m_HoverTime = 0.5;	//悬停计时，单位（s）
		bool bVisible = true;     // 可见性
		bool bFocusable = false;
		bool bHasFocus = false;
		bool bHoverable = false;
		bool bHovered = false;
		bool bSizeDirty = false;
		
		//处理子控件最小尺寸发生变化的情况
		virtual void HandleChildSizeDirty(){}

		//控件最小尺寸发生变化时调用
		virtual void MarkSizeDirty();
	public:
		ImWidget(const std::string& WidgetName)
			:m_selfRef(this),// 新对象创建新的自我引用
			m_WidgetID(GetRegisterTypeName()),
			m_WidgetName(WidgetName),
			m_Slot(nullptr),
			m_Parents(nullptr),
			bSizeDirty(false)
		{
			m_WidgetID += std::to_string(GetConstructCounter());
		}
		// 拷贝构造函数（深拷贝自身属性）
		ImWidget(const ImWidget& other)
			: m_selfRef(this),// 新对象创建新的自我引用
			m_WidgetID(GetRegisterTypeName()), // 生成新ID（唯一标识）
			m_WidgetName(other.m_WidgetName),
			Position(other.Position),
			Size(other.Size),
			bVisible(other.bVisible),
			m_Slot(nullptr),        // 不拷贝slot指针
			m_Parents(nullptr),     // 不拷贝父项指针
			bSizeDirty(other.bSizeDirty)
		{
			m_WidgetID += std::to_string(GetConstructCounter());
		}
		virtual ~ImWidget()
		{
			m_selfRef.Invalidate();
		}
		// 赋值运算符（深拷贝自身属性）
		ImWidget& operator=(const ImWidget& other) 
		{
			if (this != &other) 
			{
				// 仅拷贝可复制属性
				// 注意：不能赋值m_selfRef
				m_WidgetName = other.m_WidgetName;
				Position = other.Position;
				Size = other.Size;
				bVisible = other.bVisible;
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
		void SetWidgetName(const std::string& NewName) { m_WidgetName = NewName; }

		void SetParents(ImWidget* parents)
		{
			m_Parents = parents;
		}

		virtual ImSlot* InsertChildAt(int index, ImWidget* child) { return nullptr; }
		virtual ImSlot* AddChild(ImWidget* child, ImVec2 RelativePosition = ImVec2(FLT_MIN, FLT_MIN)) { return nullptr; }
		virtual int GetChildNum() { return 0; }
		virtual int GetAllowMaxChildNum() { return 0; }
		virtual void RemoveAllChild(bool bDeleteContent = false){}
		virtual bool RemoveChildAt(int index, bool bDeleteOld = true) { return false; }
		virtual bool RemoveChild(ImWidget* child, bool bDeleteOld = false) { return false; }
		virtual ImWidget* ExtractChildAt(int index) { return nullptr; }
		virtual ImWidget* GetChildAt(int index) { return nullptr; }
		virtual ImSlot* GetSlotAt(int index) { return nullptr; }

		ImPanelWidget* GetParents()
		{
			return (ImPanelWidget*)m_Parents;
		}
		void SetSlot(ImSlot* slot)
		{
			m_Slot = slot;
		}
		ImSlot* GetSlotAt()
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

		virtual std::unordered_set<PropertyInfo, PropertyInfo::Hasher> GetProperties() override
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

		virtual std::string GetRegisterTypeName() { return "ImWidget"; }

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

		bool IsVisible() const { return bVisible; }

		virtual ImWidget* CopyWidget()
		{
			return nullptr;
		}

		// 事件系统新增：事件处理入口
		virtual void HandleEvent(class ImEvent* event);
	protected:
		virtual void HandleEventInternal(class ImEvent* event)
		{}

		virtual void OnHoverStart() {};
		virtual void OnHover() {};
		virtual void OnHoverEnd() {};
	public:
		// 设置控件是否可获取焦点
		void SetFocusable(bool focusable)
		{
			bFocusable = focusable;
		}

		// 检查控件是否可获取焦点
		bool IsFocusable() const
		{
			return bFocusable && IsVisible();
		}

		// 检查控件当前是否有焦点
		bool HasFocus() const
		{
			return bHasFocus;
		}

		void SetFocused(bool focused)
		{
			bHasFocus = focused;
		}

		void GetFocus()
		{
			bHasFocus = true;
		}

		// 请求焦点
		bool RequestFocus();

		// 失去焦点
		void LoseFocus()
		{
			if (bHasFocus)
			{
				bHasFocus = false;
			}
		}

		ImWidgetRef GetWidgetRef() const
		{
			return m_selfRef;  // 返回拷贝，增加引用计数
		}

		bool IsHoverable() const{ return bHoverable && IsVisible(); }

		void SetHoverable(bool hoverable) { bHoverable = hoverable; }

		float GetHoverTime()const { return m_HoverTime; }

		void SetHoverTime(float Hovertime_Second) { m_HoverTime = Hovertime_Second; }
	};
}