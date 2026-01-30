#pragma once
#include "ImWidget.h"
#include "ImSlot.h"
#include <imgui_internal.h>
#include <vector>
#include <algorithm> // 添加algorithm头文件用于std::find_if

namespace ImGuiWidget
{
	class ImPanelWidget :public ImWidget
	{
	public:
		ImU32 BgColor;
		ImU32 BorderColor;
		bool bHaveBorder = true;
		bool bHaveBackGround = false;
		float BorderThickness = 1.f;
	private:
		std::vector<ImSlot*> m_Slots;
	protected:
		float WidgetHitTestPadding = 5.f;

		virtual void Relayout() {}
		void RenderChild()
		{
			for (auto slot : m_Slots)
			{
				if(slot)
				slot->Render();
			}
		}
		
		void HandleLayout()
		{
			if (bLayOutDirty)
			{
				Relayout();
				ClearLayoutDirty();
			}
		}

		void SetChildAt(int index, ImWidget* child,bool DeleteOld = true)
		{
			if (m_Slots.size() > index)
			{
				if (m_Slots[index] && DeleteOld)
				{
					delete m_Slots[index]->GetContent();
				}
				delete m_Slots[index];
				if (!child)
				{
					auto it = m_Slots.begin();
					it += index;
					m_Slots.erase(it);
				}
				else
				{
					m_Slots[index] = CreateSlot(child);
				}
			}
			else if(m_Slots.size() == index && child)
			{
				for (int i = m_Slots.size(); i < index; i++)
				{
					m_Slots.push_back(nullptr);
				}
				m_Slots.push_back(CreateSlot(child));
			}
			else
			{
				//非法操作
			}
			MarkLayoutDirty();
			if (child)
			{
				child->SetParents(this);
			}
		}

		virtual void HandleChildSizeDirty() 
		{
			MarkLayoutDirty();
		}
	public:
		ImPanelWidget(const std::string& WidgetName)
			:ImWidget(WidgetName),
			BgColor(IM_COL32(255, 255, 255, 255)),
			BorderColor(IM_COL32(0, 0, 0, 255)),
			bHaveBorder(true),
			bHaveBackGround(false)
		{}
		// 拷贝构造函数（深拷贝自身属性）
		ImPanelWidget(const ImPanelWidget& other)
			: ImWidget(other),  // 调用基类拷贝构造
			WidgetHitTestPadding(other.WidgetHitTestPadding),
			BgColor(other.BgColor),
			BorderColor(other.BorderColor),
			bHaveBorder(other.bHaveBorder),
			bHaveBackGround(other.bHaveBackGround)
			// 注意：m_Slots 不拷贝（保持为空vector）
		{
			for (auto& slot : other.m_Slots)
			{
				if (slot&&slot->GetContent())
				{
					auto newwidget = slot->GetContent()->CopyWidget();
					auto newslot = slot->CopySlot()->CopySlot();
					newslot->SetContent(newwidget);
					newwidget->SetParents(this);
					m_Slots.push_back(newslot);
				}
			}
			MarkLayoutDirty();
		}

		// 插入子控件到指定位置
		virtual ImSlot* InsertChildAt(int index, ImWidget* child)override
		{
			if (index < 0 || index > static_cast<int>(m_Slots.size()))
			{
				return nullptr;
			}
			ImSlot* newSlot = CreateSlot(child);
			m_Slots.insert(m_Slots.begin() + index, newSlot);
			MarkLayoutDirty();
			child->SetParents(this);
			return newSlot;
		}
		virtual int GetAllowMaxChildNum()override { return INT_MAX; }
		// 赋值运算符（深拷贝自身属性）
		//ImPanelWidget& operator=(const ImPanelWidget& other) {
		//	if (this != &other) {
		//		// 拷贝基类属性
		//		ImWidget::operator=(other);

		//		// 拷贝自身属性
		//		WidgetHitTestPadding = other.WidgetHitTestPadding;
		//		BgColor = other.BgColor;
		//		BorderColor = other.BorderColor;
		//		bHaveBorder = other.bHaveBorder;
		//		bHaveBackGround = other.bHaveBackGround;
		//		bLayOutDirty = other.bLayOutDirty;

