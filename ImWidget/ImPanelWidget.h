#pragma once
#include "ImWidget.h"
#include "ImSolt.h"
#include <imgui_internal.h>
#include <vector>
#include <algorithm> // 添加algorithm头文件用于std::find_if

namespace ImGuiWidget
{
	class ImPanelWidget :public ImWidget
	{
	private:
		std::vector<ImSlot*> m_Slots;
	protected:
		float  WidgetHitTestPadding = 5.f;
		ImU32 BgColor;
		ImU32 BorderColor;
		bool bHaveBorder;
		bool bHaveBackGround;
		bool bLayOutDirty = false;


		virtual void Relayout() {}
		void RenderChild()
		{
			for (auto slot : m_Slots)
			{
				if(slot)
				slot->Render();
			}
		}
		virtual ImSlot* CreateSlot(ImWidget* Content)
		{
			return new ImSlot(Content);
		}
		void SetLayoutDirty()
		{
			bLayOutDirty = true;
		}
		void ClearLayoutDirty()
		{
			bLayOutDirty = false;
		}

		void HandleLayout()
		{
			if (bLayOutDirty)
			{
				Relayout();
				ClearLayoutDirty();
			}
		}

		template<typename SlotType>
		SlotType* AddChildInternal(ImWidget* child)
		{
			SlotType* newslot = new SlotType(child);
			m_Slots.push_back(newslot);
			SetLayoutDirty();
			child->SetParents(this);
			return newslot;
		}
		void SetChildAt(int index, ImWidget* child)
		{
			if (m_Slots.size() > index)
			{
				if (m_Slots[index])
				{
					delete m_Slots[index]->GetContent();
				}
				delete m_Slots[index];
				m_Slots[index] = CreateSlot(child);
			}
			else
			{
				for (int i = m_Slots.size(); i < index; i++)
				{
					m_Slots.push_back(nullptr);
				}
				m_Slots.push_back(CreateSlot(child));
			}
			SetLayoutDirty();
			child->SetParents(this);
		}
		// 插入子控件到指定位置
		ImSlot* InsertChildAt(int index, ImWidget* child)
		{
			if (index < 0 || index > static_cast<int>(m_Slots.size())) {
				return AddChild(child);
			}
			ImSlot* newSlot = CreateSlot(child);
			m_Slots.insert(m_Slots.begin() + index, newSlot);
			SetLayoutDirty();
			child->SetParents(this);
			return newSlot;
		}
		virtual void HandleChildSizeDirty() 
		{
			SetLayoutDirty();
		}
	public:
		ImPanelWidget(const std::string& WidgetName)
			:ImWidget(WidgetName),
			BgColor(IM_COL32(255, 255, 255, 255)),
			BorderColor(IM_COL32(0, 0, 0, 255)),
			bHaveBorder(true),
			bHaveBackGround(true)
		{}

		virtual ~ImPanelWidget() // 添加析构函数管理内存
		{
			RemoveAllChild();
		}

		virtual ImSlot* AddChild(ImWidget* child,ImVec2 RelativePosition=ImVec2(FLT_MIN,FLT_MIN))
		{
			return AddChildInternal<ImSlot>(child);
		}

		void RemoveAllChild()
		{
			for (auto& slot : m_Slots)
			{
				if (slot)
				{
					if (slot->GetContent())
					{
						delete slot->GetContent();
					}
					delete slot;
				}
			}
			m_Slots.clear();
		}

		// 按索引移除子控件
		void RemoveChildAt(int index)
		{
			if (index >= 0 && index < static_cast<int>(m_Slots.size()))
			{
				if (m_Slots[index])
				{
					delete m_Slots[index]->GetContent();
				}
				delete m_Slots[index]; // 删除slot对象
				m_Slots.erase(m_Slots.begin() + index);
				SetLayoutDirty();
			}
		}

		// 按指针移除子控件
		bool RemoveChild(ImWidget* child)
		{
			auto it = std::find_if(m_Slots.begin(), m_Slots.end(),
				[child](ImSlot* slot) {
					return slot->GetContent() == child;
				});

			if (it != m_Slots.end())
			{
				delete* it; // 删除slot对象
				m_Slots.erase(it);
				SetLayoutDirty();
				delete child;
				return true;
			}
			return false;
		}

		// 新增：取出子控件（移除slot但保留子控件指针）
		ImWidget* ExtractChildAt(int index)
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
				SetLayoutDirty();
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
		ImWidget* GetChildAt(int index)
		{
			if (index >= 0 && index < static_cast<int>(m_Slots.size()))
				return m_Slots[index]->GetContent();
			return nullptr;
		}

		ImSlot* GetSlotAt(int index)
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

		int GetSlotNum() { return static_cast<int>(m_Slots.size()); }

		ImSlot* GetSlot(int Index) { return m_Slots[Index]; }

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
						ImVec2 Min = Position;
						ImVec2 Max = Position + Size;
						if (Size.x > WidgetHitTestPadding * 2 && Size.y > WidgetHitTestPadding * 2)
						{
							Min.x += WidgetHitTestPadding;
							Min.y += WidgetHitTestPadding;
							Max.x -= WidgetHitTestPadding;
							Max.y -= WidgetHitTestPadding;
						}
						ImRect ChildHitRect(Min, Max);
						if (ChildHitRect.Contains(Pos))
						{
							return ChildHitWidget;
						}
						else
						{
							return this;
						}
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
			Position = Pos;
			SetLayoutDirty();
		}

		virtual void SetSize(ImVec2 size)override
		{
			Size = size;
			SetLayoutDirty();
		}

		virtual std::vector<PropertyInfo> GetProperties() override
		{
			auto baseProps = ImWidget::GetProperties();

			baseProps.push_back(
				{
					"BackGroundColor",
					PropertyType::Color,
					"Style",
					[this](void* v) { BgColor = *static_cast<ImU32*>(v); },
					[this]() -> void* { return &BgColor; }
				}
			);

			baseProps.push_back(
				{
					"HaveBorder",
					PropertyType::Bool,
					"Style",
					[this](void* v){bHaveBorder= *static_cast<bool*>(v); },
					[this]()->void* {return &bHaveBorder; }
				}
			);

			baseProps.push_back(
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
	};
}