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
    class ImTabContainer : public ImUserWidget
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
            textBlock->SetHorizontalAlignment(ImTextBlock::TextAlignment_Horizontal::Left);
            textBlock->SetVerticalAlignment(ImTextBlock::TextAlignment_Vertical::Center);

            // 创建关闭按钮
            ImButton* closeButton = new ImButton("CloseButton_" + pageName);
            closeButton->SetOriginalMinSize(ImVec2(20, 20));

            ImTextBlock* closeText = new ImTextBlock("CloseText_" + pageName);
            closeText->SetText("×");
            closeText->SetTextColor(IM_COL32(255, 255, 255, 255));
            closeButton->SetContent(closeText);

            // 设置关闭按钮样式
            ButtonStateStyle closeStyle;
            closeStyle.BackgroundColor = IM_COL32(100, 100, 100, 255);
            closeStyle.Rounding = 10.0f;
            closeButton->SetNormalStyle(closeStyle);

            ButtonStateStyle closeHoverStyle;
            closeHoverStyle.BackgroundColor = IM_COL32(255, 100, 100, 255);
            closeHoverStyle.Rounding = 10.0f;
            closeButton->SetHoveredStyle(closeHoverStyle);

            // 添加到水平框
            tabHBox->AddChildToHorizontalBox(textBlock);
            tabHBox->AddChildToHorizontalBox(closeButton);

            // 创建Tab按钮
            ImButton* tabButton = new ImButton("TabButton_" + pageName);
            tabButton->SetContent(tabHBox);

            // 设置Tab按钮默认样式
            SetupTabButtonStyle(tabButton);

            return tabButton;
        }

        // 设置Tab按钮样式
        void SetupTabButtonStyle(ImButton* button)
        {
            // 正常状态样式
            ButtonStateStyle normalStyle;
            normalStyle.BackgroundColor = IM_COL32(70, 70, 70, 255);
            normalStyle.Rounding = 4.0f;
            normalStyle.HasBorder = true;
            normalStyle.BorderColor = IM_COL32(100, 100, 100, 255);
            button->SetNormalStyle(normalStyle);

            // 悬停状态样式
            ButtonStateStyle hoverStyle;
            hoverStyle.BackgroundColor = IM_COL32(90, 90, 90, 255);
            hoverStyle.Rounding = 4.0f;
            hoverStyle.HasBorder = true;
            hoverStyle.BorderColor = IM_COL32(120, 120, 120, 255);
            button->SetHoveredStyle(hoverStyle);

            // 按下状态样式
            ButtonStateStyle pressedStyle;
            pressedStyle.BackgroundColor = IM_COL32(50, 50, 50, 255);
            pressedStyle.Rounding = 4.0f;
            pressedStyle.HasBorder = true;
            pressedStyle.BorderColor = IM_COL32(80, 80, 80, 255);
            button->SetPressedStyle(pressedStyle);

            // 选中状态样式（焦点状态）
            ButtonStateStyle selectedStyle;
            selectedStyle.BackgroundColor = IM_COL32(30, 144, 255, 255);
            selectedStyle.Rounding = 4.0f;
            selectedStyle.HasBorder = true;
            selectedStyle.BorderColor = IM_COL32(70, 130, 180, 255);
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
        ImTabContainer(const std::string& widgetName)
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

        virtual ~ImTabContainer()
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
            if (tabHBox && tabHBox->GetSlotNum() >= 2)
            {
                newPage.tabText = static_cast<ImTextBlock*>(tabHBox->GetSlotAt(0)->GetContent());
                newPage.closeButton = static_cast<ImButton*>(tabHBox->GetSlotAt(1)->GetContent());

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
            m_tabBar->AddChildToHorizontalBox(tabButton);

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
                m_rootVerticalBox->AddChildToVerticalBox(m_tabBar);
                m_rootVerticalBox->AddChildToVerticalBox(m_contentBorder);
            }
            else
            {
                m_rootVerticalBox->AddChildToVerticalBox(m_contentBorder);
                m_rootVerticalBox->AddChildToVerticalBox(m_tabBar);
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

			return props;
		}

        virtual std::string GetRegisterTypeName() override { return "ImTabContainer"; }

        //virtual ImWidget* CopyWidget() override
        //{
        //    return new ImTabContainer(*this);
        //}
    };
}