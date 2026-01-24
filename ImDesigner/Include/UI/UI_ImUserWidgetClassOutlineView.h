#pragma once
#include "ImUserWidget.h"
#include "ImExpandableBox.h"
#include "ImVerticalBox.h"
#include "ImHorizontalBox.h"
#include "ImButton.h"
#include "ImTextBlock.h"
#include "ImUserWidgetClass.h"
#include "ImScrollBox.h"  // 添加滚动框支持
#include "ImImage.h"      // 添加图标支持

namespace ImGuiWidget
{
    // 选选择信息结构体（保持不变）
    struct OutlineViewSelectionInfo
    {
        std::string VariableName;     // 选中的目标节点名称
        std::string VariableType;     // 变量类型：BasicVariable/ObjectVariable/Widget
        bool IsRootWidget;            // 是否为根控件（控件树根节点）
        bool IsChildWidget;           // 是否为控件的子控件
        void* DataPointer;            // 指向变量/ImObject对应的指针，如果是控件则为实际选中的目标控件实例指针
        std::string WidgetRegisterTypeName; // 控件注册类型名称
        ImGuiWidget::ImButton* ItemButton;//item按钮
        OutlineViewSelectionInfo()
            : VariableName("")
            , VariableType("")
            , IsRootWidget(false)
            , IsChildWidget(false)
            , DataPointer(nullptr)
            , WidgetRegisterTypeName("")
            , ItemButton(nullptr)
        {
        }

        OutlineViewSelectionInfo(const std::string& varName, const std::string& varType,
            bool isRoot, bool isChild, void* dataPtr, ImGuiWidget::ImButton* ItemButton,const std::string& widgetType = "")
            : VariableName(varName)
            , VariableType(varType)
            , IsRootWidget(isRoot)
            , IsChildWidget(isChild)
            , DataPointer(dataPtr)
            , WidgetRegisterTypeName(widgetType)
            , ItemButton(ItemButton)
        {
        }
    };

    // 大纲视图控件（改进版）
    class ImUserWidgetClassOutlineView : public ImUserWidget
    {
    public:
        // 选择回调
        using SelectionCallback = std::function<void(const OutlineViewSelectionInfo& selectionInfo)>;

    private:
        ImUserWidgetClass* m_TargetClass; // 目标类实例
        SelectionCallback m_SelectionCallback; // 选择回调

        // UI控件
        ImScrollBox* m_ScrollBox;           // 最外层滚动框
        ImExpandableBox* m_BasicVarsSection;
        ImExpandableBox* m_ObjectVarsSection;
        ImExpandableBox* m_WidgetTreeSection;
        ImVerticalBox* m_MainContainer;

        // 当前选择
        OutlineViewSelectionInfo m_CurrentSelection;

        // 控件树缓存：记录哪些控件为根控件
        std::unordered_set<std::string> m_RootWidgetNames;

        // 样式颜色
        ImU32 m_SelectedBgColor = IM_COL32(65, 105, 225, 255);   // 选中背景色（蓝色）
        ImU32 m_HoverBgColor = IM_COL32(70, 130, 180, 255);      // 悬停背景色（浅蓝）
        ImU32 m_NormalBgColor = IM_COL32(240, 240, 240, 255);       // 正常背景色（浅灰）
        ImU32 m_SectionHeaderColor = IM_COL32(80, 80, 80, 255);  // 分区标题色
        ImU32 m_TextColor = IM_COL32(0, 0, 0, 255);        // 文本颜色
        ImU32 m_TypeTextColor = IM_COL32(180, 180, 180, 255);     // 类型文本颜色

        std::unordered_map<std::string, OutlineViewSelectionInfo> ItemName_To_SelectionInfo;
        // 存储展开状态
        std::unordered_map<std::string, bool> m_ExpandedStateMap;
    public:
        ImUserWidgetClassOutlineView(const std::string& widgetName, ImUserWidgetClass* targetClass)
            : ImUserWidget(widgetName)
            , m_TargetClass(targetClass)
        {
            BuildRootWidgetCache();
            BuildUI();
        }

        virtual ~ImUserWidgetClassOutlineView()
        {
            // 注意：m_ScrollBox等控件会被ImUserWidget自动销毁
        }

        // 选择回调设置
        void SetSelectionCallback(SelectionCallback callback)
        {
            m_SelectionCallback = callback;
        }

