#pragma once
#include "ImWidget/ImHorizontalBox.h"
#include "ImWidget/ImVerticalBox.h"
#include "ImWidget/ImButton.h"
#include "ImWidget/ImTextBlock.h"
#include <vector>
#include <functional>

namespace ImWindows
{
    // 选项卡停靠位置
    enum class TabDockPosition
    {
        Top,
        Bottom,
        Left,
        Right
    };

    // 选项卡控件（包含标题文本和关闭按钮）
    class ImTabItem : public ImGuiWidget::ImWidget
    {
    private:
        ImGuiWidget::ImTextBlock* m_TextBlock;
        ImGuiWidget::ImButton* m_CloseButton;
        std::function<void()> m_OnClose;
        bool m_IsVerticalLayout;

    public:
        ImTabItem(const std::string& title, bool isVerticalLayout)
            : ImWidget("TabItem_" + title),
            m_IsVerticalLayout(isVerticalLayout)
        {
            // 创建标题文本
            m_TextBlock = new ImGuiWidget::ImTextBlock("TextBlock_" + title);
            m_TextBlock->SetText(title);
            m_TextBlock->SetTextColor(IM_COL32(255, 255, 255, 255));
            m_TextBlock->SetHorizontalAlignment(ImGuiWidget::ImTextBlock::TextAlignment_Horizontal::Left);
            // 创建关闭按钮
            m_CloseButton = new ImGuiWidget::ImButton("CloseButton_" + title);

            // 设置关闭按钮样式
            ImGuiWidget::ButtonStateStyle closeNormalStyle;
            closeNormalStyle.BackgroundColor = IM_COL32(200, 50, 50, 150);
            closeNormalStyle.Rounding = 2.0f;
            closeNormalStyle.HasBorder = true;
            closeNormalStyle.BorderThickness = 1.0f;
            closeNormalStyle.BorderColor = IM_COL32(255, 255, 255, 200);

            ImGuiWidget::ButtonStateStyle closeHoveredStyle = closeNormalStyle;
            closeHoveredStyle.BackgroundColor = IM_COL32(220, 70, 70, 200);

            ImGuiWidget::ButtonStateStyle closePressedStyle = closeNormalStyle;
            closePressedStyle.BackgroundColor = IM_COL32(180, 30, 30, 200);

            m_CloseButton->SetNormalStyle(closeNormalStyle);
            m_CloseButton->SetHoveredStyle(closeHoveredStyle);
            m_CloseButton->SetPressedStyle(closePressedStyle);

            // 设置关闭按钮回调
            m_CloseButton->SetOnPressed([this]() {
                if (m_OnClose) m_OnClose();
                });
        }

        virtual ~ImTabItem()
        {
            delete m_TextBlock;
            delete m_CloseButton;
        }

        void SetOnClose(std::function<void()> callback) { m_OnClose = callback; }

        virtual ImVec2 GetMinSize() override
        {
            ImVec2 textSize = m_TextBlock->GetMinSize();
            ImVec2 buttonSize = { 16.0f, 16.0f }; // 关闭按钮固定大小

            if (m_IsVerticalLayout)
            {
                // 垂直布局：文本在上，关闭按钮在下
                return ImVec2(
                    ImMax(textSize.x, buttonSize.x),
                    textSize.y + buttonSize.y + 4.0f // 4.0f为间距
                );
            }
            else
            {
                // 水平布局：文本在左，关闭按钮在右
                return ImVec2(
                    textSize.x + buttonSize.x + 4.0f+6.f, // 4.0f为间距,6.f为左右padding
                    ImMax(textSize.y, buttonSize.y)
                );
            }
        }

        virtual void Render() override
        {
            ImVec2 textSize = m_TextBlock->GetMinSize();
            float buttonwidth = min(Size.x, Size.y) * 0.6f;
            ImVec2 buttonSize = { buttonwidth, buttonwidth };

            if (m_IsVerticalLayout)
            {
                // 垂直布局：文本在上，关闭按钮在下
                m_TextBlock->SetPosition(Position);
                m_TextBlock->SetSize(ImVec2(Size.x, textSize.y));

                m_CloseButton->SetPosition(ImVec2(
                    Position.x + (Size.x - buttonSize.x) / 2.0f,
                    Position.y + textSize.y + 4.0f
                ));
                m_CloseButton->SetSize(buttonSize);
            }
            else
            {
                // 水平布局：文本在左，关闭按钮在右
                m_TextBlock->SetPosition(Position + ImVec2(3.f, 0.f));
                m_TextBlock->SetSize(ImVec2(Size.x - buttonSize.x - 4.0f, Size.y));

                m_CloseButton->SetPosition(ImVec2(
                    Position.x + Size.x - buttonSize.x-3.f,
                    Position.y + (Size.y - buttonSize.y) / 2.0f
                ));
                m_CloseButton->SetSize(buttonSize);
            }

            m_TextBlock->Render();
            m_CloseButton->Render();
        }
    };

