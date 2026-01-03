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
            std::string pageID;          // 页面唯一标识（不可更改）
            std::string displayName;     // 显示名称（可随时修改）
            ImWidget* content;
            ImButton* tabButton;
            ImTextBlock* tabText;
            ImButton* closeButton;
            ImTextureID iconTextureID;
            bool hasIcon;
            int iconWidth;
            int iconHeight;

            PageData(const std::string& id, const std::string& name, ImWidget* pageContent, ImTextureID iconID = 0, int iconW = 16, int iconH = 16)
                : pageID(id), displayName(name), content(pageContent),
                tabButton(nullptr), tabText(nullptr), closeButton(nullptr),
                iconTextureID(iconID), hasIcon(iconID != 0), iconWidth(iconW), iconHeight(iconH)
            {
            }
        };

        // 成员变量
        TabPosition m_tabPosition;
        std::vector<PageData> m_pages;
        std::string m_currentPageID;  // 改为存储PageID

        // 新增：图标相关设置
        int m_defaultIconWidth = 16;    // 默认图标宽度
        int m_defaultIconHeight = 16;  // 默认图标高度
        float m_iconTextSpacing = 4.0f; // 图标和文本之间的间距
        // 新增：关闭按钮相关设置
        bool m_showCloseButton = true;           // 是否显示关闭按钮
        float m_closeButtonSize = 6.0f;        // 关闭按钮边长
        float m_closeButtonPadding = 3.0f;      // 关闭按钮padding
        ImU32 m_selectedTabColor = IM_COL32(100, 149, 237, 255); // 默认强调色：矢车菊蓝
        ImU32 m_normalTabColor = IM_COL32(240, 245, 255, 255);   // 正常选项卡颜色

        // 控件指针
        ImVerticalBox* m_rootVerticalBox;
        ImHorizontalBox* m_tabBar;
        ImBorder* m_contentBorder;

        // 多播委托
        ImMultiDelegate<const std::string&> m_onPageClosed;     // 页面关闭回调
        ImMultiDelegate<const std::string&> m_onPageSelected;   // 页面选中回调

        // 创建Tab按钮
        ImWidget* CreateTabWidget(const PageData& pageData, bool isSelected = false)
        {
            // 创建水平框容纳Tab内容
            ImHorizontalBox* tabHBox = new ImHorizontalBox("TabHBox_" + pageData.pageID);

            // 如果有图标，创建图标控件
            ImImage* iconImage = nullptr;
            if (pageData.hasIcon && pageData.iconTextureID != 0)
            {
                iconImage = new ImImage("TabIcon_" + pageData.pageID);
                iconImage->SetTextureID(pageData.iconTextureID, pageData.iconWidth, pageData.iconHeight);
                iconImage->SetOriginalSize(pageData.iconWidth, pageData.iconHeight);

                auto iconSlot = tabHBox->AddChildToHorizontalBox(iconImage);
                iconSlot->SetIfAutoSize(false);
                iconSlot->PaddingRight = m_iconTextSpacing; // 图标和文本之间的间距
            }

            // 创建文本块显示页面名称
            ImTextBlock* textBlock = new ImTextBlock("TabText_" + pageData.pageID);
            textBlock->SetText(pageData.displayName);  // 使用displayName
            textBlock->SetHorizontalAlignment(ImTextBlock::TextAlignment_Horizontal::Center);
            textBlock->SetVerticalAlignment(ImTextBlock::TextAlignment_Vertical::Center);

            auto textSlot = tabHBox->AddChildToHorizontalBox(textBlock);
            textSlot->PaddingBottom = 2.f;
            textSlot->PaddingLeft = 2.f;
            textSlot->PaddingRight = 2.f;
            textSlot->PaddingTop = 2.f;

            // 创建关闭按钮（根据设置决定是否创建）
            ImButton* closeButton = nullptr;
            if (m_showCloseButton)
            {
                closeButton = new ImButton("CloseButton_" + pageData.pageID);
                closeButton->SetOriginalMinSize(ImVec2(m_closeButtonSize, m_closeButtonSize));

                ImTextBlock* closeText = new ImTextBlock("CloseText_" + pageData.pageID);
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

                auto buttonSlot = tabHBox->AddChildToHorizontalBox(closeButton);
                buttonSlot->SetIfAutoSize(false);
                buttonSlot->PaddingBottom = m_closeButtonPadding;
                buttonSlot->PaddingLeft = m_closeButtonPadding;
                buttonSlot->PaddingRight = m_closeButtonPadding;
                buttonSlot->PaddingTop = m_closeButtonPadding;
            }

            tabHBox->SetBackGroundColor(IM_COL32(0, 0, 0, 0));
            tabHBox->bHaveBorder = false;

            // 创建Tab按钮
            ImButton* tabButton = new ImButton("TabButton_" + pageData.pageID);
            tabButton->SetContent(tabHBox);

            // 设置Tab按钮样式，根据是否选中使用不同的样式
            SetupTabButtonStyle(tabButton, isSelected);

            return tabButton;
        }


        // 设置Tab按钮样式 - 浅色灰色系
        void SetupTabButtonStyle(ImButton* button, bool isSelected = false)
        {
            if (isSelected)
            {
                // 选中状态的样式 - 使用强调色
                ButtonStateStyle selectedStyle;
                selectedStyle.BackgroundColor = m_selectedTabColor;
                selectedStyle.Rounding = 4.0f;
                selectedStyle.HasBorder = false;
                selectedStyle.BorderThickness = 1.0f;
                selectedStyle.BorderColor = IM_COL32(70, 130, 180, 255);
                button->SetNormalStyle(selectedStyle);

                // 悬停和按下状态也使用强调色的变体
                ButtonStateStyle selectedHoverStyle;
                selectedHoverStyle.BackgroundColor = IM_COL32(
                    std::min(ImGui::ColorConvertU32ToFloat4(m_selectedTabColor).x * 1.2f * 255, 255.0f),
                    std::min(ImGui::ColorConvertU32ToFloat4(m_selectedTabColor).y * 1.2f * 255, 255.0f),
                    std::min(ImGui::ColorConvertU32ToFloat4(m_selectedTabColor).z * 1.2f * 255, 255.0f),
                    255
                );
                selectedHoverStyle.Rounding = 4.0f;
                selectedHoverStyle.HasBorder = false;
                button->SetHoveredStyle(selectedHoverStyle);

                ButtonStateStyle selectedPressedStyle;
                selectedPressedStyle.BackgroundColor = IM_COL32(
                    std::max(ImGui::ColorConvertU32ToFloat4(m_selectedTabColor).x * 0.8f * 255, 0.0f),
                    std::max(ImGui::ColorConvertU32ToFloat4(m_selectedTabColor).y * 0.8f * 255, 0.0f),
                    std::max(ImGui::ColorConvertU32ToFloat4(m_selectedTabColor).z * 0.8f * 255, 0.0f),
                    255
                );
                selectedPressedStyle.Rounding = 4.0f;
                selectedPressedStyle.HasBorder = false;
                button->SetPressedStyle(selectedPressedStyle);
            }
            else
            {
                // 正常状态样式 - 浅灰色
                ButtonStateStyle normalStyle;
                normalStyle.BackgroundColor = m_normalTabColor;
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
            }

            // 焦点状态样式（保持原有逻辑，但使用较浅的颜色）
            ButtonStateStyle focusedStyle;
            focusedStyle.BackgroundColor = IM_COL32(150, 200, 255, 255);
            focusedStyle.Rounding = 4.0f;
            focusedStyle.HasBorder = false;
            focusedStyle.BorderThickness = 1.0f;
            focusedStyle.BorderColor = IM_COL32(100, 150, 200, 255);
            button->SetFocusedStyle(focusedStyle);
        }



        // 更新Tab按钮的选中状态
        void UpdateTabButtons()
        {
            for (auto& page : m_pages)
            {
                if (page.tabButton)
                {
                    bool isSelected = (page.pageID == m_currentPageID);
                    SetupTabButtonStyle(page.tabButton, isSelected);

                    if (isSelected)
                    {
                        page.tabButton->RequestFocus();
                    }
                    else
                    {
                        page.tabButton->LoseFocus();
                    }
                }
            }
        }

        ImWidget* FindPageContent(const std::string& pageID)
        {
            for (auto& page : m_pages)
            {
                if (page.pageID == pageID)
                {
                    return page.content;
                }
            }
            return nullptr;
        }

        PageData* FindPageData(const std::string& pageID)
        {
            for (auto& page : m_pages)
            {
                if (page.pageID == pageID)
                {
                    return &page;
                }
            }
            return nullptr;
        }

        const PageData* FindPageData(const std::string& pageID) const
        {
            for (const auto& page : m_pages)
            {
                if (page.pageID == pageID)
                {
                    return &page;
                }
            }
            return nullptr;
        }
        // 移除页面内部实现
        void RemovePageInternal(const std::string& pageID)
        {
            auto it = std::remove_if(m_pages.begin(), m_pages.end(),
                [&](const PageData& page) { return page.pageID == pageID; });

            if (it != m_pages.end())
            {
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
        void AddPage(
            const std::string& pageID,
            ImWidget* pageContent,  // 新增显示名称参数
            ImTextureID iconTextureID = 0,
            const std::string& displayName = "",
            int iconWidth = 16,
            int iconHeight = 16)
        {
            if (pageID.empty() || !pageContent) return;

            // 检查是否已存在同PageID页面
            if (FindPageData(pageID)) return;

            // 如果displayName为空，使用pageID作为显示名称
            std::string actualDisplayName = displayName.empty() ? pageID : displayName;

            // 创建页面数据
            PageData newPage(pageID, actualDisplayName, pageContent, iconTextureID, iconWidth, iconHeight);

            // 创建Tab按钮（使用displayName）
            ImButton* tabButton = static_cast<ImButton*>(CreateTabWidget(newPage));
            newPage.tabButton = tabButton;

            // 获取Tab内部的控件引用
            ImHorizontalBox* tabHBox = static_cast<ImHorizontalBox*>(tabButton->GetContentSlot()->GetContent());
            if (tabHBox)
            {
                int slotIndex = 0;

                if (newPage.hasIcon)
                {
                    slotIndex++;
                }

                if (tabHBox->GetSlotNum() > slotIndex)
                {
                    newPage.tabText = static_cast<ImTextBlock*>(tabHBox->GetSlotAt(slotIndex)->GetContent());
                    // 设置显示文本为displayName
                    newPage.tabText->SetText(newPage.displayName);
                }

                if (m_showCloseButton && tabHBox->GetSlotNum() > slotIndex + 1)
                {
                    newPage.closeButton = static_cast<ImButton*>(tabHBox->GetSlotAt(slotIndex + 1)->GetContent());
                }

                // 设置Tab按钮点击回调（使用pageID）
                tabButton->SetOnPressed([this, pageID]()
                    {
                        SwitchToPage(pageID);
                    });

                if (newPage.closeButton)
                {
                    newPage.closeButton->SetOnPressed([this, pageID]()
                        {
                            RemovePage(pageID);
                        });
                }
            }

            m_pages.push_back(newPage);

            // 添加到Tab栏
            m_tabBar->AddChildToHorizontalBox(tabButton)->SetIfAutoSize(false);

            // 如果是第一个页面，自动切换到该页面
            if (m_pages.size() == 1)
            {
                SwitchToPage(pageID);
            }
        }

        // 切换到指定页面
        void SwitchToPage(const std::string& pageID)
        {
            if (m_currentPageID == pageID) return;

            m_currentPageID = pageID;
            UpdateTabButtons();

            ImWidget* content = FindPageContent(pageID);
            if (content)
            {
                m_contentBorder->SetContent(content, false);
            }
            else
            {
                m_contentBorder->SetContent(nullptr, false);
            }

            m_onPageSelected.Broadcast(pageID);
        }

        ImWidget* GetPageContentByID(const std::string& pageID)
        {
            return FindPageContent(pageID);
        }

        // 2. 新增：设置页面显示名称
        void SetPageDisplayName(const std::string& pageID, const std::string& newDisplayName)
        {
            PageData* pageData = FindPageData(pageID);
            if (!pageData || newDisplayName.empty()) return;

            pageData->displayName = newDisplayName;

            // 更新Tab文本显示
            if (pageData->tabText)
            {
                pageData->tabText->SetText(newDisplayName);
            }
        }

        // 3. 获取页面显示名称
        std::string GetPageDisplayName(const std::string& pageID) const
        {
            const PageData* pageData = FindPageData(pageID);
            return pageData ? pageData->displayName : "";
        }

        // 2. 移除页面
        void RemovePage(const std::string& pageID)
        {
            if (pageID.empty()) return;

            m_onPageClosed.Broadcast(pageID);

            bool removingCurrent = (m_currentPageID == pageID);
            RemovePageInternal(pageID);

            if (removingCurrent && !m_pages.empty())
            {
                SwitchToPage(m_pages[0].pageID);
            }
            else if (m_pages.empty())
            {
                m_currentPageID.clear();
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
        std::string GetCurrentPage() const { return m_currentPageID; }

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
                [this](const PageData& page) { return page.pageID == m_currentPageID; });

            if (it != m_pages.end())
            {
                ++it;
                if (it != m_pages.end())
                {
                    SwitchToPage(it->pageID);
                }
                else
                {
                    SwitchToPage(m_pages[0].pageID);
                }
            }
            else if (!m_pages.empty())
            {
                SwitchToPage(m_pages[0].pageID);
            }
        }

        // 切换到上一个页面
        void PreviousPage()
        {
            if (m_pages.empty()) return;

            auto it = std::find_if(m_pages.rbegin(), m_pages.rend(),
                [this](const PageData& page) { return page.pageID == m_currentPageID; });

            if (it != m_pages.rend())
            {
                ++it;
                if (it != m_pages.rend())
                {
                    SwitchToPage(it->pageID);
                }
                else
                {
                    SwitchToPage(m_pages.back().pageID);
                }
            }
            else if (!m_pages.empty())
            {
                SwitchToPage(m_pages.back().pageID);
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

        // 3. 添加设置页面图标的方法
        void SetPageIcon(const std::string& pageName, ImTextureID iconTextureID, int iconWidth = 16, int iconHeight = 16)
        {
            PageData* pageData = FindPageData(pageName);
            if (!pageData) return;

            pageData->iconTextureID = iconTextureID;
            pageData->hasIcon = (iconTextureID != 0);
            pageData->iconWidth = iconWidth;
            pageData->iconHeight = iconHeight;

            // 重新创建Tab按钮以应用图标更改
            RecreateTabForPage(pageName);
        }

        // 4. 添加移除页面图标的方法
        void RemovePageIcon(const std::string& pageName)
        {
            SetPageIcon(pageName, 0, 0, 0);
        }

        // 5. 添加设置默认图标尺寸的方法
        void SetDefaultIconSize(int width, int height)
        {
            m_defaultIconWidth = width;
            m_defaultIconHeight = height;
        }

        void SetIconTextSpacing(float spacing)
        {
            if (m_iconTextSpacing != spacing)
            {
                m_iconTextSpacing = spacing;
                RecreateAllTabs();
            }
        }

        // 获取图标相关设置
        int GetDefaultIconWidth() const { return m_defaultIconWidth; }
        int GetDefaultIconHeight() const { return m_defaultIconHeight; }
        float GetIconTextSpacing() const { return m_iconTextSpacing; }

        // 设置选中选项卡的强调色
        void SetSelectedTabColor(ImU32 color)
        {
            if (m_selectedTabColor != color)
            {
                m_selectedTabColor = color;
                UpdateTabButtons(); // 立即应用更改
            }
        }

        ImU32 GetSelectedTabColor() const { return m_selectedTabColor; }

        // 设置正常选项卡颜色
        void SetNormalTabColor(ImU32 color)
        {
            if (m_normalTabColor != color)
            {
                m_normalTabColor = color;
                UpdateTabButtons(); // 立即应用更改
            }
        }

        ImU32 GetNormalTabColor() const { return m_normalTabColor; }
    private:
        // 6. 添加辅助方法：为单个页面重新创建Tab
        void RecreateTabForPage(const std::string& pageID)
        {
            PageData* pageData = FindPageData(pageID);
            if (!pageData || !m_tabBar) return;

            std::string currentPageID = m_currentPageID;
            bool wasCurrent = (pageID == currentPageID);

            if (pageData->tabButton)
            {
                m_tabBar->RemoveChild(pageData->tabButton);
                delete pageData->tabButton;
                pageData->tabButton = nullptr;
                pageData->tabText = nullptr;
                pageData->closeButton = nullptr;
            }

            ImButton* newTabButton = static_cast<ImButton*>(CreateTabWidget(*pageData));
            pageData->tabButton = newTabButton;

            ImHorizontalBox* tabHBox = static_cast<ImHorizontalBox*>(newTabButton->GetContentSlot()->GetContent());
            if (tabHBox)
            {
                int slotIndex = 0;
                if (pageData->hasIcon) slotIndex++;

                if (tabHBox->GetSlotNum() > slotIndex)
                {
                    pageData->tabText = static_cast<ImTextBlock*>(tabHBox->GetSlotAt(slotIndex)->GetContent());
                }

                if (m_showCloseButton && tabHBox->GetSlotNum() > slotIndex + 1)
                {
                    pageData->closeButton = static_cast<ImButton*>(tabHBox->GetSlotAt(slotIndex + 1)->GetContent());
                }

                newTabButton->SetOnPressed([this, id = pageID]()
                    {
                        SwitchToPage(id);
                    });

                if (pageData->closeButton)
                {
                    pageData->closeButton->SetOnPressed([this, id = pageID]()
                        {
                            RemovePage(id);
                        });
                }
            }

            m_tabBar->AddChildToHorizontalBox(newTabButton)->SetIfAutoSize(false);

            if (wasCurrent)
            {
                SwitchToPage(pageID);
            }
            else if (!currentPageID.empty() && HasPage(currentPageID))
            {
                SwitchToPage(currentPageID);
            }
        }
        // 重新创建所有Tab按钮
        void RecreateAllTabs()
        {
            if (!m_tabBar) return;

            // 保存当前页面
            std::string currentPage = m_currentPageID;

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
                ImButton* tabButton = static_cast<ImButton*>(CreateTabWidget(page));
                page.tabButton = tabButton;

                // 设置回调
                ImHorizontalBox* tabHBox = static_cast<ImHorizontalBox*>(tabButton->GetContentSlot()->GetContent());
                if (tabHBox)
                {
                    int slotIndex = 0;
                    if (page.hasIcon) slotIndex++;

                    // 获取文本控件
                    if (tabHBox->GetSlotNum() > slotIndex)
                    {
                        page.tabText = static_cast<ImTextBlock*>(tabHBox->GetSlotAt(slotIndex)->GetContent());
                    }

                    // 获取关闭按钮（如果存在）
                    if (m_showCloseButton && tabHBox->GetSlotNum() > slotIndex + 1)
                    {
                        page.closeButton = static_cast<ImButton*>(tabHBox->GetSlotAt(slotIndex + 1)->GetContent());
                    }

                    // 设置Tab按钮点击回调
                    tabButton->SetOnPressed([this, name = page.pageID]()
                    {
                        SwitchToPage(name);
                    });

                    // 设置关闭按钮点击回调
                    if (page.closeButton)
                    {
                        page.closeButton->SetOnPressed([this, name = page.pageID]()
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
                SwitchToPage(m_pages[0].pageID);
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

            // 关闭按钮相关属性
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

            // 新增：图标相关属性
            props.insert({
                "DefaultIconWidth", PropertyType::Int, "Icon",
                [this](void* v) { m_defaultIconWidth = *static_cast<int*>(v); RecreateAllTabs(); },
                [this]() -> void* { return &m_defaultIconWidth; }
                });

            props.insert({
                "DefaultIconHeight", PropertyType::Int, "Icon",
                [this](void* v) { m_defaultIconHeight = *static_cast<int*>(v); RecreateAllTabs(); },
                [this]() -> void* { return &m_defaultIconHeight; }
                });

            props.insert({
                "IconTextSpacing", PropertyType::Float, "Icon",
                [this](void* v) { SetIconTextSpacing(*static_cast<float*>(v)); },
                [this]() -> void* { return &m_iconTextSpacing; }
                });
            // 添加选项卡颜色属性
            props.insert({
                "SelectedTabColor", PropertyType::Color, "Tab Style",
                [this](void* v) { SetSelectedTabColor(*static_cast<ImU32*>(v)); },
                [this]() -> void* { return &m_selectedTabColor; }
                });

            props.insert({
                "NormalTabColor", PropertyType::Color, "Tab Style",
                [this](void* v) { SetNormalTabColor(*static_cast<ImU32*>(v)); },
                [this]() -> void* { return &m_normalTabColor; }
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