        // 获取当前选择信息
        const OutlineViewSelectionInfo& GetCurrentSelection() const { return m_CurrentSelection; }

        // 刷新视图
        void RefreshView()
        {
            BuildRootWidgetCache();
            if (m_MainContainer)
            {
                m_MainContainer->RemoveAllChild(true);
                BuildContent();
            }
        }

        // 设置目标类（显示其他类的结构）
        void SetTargetClass(ImUserWidgetClass* targetClass)
        {
            m_TargetClass = targetClass;
            BuildRootWidgetCache();
            RefreshView();
        }

        // 设置样式颜色
        void SetSelectedColor(ImU32 color) { m_SelectedBgColor = color; }
        void SetHoverColor(ImU32 color) { m_HoverBgColor = color; }
        void SetNormalColor(ImU32 color) { m_NormalBgColor = color; }

    protected:
        // 构建根控件缓存
        void BuildRootWidgetCache()
        {
            m_RootWidgetNames.clear();
            if (m_TargetClass)
            {
                auto widgetVarNames = m_TargetClass->GetWidgetVariableNames();
                m_RootWidgetNames.insert(widgetVarNames.begin(), widgetVarNames.end());
            }
        }

        // 判断是否为根控件
        bool IsRootWidget(const std::string& widgetName) const
        {
            return m_RootWidgetNames.find(widgetName) != m_RootWidgetNames.end();
        }

        // 构建UI（使用滚动框）
        void BuildUI()
        {
            // 创建滚动框作为最外层容器
            m_ScrollBox = new ImScrollBox("OutlineScrollBox");
            m_ScrollBox->EnableVerticalScroll(true);
            m_ScrollBox->ShowVerticalScrollbar(true);

            // 主容器放在滚动框内
            m_MainContainer = new ImVerticalBox("MainContainer");
            m_ScrollBox->SetContent(m_MainContainer);

            SetRootWidget(m_ScrollBox);
            BuildContent();
        }

        // 构建内容
        void BuildContent()
        {
            if (!m_TargetClass || !m_MainContainer) return;

            // 1. 类信息显示
            BuildClassInfoSection();

            // 2. 基本变量
            BuildBasicVariablesSection();

            // 3. ImObject变量
            BuildObjectVariablesSection();

            // 4. 控件树
            BuildWidgetTreeSection();
        }

        // 类信息分区
        void BuildClassInfoSection()
        {
            auto* classInfoBox = new ImVerticalBox("ClassInfoBox");
            classInfoBox->SetBackGroundColor(IM_COL32(45, 45, 45, 255));

            // 类名
            auto* classNameText = new ImTextBlock("ClassNameText");
            classNameText->SetText(u8"类名: " + m_TargetClass->GetClassName());
            classNameText->SetTextColor(m_TextColor);
            classInfoBox->AddChild(classNameText);

            // 命名空间
            auto* namespaceText = new ImTextBlock("NamespaceText");
            namespaceText->SetText(u8"命名空间: " + m_TargetClass->GetNamespace());
            namespaceText->SetTextColor(m_TextColor);
            classInfoBox->AddChild(namespaceText);

            // 基类
            auto* baseClassText = new ImTextBlock("BaseClassText");
            baseClassText->SetText(u8"基类: " + m_TargetClass->GetBaseClass());
            baseClassText->SetTextColor(m_TextColor);
            classInfoBox->AddChild(baseClassText);

            // 统计信息
            auto* statsText = new ImTextBlock("StatsText");
            statsText->SetText(u8"统计: " +
                std::to_string(m_TargetClass->GetBasicVariableNames().size()) + u8"个基本变量, " +
                std::to_string(m_TargetClass->GetObjectVariableNames().size()) + u8"个Object, " +
                std::to_string(m_TargetClass->GetWidgetVariableNames().size()) + u8"个控件");
            statsText->SetTextColor(m_TextColor);
            classInfoBox->AddChild(statsText);

            m_MainContainer->AddChild(classInfoBox)->SetIfAutoSize(false);
        }