    // 页面管理器
    class ImPageManager : public ImGuiWidget::ImPanelWidget
    {
    private:
        struct Page
        {
            std::string title;
            ImGuiWidget::ImWidget* content;
            ImGuiWidget::ImButton* tabButton;
            ImTabItem* tabItem;
        };

        TabDockPosition m_DockPosition;
        ImPanelWidget* m_TabBar;
        //ImPanelWidget* m_ContentArea;
        std::vector<Page> m_Pages;
        int m_ActivePageIndex = -1;
        float m_TabBarThickness = 30.0f; // 选项卡栏厚度

        // 页面关闭回调函数
        std::function<void(const std::string&)> m_OnPageClosed;

    public:
        ImPageManager(const std::string& name, TabDockPosition dockPosition = TabDockPosition::Top)
            : ImPanelWidget(name),
            m_DockPosition(dockPosition)
        {
            // 根据停靠位置创建选项卡栏
            if (m_DockPosition == TabDockPosition::Top || m_DockPosition == TabDockPosition::Bottom)
            {
                m_TabBar = new ImGuiWidget::ImHorizontalBox("TabBar");
            }
            else // Left or Right
            {
                m_TabBar = new ImGuiWidget::ImVerticalBox("TabBar");
            }

            // 创建内容区域
            //m_ContentArea = new ImPanelWidget("ContentArea");

            // 添加为子控件
            AddChild<ImGuiWidget::ImSlot>(m_TabBar);
            //AddChild<ImGuiWidget::ImSlot>(m_ContentArea);
        }

        virtual ~ImPageManager()
        {
            for (auto& page : m_Pages)
            {
                delete page.tabButton;
                delete page.tabItem;
                // 注意：page.content由外部管理，不在这里删除
            }
        }

        // 添加新页面
        void AddPage(const std::string& title, ImWidget* content)
        {
            Page newPage;
            newPage.title = title;
            newPage.content = content;

            // 创建选项卡项
            bool isVertical = (m_DockPosition == TabDockPosition::Left || m_DockPosition == TabDockPosition::Right);
            newPage.tabItem = new ImTabItem(title, isVertical);

            // 创建选项卡按钮
            newPage.tabButton = new ImGuiWidget::ImButton("TabButton_" + title);
            newPage.tabButton->SetContent(newPage.tabItem);

            // 设置按钮样式
            SetupButtonStyle(newPage.tabButton);

            // 设置按钮回调
            newPage.tabButton->SetOnPressed([this, index = m_Pages.size()]() {
                SetActivePage(index);
            });

            // 设置关闭回调
            newPage.tabItem->SetOnClose([this, title]() {
                ClosePage(title);
                });

            // 添加到选项卡栏
            if (auto hbox = dynamic_cast<ImGuiWidget::ImHorizontalBox*>(m_TabBar))
            {
                auto hslot = hbox->AddChildToHorizontalBox(newPage.tabButton);
                hslot->SetIfAutoSize(false);
            }
            else if (auto vbox = dynamic_cast<ImGuiWidget::ImVerticalBox*>(m_TabBar))
            {
                vbox->AddChildToVerticalBox(newPage.tabButton);
            }

            // 添加到页面列表
            m_Pages.push_back(newPage);

            // 如果是第一个页面，设为激活状态
            if (m_ActivePageIndex == -1)
            {
                SetActivePage(0);
            }
        }

        // 关闭页面
        void ClosePage(const std::string& title)
        {
            for (auto it = m_Pages.begin(); it != m_Pages.end(); ++it)
            {
                if (it->title == title)
                {
                    int index = std::distance(m_Pages.begin(), it);

                    // 触发回调
                    if (m_OnPageClosed)
                    {
                        m_OnPageClosed(title);
                    }

                    // 从选项卡栏移除
                    if (auto hbox = dynamic_cast<ImGuiWidget::ImHorizontalBox*>(m_TabBar))
                    {
                        hbox->RemoveChildAt(index);
                        //hbox->GetSlot(index)->GetContent()->SetVisibility(false);
                    }
                    else if (auto vbox = dynamic_cast<ImGuiWidget::ImVerticalBox*>(m_TabBar))
                    {
                        vbox->RemoveChildAt(index);
                        //vbox->GetSlot(index)->GetContent()->SetVisibility(false);
                    }

                    // 删除资源
                    delete it->tabButton;
                    delete it->tabItem;

                    // 从列表中移除
                    m_Pages.erase(it);

                    // 调整激活页面
                    if (index == m_ActivePageIndex)
                    {
                        m_ActivePageIndex = -1;
                        if (!m_Pages.empty())
                        {
                            SetActivePage(index >= (int)m_Pages.size() ? m_Pages.size() - 1 : index);
                        }
                        else
                        {
                            // 没有页面时清空内容区域
                            //m_ContentArea->SetSize(ImVec2(0, 0));
                        }
                    }
                    else if (index < m_ActivePageIndex)
                    {
                        m_ActivePageIndex--;
                    }

                    break;
                }
            }
        }

