#pragma once
#include "ImWidget/ImButton.h"
#include "ImWidget/ImVerticalBox.h"

namespace ImWindows
{
	// 停靠方向枚举
	enum class MenuDockDirection
	{
		Dock_Up,
		Dock_Down,
		Dock_Left,
		Dock_Right
	};

	class ImMenuButton :public ImGuiWidget::ImButton
	{
	protected:
		ImGuiWidget::ImVerticalBox* m_Menu;//子菜单
		ImMenuButton* Root;//菜单树之根
		ImMenuButton* Parents;
		MenuDockDirection m_DockDirection = MenuDockDirection::Dock_Down;
		bool m_IsMenuOpen = false;
		bool m_JustOpened = false;
		bool m_HaveChildActived = false;
		void SetParents(ImMenuButton* ParentButton)
		{
			Parents = ParentButton;
			ImMenuButton* temp_p = Parents;
			while (temp_p->Parents)
			{
				temp_p = temp_p->Parents;
			}
			Root = temp_p;
		}
		// 计算菜单位置
		ImVec2 CalculateMenuPosition()
		{
			ImVec2 menuPos = Position;
			ImVec2 m_MenuWindowSize = m_Menu->GetMinSize();
			switch (m_DockDirection)
			{
			case MenuDockDirection::Dock_Up:
				menuPos.y -= m_MenuWindowSize.y;
				break;
			case MenuDockDirection::Dock_Down:
				menuPos.y += Size.y;
				break;
			case MenuDockDirection::Dock_Left:
				menuPos.x -= m_MenuWindowSize.x;
				break;
			case MenuDockDirection::Dock_Right:
				menuPos.x += Size.x;
				break;
			}

			// 简单屏幕边界检查
			ImVec2 screenSize = ImGui::GetIO().DisplaySize;
			if (menuPos.x + m_MenuWindowSize.x > screenSize.x) {
				menuPos.x = screenSize.x - m_MenuWindowSize.x;
			}
			if (menuPos.y + m_MenuWindowSize.y > screenSize.y) {
				menuPos.y = screenSize.y - m_MenuWindowSize.y;
			}
			if (menuPos.x < 0) menuPos.x = 0;
			if (menuPos.y < 0) menuPos.y = 0;

			return menuPos;
		}

	public:
		ImMenuButton(const std::string& WidgetName) 
			:ImGuiWidget::ImButton(WidgetName),
			Root(nullptr),
			Parents(nullptr)
		{
			m_Menu = new ImGuiWidget::ImVerticalBox(WidgetName + "_Menu");
			SetOnPressed([this]() {m_IsMenuOpen = !m_IsMenuOpen; });
		}
		
		void AddMenuOption(ImMenuButton* ChildButtton)
		{
			m_Menu->AddChildToVerticalBox(ChildButtton)->SetIfAutoSize(false);
			ChildButtton->SetParents(this);
		}

		void AddMenuOption(ImWidget* OptionWidget)
		{
			m_Menu->AddChildToVerticalBox(OptionWidget)->SetIfAutoSize(false);
		}

		// 设置停靠方向
		void SetDockDirection(MenuDockDirection direction)
		{
			m_DockDirection = direction;
		}

		virtual void Render() override
		{
			// 先渲染按钮
			ImButton::Render();

			m_HaveChildActived = false;

			// 如果菜单需要打开
			if (m_IsMenuOpen)
			{
				//通知父项子项还在活跃状态
				//if (Parents)
				//{
				//	Parents->m_HaveChildActived = true;
				//}

				// 计算菜单位置
				ImVec2 menuPos = CalculateMenuPosition();
				ImVec2 m_MenuWindowSize = m_Menu->GetMinSize();
				// 设置菜单窗口位置和大小
				ImGui::SetNextWindowPos(menuPos);
				ImGui::SetNextWindowSize(m_MenuWindowSize);
				// 菜单窗口样式
				ImGuiWindowFlags flags =
					ImGuiWindowFlags_NoTitleBar |
					ImGuiWindowFlags_NoResize |
					ImGuiWindowFlags_NoMove |
					ImGuiWindowFlags_NoScrollbar |
					ImGuiWindowFlags_NoSavedSettings |
					ImGuiWindowFlags_NoFocusOnAppearing;

				ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(200, 200, 200, 255)); // 红色背景

				// 开始菜单窗口
				if (ImGui::Begin(("##Menu_" + m_WidgetID).c_str(), nullptr, flags))
				{
					// 渲染菜单内容
					if (m_Menu)
					{
						m_Menu->SetPosition(menuPos);
						m_Menu->SetSize(m_MenuWindowSize);
						m_Menu->Render();
					}
					ImGui::End();
				}

				ImGui::PopStyleColor();

				// 检测外部点击（排除刚刚打开的情况）
				if (!m_JustOpened && ImGui::IsMouseClicked(0))
				{
					ImVec2 mousePos = ImGui::GetMousePos();
					ImRect buttonRect(Position, Position + Size);
					ImRect menuRect(menuPos, menuPos + m_MenuWindowSize);

					// 如果点击在按钮和菜单外部
					if (!buttonRect.Contains(mousePos) && !menuRect.Contains(mousePos))
					{
						if (!m_HaveChildActived)//如果子项都不活跃
						{
							m_IsMenuOpen = false;
						}
						
					}
				}

				// 重置刚刚打开标记
				m_JustOpened = false;
			}

			if (m_IsMenuOpen)
			{
				//通知父项子项还在活跃状态
				if (Parents)
				{
					Parents->m_HaveChildActived = true;
				}
			}
		}
	};
}