        // 基本变量分区
        void BuildBasicVariablesSection()
        {
            m_BasicVarsSection = new ImExpandableBox("BasicVarsSection");
            m_BasicVarsSection->SetHead(CreateSectionHeader(u8"基本变量"));

            auto* basicVarsContainer = new ImVerticalBox("BasicVarsContainer");
            m_BasicVarsSection->SetBody(basicVarsContainer);

            // 设置展开状态回调
            m_BasicVarsSection->SetOnExpandedStateChanged([this](bool expanded)
                {
                    m_ExpandedStateMap["BasicVarsSection"] = expanded;
                });

            // 应用保存的展开状态
            bool expanded = true; // 默认展开
            auto it = m_ExpandedStateMap.find("BasicVarsSection");
            if (it != m_ExpandedStateMap.end())
            {
                expanded = it->second;
            }
            m_BasicVarsSection->SetExpandedState(expanded);

            // 获取基本变量列表
            auto basicVarNames = m_TargetClass->GetBasicVariableNames();

            for (const auto& varName : basicVarNames)
            {
                auto* varItem = CreateBasicVariableItem(varName);
                basicVarsContainer->AddChild(varItem);
            }

            if (basicVarNames.empty())
            {
                auto* emptyText = new ImTextBlock("BasicVarsEmptyText");
                emptyText->SetText(u8"无基本变量");
                emptyText->SetTextColor(IM_COL32(128, 128, 128, 255));
                basicVarsContainer->AddChild(emptyText);
            }

            m_MainContainer->AddChild(m_BasicVarsSection)->SetIfAutoSize(false);
        }

        // ImObject变量分区
        void BuildObjectVariablesSection()
        {
            m_ObjectVarsSection = new ImExpandableBox("ObjectVarsSection");
            m_ObjectVarsSection->SetHead(CreateSectionHeader(u8"ImObject变量"));

            auto* objectVarsContainer = new ImVerticalBox("ObjectVarsContainer");
            m_ObjectVarsSection->SetBody(objectVarsContainer);

            // 设置展开状态回调
            m_ObjectVarsSection->SetOnExpandedStateChanged([this](bool expanded)
                {
                    m_ExpandedStateMap["ObjectVarsSection"] = expanded;
                });

            // 应用保存的展开状态
            bool expanded = true;
            auto it = m_ExpandedStateMap.find("ObjectVarsSection");
            if (it != m_ExpandedStateMap.end())
            {
                expanded = it->second;
            }
            m_ObjectVarsSection->SetExpandedState(expanded);

            // 获取Object变量列表
            auto objectVarNames = m_TargetClass->GetObjectVariableNames();

            for (const auto& varName : objectVarNames)
            {
                auto* varItem = CreateObjectVariableItem(varName);
                objectVarsContainer->AddChild(varItem);
            }

            if (objectVarNames.empty())
            {
                auto* emptyText = new ImTextBlock("ObjectVarsEmptyText");
                emptyText->SetText(u8"无ImObject变量");
                emptyText->SetTextColor(IM_COL32(128, 128, 128, 255));
                objectVarsContainer->AddChild(emptyText);
            }

            m_MainContainer->AddChild(m_ObjectVarsSection)->SetIfAutoSize(false);
        }

        // 控件树分区
        void BuildWidgetTreeSection()
        {
            m_WidgetTreeSection = new ImExpandableBox("WidgetTreeSection");
            m_WidgetTreeSection->SetHead(CreateSectionHeader(u8"控件树"));

            auto* widgetTreeContainer = new ImVerticalBox("WidgetTreeContainer");
            m_WidgetTreeSection->SetBody(widgetTreeContainer);


            // 设置展开状态回调
            m_WidgetTreeSection->SetOnExpandedStateChanged([this](bool expanded)
                {
                    m_ExpandedStateMap["WidgetTreeSection"] = expanded;
                });

            // 应用保存的展开状态
            bool expanded = true;
            auto it = m_ExpandedStateMap.find("WidgetTreeSection");
            if (it != m_ExpandedStateMap.end())
            {
                expanded = it->second;
            }
            m_WidgetTreeSection->SetExpandedState(expanded);

            // 获取所有根控件
            auto widgetVarNames = m_TargetClass->GetWidgetVariableNames();

            for (const auto& varName : widgetVarNames)
            {
                ImWidget* widget = m_TargetClass->GetWidgetVariable(varName);
                if (widget)
                {
                    auto* widgetNode = CreateWidgetTreeNode(widget, varName, true);
                    if (widgetNode)
                    {
                        widgetTreeContainer->AddChild(widgetNode);
                    }
                }
            }

            if (widgetVarNames.empty())
            {
                auto* emptyText = new ImTextBlock("WidgetTreeEmptyText");
                emptyText->SetText(u8"无控件");
                emptyText->SetTextColor(IM_COL32(128, 128, 128, 255));
                widgetTreeContainer->AddChild(emptyText);
            }

            m_MainContainer->AddChild(m_WidgetTreeSection)->SetIfAutoSize(false);
        }

