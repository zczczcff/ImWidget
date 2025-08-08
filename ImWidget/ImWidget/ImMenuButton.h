#pragma once
#include "ImButton.h"

namespace ImGuiWidget
{
    class ImMenuButton : public ImButton
    {
    public:
        // 停靠方向枚举
        enum class MenuDockDirection
        {
            Dock_Up,
            Dock_Down,
            Dock_Left,
            Dock_Right
        };

    protected:
        ImWidget* m_MenuWidget = nullptr;         // 菜单部件
        bool m_IsMenuOpen = false;                // 菜单是否打开
        MenuDockDirection m_DockDirection = MenuDockDirection::Dock_Down; // 默认停靠方向
        bool m_JustOpened = false;                // 标记是否刚刚打开菜单
        ImVec2 m_MenuWindowSize = ImVec2(200, 150); // 默认菜单大小

    public:
        ImMenuButton(const std::string& WidgetName)
            : ImButton(WidgetName)
        {
            // 设置按钮点击回调
            SetOnPressed([this]() {
                if (!m_IsMenuOpen) {
                    m_IsMenuOpen = true;
                    m_JustOpened = true; // 标记为刚刚打开
                }
                else {
                    m_IsMenuOpen = false;
                }
                });
        }

        // 设置菜单部件
        void SetMenu(ImWidget* MenuWidget)
        {
            m_MenuWidget = MenuWidget;
            if (m_MenuWidget) {
                m_MenuWindowSize = m_MenuWidget->GetMinSize();
            }
        }

        // 设置停靠方向
        void SetDockDirection(MenuDockDirection direction)
        {
            m_DockDirection = direction;
        }

        // 设置菜单窗口大小
        void SetMenuWindowSize(const ImVec2& size)
        {
            m_MenuWindowSize = size;
        }

        virtual void Render() override
        {
            // 先渲染按钮
            ImButton::Render();

            // 如果菜单需要打开
            if (m_IsMenuOpen)
            {
                // 计算菜单位置
                ImVec2 menuPos = CalculateMenuPosition();
                m_MenuWindowSize = m_MenuWidget->GetMinSize();
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
                    if (m_MenuWidget)
                    {
                        m_MenuWidget->SetPosition(menuPos);
                        m_MenuWidget->SetSize(m_MenuWindowSize);
                        m_MenuWidget->Render();
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
                        m_IsMenuOpen = false;
                    }
                }

                // 重置刚刚打开标记
                m_JustOpened = false;
            }
        }

    private:
        // 计算菜单位置
        ImVec2 CalculateMenuPosition()
        {
            ImVec2 menuPos = Position;

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
    };
}