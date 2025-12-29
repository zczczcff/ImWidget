#pragma once
#include "ImWidget/ImUserWidget.h"
#include "ImWidget/ImBorder.h"
#include "ImWidget/ImHorizontalBox.h"
#include "ImWidget/ImButton.h"
#include "ImWidget/ImTextBlock.h"
#include "ImWidget/ImVerticalBox.h"
#include <unordered_map>

namespace ImGuiWidget
{
    class ImPageManager : public ImUserWidget
    {
    public:
        // Tab位置枚举
        enum class TabPosition
        {
            Top,    // Tab在上方
            Bottom  // Tab在下方
        };

    private:
        // 页面数据结构
        struct PageData
        {
            std::string name;
            ImWidget* content;
            ImButton* tabButton;
            ImTextBlock* tabText;
            ImButton* closeButton;

            PageData(const std::string& pageName, ImWidget* pageContent)
                : name(pageName), content(pageContent),
                tabButton(nullptr), tabText(nullptr), closeButton(nullptr)
            {
            }
        };

        // 成员变量
        TabPosition m_tabPosition;
        std::vector<PageData> m_pages;
        std::string m_currentPage;

        // 新增：关闭按钮相关设置
        bool m_showCloseButton = true;           // 是否显示关闭按钮
        float m_closeButtonSize = 6.0f;        // 关闭按钮边长
        float m_closeButtonPadding = 3.0f;      // 关闭按钮padding

        // 控件指针
        ImVerticalBox* m_rootVerticalBox;
        ImHorizontalBox* m_tabBar;
        ImBorder* m_contentBorder;

        // 多播委托
        ImMultiDelegate<const std::string&> m_onPageClosed;     // 页面关闭回调
        ImMultiDelegate<const std::string&> m_onPageSelected;   // 页面选中回调

        // 创建Tab按钮
        ImWidget* CreateTabWidget(const std::string& pageName)
        {
            // 创建水平框容纳Tab内容
            ImHorizontalBox* tabHBox = new ImHorizontalBox("Tab_" + pageName);

            // 创建文本块显示页面名称
            ImTextBlock* textBlock = new ImTextBlock("TabText_" + pageName);
            textBlock->SetText(pageName);
            textBlock->SetHorizontalAlignment(ImTextBlock::TextAlignment_Horizontal::Center);
            textBlock->SetVerticalAlignment(ImTextBlock::TextAlignment_Vertical::Center);

            // 创建关闭按钮（根据设置决定是否创建）
            ImButton* closeButton = nullptr;
            if (m_showCloseButton)
            {
                closeButton = new ImButton("CloseButton_" + pageName);
                closeButton->SetOriginalMinSize(ImVec2(m_closeButtonSize, m_closeButtonSize));

                ImTextBlock* closeText = new ImTextBlock("CloseText_" + pageName);
                closeText->SetText(u8"×");
                closeText->SetTextColor(IM_COL32(100, 100, 100, 255)); // 浅灰色文字
                closeButton->SetContent(closeText);

                // 设置关闭按钮样式 - 浅色灰色系
                ButtonStateStyle closeStyle;
                closeStyle.BackgroundColor = IM_COL32(220, 220, 220, 255); // 浅灰色背景
                closeStyle.Rounding = 0.0f;
                closeStyle.HasBorder = false;
                closeStyle.BorderThickness = 0.f;
                closeButton->SetNormalStyle(closeStyle);

                ButtonStateStyle closeHoverStyle;
                closeHoverStyle.BackgroundColor = IM_COL32(255, 100, 100, 255); // 悬停时红色
                closeHoverStyle.Rounding = 0.0f;
                closeHoverStyle.HasBorder = false;
                closeHoverStyle.BorderThickness = 0.f;
                closeButton->SetHoveredStyle(closeHoverStyle);

                ButtonStateStyle closePressedStyle;
                closePressedStyle.BackgroundColor = IM_COL32(200, 80, 80, 255); // 按下时深红色
                closePressedStyle.Rounding = 0.0f;
                closeHoverStyle.HasBorder = false;
                closeHoverStyle.BorderThickness = 0.f;
                closeButton->SetPressedStyle(closePressedStyle);
            }

            // 添加到水平框
            auto textslot = tabHBox->AddChildToHorizontalBox(textBlock);
            textslot->PaddingBottom = 2.f;
            textslot->PaddingLeft = 2.f;
            textslot->PaddingRight = 2.f;
            textslot->PaddingTop = 2.f;

            if (closeButton)
            {
                auto buttonslot = tabHBox->AddChildToHorizontalBox(closeButton);
                buttonslot->SetIfAutoSize(false);
                buttonslot->PaddingBottom = m_closeButtonPadding;
                buttonslot->PaddingLeft = m_closeButtonPadding;
                buttonslot->PaddingRight = m_closeButtonPadding;
                buttonslot->PaddingTop = m_closeButtonPadding;
            }

            tabHBox->SetBackGroundColor(IM_COL32(0, 0, 0, 0));
            tabHBox->bHaveBorder = false;
            // 创建Tab按钮
            ImButton* tabButton = new ImButton("TabButton_" + pageName);
            tabButton->SetContent(tabHBox);

            // 设置Tab按钮样式
            SetupTabButtonStyle(tabButton);

            return tabButton;
        }