        // 创建分区标题（改进版，带图标）
        ImWidget* CreateSectionHeader(const std::string& title)
        {
            auto* headerContainer = new ImHorizontalBox(title + "HeaderContainer");

            // 图标
            //auto* iconText = new ImTextBlock(title + "Icon");
            //iconText->SetText(icon);
            //iconText->SetTextColor(IM_COL32(100, 200, 255, 255));

            // 标题文本
            auto* headerText = new ImTextBlock(title + "Header");
            headerText->SetText(title);
            headerText->SetTextColor(m_TextColor);

            //headerContainer->AddChild(iconText);
            headerContainer->AddChild(headerText);
            headerContainer->bHaveBackGround = false;
            auto* headerButton = new ImButton(title + "Button");
            headerButton->SetContent(headerContainer);
            //headerButton->SetBackGroundColor(m_SectionHeaderColor);

            return headerButton;
        }

        // 创建基本变量项（细化风格）
        ImWidget* CreateBasicVariableItem(const std::string& varName)
        {
            ImBasicVariable* basicVar = m_TargetClass->GetBasicVariable(varName);
            if (!basicVar) return nullptr;

            return CreateVariableItem(varName, "BasicVariable",
                GetBasicVariableTypeName(basicVar->GetBasicType()),
                basicVar, IM_COL32(100, 200, 255, 255));
        }

        // 创建Object变量项
        ImWidget* CreateObjectVariableItem(const std::string& varName)
        {
            ImObject* objectVar = m_TargetClass->GetObjectVariable(varName);
            if (!objectVar) return nullptr;

            return CreateVariableItem(varName, "ObjectVariable", 
                objectVar->GetRegisterTypeName(),
                objectVar, IM_COL32(200, 100, 255, 255));
        }

        // 通用的变量项创建方法（统一风格）
        ImWidget* CreateVariableItem(const std::string& varName, const std::string& varType,
             const std::string& typeName,
            void* dataPtr, ImU32 iconColor)
        {
            auto* itemButton = new ImButton(varName + "Button");

            // 水平布局容器
            auto* contentContainer = new ImHorizontalBox(varName + "Content");

            // 图标//暂不使用
            //auto* iconText = new ImTextBlock(varName + "Icon");
            //iconText->SetText(icon);
            //iconText->SetTextColor(iconColor);

            // 变量名
            auto* nameText = new ImTextBlock(varName + "Name");
            nameText->SetText(varName);
            nameText->SetTextColor(m_TextColor);
            nameText->SetHorizontalAlignment(ImGuiWidget::ImTextBlock::TextAlignment_Horizontal::Left);
            // 类型信息
            auto* typeText = new ImTextBlock(varName + "Type");
            typeText->SetText(" [" + typeName + "]");
            typeText->SetTextColor(m_TypeTextColor);
            typeText->SetHorizontalAlignment(ImGuiWidget::ImTextBlock::TextAlignment_Horizontal::Left);
            //contentContainer->AddChild(iconText);
            contentContainer->AddChild(nameText)->SetIfAutoSize(false);
            contentContainer->AddChild(typeText)->SetIfAutoSize(false);

            itemButton->SetContent(contentContainer);

            // 设置按钮样式
            ConfigureItemButton(itemButton, varType, varName, dataPtr);

            return itemButton;
        }

        // 控件树节点创建（递归函数，参考UI_WidgetTreeView的实现）
        ImWidget* CreateWidgetTreeNode(ImWidget* widget, const std::string& widgetName, bool isRootLevel)
        {
            if (!widget) return nullptr;

            bool hasChildren = (widget->GetChildNum() > 0);

            if (hasChildren)
            {
                // 有子控件：使用展开框
                return CreateExpandableWidgetNode(widget, widgetName, isRootLevel);
            }
            else
            {
                // 无子控件：直接创建按钮项
                return CreateWidgetItem(widget, widgetName, isRootLevel);
            }
        }