        // 设置活动页面
        void SetActivePage(int index)
        {
            if (index < 0 || index >= (int)m_Pages.size()) return;

            m_ActivePageIndex = index;

            // 更新所有按钮样式
            for (int i = 0; i < (int)m_Pages.size(); ++i)
            {
                UpdateButtonStyle(m_Pages[i].tabButton, i == index);
            }

            // 更新内容区域
            //m_ContentArea->SetContent(m_Pages[index].content);
        }

        // 设置页面关闭回调
        void SetOnPageClosed(std::function<void(const std::string&)> callback)
        {
            m_OnPageClosed = callback;
        }

        // 设置选项卡栏厚度
        void SetTabBarThickness(float thickness) { m_TabBarThickness = thickness; }

        virtual void Render() override
        {
            // 计算布局
            ImVec2 tabBarSize, contentPos, contentSize;

            switch (m_DockPosition)
            {
            case TabDockPosition::Top:
                tabBarSize = ImVec2(Size.x, m_TabBarThickness);
                contentPos = ImVec2(Position.x, Position.y + m_TabBarThickness);
                contentSize = ImVec2(Size.x, Size.y - m_TabBarThickness);
                break;

            case TabDockPosition::Bottom:
                tabBarSize = ImVec2(Size.x, m_TabBarThickness);
                contentPos = Position;
                contentSize = ImVec2(Size.x, Size.y - m_TabBarThickness);
                break;

            case TabDockPosition::Left:
                tabBarSize = ImVec2(m_TabBarThickness, Size.y);
                contentPos = ImVec2(Position.x + m_TabBarThickness, Position.y);
                contentSize = ImVec2(Size.x - m_TabBarThickness, Size.y);
                break;

            case TabDockPosition::Right:
                tabBarSize = ImVec2(m_TabBarThickness, Size.y);
                contentPos = Position;
                contentSize = ImVec2(Size.x - m_TabBarThickness, Size.y);
                break;
            }

            // 设置选项卡栏位置和大小
            m_TabBar->SetPosition(Position);
            m_TabBar->SetSize(tabBarSize);
            m_TabBar->Render();
            // 设置内容区域位置和大小
            if (m_ActivePageIndex >= 0)
            {
                m_Pages[m_ActivePageIndex].content->SetPosition(contentPos);
                m_Pages[m_ActivePageIndex].content->SetSize(contentSize);
                m_Pages[m_ActivePageIndex].content->Render();
            }

            // 渲染子控件
            //ImPanelWidget::Render();
        }

    private:
        // 初始化按钮样式
        void SetupButtonStyle(ImGuiWidget::ImButton* button)
        {
            ImGuiWidget::ButtonStateStyle normalStyle;
            normalStyle.BackgroundColor = IM_COL32(70, 70, 70, 220);
            normalStyle.Rounding = 4.0f;
            normalStyle.HasBorder = true;
            normalStyle.BorderThickness = 1.0f;
            normalStyle.BorderColor = IM_COL32(100, 100, 100, 200);

            ImGuiWidget::ButtonStateStyle hoveredStyle = normalStyle;
            hoveredStyle.BackgroundColor = IM_COL32(90, 90, 90, 220);

            ImGuiWidget::ButtonStateStyle pressedStyle = normalStyle;
            pressedStyle.BackgroundColor = IM_COL32(50, 50, 50, 220);

            button->SetNormalStyle(normalStyle);
            button->SetHoveredStyle(hoveredStyle);
            button->SetPressedStyle(pressedStyle);
        }


        void SetButtonActiveStyle(ImGuiWidget::ImButton* button)
        {
            ImGuiWidget::ButtonStateStyle normalStyle;
            normalStyle.BackgroundColor = IM_COL32(70, 70, 100, 220);
            normalStyle.Rounding = 4.0f;
            normalStyle.HasBorder = true;
            normalStyle.BorderThickness = 1.0f;
            normalStyle.BorderColor = IM_COL32(100, 100, 100, 200);

            ImGuiWidget::ButtonStateStyle hoveredStyle = normalStyle;
            hoveredStyle.BackgroundColor = IM_COL32(90, 90, 120, 220);

            ImGuiWidget::ButtonStateStyle pressedStyle = normalStyle;
            pressedStyle.BackgroundColor = IM_COL32(50, 50, 80, 220);

            button->SetNormalStyle(normalStyle);
            button->SetHoveredStyle(hoveredStyle);
            button->SetPressedStyle(pressedStyle);
        }
        // 更新按钮样式（激活/非激活状态）
        void UpdateButtonStyle(ImGuiWidget::ImButton* button, bool isActive)
        {
            if (isActive)
            {
                SetButtonActiveStyle(button);
            }
            else
            {
                SetupButtonStyle(button);
            }
        }
    };
}