        // 设置Tab按钮样式 - 浅色灰色系
        void SetupTabButtonStyle(ImButton* button)
        {
            // 正常状态样式 - 浅灰色
            ButtonStateStyle normalStyle;
            normalStyle.BackgroundColor = IM_COL32(240, 245, 255, 255);
            normalStyle.Rounding = 4.0f;
            normalStyle.HasBorder = false;
            normalStyle.BorderThickness = 1.0f;
            normalStyle.BorderColor = IM_COL32(200, 200, 200, 255);
            button->SetNormalStyle(normalStyle);

            // 悬停状态样式 - 稍深的灰色
            ButtonStateStyle hoverStyle;
            hoverStyle.BackgroundColor = IM_COL32(225, 235, 255, 255);
            hoverStyle.Rounding = 4.0f;
            hoverStyle.HasBorder = false;
            hoverStyle.BorderThickness = 1.0f;
            hoverStyle.BorderColor = IM_COL32(180, 180, 180, 255);
            button->SetHoveredStyle(hoverStyle);

            // 按下状态样式 - 更深的灰色
            ButtonStateStyle pressedStyle;
            pressedStyle.BackgroundColor = IM_COL32(200, 220, 250, 255);
            pressedStyle.Rounding = 4.0f;
            pressedStyle.HasBorder = false;
            pressedStyle.BorderThickness = 1.0f;
            pressedStyle.BorderColor = IM_COL32(160, 160, 160, 255);
            button->SetPressedStyle(pressedStyle);

            // 选中状态样式（焦点状态）- 蓝色高亮
            ButtonStateStyle selectedStyle;
            selectedStyle.BackgroundColor = IM_COL32(100, 149, 237, 255); // 矢车菊蓝
            selectedStyle.Rounding = 4.0f;
            selectedStyle.HasBorder = false;
            selectedStyle.BorderThickness = 1.0f;
            selectedStyle.BorderColor = IM_COL32(70, 130, 180, 255); // 钢蓝色
            button->SetFocusedStyle(selectedStyle);
        }

        // 切换到指定页面
        void SwitchToPage(const std::string& pageName)
        {
            if (m_currentPage == pageName) return;

            // 更新当前页面
            m_currentPage = pageName;

            // 更新Tab按钮状态
            UpdateTabButtons();

            // 设置内容
            ImWidget* content = FindPageContent(pageName);
            if (content)
            {
                m_contentBorder->SetContent(content, false);
            }
            else
            {
                m_contentBorder->SetContent(nullptr, false);
            }

            // 触发页面选中回调
            m_onPageSelected.Broadcast(pageName);
        }

        // 更新Tab按钮的选中状态
        void UpdateTabButtons()
        {
            for (auto& page : m_pages)
            {
                if (page.tabButton)
                {
                    if (page.name == m_currentPage)
                    {
                        page.tabButton->RequestFocus(); // 设置为选中状态
                    }
                    else
                    {
                        page.tabButton->LoseFocus(); // 取消选中状态
                    }
                }
            }
        }