        // 创建展开器标题
        ImWidget* CreateExpanderHeader(int childCount)
        {
            auto* headerContainer = new ImHorizontalBox("ExpanderHeader");

            auto* arrowIcon = new ImTextBlock("ExpanderArrow");
            arrowIcon->SetText("▶");
            arrowIcon->SetTextColor(IM_COL32(150, 150, 150, 255));

            auto* countText = new ImTextBlock("ExpanderCount");
            countText->SetText(u8"子控件 (" + std::to_string(childCount) + ")");
            countText->SetTextColor(IM_COL32(180, 180, 180, 255));

            headerContainer->AddChild(arrowIcon);
            headerContainer->AddChild(countText);

            return headerContainer;
        }

        // 选择项处理
        void OnItemSelected(const OutlineViewSelectionInfo& selectionInfo)
        {
            m_CurrentSelection = selectionInfo;

            if (m_SelectionCallback)
            {
                m_SelectionCallback(selectionInfo);
            }

            //RefreshView(); // 刷新以更新样式
        }

        // 基本变量类型图标
        std::string GetBasicVariableTypeIcon(ImBasicVariable::BasicType type) const
        {
            switch (type)
            {
            case ImBasicVariable::BasicType::Int: return "🔢";
            case ImBasicVariable::BasicType::Float: return "📐";
            case ImBasicVariable::BasicType::Bool: return "✅";
            case ImBasicVariable::BasicType::String: return "📝";
            case ImBasicVariable::BasicType::Color: return "🎨";
            default: return "❓";
            }
        }

        // 基本变量类型名称
        std::string GetBasicVariableTypeName(ImBasicVariable::BasicType type) const
        {
            switch (type)
            {
            case ImBasicVariable::BasicType::Int: return u8"整数";
            case ImBasicVariable::BasicType::Float: return u8"浮点数";
            case ImBasicVariable::BasicType::Bool: return u8"布尔值";
            case ImBasicVariable::BasicType::String: return u8"字符串";
            case ImBasicVariable::BasicType::Color: return u8"颜色";
            default: return u8"未知";
            }
        }

        // 创建可展开的控件节点
        ImWidget* CreateExpandableWidgetNode(ImWidget* widget, const std::string& widgetName, bool isRootLevel)
        {
            auto* expandableBox = new ImExpandableBox(widgetName + "ExpandableBox");

            // 创建头部：包含控件信息和展开箭头
            auto* headerWidget = CreateWidgetNodeHeader(widget, widgetName, isRootLevel, true);
            expandableBox->SetHead(headerWidget);

            // 创建身体：包含子控件
            auto* bodyContainer = new ImVerticalBox(widgetName + "BodyContainer");
            //bodyContainer->SetPadding(20, 0, 0, 0); // 子节点缩进

                // 设置展开状态回调
            expandableBox->SetOnExpandedStateChanged([this, widgetName](bool expanded)
                {
                    m_ExpandedStateMap[widgetName] = expanded;
                });

            // 应用保存的展开状态
            bool expanded = false; // 默认收起
            auto it = m_ExpandedStateMap.find(widgetName);
            if (it != m_ExpandedStateMap.end())
            {
                expanded = it->second;
            }
            expandableBox->SetExpandedState(expanded);

            int childCount = widget->GetChildNum();
            for (int i = 0; i < childCount; i++)
            {
                ImWidget* child = widget->GetChildAt(i);
                if (child)
                {
                    auto* childNode = CreateWidgetTreeNode(child, child->GetWidgetName(), false);
                    if (childNode)
                    {
                        bodyContainer->AddChild(childNode);
                    }
                }
            }

            expandableBox->SetBody(bodyContainer);

            // 设置展开状态和回调
            expandableBox->SetExpandedState(true); // 默认展开
            expandableBox->SetOnExpandedStateChanged([this, widgetName](bool expanded)
                {
                    // 可以在这里保存/恢复展开状态
                    OnWidgetNodeExpanded(widgetName, expanded);
                });

            return expandableBox;
        }

