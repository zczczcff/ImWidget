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
	protected:
		std::vector<ImSlot*> m_Slots;
		ImU32 BgColor;
		ImU32 BorderColor;
		bool bHaveBorder;

	public:
		ImPanelWidget(const std::string& WidgetName)
			:ImWidget(WidgetName),
			BgColor(IM_COL32(255, 255, 255, 255)),
			BorderColor(IM_COL32(0, 0, 0, 255)),
			bHaveBorder(true)
		{}

		virtual ~ImPanelWidget() // 添加析构函数管理内存
		{
			for (auto slot : m_Slots)
				delete slot;
			m_Slots.clear();
		}

		template<typename SlotType>
		SlotType* AddChild(ImWidget* child)
		{
			SlotType* newslot = new SlotType(child);
			m_Slots.push_back(newslot);
			return newslot;
		}

		// 按索引移除子控件
		void RemoveChildAt(int index)
		{
			if (index >= 0 && index < static_cast<int>(m_Slots.size()))
			{
				delete m_Slots[index]; // 删除slot对象
				m_Slots.erase(m_Slots.begin() + index);
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
				return true;
			}
			return false;
		}

		// 新增：取出子控件（移除slot但保留子控件指针）
		ImWidget* ExtractChildAt(int index)
		{
			if (index >= 0 && index < static_cast<int>(m_Slots.size()))
			{
				ImWidget* child = m_Slots[index]->GetContent();
				delete m_Slots[index]; // 删除slot对象
				m_Slots.erase(m_Slots.begin() + index);
				return child;
			}
			return nullptr;
		}

		// 新增：取出子控件（通过指针查找）
		ImWidget* ExtractChild(ImWidget* child)
		{
			auto it = std::find_if(m_Slots.begin(), m_Slots.end(),
				[child](ImSlot* slot) {
					return slot->GetContent() == child;
				});

			if (it != m_Slots.end())
			{
				ImWidget* result = (*it)->GetContent();
				delete* it; // 删除slot对象
				m_Slots.erase(it);
				return result;
			}
			return nullptr;
		}

		// 获取子控件指针
		ImWidget* GetChildAt(int index)
		{
			if (index >= 0 && index < static_cast<int>(m_Slots.size()))
				return m_Slots[index]->GetContent();
			return nullptr;
		}

		virtual void Render() override
		{

		}

		void RenderBackGround()
		{
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			window->DrawList->AddRectFilled(Position, Position + Size, BgColor);
			if (bHaveBorder)
			{
				window->DrawList->AddRect(Position, Position + Size, BorderColor);
			}
		}

		void SetBackGroundColor(ImU32 color) { BgColor = color; }

		void SetBorderColor(ImU32 color) { BorderColor = color; }

		int GetChildNum() { return static_cast<int>(m_Slots.size()); }

		ImSlot* GetSlot(int Index) { return m_Slots[Index]; }

		virtual ImWidget* ChildHitTest(ImVec2 Pos) override
		{
			if (ImWidget::ChildHitTest(Pos))
			{
				for (int i = m_Slots.size(); i >= 0; i--)
				{
					ImWidget* ChildHitWidget = m_Slots[i]->GetContent()->ChildHitTest(Pos);
					if (ChildHitWidget)
					{
						return ChildHitWidget;
					}
				}
				return this;
			}
			else
			{
				return nullptr;
			}
		}
	};
}