        // 查找页面内容
        ImWidget* FindPageContent(const std::string& pageName)
        {
            for (auto& page : m_pages)
            {
                if (page.name == pageName)
                {
                    return page.content;
                }
            }
            return nullptr;
        }

        // 查找页面数据
        PageData* FindPageData(const std::string& pageName)
        {
            for (auto& page : m_pages)
            {
                if (page.name == pageName)
                {
                    return &page;
                }
            }
            return nullptr;
        }

        // 移除页面内部实现
        void RemovePageInternal(const std::string& pageName)
        {
            auto it = std::remove_if(m_pages.begin(), m_pages.end(),
                [&](const PageData& page) { return page.name == pageName; });

            if (it != m_pages.end())
            {
                // 从Tab栏移除按钮
                if (it->tabButton)
                {
                    m_tabBar->RemoveChild(it->tabButton);
                }

                m_pages.erase(it, m_pages.end());
            }
        }

    public:
        ImPageManager(const std::string& widgetName)
            : ImUserWidget(widgetName),
            m_tabPosition(TabPosition::Top),
            m_rootVerticalBox(nullptr),
            m_tabBar(nullptr),
            m_contentBorder(nullptr)
        {
            // 创建根垂直框
            m_rootVerticalBox = new ImVerticalBox("RootVerticalBox");
            SetRootWidget(m_rootVerticalBox);

            // 创建Tab栏水平框
            m_tabBar = new ImHorizontalBox("TabBar");

            // 创建内容Border
            m_contentBorder = new ImBorder("ContentBorder");

            // 根据Tab位置初始布局
            UpdateLayout();
        }

        virtual ~ImPageManager()
        {
            // 清理页面数据
            for (auto& page : m_pages)
            {
                // 注意：这里只删除Tab相关的控件，页面内容由调用者管理
                if (page.tabButton) delete page.tabButton;
            }
        }

        // 1. 添加页面
        void AddPage(const std::string& pageName, ImWidget* pageContent)
        {
            if (pageName.empty() || !pageContent) return;

            // 检查是否已存在同名页面
            if (FindPageData(pageName)) return;

            // 创建页面数据
            PageData newPage(pageName, pageContent);

            // 创建Tab按钮
            ImButton* tabButton = static_cast<ImButton*>(CreateTabWidget(pageName));
            newPage.tabButton = tabButton;

            // 获取Tab内部的控件引用
            ImHorizontalBox* tabHBox = static_cast<ImHorizontalBox*>(tabButton->GetContentSlot()->GetContent());
            if (tabHBox)
            {
                // 获取文本控件
                if (tabHBox->GetSlotNum() >= 1)
                {
                    newPage.tabText = static_cast<ImTextBlock*>(tabHBox->GetSlotAt(0)->GetContent());
                }

                // 获取关闭按钮（如果存在）
                if (m_showCloseButton && tabHBox->GetSlotNum() >= 2)
                {
                    newPage.closeButton = static_cast<ImButton*>(tabHBox->GetSlotAt(1)->GetContent());
                }

                // 设置Tab按钮点击回调
                tabButton->SetOnPressed([this, pageName]()
                    {
                        SwitchToPage(pageName);
                    });

                // 设置关闭按钮点击回调
                if (newPage.closeButton)
                {
                    newPage.closeButton->SetOnPressed([this, pageName]()
                        {
                            RemovePage(pageName);
                        });
                }
            }

            m_pages.push_back(newPage);

            // 添加到Tab栏
            m_tabBar->AddChildToHorizontalBox(tabButton)->SetIfAutoSize(false);

            // 如果是第一个页面，自动切换到该页面
            if (m_pages.size() == 1)
            {
                SwitchToPage(pageName);
            }
        }