        // 创建控件节点头部（包含选中按钮和展开箭头）
        ImWidget* CreateWidgetNodeHeader(ImWidget* widget, const std::string& widgetName, bool isRootLevel, bool expandable)
        {
            auto* headerContainer = new ImHorizontalBox(widgetName + "HeaderContainer");

            // 展开箭头（仅当有子控件时显示）
            //if (expandable)
            //{
            //    auto* arrowIcon = new ImTextBlock(widgetName + "Arrow");
            //    arrowIcon->SetText("▼"); // 默认展开状态显示向下箭头
            //    arrowIcon->SetTextColor(IM_COL32(150, 150, 150, 255));
            //    arrowIcon->SetSize(ImVec2(12, 12));
            //    headerContainer->AddChild(arrowIcon);
            //}
            //else
            //{
            //    // 占位空间，保持对齐
            //    auto* spacer = new ImTextBlock(widgetName + "Spacer");
            //    spacer->SetText("  ");
            //    spacer->SetSize(ImVec2(12, 12));
            //    headerContainer->AddChild(spacer);
            //}

            // 控件信息按钮
            auto* widgetButton = CreateWidgetItemButton(widget, widgetName, isRootLevel);
            headerContainer->AddChild(widgetButton);

            return headerContainer;
        }

        // 创建控件项按钮（纯粹的按钮，不包含布局）
        ImButton* CreateWidgetItemButton(ImWidget* widget, const std::string& widgetName, bool isRootLevel)
        {
            auto* itemButton = new ImButton(widgetName + "Button");

            // 水平布局容器
            auto* contentContainer = new ImHorizontalBox(widgetName + "Content");

            // 控件类型图标//暂不用
            //auto* typeIcon = new ImTextBlock(widgetName + "Icon");
            //typeIcon->SetText(isRootLevel ? "🖼️" : "📄"); // 根控件和子控件使用不同图标
            //typeIcon->SetTextColor(isRootLevel ? IM_COL32(100, 255, 100, 255) : IM_COL32(255, 200, 100, 255));

            // 控件名称
            auto* nameText = new ImTextBlock(widgetName + "Name");
            nameText->SetText(widgetName);
            nameText->SetTextColor(m_TextColor);
            nameText->SetHorizontalAlignment(ImGuiWidget::ImTextBlock::TextAlignment_Horizontal::Left);
            // 控件类型（在括号中）
            auto* typeText = new ImTextBlock(widgetName + "Type");
            typeText->SetText(" [" + widget->GetRegisterTypeName() + "]");
            typeText->SetTextColor(m_TypeTextColor);
            typeText->SetHorizontalAlignment(ImGuiWidget::ImTextBlock::TextAlignment_Horizontal::Left);

            // 子控件数量提示（如果有子控件）
            int childCount = widget->GetChildNum();
            if (childCount > 0)
            {
                auto* countText = new ImTextBlock(widgetName + "Count");
                countText->SetText(" (" + std::to_string(childCount) + ")");
                countText->SetTextColor(IM_COL32(150, 150, 200, 255));
                countText->SetHorizontalAlignment(ImGuiWidget::ImTextBlock::TextAlignment_Horizontal::Left);
                contentContainer->AddChild(countText)->SetIfAutoSize(false);
            }

            //contentContainer->AddChild(typeIcon);
            contentContainer->AddChild(nameText)->SetIfAutoSize(false);
            contentContainer->AddChild(typeText)->SetIfAutoSize(false);

            itemButton->SetContent(contentContainer);

            // 设置按钮样式和回调
            ConfigureItemButton(itemButton, "Widget", widgetName, widget);

            return itemButton;
        }

        // 创建简单的控件项（无子控件的情况）
        ImWidget* CreateWidgetItem(ImWidget* widget, const std::string& widgetName, bool isRootLevel)
        {
            auto* itemContainer = new ImHorizontalBox(widgetName + "ItemContainer");

            // 添加缩进占位（保持与有子控件的节点对齐）
            //auto* spacer = new ImTextBlock(widgetName + "Spacer");
            //spacer->SetText("  ");
            //spacer->SetSize(ImVec2(12, 12));
            //itemContainer->AddChild(spacer);

            // 创建控件按钮
            auto* widgetButton = CreateWidgetItemButton(widget, widgetName, isRootLevel);
            itemContainer->AddChild(widgetButton);

            return itemContainer;
        }

        // 控件节点展开状态变化处理
        void OnWidgetNodeExpanded(const std::string& widgetName, bool expanded)
        {
            // 可以在这里保存展开状态，用于持久化
            // 例如：m_ExpandedNodes[widgetName] = expanded;
        }