		//		// 明确不拷贝的成员：
		//		// m_Slots 保持为空（不拷贝子项列表）
		//	}
		//	return *this;
		//}
		virtual ImSlot* CreateSlot(ImWidget* Content) = 0;

		virtual ~ImPanelWidget() // 添加析构函数管理内存
		{
			RemoveAllChild(true);
		}

		virtual ImSlot* AddChild(ImWidget* child,ImVec2 RelativePosition=ImVec2(FLT_MIN,FLT_MIN))override
		{
			if (m_Slots.size() >= GetAllowMaxChildNum()) return nullptr;
			ImSlot* newslot = CreateSlot(child);
			m_Slots.push_back(newslot);
			MarkLayoutDirty();
			MarkSizeDirty();
			child->SetParents(this);
			return newslot;
		}

		virtual void RemoveAllChild(bool bDeleteContent = false)override
		{
			for (auto& slot : m_Slots)
			{
				if (slot)
				{
					if (slot->GetContent() && bDeleteContent)
					{
						delete slot->GetContent();
					}
					delete slot;
				}
			}
			m_Slots.clear();
		}

		// 按索引移除子控件
		virtual bool RemoveChildAt(int index,bool bDeleteOld=true)override
		{
			if (index >= 0 && index < static_cast<int>(m_Slots.size()))
			{
				if (m_Slots[index])
				{
					if (bDeleteOld)
					{
						delete m_Slots[index]->GetContent();
					}	
				}
				delete m_Slots[index]; // 删除slot对象
				m_Slots.erase(m_Slots.begin() + index);
				MarkLayoutDirty();
				return true;
			}
			else
			{
				return false;
			}
		}

		// 按指针移除子控件
		virtual bool RemoveChild(ImWidget* child, bool bDeleteOld = false)override
		{
			auto it = std::find_if(m_Slots.begin(), m_Slots.end(),
				[child](ImSlot* slot) {
					return slot->GetContent() == child;
				});

			if (it != m_Slots.end())
			{
				delete* it; // 删除slot对象
				m_Slots.erase(it);
				MarkLayoutDirty();
				if (bDeleteOld)
				{
					delete child;
				}
				return true;
			}
			return false;
		}

		// 新增：取出子控件（移除slot但保留子控件指针）
		virtual ImWidget* ExtractChildAt(int index)override
		{
			if (index >= 0 && index < static_cast<int>(m_Slots.size()))
			{
				if (!m_Slots[index])
				{
					return nullptr;
				}
				ImWidget* child = m_Slots[index]->GetContent();
				delete m_Slots[index]; // 删除slot对象
				m_Slots.erase(m_Slots.begin() + index);
				MarkLayoutDirty();
				return child;
			}
			return nullptr;
		}

		// 新增：取出子控件（通过指针查找）
		//ImWidget* ExtractChild(ImWidget* child)
		//{
		//	auto it = std::find_if(m_Slots.begin(), m_Slots.end(),
		//		[child](ImSlot* slot) {
		//			return slot->GetContent() == child;
		//		});

		//	if (it != m_Slots.end())
		//	{
		//		ImWidget* result = (*it)->GetContent();
		//		delete* it; // 删除slot对象
		//		m_Slots.erase(it);
		//		return result;
		//	}
		//	return nullptr;
		//}

		// 获取子控件指针
		virtual ImWidget* GetChildAt(int index)override
		{
			if (index >= 0 && index < static_cast<int>(m_Slots.size()))
				return m_Slots[index]->GetContent();
			return nullptr;
		}

		virtual ImSlot* GetSlotAt(int index)override
		{
			if (index >= 0 && index < static_cast<int>(m_Slots.size()))
				return m_Slots[index];
			return nullptr;
		}

		virtual void Render() override
		{

		}

		void RenderBackGround()
		{
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			if (bHaveBackGround)
			{
				window->DrawList->AddRectFilled(Position, Position + Size, BgColor);
			}
			if (bHaveBorder)
			{
				window->DrawList->AddRect(Position, Position + Size, BorderColor);
			}
		}