        // 2. 移除页面
        void RemovePage(const std::string& pageName)
        {
            if (pageName.empty()) return;

            // 触发页面关闭回调
            m_onPageClosed.Broadcast(pageName);

            // 如果移除的是当前页面，需要切换到其他页面
            bool removingCurrent = (m_currentPage == pageName);

            // 移除页面
            RemovePageInternal(pageName);

            // 如果移除了当前页面，切换到下一个可用页面
            if (removingCurrent && !m_pages.empty())
            {
                SwitchToPage(m_pages[0].name);
            }
            else if (m_pages.empty())
            {
                m_currentPage.clear();
                m_contentBorder->SetContent(nullptr, false);
            }
        }

        // 3. 设置Tab显示位置
        void SetTabPosition(TabPosition position)
        {
            if (m_tabPosition == position) return;

            m_tabPosition = position;
            UpdateLayout();
        }

        TabPosition GetTabPosition() const { return m_tabPosition; }

        // 更新布局
        void UpdateLayout()
        {
            if (!m_rootVerticalBox) return;

            // 清空当前子项
            m_rootVerticalBox->RemoveAllChild();

            // 根据Tab位置添加子项
            if (m_tabPosition == TabPosition::Top)
            {
                m_rootVerticalBox->AddChildToVerticalBox(m_tabBar)->SetIfAutoSize(false);
                m_rootVerticalBox->AddChildToVerticalBox(m_contentBorder);
            }
            else
            {
                m_rootVerticalBox->AddChildToVerticalBox(m_contentBorder);
                m_rootVerticalBox->AddChildToVerticalBox(m_tabBar)->SetIfAutoSize(false);
            }
        }

        // 4. 页面关闭回调委托
        ImMultiDelegate<const std::string&>& OnPageClosed() { return m_onPageClosed; }

        // 5. 页面选中回调委托
        ImMultiDelegate<const std::string&>& OnPageSelected() { return m_onPageSelected; }

        // 获取当前页面名称
        std::string GetCurrentPage() const { return m_currentPage; }

        // 获取页面数量
        size_t GetPageCount() const { return m_pages.size(); }

        // 检查页面是否存在
        bool HasPage(const std::string& pageName)
        {
            return FindPageData(pageName) != nullptr;
        }

        // 切换到下一个页面
        void NextPage()
        {
            if (m_pages.empty()) return;

            auto it = std::find_if(m_pages.begin(), m_pages.end(),
                [this](const PageData& page) { return page.name == m_currentPage; });

            if (it != m_pages.end())
            {
                ++it;
                if (it != m_pages.end())
                {
                    SwitchToPage(it->name);
                }
                else
                {
                    SwitchToPage(m_pages[0].name);
                }
            }
            else if (!m_pages.empty())
            {
                SwitchToPage(m_pages[0].name);
            }
        }

        // 切换到上一个页面
        void PreviousPage()
        {
            if (m_pages.empty()) return;

            auto it = std::find_if(m_pages.rbegin(), m_pages.rend(),
                [this](const PageData& page) { return page.name == m_currentPage; });

            if (it != m_pages.rend())
            {
                ++it;
                if (it != m_pages.rend())
                {
                    SwitchToPage(it->name);
                }
                else
                {
                    SwitchToPage(m_pages.back().name);
                }
            }
            else if (!m_pages.empty())
            {
                SwitchToPage(m_pages.back().name);
            }
        }

        // 新增：设置是否显示关闭按钮
        void SetShowCloseButton(bool show)
        {
            if (m_showCloseButton != show)
            {
                m_showCloseButton = show;
                // 重新创建所有Tab按钮以应用更改
                RecreateAllTabs();
            }
        }

        bool GetShowCloseButton() const { return m_showCloseButton; }

        // 新增：设置关闭按钮尺寸
        void SetCloseButtonSize(float size)
        {
            if (m_closeButtonSize != size)
            {
                m_closeButtonSize = size;
                // 重新创建所有Tab按钮以应用更改
                RecreateAllTabs();
            }
        }

        float GetCloseButtonSize() const { return m_closeButtonSize; }

        // 新增：设置关闭按钮padding
        void SetCloseButtonPadding(float padding)
        {
            if (m_closeButtonPadding != padding)
            {
                m_closeButtonPadding = padding;
                // 重新创建所有Tab按钮以应用更改
                RecreateAllTabs();
            }
        }

        float GetCloseButtonPadding() const { return m_closeButtonPadding; }