        // 配置项按钮样式和回调（保持原有逻辑，但针对控件类型做特殊处理）
        void ConfigureItemButton(ImButton* button, const std::string& itemType,
            const std::string& itemName, void* dataPtr)
        {
            // 判断是否为当前选中项
            bool isSelected = (m_CurrentSelection.VariableType == itemType &&
                m_CurrentSelection.VariableName == itemName);

            // 设置样式
            auto& normalStyle = button->GetNormalStyle();
            auto& hoveredStyle = button->GetHoveredStyle();

            if (isSelected)
            {
                normalStyle.BackgroundColor = m_SelectedBgColor;
                hoveredStyle.BackgroundColor = m_HoverBgColor;
            }
            else
            {
                normalStyle.BackgroundColor = m_NormalBgColor;
                hoveredStyle.BackgroundColor = m_HoverBgColor;
            }

            normalStyle.Rounding = 3.0f;
            hoveredStyle.Rounding = 3.0f;

            // 控件按钮使用不同的边框样式
            if (itemType == "Widget")
            {
                normalStyle.HasBorder = false;
                hoveredStyle.HasBorder = true;
                hoveredStyle.BorderColor = IM_COL32(100, 150, 255, 255);
            }

            button->SetNormalStyle(normalStyle);
            button->SetHoveredStyle(hoveredStyle);

            // 设置按钮大小策略
            button->SetOriginalMinSize(ImVec2(200, 28));

            OutlineViewSelectionInfo selectionInfo;
            selectionInfo.VariableName = itemName;
            selectionInfo.VariableType = itemType;
            selectionInfo.ItemButton = button;
            if (itemType == "Widget")
            {
                ImWidget* widget = static_cast<ImWidget*>(dataPtr);
                selectionInfo.IsRootWidget = IsRootWidget(itemName);
                selectionInfo.IsChildWidget = !selectionInfo.IsRootWidget;
                selectionInfo.DataPointer = widget;
                selectionInfo.WidgetRegisterTypeName = widget->GetRegisterTypeName();
            }
            else if (itemType == "BasicVariable")
            {
                ImBasicVariable* var = static_cast<ImBasicVariable*>(dataPtr);
                selectionInfo.IsRootWidget = false;
                selectionInfo.IsChildWidget = false;
                selectionInfo.DataPointer = var;
                selectionInfo.WidgetRegisterTypeName = var->GetRegisterTypeName();
            }
            else if (itemType == "ObjectVariable")
            {
                ImObject* obj = static_cast<ImObject*>(dataPtr);
                selectionInfo.IsRootWidget = false;
                selectionInfo.IsChildWidget = false;
                selectionInfo.DataPointer = obj;
                selectionInfo.WidgetRegisterTypeName = obj->GetRegisterTypeName();
            }

            ItemName_To_SelectionInfo.insert(std::make_pair(itemName, selectionInfo));
            
            // 设置回调
            button->SetOnPressed([this,itemName]()
                {
                    SelectItemByName(itemName);
                });
        }

        // 判断是否是分区根展开框
        bool IsRootSectionExpandableBox(ImGuiWidget::ImWidget* expandableBox)
        {
            return (expandableBox == m_BasicVarsSection ||
                expandableBox == m_ObjectVarsSection ||
                expandableBox == m_WidgetTreeSection);
        }

        //外部设置选中item
        void SelectItemByName(const std::string& ItemName,bool OutSideSet = false/*表示是否由外部设置*/)
        {
            auto it = ItemName_To_SelectionInfo.find(m_CurrentSelection.VariableName);
            if (it != ItemName_To_SelectionInfo.end())
            {
                it->second.ItemButton->GetNormalStyle().BackgroundColor = m_NormalBgColor;
            }
            
            it = ItemName_To_SelectionInfo.find(ItemName);
            if (it != ItemName_To_SelectionInfo.end())
			{
				m_CurrentSelection = it->second;
				it->second.ItemButton->GetNormalStyle().BackgroundColor = m_SelectedBgColor;
				if (m_SelectionCallback && !OutSideSet)
				{
					m_SelectionCallback(m_CurrentSelection);
				}

                if (OutSideSet)//如果是外部选中，递归展开ExpandableBox
                {
                    ImGuiWidget::ImWidget* current = it->second.ItemButton;
                    while (!IsRootSectionExpandableBox(current))
                    {
                        if (current->GetRegisterTypeName() == "ImExpandableBox")
                        {
                            if (ImGuiWidget::ImExpandableBox* eBox = dynamic_cast<ImGuiWidget::ImExpandableBox*>(current))
                            {
                                eBox->SetExpandedState(true);
                                m_ExpandedStateMap[ItemName] = true;
                            }
                        }
                        current = current->GetParents();
                    }
                }

			}
        }
    };
}