		void SetBackGroundColor(ImU32 color) { BgColor = color; }

		void SetBorderColor(ImU32 color) { BorderColor = color; }

		virtual int GetChildNum()override { return static_cast<int>(m_Slots.size()); }

		virtual ImWidget* ChildHitTest(ImVec2 Pos) override
		{

			ImRect PanelHitRect(Position, Position + Size);
			if (PanelHitRect.Contains(Pos))
			{
				for (int i = m_Slots.size(); i > 0; i--)
				{
					ImWidget* ChildHitWidget = m_Slots[i - 1]->GetContent()->ChildHitTest(Pos);
					if (ChildHitWidget)
					{
						return ChildHitWidget;
						//ImVec2 Min = Position;
						//ImVec2 Max = Position + Size;
						//if (Size.x > WidgetHitTestPadding * 2 && Size.y > WidgetHitTestPadding * 2)
						//{
						//	Min.x += WidgetHitTestPadding;
						//	Min.y += WidgetHitTestPadding;
						//	Max.x -= WidgetHitTestPadding;
						//	Max.y -= WidgetHitTestPadding;
						//}
						//ImRect ChildHitRect(Min, Max);
						//if (ChildHitRect.Contains(Pos))
						//{
						//	return ChildHitWidget;
						//}
						//else
						//{
						//	return this;
						//}
					}
				}
				return this;
			}
			else
			{
				return nullptr;
			}

			//if (ImWidget::ChildHitTest(Pos))
			//{
			//	for (int i = m_Slots.size(); i > 0; i--)
			//	{
			//		ImWidget* ChildHitWidget = m_Slots[i-1]->GetContent()->ChildHitTest(Pos);
			//		if (ChildHitWidget)
			//		{
			//			return ChildHitWidget;
			//		}
			//	}
			//	return this;
			//}
			//else
			//{
			//	return nullptr;
			//}
		}

		virtual void SetPosition(ImVec2 Pos)override
		{
			if (Pos == Position) return;
			Position = Pos;
			MarkLayoutDirty();
		}

		virtual void SetSize(ImVec2 size)override
		{
			if (size == Size) return;
			Size = size;
			MarkLayoutDirty();
		}

		virtual std::unordered_set<PropertyInfo, PropertyInfo::Hasher> GetProperties() override
		{
			auto baseProps = ImWidget::GetProperties();

			baseProps.insert(
				{
					"BackGroundColor",
					PropertyType::Color,
					"Style",
					[this](void* v) { BgColor = *static_cast<ImU32*>(v); },
					[this]() -> void* { return &BgColor; }
				}
			);

			baseProps.insert(
				{
					"HaveBorder",
					PropertyType::Bool,
					"Style",
					[this](void* v){bHaveBorder= *static_cast<bool*>(v); },
					[this]()->void* {return &bHaveBorder; }
				}
			);

			baseProps.insert(
				{
					"BorderColor",
					PropertyType::Color,
					"Style",
					[this](void* v) { BorderColor = *static_cast<ImU32*>(v); },
					[this]() -> void* { return &BorderColor; }
				}
			);

			return baseProps;
		}

		virtual std::string GetRegisterTypeName()override { return "ImPanelWidget"; }

		virtual ImWidget* CopyWidget() = 0;

		DECLARE_IMOBJECT(ImPanelWidget, ImWidget)
		registrar
			.RegisterProperty(PropertyType::Color, "BackGroundColor", &ImPanelWidget::BgColor, u8"背景颜色")
			.RegisterProperty(PropertyType::Bool, "HaveBorder", &ImPanelWidget::bHaveBorder, u8"是否有边框")
			.RegisterProperty(PropertyType::Color, "BorderColor", &ImPanelWidget::BorderColor, u8"边框颜色")
			.RegisterProperty(PropertyType::Bool, "HaveBackGround", &ImPanelWidget::bHaveBackGround, u8"是否有背景")
			.RegisterProperty(PropertyType::Float, "BorderThickness", &ImPanelWidget::BorderThickness, u8"边框粗细");
		END_DECLARE_IMOBJECT()
	};
}