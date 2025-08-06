#pragma once
#include "ImWidget/ImHorizontalBox.h"
#include "ImWidget/ImVerticalBox.h"
#include "ImWidget/ImButton.h"
#include "ImWidget/ImTextBlock.h"
#include "ImWidget/ImImage.h" // 添加图像支持
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

    // 选项卡控件（包含图标、标题文本和关闭按钮）
    class ImTabItem : public ImGuiWidget::ImWidget
    {
    private:
        ImGuiWidget::ImImage* m_Image = nullptr;    // 图标
        ImGuiWidget::ImTextBlock* m_TextBlock;       // 标题
        ImGuiWidget::ImButton* m_CloseButton;        // 关闭按钮
        std::function<void()> m_OnClose;
        bool m_IsVerticalLayout;

        // 可见性控制
        bool m_ShowImage = true;
        bool m_ShowText = true;
        bool m_ShowCloseButton = true;
        float m_IconSize = 16.0f; // 图标大小

    public:
        ImTabItem(const std::string& title, bool isVerticalLayout, ImTextureID iconID = 0)
            : ImWidget("TabItem_" + title),
            m_IsVerticalLayout(isVerticalLayout)
        {
            // 创建图标
            if (iconID != 0) {
                m_Image = new ImGuiWidget::ImImage("Image_" + title);
                m_Image->SetTextureID(iconID);
            }

            // 创建标题文本
            m_TextBlock = new ImGuiWidget::ImTextBlock("TextBlock_" + title);
            m_TextBlock->SetText(title);
            m_TextBlock->SetTextColor(IM_COL32(255, 255, 255, 255));
            m_TextBlock->SetHorizontalAlignment(ImGuiWidget::ImTextBlock::TextAlignment_Horizontal::Left);

            // 创建关闭按钮
            m_CloseButton = new ImGuiWidget::ImButton("CloseButton_" + title);
            SetupCloseButtonStyle();
        }

        virtual ~ImTabItem()
        {
            delete m_Image;
            delete m_TextBlock;
            delete m_CloseButton;
        }

        // 设置关闭按钮样式
        void SetupCloseButtonStyle()
        {
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

        // 设置关闭回调
        void SetOnClose(std::function<void()> callback) { m_OnClose = callback; }

        // 设置图标大小
        void SetIconSize(float size) { m_IconSize = size; }

        // 设置选项卡内容可见性
        void SetShowImage(bool show) { m_ShowImage = show; }
        void SetShowText(bool show) { m_ShowText = show; }
        void SetShowCloseButton(bool show) { m_ShowCloseButton = show; }

        // 设置图标纹理
        void SetIconTexture(ImTextureID textureID)
        {
            if (!m_Image) {
                m_Image = new ImGuiWidget::ImImage("Image_" + m_WidgetID);
            }
            m_Image->SetTextureID(textureID);
        }

        virtual ImVec2 GetMinSize() override
        {
            ImVec2 size(0, 0);
            float spacing = 4.0f; // 元素间距

            // 计算图标尺寸
            ImVec2 imageSize(0, 0);
            if (m_ShowImage && m_Image) {
                imageSize = ImVec2(m_IconSize, m_IconSize);
            }

            // 计算文本尺寸
            ImVec2 textSize(0, 0);
            if (m_ShowText && m_TextBlock) {
                textSize = m_TextBlock->GetMinSize();
            }

            // 计算关闭按钮尺寸
            ImVec2 buttonSize(0, 0);
            if (m_ShowCloseButton && m_CloseButton) {
                buttonSize = ImVec2(16.0f, 16.0f);
            }

            if (m_IsVerticalLayout)
            {
                // 垂直布局：图标、文本、关闭按钮垂直排列
                size.x = ImMax(ImMax(imageSize.x, textSize.x), buttonSize.x);
                size.y = (m_ShowImage ? imageSize.y + spacing : 0) +
                    (m_ShowText ? textSize.y + spacing : 0) +
                    (m_ShowCloseButton ? buttonSize.y : 0);

                // 移除多余的间距
                if (size.y > 0) size.y -= spacing;
            }
            else
            {
                // 水平布局：图标、文本、关闭按钮水平排列
                size.x = (m_ShowImage ? imageSize.x + spacing : 0) +
                    (m_ShowText ? textSize.x + spacing : 0) +
                    (m_ShowCloseButton ? buttonSize.x : 0);
                size.y = ImMax(ImMax(imageSize.y, textSize.y), buttonSize.y);

                // 移除多余的间距
                if (size.x > 0) size.x -= spacing;
            }

            // 添加内边距
            size.x += 6.0f; // 左右内边距
            size.y += 4.0f;  // 上下内边距

            return size;
        }

        virtual void Render() override
        {
            ImVec2 currentPos = Position + ImVec2(3.0f, 2.0f); // 起始位置带内边距

            float buttonSize = min(Size.x, Size.y) * 0.6f;
            //ImVec2 buttonSize = { buttonwidth, buttonwidth };

            float spacing = 4.0f; // 元素间距

            if (m_IsVerticalLayout)
            {
                // 垂直布局：图标 -> 文本 -> 关闭按钮
                if (m_ShowImage && m_Image) {
                    m_Image->SetPosition(currentPos + ImVec2((Size.x - 6.0f - m_IconSize) * 0.5f, 0));
                    m_Image->SetSize(ImVec2(m_IconSize, m_IconSize));
                    m_Image->Render();
                    currentPos.y += m_IconSize + spacing;
                }

                if (m_ShowText && m_TextBlock) {
                    m_TextBlock->SetPosition(currentPos);
                    m_TextBlock->SetSize(ImVec2(Size.x - 6.0f, Size.y - currentPos.y));
                    m_TextBlock->Render();
                    currentPos.y += m_TextBlock->GetMinSize().y + spacing;
                }

                if (m_ShowCloseButton && m_CloseButton) {
                    //float buttonSize = 16.0f;
                    m_CloseButton->SetPosition(ImVec2(
                        Position.x + (Size.x - buttonSize) * 0.5f,
                        currentPos.y
                    ));
                    m_CloseButton->SetSize(ImVec2(buttonSize, buttonSize));
                    m_CloseButton->Render();
                }
            }
            else
            {
                // 水平布局：图标 -> 文本 -> 关闭按钮
                if (m_ShowImage && m_Image) {
                    m_Image->SetPosition(currentPos);
                    m_Image->SetSize(ImVec2(m_IconSize, m_IconSize));
                    m_Image->Render();
                    currentPos.x += m_IconSize + spacing;
                }

                if (m_ShowText && m_TextBlock) {
                    float textWidth = Size.x - (currentPos.x-Position.x) - (m_ShowCloseButton ? 16.0f + spacing : 0) - 3.0f;
                    if (textWidth > 0) {
                        m_TextBlock->SetPosition(currentPos);
                        m_TextBlock->SetSize(ImVec2(textWidth, Size.y - 4.0f));
                        m_TextBlock->Render();
                    }
                    currentPos.x += textWidth + spacing;
                }

                if (m_ShowCloseButton && m_CloseButton) {
                    //float buttonSize = 16.0f;
                    m_CloseButton->SetPosition(ImVec2(
                        Position.x + Size.x - buttonSize - 3.0f,
                        Position.y + (Size.y - buttonSize) * 0.5f
                    ));
                    m_CloseButton->SetSize(ImVec2(buttonSize, buttonSize));
                    m_CloseButton->Render();
                }
            }
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

            // 添加为子控件
            AddChildInternal<ImGuiWidget::ImSlot>(m_TabBar);
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

        // 添加新页面（带图标）
        void AddPage(const std::string& title, ImWidget* content, ImTextureID iconID = 0)
        {
            Page newPage;
            newPage.title = title;
            newPage.content = content;

            // 创建选项卡项（传递图标）
            bool isVertical = (m_DockPosition == TabDockPosition::Left || m_DockPosition == TabDockPosition::Right);
            newPage.tabItem = new ImTabItem(title, isVertical, iconID);

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
                auto vslot = vbox->AddChildToVerticalBox(newPage.tabButton);
                vslot->SetIfAutoSize(false);
            }

            // 添加到页面列表
            m_Pages.push_back(newPage);

            // 如果是第一个页面，设为激活状态
            if (m_ActivePageIndex == -1)
            {
                SetActivePage(0);
            }
        }

        // 设置选项卡内容可见性
        void SetTabItemVisibility(int index, bool showImage, bool showText, bool showCloseButton)
        {
            if (index >= 0 && index < static_cast<int>(m_Pages.size()))
            {
                m_Pages[index].tabItem->SetShowImage(showImage);
                m_Pages[index].tabItem->SetShowText(showText);
                m_Pages[index].tabItem->SetShowCloseButton(showCloseButton);
            }
        }

        // 设置所有选项卡内容可见性
        void SetAllTabItemsVisibility(bool showImage, bool showText, bool showCloseButton)
        {
            for (auto& page : m_Pages)
            {
                page.tabItem->SetShowImage(showImage);
                page.tabItem->SetShowText(showText);
                page.tabItem->SetShowCloseButton(showCloseButton);
            }
        }

        // 设置选项卡图标
        void SetTabItemIcon(int index, ImTextureID iconID)
        {
            if (index >= 0 && index < static_cast<int>(m_Pages.size()))
            {
                m_Pages[index].tabItem->SetIconTexture(iconID);
            }
        }

        // 设置选项卡图标大小
        void SetTabItemIconSize(int index, float size)
        {
            if (index >= 0 && index < static_cast<int>(m_Pages.size()))
            {
                m_Pages[index].tabItem->SetIconSize(size);
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
                    }
                    else if (auto vbox = dynamic_cast<ImGuiWidget::ImVerticalBox*>(m_TabBar))
                    {
                        vbox->RemoveChildAt(index);
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
        }

        void SetActivePageByContent(ImWidget* content)
        {
            int i = 0;
            for (auto& page : m_Pages)
            {
                if (page.content == content)
                {
                    SetActivePage(i);
                    return;
                }
                i++;
            }
        }

        ImWidget* GetActivePageContent()
        {
            if (m_ActivePageIndex >= 0)
                return m_Pages[m_ActivePageIndex].content;
            else
                return nullptr;
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
            ImVec2 tabPos, tabBarSize, contentPos, contentSize;

            switch (m_DockPosition)
            {
            case TabDockPosition::Top:
                tabPos = Position;
                tabBarSize = ImVec2(Size.x, m_TabBarThickness);
                contentPos = ImVec2(Position.x, Position.y + m_TabBarThickness);
                contentSize = ImVec2(Size.x, Size.y - m_TabBarThickness);
                break;

            case TabDockPosition::Bottom:
                tabPos = ImVec2(Position.x, Position.y + Size.y - m_TabBarThickness);
                tabBarSize = ImVec2(Size.x, m_TabBarThickness);
                contentPos = Position;
                contentSize = ImVec2(Size.x, Size.y - m_TabBarThickness);
                break;

            case TabDockPosition::Left:
                tabPos = Position;
                tabBarSize = ImVec2(m_TabBarThickness, Size.y);
                contentPos = ImVec2(Position.x + m_TabBarThickness, Position.y);
                contentSize = ImVec2(Size.x - m_TabBarThickness, Size.y);
                break;

            case TabDockPosition::Right:
                tabPos = ImVec2(Position.x + Size.x - m_TabBarThickness, Position.y);
                tabBarSize = ImVec2(m_TabBarThickness, Size.y);
                contentPos = Position;
                contentSize = ImVec2(Size.x - m_TabBarThickness, Size.y);
                break;
            }

            // 设置选项卡栏位置和大小
            m_TabBar->SetPosition(tabPos);
            m_TabBar->SetSize(tabBarSize);
            m_TabBar->Render();

            // 设置内容区域位置和大小
            if (m_ActivePageIndex >= 0)
            {
                m_Pages[m_ActivePageIndex].content->SetPosition(contentPos);
                m_Pages[m_ActivePageIndex].content->SetSize(contentSize);
                m_Pages[m_ActivePageIndex].content->Render();
            }
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