    private:
        // 重新创建所有Tab按钮
        void RecreateAllTabs()
        {
            if (!m_tabBar) return;

            // 保存当前页面
            std::string currentPage = m_currentPage;

            // 移除所有Tab按钮但不删除页面内容
            for (auto& page : m_pages)
            {
                if (page.tabButton)
                {
                    m_tabBar->RemoveChild(page.tabButton);
                    delete page.tabButton;
                    page.tabButton = nullptr;
                    page.tabText = nullptr;
                    page.closeButton = nullptr;
                }
            }

            // 重新创建所有Tab按钮
            for (auto& page : m_pages)
            {
                ImButton* tabButton = static_cast<ImButton*>(CreateTabWidget(page.name));
                page.tabButton = tabButton;

                // 设置回调
                ImHorizontalBox* tabHBox = static_cast<ImHorizontalBox*>(tabButton->GetContentSlot()->GetContent());
                if (tabHBox)
                {
                    // 获取文本控件
                    if (tabHBox->GetSlotNum() >= 1)
                    {
                        page.tabText = static_cast<ImTextBlock*>(tabHBox->GetSlotAt(0)->GetContent());
                    }

                    // 获取关闭按钮（如果存在）
                    if (m_showCloseButton && tabHBox->GetSlotNum() >= 2)
                    {
                        page.closeButton = static_cast<ImButton*>(tabHBox->GetSlotAt(1)->GetContent());
                    }

                    // 设置Tab按钮点击回调
                    tabButton->SetOnPressed([this, name = page.name]()
                    {
                        SwitchToPage(name);
                    });

                    // 设置关闭按钮点击回调
                    if (page.closeButton)
                    {
                        page.closeButton->SetOnPressed([this, name = page.name]()
                        {
                            RemovePage(name);
                        });
                    }
                }

                // 添加到Tab栏
                m_tabBar->AddChildToHorizontalBox(tabButton)->SetIfAutoSize(false);
            }

            // 恢复当前页面
            if (!currentPage.empty() && HasPage(currentPage))
            {
                SwitchToPage(currentPage);
            }
            else if (!m_pages.empty())
            {
                SwitchToPage(m_pages[0].name);
            }
        }

    public:
        // 属性系统
        virtual std::unordered_set<PropertyInfo, PropertyInfo::Hasher> GetProperties() override
        {
            auto props = ImUserWidget::GetProperties();

            // Tab位置属性
            props.insert({
                "TabPosition", PropertyType::Enum, "Layout",
                [this](void* v)
                     {
                    std::string str = *static_cast<std::string*>(v);
                    if (str == "Top") SetTabPosition(TabPosition::Top);
                    else if (str == "Bottom") SetTabPosition(TabPosition::Bottom);
                    },
                    [this]() -> void*
                    {
                    static std::vector<std::string> options;
                    options = {"Top", "Bottom"};
                    switch (m_tabPosition)
                    {
                    case TabPosition::Top: options.push_back("Top"); break;
                    case TabPosition::Bottom: options.push_back("Bottom"); break;
                    }
                    return static_cast<void*>(&options);
                    }
                });

            // 新增：关闭按钮相关属性
            props.insert({
                "ShowCloseButton", PropertyType::Bool, "Close Button",
                [this](void* v) { SetShowCloseButton(*static_cast<bool*>(v)); },
                [this]() -> void* { return &m_showCloseButton; }
                });

            props.insert({
                "CloseButtonSize", PropertyType::Float, "Close Button",
                [this](void* v) { SetCloseButtonSize(*static_cast<float*>(v)); },
                [this]() -> void* { return &m_closeButtonSize; }
                });

            props.insert({
                "CloseButtonPadding", PropertyType::Float, "Close Button",
                [this](void* v) { SetCloseButtonPadding(*static_cast<float*>(v)); },
                [this]() -> void* { return &m_closeButtonPadding; }
                });

            return props;
        }

        virtual std::string GetRegisterTypeName() override { return "ImPageManager"; }

        //virtual ImWidget* CopyWidget() override
        //{
        //    return new ImPageManager(*this);
        //}
    };
}