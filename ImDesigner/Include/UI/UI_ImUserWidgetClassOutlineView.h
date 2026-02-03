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
#include "ImGlobal.h"
#include "EditorAction.h"
#include "EditorEvents.h"
#include "Tools/JLog.h"


namespace ImGuiWidget
{
    // 选择信息结构体
    struct OutlineViewSelectionInfo
    {
        std::string VariableName;
        std::string VariableType;
        bool IsRootWidget;
        bool IsChildWidget;
        void* DataPointer;
        std::string WidgetRegisterTypeName;
        ImGuiWidget::ImButton* ItemButton;

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
            bool isRoot, bool isChild, void* dataPtr, ImGuiWidget::ImButton* ItemButton, const std::string& widgetType = "")
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

    // 变更类型枚举
    enum class OutlineViewChangeType
    {
        VariableAdded,      // 变量添加
        VariableRemoved,    // 变量删除
        VariableRenamed,    // 变量重命名
        WidgetChildAdded,   // 子控件添加
        WidgetChildRemoved, // 子控件删除
        WidgetChildRenamed, // 子控件重命名
        FullRefresh         // 完全刷新
    };

    // 变更信息结构
    struct OutlineViewChangeInfo
    {
        OutlineViewChangeType ChangeType;
        std::string VariableName;        // 变量名（对于变量操作）
        std::string OldName;             // 旧名称（对于重命名）
        std::string NewName;             // 新名称（对于重命名）
        std::string VariableType;        // 变量类型：BasicVariable/ObjectVariable/Widget
        std::string ParentVarName;       // 父变量名（对于子控件操作）
        ImWidget* ParentWidget;          // 父控件指针（对于子控件操作）
        ImWidget* ChangedWidget;         // 变更的控件（对于子控件操作）

        OutlineViewChangeInfo(OutlineViewChangeType type) : ChangeType(type) {}
    };

    // 大纲视图控件（支持增量更新）
    class ImUserWidgetClassOutlineView : public ImUserWidget, public EditorGlobalInterface
    {
    private:
        ImUserWidgetClass* m_TargetClass;

        // UI控件
        ImScrollBox* m_ScrollBox;
        ImExpandableBox* m_BasicVarsSection;
        ImExpandableBox* m_ObjectVarsSection;
        ImExpandableBox* m_WidgetTreeSection;
        ImVerticalBox* m_MainContainer;

        // 分区内容容器
        ImVerticalBox* m_BasicVarsContainer;
        ImVerticalBox* m_ObjectVarsContainer;
        ImVerticalBox* m_WidgetTreeContainer;

        // 当前选择
        OutlineViewSelectionInfo m_CurrentSelection;

        // 控件树缓存
        std::unordered_set<std::string> m_RootWidgetNames;

        // 样式颜色
        ImU32 m_SelectedBgColor = IM_COL32(65, 105, 225, 255);
        ImU32 m_HoverBgColor = IM_COL32(70, 130, 180, 255);
        ImU32 m_NormalBgColor = IM_COL32(240, 240, 240, 255);
        ImU32 m_SectionHeaderColor = IM_COL32(80, 80, 80, 255);
        ImU32 m_TextColor = IM_COL32(0, 0, 0, 255);
        ImU32 m_TypeTextColor = IM_COL32(180, 180, 180, 255);

        // 缓存映射
        std::unordered_map<std::string, OutlineViewSelectionInfo> ItemName_To_SelectionInfo;
        std::unordered_map<std::string, ImButton*> VariableName_To_Button;      // 变量名到按钮的映射
        std::unordered_map<std::string, ImExpandableBox*> WidgetPath_To_Expander; // 控件路径到展开框的映射
        std::unordered_map<ImExpandableBox*, std::string> Expander_To_WidgetPath; // 展开框到控件路径的映射
        std::unordered_map<std::string, ImVerticalBox*> WidgetPath_To_Container; // 控件路径到容器的映射

        // 存储展开状态
        std::unordered_map<std::string, bool> m_ExpandedStateMap;

        // 弹出菜单系统
        struct PopupMenuSystem
        {
            ImWindow* BasicVarsSectionMenu = nullptr;
            ImWindow* ObjectVarsSectionMenu = nullptr;
            ImWindow* WidgetTreeSectionMenu = nullptr;
            ImWindow* WidgetRootMenu = nullptr;
            ImWindow* WidgetChildMenu = nullptr;
            ImWindow* InsertWidgetMenu = nullptr;

            ImVerticalBox* BasicVarsMenuContent = nullptr;
            ImVerticalBox* ObjectVarsMenuContent = nullptr;
            ImVerticalBox* WidgetTreeMenuContent = nullptr;
            ImVerticalBox* WidgetRootMenuContent = nullptr;
            ImVerticalBox* WidgetChildMenuContent = nullptr;
            ImVerticalBox* InsertWidgetMenuContent = nullptr;

            enum class MenuMode
            {
                None,
                SectionRoot,
                WidgetRoot,
                WidgetChild
            };

            MenuMode CurrentMode = MenuMode::None;
            std::string TargetVarName;
            ImWidget* TargetWidget = nullptr;

            enum class InsertChildMode
            {
                InsertToThis,
                InsertPrevious,
                InsertNext
            };

            InsertChildMode InsertMode = InsertChildMode::InsertToThis;
        };

        PopupMenuSystem m_PopupMenus;

        // 添加：编辑文件路径
        std::string m_EditedFileFullPath;

        // 添加：Action和Event容器
        std::vector<EditorActionID> m_FileActions;
        std::vector<EditorEventID> m_FileEvents;

    public:
        ImUserWidgetClassOutlineView(const std::string& widgetName, ImUserWidgetClass* targetClass, const std::string& editedFileFullPath)
            : ImUserWidget(widgetName)
            , EditorGlobalInterface()
            , m_TargetClass(targetClass)
            , m_EditedFileFullPath(editedFileFullPath)
            , m_BasicVarsContainer(nullptr)
            , m_ObjectVarsContainer(nullptr)
            , m_WidgetTreeContainer(nullptr)
        {
            BuildRootWidgetCache();
            BuildUI();
            InitPopupMenus();
            ActionInit();
        }

        virtual ~ImUserWidgetClassOutlineView()
        {
            // 注意：m_ScrollBox等控件会被ImUserWidget自动销毁
        }

        // 获取当前选择信息
        const OutlineViewSelectionInfo& GetCurrentSelection() const { return m_CurrentSelection; }

        // 完全刷新视图
        void RefreshView()
        {
            BuildRootWidgetCache();
            ClearAllCaches();
            if (m_MainContainer)
            {
                m_MainContainer->RemoveAllChild(true);
                BuildContent();
            }
        }

        // 设置目标类
        void SetTargetClass(ImUserWidgetClass* targetClass)
        {
            m_TargetClass = targetClass;
            BuildRootWidgetCache();
            ClearAllCaches();
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

        // 构建UI
        void BuildUI()
        {
            m_ScrollBox = new ImScrollBox("OutlineScrollBox");
            m_ScrollBox->EnableVerticalScroll(true);
            m_ScrollBox->ShowVerticalScrollbar(true);

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

            auto* classNameText = new ImTextBlock("ClassNameText");
            classNameText->SetText(u8"类名: " + m_TargetClass->GetClassName());
            classNameText->SetTextColor(m_TextColor);
            classInfoBox->AddChild(classNameText);

            auto* namespaceText = new ImTextBlock("NamespaceText");
            namespaceText->SetText(u8"命名空间: " + m_TargetClass->GetNamespace());
            namespaceText->SetTextColor(m_TextColor);
            classInfoBox->AddChild(namespaceText);

            auto* baseClassText = new ImTextBlock("BaseClassText");
            baseClassText->SetText(u8"基类: " + m_TargetClass->GetBaseClass());
            baseClassText->SetTextColor(m_TextColor);
            classInfoBox->AddChild(baseClassText);

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
            m_BasicVarsSection->SetHead(CreateSectionHeader(u8"基本变量", "BasicVarsSection"));

            m_BasicVarsContainer = new ImVerticalBox("BasicVarsContainer");
            m_BasicVarsSection->SetBody(m_BasicVarsContainer);

            m_BasicVarsSection->SetOnExpandedStateChanged([this](bool expanded)
                {
                    m_ExpandedStateMap["BasicVarsSection"] = expanded;
                });

            bool expanded = true;
            auto it = m_ExpandedStateMap.find("BasicVarsSection");
            if (it != m_ExpandedStateMap.end())
            {
                expanded = it->second;
            }
            m_BasicVarsSection->SetExpandedState(expanded);

            RefreshBasicVariablesContent();

            m_MainContainer->AddChild(m_BasicVarsSection)->SetIfAutoSize(false);
        }

        // 刷新基本变量内容（可独立调用）
        void RefreshBasicVariablesContent()
        {
            if (!m_BasicVarsContainer) return;

            m_BasicVarsContainer->RemoveAllChild(true);
            VariableName_To_Button.clear();

            auto basicVarNames = m_TargetClass->GetBasicVariableNames();
            for (const auto& varName : basicVarNames)
            {
                auto* varItem = CreateBasicVariableItem(varName);
                if (varItem)
                {
                    m_BasicVarsContainer->AddChild(varItem);
                }
            }

            if (basicVarNames.empty())
            {
                auto* emptyText = new ImTextBlock("BasicVarsEmptyText");
                emptyText->SetText(u8"无基本变量");
                emptyText->SetTextColor(IM_COL32(128, 128, 128, 255));
                m_BasicVarsContainer->AddChild(emptyText);
            }
        }

        // ImObject变量分区
        void BuildObjectVariablesSection()
        {
            m_ObjectVarsSection = new ImExpandableBox("ObjectVarsSection");
            m_ObjectVarsSection->SetHead(CreateSectionHeader(u8"ImObject变量", "ObjectVarsSection"));

            m_ObjectVarsContainer = new ImVerticalBox("ObjectVarsContainer");
            m_ObjectVarsSection->SetBody(m_ObjectVarsContainer);

            m_ObjectVarsSection->SetOnExpandedStateChanged([this](bool expanded)
                {
                    m_ExpandedStateMap["ObjectVarsSection"] = expanded;
                });

            bool expanded = true;
            auto it = m_ExpandedStateMap.find("ObjectVarsSection");
            if (it != m_ExpandedStateMap.end())
            {
                expanded = it->second;
            }
            m_ObjectVarsSection->SetExpandedState(expanded);

            RefreshObjectVariablesContent();

            m_MainContainer->AddChild(m_ObjectVarsSection)->SetIfAutoSize(false);
        }

        // 刷新对象变量内容
        void RefreshObjectVariablesContent()
        {
            if (!m_ObjectVarsContainer) return;

            m_ObjectVarsContainer->RemoveAllChild(true);
            VariableName_To_Button.clear();

            auto objectVarNames = m_TargetClass->GetObjectVariableNames();
            for (const auto& varName : objectVarNames)
            {
                auto* varItem = CreateObjectVariableItem(varName);
                if (varItem)
                {
                    m_ObjectVarsContainer->AddChild(varItem);
                }
            }

            if (objectVarNames.empty())
            {
                auto* emptyText = new ImTextBlock("ObjectVarsEmptyText");
                emptyText->SetText(u8"无ImObject变量");
                emptyText->SetTextColor(IM_COL32(128, 128, 128, 255));
                m_ObjectVarsContainer->AddChild(emptyText);
            }
        }

        // 控件树分区
        void BuildWidgetTreeSection()
        {
            m_WidgetTreeSection = new ImExpandableBox("WidgetTreeSection");
            m_WidgetTreeSection->SetHead(CreateSectionHeader(u8"控件树", "WidgetTreeSection"));

            m_WidgetTreeContainer = new ImVerticalBox("WidgetTreeContainer");
            m_WidgetTreeSection->SetBody(m_WidgetTreeContainer);

            m_WidgetTreeSection->SetOnExpandedStateChanged([this](bool expanded)
                {
                    m_ExpandedStateMap["WidgetTreeSection"] = expanded;
                });

            bool expanded = true;
            auto it = m_ExpandedStateMap.find("WidgetTreeSection");
            if (it != m_ExpandedStateMap.end())
            {
                expanded = it->second;
            }
            m_WidgetTreeSection->SetExpandedState(expanded);

            RefreshWidgetTreeContent();

            m_MainContainer->AddChild(m_WidgetTreeSection)->SetIfAutoSize(false);
        }

        // 刷新控件树内容（可指定从某个控件开始刷新）
        void RefreshWidgetTreeContent(ImWidget* startWidget = nullptr, const std::string& startWidgetPath = "")
        {
            if (!m_WidgetTreeContainer) return;

            if (!startWidget)
            {
                // 完全刷新
                m_WidgetTreeContainer->RemoveAllChild(true);
                WidgetPath_To_Expander.clear();
                Expander_To_WidgetPath.clear();
                WidgetPath_To_Container.clear();
                VariableName_To_Button.clear();

                auto widgetVarNames = m_TargetClass->GetWidgetVariableNames();
                for (const auto& varName : widgetVarNames)
                {
                    ImWidget* widget = m_TargetClass->GetWidgetVariable(varName);
                    if (widget)
                    {
                        auto* widgetNode = CreateWidgetTreeNode(widget, varName, "", true);
                        if (widgetNode)
                        {
                            m_WidgetTreeContainer->AddChild(widgetNode)->SetIfAutoSize(false);
                        }
                    }
                }

                if (widgetVarNames.empty())
                {
                    auto* emptyText = new ImTextBlock("WidgetTreeEmptyText");
                    emptyText->SetText(u8"无控件");
                    emptyText->SetTextColor(IM_COL32(128, 128, 128, 255));
                    m_WidgetTreeContainer->AddChild(emptyText);
                }
            }
            else
            {
                // 部分刷新：刷新指定控件节点
                RefreshWidgetTreeNode(startWidget, startWidgetPath);
            }
        }

        // 创建分区标题
        ImWidget* CreateSectionHeader(const std::string& title, const std::string& sectionType)
        {
            auto* headerContainer = new ImHorizontalBox(title + "HeaderContainer");
            auto* headerText = new ImTextBlock(title + "Header");
            headerText->SetText(title);
            headerText->SetTextColor(m_TextColor);
            headerContainer->AddChild(headerText);
            headerContainer->bHaveBackGround = false;

            auto* headerButton = new ImButton(title + "Button");
            headerButton->SetContent(headerContainer);

            headerButton->OnRightClicked.Add([this, sectionType]()
                {
                    ImVec2 mousePos = ImGuiWidget::GetMousePos();
                    ShowSectionRootMenu(sectionType, mousePos);
                });

            return headerButton;
        }

        // 创建基本变量项
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

        // 通用的变量项创建方法
        ImWidget* CreateVariableItem(const std::string& varName, const std::string& varType,
            const std::string& typeName,
            void* dataPtr, ImU32 iconColor)
        {
            auto* itemButton = new ImButton(varName + "Button");
            auto* contentContainer = new ImHorizontalBox(varName + "Content");

            auto* nameText = new ImTextBlock(varName + "Name");
            nameText->SetText(varName);
            nameText->SetTextColor(m_TextColor);
            nameText->SetHorizontalAlignment(ImGuiWidget::ImTextBlock::TextAlignment_Horizontal::Left);

            auto* typeText = new ImTextBlock(varName + "Type");
            typeText->SetText(" [" + typeName + "]");
            typeText->SetTextColor(m_TypeTextColor);
            typeText->SetHorizontalAlignment(ImGuiWidget::ImTextBlock::TextAlignment_Horizontal::Left);

            contentContainer->AddChild(nameText)->SetIfAutoSize(false);
            contentContainer->AddChild(typeText)->SetIfAutoSize(false);

            itemButton->SetContent(contentContainer);

            ConfigureItemButton(itemButton, varType, varName, dataPtr);

            // 缓存按钮引用
            VariableName_To_Button[varName] = itemButton;

            return itemButton;
        }

        // 控件树节点创建
        ImWidget* CreateWidgetTreeNode(ImWidget* widget, const std::string& widgetName,
            const std::string& parentPath, bool isRootLevel)
        {
            if (!widget) return nullptr;

            std::string widgetPath = parentPath.empty() ? widgetName : parentPath + "/" + widgetName;
            bool hasChildren = (widget->GetChildNum() > 0);

            if (hasChildren)
            {
                // 有子控件：使用展开框
                return CreateExpandableWidgetNode(widget, widgetName, widgetPath, isRootLevel);
            }
            else
            {
                // 无子控件：直接创建按钮项
                return CreateWidgetItem(widget, widgetName, widgetPath, isRootLevel);
            }
        }

        // 创建可展开的控件节点
        ImWidget* CreateExpandableWidgetNode(ImWidget* widget, const std::string& widgetName,
            const std::string& widgetPath, bool isRootLevel)
        {
            auto* expandableBox = new ImExpandableBox(widgetName + "ExpandableBox");

            // 创建头部
            auto* headerWidget = CreateWidgetNodeHeader(widget, widgetName, widgetPath, isRootLevel, true);
            expandableBox->SetHead(headerWidget);

            // 创建身体：包含子控件
            auto* bodyContainer = new ImVerticalBox(widgetName + "BodyContainer");

            // 缓存映射
            WidgetPath_To_Expander[widgetPath] = expandableBox;
            Expander_To_WidgetPath[expandableBox] = widgetPath;
            WidgetPath_To_Container[widgetPath] = bodyContainer;
            VariableName_To_Button[widgetName] = dynamic_cast<ImButton*>(headerWidget->GetChildAt(0));

            // 设置展开状态回调
            expandableBox->SetOnExpandedStateChanged([this, widgetPath](bool expanded)
                {
                    m_ExpandedStateMap[widgetPath] = expanded;
                });

            // 应用保存的展开状态
            bool expanded = false;
            auto it = m_ExpandedStateMap.find(widgetPath);
            if (it != m_ExpandedStateMap.end())
            {
                expanded = it->second;
            }
            expandableBox->SetExpandedState(expanded);

            // 添加子控件
            int childCount = widget->GetChildNum();
            for (int i = 0; i < childCount; i++)
            {
                ImWidget* child = widget->GetChildAt(i);
                if (child)
                {
                    auto* childNode = CreateWidgetTreeNode(child, child->GetWidgetName(), widgetPath, false);
                    if (childNode)
                    {
                        bodyContainer->AddChild(childNode);
                    }
                }
            }

            expandableBox->SetBody(bodyContainer);
            expandableBox->SetExpandedState(true);

            return expandableBox;
        }

        // 创建控件节点头部
        ImWidget* CreateWidgetNodeHeader(ImWidget* widget, const std::string& widgetName,
            const std::string& widgetPath, bool isRootLevel, bool expandable)
        {
            auto* headerContainer = new ImHorizontalBox(widgetName + "HeaderContainer");
            auto* widgetButton = CreateWidgetItemButton(widget, widgetName, widgetPath, isRootLevel);
            headerContainer->AddChild(widgetButton);
            return headerContainer;
        }

        // 创建控件项按钮
        ImButton* CreateWidgetItemButton(ImWidget* widget, const std::string& widgetName,
            const std::string& widgetPath, bool isRootLevel)
        {
            auto* itemButton = new ImButton(widgetName + "Button");
            auto* contentContainer = new ImHorizontalBox(widgetName + "Content");

            auto* nameText = new ImTextBlock(widgetName + "Name");
            nameText->SetText(widgetName);
            nameText->SetTextColor(m_TextColor);
            nameText->SetHorizontalAlignment(ImGuiWidget::ImTextBlock::TextAlignment_Horizontal::Left);

            auto* typeText = new ImTextBlock(widgetName + "Type");
            typeText->SetText(" [" + widget->GetRegisterTypeName() + "]");
            typeText->SetTextColor(m_TypeTextColor);
            typeText->SetHorizontalAlignment(ImGuiWidget::ImTextBlock::TextAlignment_Horizontal::Left);

            contentContainer->AddChild(nameText)->SetIfAutoSize(false);
            contentContainer->AddChild(typeText)->SetIfAutoSize(false);

            // 子控件数量提示
            int childCount = widget->GetChildNum();
            if (childCount > 0)
            {
                auto* countText = new ImTextBlock(widgetName + "Count");
                countText->SetText(" (" + std::to_string(childCount) + ")");
                countText->SetTextColor(IM_COL32(150, 150, 200, 255));
                countText->SetHorizontalAlignment(ImGuiWidget::ImTextBlock::TextAlignment_Horizontal::Left);
                contentContainer->AddChild(countText)->SetIfAutoSize(false);
            }

            itemButton->SetContent(contentContainer);
            ConfigureItemButton(itemButton, "Widget", widgetName, widget);

            // 缓存按钮引用
            VariableName_To_Button[widgetName] = itemButton;

            return itemButton;
        }

        // 创建简单的控件项（无子控件的情况）
        ImWidget* CreateWidgetItem(ImWidget* widget, const std::string& widgetName,
            const std::string& widgetPath, bool isRootLevel)
        {
            auto* itemContainer = new ImHorizontalBox(widgetName + "ItemContainer");
            auto* widgetButton = CreateWidgetItemButton(widget, widgetName, widgetPath, isRootLevel);
            itemContainer->AddChild(widgetButton);
            return itemContainer;
        }

        // 配置项按钮样式和回调
        void ConfigureItemButton(ImButton* button, const std::string& itemType,
            const std::string& itemName, void* dataPtr)
        {
            bool isSelected = (m_CurrentSelection.VariableType == itemType &&
                m_CurrentSelection.VariableName == itemName);

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

            if (itemType == "Widget")
            {
                normalStyle.HasBorder = false;
                hoveredStyle.HasBorder = true;
                hoveredStyle.BorderColor = IM_COL32(100, 150, 255, 255);
            }

            button->SetNormalStyle(normalStyle);
            button->SetHoveredStyle(hoveredStyle);
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
            }
            else if (itemType == "ObjectVariable")
            {
                ImObject* obj = static_cast<ImObject*>(dataPtr);
                selectionInfo.IsRootWidget = false;
                selectionInfo.IsChildWidget = false;
                selectionInfo.DataPointer = obj;
                selectionInfo.WidgetRegisterTypeName = obj->GetRegisterTypeName();
            }

            ItemName_To_SelectionInfo[itemName] = selectionInfo;

            button->SetOnPressed([this, itemName]()
                {
                    SelectItemByName(itemName);
                });

            if (itemType == "Widget")
            {
                button->OnRightClicked.Add([this, itemType, itemName, dataPtr]()
                    {
                        ImVec2 mousePos = ImGuiWidget::GetMousePos();
                        ImWidget* widget = static_cast<ImWidget*>(dataPtr);
                        bool isRootWidget = IsRootWidget(itemName);

                        if (isRootWidget)
                        {
                            ShowWidgetRootMenu(itemName, widget, mousePos);
                        }
                        else
                        {
                            ShowWidgetChildMenu(widget, mousePos);
                        }
                    });
            }
        }

        // 判断是否是分区根展开框
        bool IsRootSectionExpandableBox(ImGuiWidget::ImWidget* expandableBox)
        {
            return (expandableBox == m_BasicVarsSection ||
                expandableBox == m_ObjectVarsSection ||
                expandableBox == m_WidgetTreeSection);
        }

        // 外部设置选中item
        void SelectItemByName(const std::string& ItemName, bool OutSideSet = false)
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

                if (OutSideSet)
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
                else
                {
                    Action_SelectItem(m_CurrentSelection);
                }
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

        //-----------增量更新处理函数--------------

    protected:
        //处理基本变量列表刷新
        bool HandelUpdateBasicVariableSection()
        {
            RefreshBasicVariablesContent();
            return true;
        }

        //处理Obejct变量列表刷新
        bool HandelUpdateObjectVariableSection()
        {
            RefreshObjectVariablesContent();
            return true;
        }

        //处理控件树变量列表刷新
        bool HandelUpdateWidgetVariableSection()
        {
            RefreshWidgetTreeContent();
            return true;
        }

        // 处理变量重命名
        bool HandleVariableRenamed(const OutlineViewChangeInfo& changeInfo)
        {
            // 更新缓存映射中的键
            if (ItemName_To_SelectionInfo.find(changeInfo.OldName) != ItemName_To_SelectionInfo.end())
            {
                auto info = ItemName_To_SelectionInfo[changeInfo.OldName];
                info.VariableName = changeInfo.NewName;
                ItemName_To_SelectionInfo[changeInfo.NewName] = info;
                ItemName_To_SelectionInfo.erase(changeInfo.OldName);
            }

            // 更新按钮文本
            auto buttonIt = VariableName_To_Button.find(changeInfo.OldName);
            if (buttonIt != VariableName_To_Button.end())
            {
                ImButton* button = buttonIt->second;
                if (button && button->GetContent())
                {
                    ImHorizontalBox* contentContainer = dynamic_cast<ImHorizontalBox*>(button->GetContent());
                    if (contentContainer)
                    {
                        // 查找名称文本块
                        for (int i = 0; i < contentContainer->GetChildNum(); i++)
                        {
                            ImWidget* child = contentContainer->GetChildAt(i);
                            if (child->GetWidgetName().find("Name") != std::string::npos)
                            {
                                ImTextBlock* nameText = dynamic_cast<ImTextBlock*>(child);
                                if (nameText)
                                {
                                    nameText->SetText(changeInfo.NewName);
                                    break;
                                }
                            }
                        }
                    }

                    // 更新缓存映射
                    VariableName_To_Button[changeInfo.NewName] = button;
                    VariableName_To_Button.erase(changeInfo.OldName);
                }
            }

            // 如果是控件，还需要更新控件树缓存
            if (changeInfo.VariableType == "Widget")
            {
                // 更新根控件缓存
                if (m_RootWidgetNames.find(changeInfo.OldName) != m_RootWidgetNames.end())
                {
                    m_RootWidgetNames.erase(changeInfo.OldName);
                    m_RootWidgetNames.insert(changeInfo.NewName);
                }
            }

            return true;
        }

        // 处理子控件添加
        bool HandleWidgetChildAdded(const OutlineViewChangeInfo& changeInfo)
        {
            if (!changeInfo.ParentWidget) return false;

            // 查找父控件的路径
            std::string parentPath = FindWidgetPath(changeInfo.ParentWidget);
            if (parentPath.empty()) return false;

            // 刷新父控件节点
            RefreshWidgetTreeNode(changeInfo.ParentWidget, parentPath);
            return true;
        }

        // 处理子控件删除
        bool HandleWidgetChildRemoved(const OutlineViewChangeInfo& changeInfo)
        {
            if (!changeInfo.ParentWidget) return false;

            // 查找父控件的路径
            std::string parentPath = FindWidgetPath(changeInfo.ParentWidget);
            if (parentPath.empty()) return false;

            // 刷新父控件节点
            RefreshWidgetTreeNode(changeInfo.ParentWidget, parentPath);
            return true;
        }

        // 处理子控件重命名
        bool HandleWidgetChildRenamed(const OutlineViewChangeInfo& changeInfo)
        {
            if (!changeInfo.ChangedWidget) return false;

            // 更新按钮文本
            std::string widgetName = changeInfo.ChangedWidget->GetWidgetName();
            auto buttonIt = VariableName_To_Button.find(changeInfo.OldName);
            if (buttonIt != VariableName_To_Button.end())
            {
                ImButton* button = buttonIt->second;
                if (button && button->GetContent())
                {
                    ImHorizontalBox* contentContainer = dynamic_cast<ImHorizontalBox*>(button->GetContent());
                    if (contentContainer)
                    {
                        // 查找名称文本块
                        for (int i = 0; i < contentContainer->GetChildNum(); i++)
                        {
                            ImWidget* child = contentContainer->GetChildAt(i);
                            if (child->GetWidgetName().find("Name") != std::string::npos)
                            {
                                ImTextBlock* nameText = dynamic_cast<ImTextBlock*>(child);
                                if (nameText)
                                {
                                    nameText->SetText(changeInfo.NewName);
                                    break;
                                }
                            }
                        }
                    }

                    // 更新缓存映射
                    VariableName_To_Button[changeInfo.NewName] = button;
                    VariableName_To_Button.erase(changeInfo.OldName);
                }
            }

            // 更新选择信息
            if (ItemName_To_SelectionInfo.find(changeInfo.OldName) != ItemName_To_SelectionInfo.end())
            {
                auto info = ItemName_To_SelectionInfo[changeInfo.OldName];
                info.VariableName = changeInfo.NewName;
                ItemName_To_SelectionInfo[changeInfo.NewName] = info;
                ItemName_To_SelectionInfo.erase(changeInfo.OldName);
            }

            return true;
        }

        // 刷新指定控件树节点
        void RefreshWidgetTreeNode(ImWidget* widget, const std::string& widgetPath)
        {
            // 查找对应的展开框
            auto expanderIt = WidgetPath_To_Expander.find(widgetPath);
            if (expanderIt != WidgetPath_To_Expander.end())
            {
                ImExpandableBox* expander = expanderIt->second;
                auto containerIt = WidgetPath_To_Container.find(widgetPath);
                if (containerIt != WidgetPath_To_Container.end())
                {
                    ImVerticalBox* container = containerIt->second;

                    // 保存当前展开状态
                    bool wasExpanded = expander->GetIfExpanded();

                    // 清空容器
                    container->RemoveAllChild(true);

                    // 重新添加子控件
                    int childCount = widget->GetChildNum();
                    for (int i = 0; i < childCount; i++)
                    {
                        ImWidget* child = widget->GetChildAt(i);
                        if (child)
                        {
                            auto* childNode = CreateWidgetTreeNode(child, child->GetWidgetName(), widgetPath, false);
                            if (childNode)
                            {
                                container->AddChild(childNode);
                            }
                        }
                    }

                    // 恢复展开状态
                    expander->SetExpandedState(wasExpanded);
                }
            }
        }

        // 查找控件的路径
        std::string FindWidgetPath(ImWidget* widget)
        {
            if (!widget) return "";

            // 先检查是否是根控件
            std::string widgetName = widget->GetWidgetName();
            if (IsRootWidget(widgetName))
            {
                return widgetName;
            }

            // 递归查找父控件路径
            ImWidget* parent = widget->GetParents();
            if (!parent) return "";

            std::string parentPath = FindWidgetPath(parent);
            if (parentPath.empty()) return "";

            return parentPath + "/" + widgetName;
        }

        // 清空所有缓存
        void ClearAllCaches()
        {
            ItemName_To_SelectionInfo.clear();
            VariableName_To_Button.clear();
            WidgetPath_To_Expander.clear();
            Expander_To_WidgetPath.clear();
            WidgetPath_To_Container.clear();
        }

//-----------弹出菜单相关--------------
	protected:
		// 新增：初始化所有弹出菜单（一次性创建所有菜单内容）
		void InitPopupMenus()
		{
			auto windowManager = ImGuiWidget::GetGlobalApp()->GetWindowManager();

			// 1. 创建基本变量分区菜单
			m_PopupMenus.BasicVarsMenuContent = BuildBasicVarsSectionMenuContent();
			m_PopupMenus.BasicVarsSectionMenu = windowManager->CreatePopupWindow(
				m_PopupMenus.BasicVarsMenuContent->GetMinSize(),
				ImVec2(0, 0),
				m_PopupMenus.BasicVarsMenuContent,
				false
			);
			m_PopupMenus.BasicVarsSectionMenu->Close();

			// 2. 创建Object变量分区菜单
			m_PopupMenus.ObjectVarsMenuContent = BuildObjectVarsSectionMenuContent();
			m_PopupMenus.ObjectVarsSectionMenu = windowManager->CreatePopupWindow(
				m_PopupMenus.ObjectVarsMenuContent->GetMinSize(),
				ImVec2(0, 0),
				m_PopupMenus.ObjectVarsMenuContent,
				false
			);
			m_PopupMenus.ObjectVarsSectionMenu->Close();

			// 3. 创建控件树分区菜单
			m_PopupMenus.WidgetTreeMenuContent = BuildWidgetTreeSectionMenuContent();
			m_PopupMenus.WidgetTreeSectionMenu = windowManager->CreatePopupWindow(
				m_PopupMenus.WidgetTreeMenuContent->GetMinSize(),
				ImVec2(0, 0),
				m_PopupMenus.WidgetTreeMenuContent,
				false
			);
			m_PopupMenus.WidgetTreeSectionMenu->Close();

			// 4. 创建控件根菜单
			m_PopupMenus.WidgetRootMenuContent = BuildWidgetRootMenuContent();
			m_PopupMenus.WidgetRootMenu = windowManager->CreatePopupWindow(
				m_PopupMenus.WidgetRootMenuContent->GetMinSize(),
				ImVec2(0, 0),
				m_PopupMenus.WidgetRootMenuContent,
				false
			);
			m_PopupMenus.WidgetRootMenu->Close();

			// 5. 创建控件子菜单（与UI_WidgetTreeView相同）
			m_PopupMenus.WidgetChildMenuContent = BuildWidgetChildMenuContent();
			m_PopupMenus.WidgetChildMenu = windowManager->CreatePopupWindow(
				m_PopupMenus.WidgetChildMenuContent->GetMinSize(),
				ImVec2(0, 0),
				m_PopupMenus.WidgetChildMenuContent,
				false
			);
			m_PopupMenus.WidgetChildMenu->Close();

			// 6. 创建插入控件子菜单（二级菜单）
			m_PopupMenus.InsertWidgetMenuContent = BuildInsertWidgetMenuContent();
			m_PopupMenus.InsertWidgetMenu = windowManager->CreatePopupWindow(
				m_PopupMenus.InsertWidgetMenuContent->GetMinSize(),
				ImVec2(0, 0),
				m_PopupMenus.InsertWidgetMenuContent,
				false,
				nullptr // 父窗口将在需要时动态设置
			);
			m_PopupMenus.InsertWidgetMenu->Close();
		}

		// 新增：构建基本变量分区菜单内容
		ImVerticalBox* BuildBasicVarsSectionMenuContent()
		{
			ImVerticalBox* content = new ImVerticalBox("BasicVarsMenuContent");

			// 创建各种类型的新建变量按钮
			std::vector<std::pair<std::string, ImGuiWidget::ImBasicVariable::BasicType>> varTypes = {
				{u8"新建整数", ImGuiWidget::ImBasicVariable::BasicType::Int},
				{u8"新建浮点数", ImGuiWidget::ImBasicVariable::BasicType::Float},
				{u8"新建布尔值", ImGuiWidget::ImBasicVariable::BasicType::Bool},
				{u8"新建字符串", ImGuiWidget::ImBasicVariable::BasicType::String},
				{u8"新建颜色", ImGuiWidget::ImBasicVariable::BasicType::Color}
			};

			for (const auto& varType : varTypes)
			{
				ImButton* button = CreateMenuButton(varType.first);
				button->OnLeftClicked.Add([this, varType]()
					{
                        OnCreateBasicVariableClicked(varType.second);
					});
				content->AddChildToVerticalBox(button)->SetIfAutoSize(false);
			}

			return content;
		}

		// 新增：构建Object变量分区菜单内容
		ImVerticalBox* BuildObjectVarsSectionMenuContent()
		{
			ImVerticalBox* content = new ImVerticalBox("ObjectVarsMenuContent");

			ImButton* button = CreateMenuButton(u8"新建ImObject");
			button->OnLeftClicked.Add([this]()
				{
                    OnCreateObjectVariableClicked("test");//ImObject暂未建立工厂模式
				});
			content->AddChildToVerticalBox(button)->SetIfAutoSize(false);

			return content;
		}

		// 新增：构建控件树分区菜单内容
		ImVerticalBox* BuildWidgetTreeSectionMenuContent()
		{
			ImVerticalBox* content = new ImVerticalBox("WidgetTreeMenuContent");

			ImButton* newWidgetBtn = CreateMenuButton(u8"新建控件", true);

			// 悬停时显示插入控件子菜单
			newWidgetBtn->OnMouseHover.Add([this, newWidgetBtn]()
				{
					m_PopupMenus.InsertMode = PopupMenuSystem::InsertChildMode::InsertToThis;
					m_PopupMenus.InsertWidgetMenu->SetParentWindow(m_PopupMenus.WidgetTreeSectionMenu);
					ImVec2 popupPos = newWidgetBtn->GetPosition() + ImVec2(newWidgetBtn->GetSize().x, 0);
					m_PopupMenus.InsertWidgetMenu->SetPopupRect(popupPos);
					m_PopupMenus.InsertWidgetMenu->SetActive();
				});

			newWidgetBtn->OnLeftClicked.Add([this]()
				{
					m_PopupMenus.WidgetTreeSectionMenu->Close();
				});

			content->AddChildToVerticalBox(newWidgetBtn)->SetIfAutoSize(false);

			return content;
		}

		// 新增：构建控件根菜单内容
		ImVerticalBox* BuildWidgetRootMenuContent()
		{
			ImVerticalBox* content = new ImVerticalBox("WidgetRootMenuContent");

			// 插入子项按钮
			ImButton* insertChildBtn = CreateMenuButton(u8"插入子项", true);
			insertChildBtn->OnMouseHover.Add([this, insertChildBtn]()
				{
					m_PopupMenus.InsertMode = PopupMenuSystem::InsertChildMode::InsertToThis;
					m_PopupMenus.InsertWidgetMenu->SetParentWindow(m_PopupMenus.WidgetRootMenu);
					ImVec2 popupPos = insertChildBtn->GetPosition() + ImVec2(insertChildBtn->GetSize().x, 0);
					m_PopupMenus.InsertWidgetMenu->SetPopupRect(popupPos);
					m_PopupMenus.InsertWidgetMenu->SetActive();
				});

			insertChildBtn->OnLeftClicked.Add([this]()
				{
					m_PopupMenus.WidgetRootMenu->Close();
				});

			// 删除控件树变量按钮
			ImButton* deleteBtn = CreateMenuButton(u8"删除控件树变量");
			deleteBtn->OnLeftClicked.Add([this]()
				{
					OnDeleteWidgetClicked();
				});

			content->AddChildToVerticalBox(insertChildBtn)->SetIfAutoSize(false);
			content->AddChildToVerticalBox(deleteBtn)->SetIfAutoSize(false);

			return content;
		}

		// 新增：构建控件子菜单内容（与UI_WidgetTreeView相同）
		ImVerticalBox* BuildWidgetChildMenuContent()
		{
			ImVerticalBox* content = new ImVerticalBox("WidgetChildMenuContent");

			// 复制按钮
			ImButton* copyBtn = CreateMenuButton(u8"复制");
			copyBtn->OnLeftClicked.Add([this]()
				{
					m_PopupMenus.WidgetChildMenu->Close();
					// TODO: 实现复制功能
				});

			// 删除按钮
			ImButton* deleteBtn = CreateMenuButton(u8"删除");
			deleteBtn->OnLeftClicked.Add([this]()
				{
					OnDeleteWidgetClicked();
				});

			// 在前一个位置插入按钮
			ImButton* insertPreviousBtn = CreateMenuButton(u8"在前一个位置插入", true);
			insertPreviousBtn->OnMouseHover.Add([this, insertPreviousBtn]()
				{
					m_PopupMenus.InsertMode = PopupMenuSystem::InsertChildMode::InsertPrevious;
					m_PopupMenus.InsertWidgetMenu->SetParentWindow(m_PopupMenus.WidgetChildMenu);
					ImVec2 popupPos = insertPreviousBtn->GetPosition() + ImVec2(insertPreviousBtn->GetSize().x, 0);
					m_PopupMenus.InsertWidgetMenu->SetPopupRect(popupPos);
					m_PopupMenus.InsertWidgetMenu->SetActive();
				});

			insertPreviousBtn->OnLeftClicked.Add([this]()
				{
					m_PopupMenus.WidgetChildMenu->Close();
				});

			// 插入控件按钮
			ImButton* insertToBtn = CreateMenuButton(u8"插入控件", true);
			insertToBtn->OnMouseHover.Add([this, insertToBtn]()
				{
					m_PopupMenus.InsertMode = PopupMenuSystem::InsertChildMode::InsertToThis;
					m_PopupMenus.InsertWidgetMenu->SetParentWindow(m_PopupMenus.WidgetChildMenu);
					ImVec2 popupPos = insertToBtn->GetPosition() + ImVec2(insertToBtn->GetSize().x, 0);
					m_PopupMenus.InsertWidgetMenu->SetPopupRect(popupPos);
					m_PopupMenus.InsertWidgetMenu->SetActive();
				});

			insertToBtn->OnLeftClicked.Add([this]()
				{
					m_PopupMenus.WidgetChildMenu->Close();
				});

			// 在后一个位置插入按钮
			ImButton* insertAfterBtn = CreateMenuButton(u8"在后一个位置插入", true);
			insertAfterBtn->OnMouseHover.Add([this, insertAfterBtn]()
				{
					m_PopupMenus.InsertMode = PopupMenuSystem::InsertChildMode::InsertNext;
					m_PopupMenus.InsertWidgetMenu->SetParentWindow(m_PopupMenus.WidgetChildMenu);
					ImVec2 popupPos = insertAfterBtn->GetPosition() + ImVec2(insertAfterBtn->GetSize().x, 0);
					m_PopupMenus.InsertWidgetMenu->SetPopupRect(popupPos);
					m_PopupMenus.InsertWidgetMenu->SetActive();
				});

			insertAfterBtn->OnLeftClicked.Add([this]()
				{
					m_PopupMenus.WidgetChildMenu->Close();
				});

			content->AddChildToVerticalBox(copyBtn)->SetIfAutoSize(false);
			content->AddChildToVerticalBox(deleteBtn)->SetIfAutoSize(false);
			content->AddChildToVerticalBox(insertPreviousBtn)->SetIfAutoSize(false);
			content->AddChildToVerticalBox(insertToBtn)->SetIfAutoSize(false);
			content->AddChildToVerticalBox(insertAfterBtn)->SetIfAutoSize(false);

			return content;
		}

		// 新增：构建插入控件子菜单内容
		ImVerticalBox* BuildInsertWidgetMenuContent()
		{
			ImVerticalBox* content = new ImVerticalBox("InsertWidgetMenuContent");

			for (auto& widgetInfo : BasicWidgetList::GetBasicWidgetList())
			{
				ImButton* button = CreateInsertWidgetButton(
					widgetInfo.CN_DisplayName,
					widgetInfo.RegisterName,
					IconManager::GetInstance()->GetIcon(widgetInfo.IconID)
				);
				content->AddChildToVerticalBox(button)->SetIfAutoSize(false);
			}

			return content;
		}

		// 新增：创建菜单按钮
		ImButton* CreateMenuButton(const std::string& text, bool hasSubMenu = false)
		{
			ImButton* button = new ImButton("MenuButton");
			button->bHaveBorder = false;

			ImHorizontalBox* hbox = new ImHorizontalBox("MenuHBox");
			ImTextBlock* textBlock = new ImTextBlock("MenuText");
			textBlock->SetText(text);
			textBlock->SetHorizontalAlignment(ImTextBlock::TextAlignment_Horizontal::Left);

			auto slot = hbox->AddChildToHorizontalBox(textBlock);
			slot->PaddingLeft = 10;
			slot->PaddingRight = 16;
			slot->PaddingBottom = 2;
			slot->PaddingTop = 2;

			if (hasSubMenu)
			{
				ImTextBlock* arrow = new ImTextBlock("MenuArrow");
				arrow->SetText(u8">");
				hbox->AddChildToHorizontalBox(arrow)->SetIfAutoSize(false);
			}

			hbox->bHaveBackGround = false;
			button->SetContent(hbox);

			// 点击按钮关闭菜单（除了有特殊处理的按钮）
			button->OnLeftClicked.Add([this, button]()
				{
					// 默认行为：关闭当前菜单
					// 具体按钮的特殊行为在构建菜单时单独设置
				});

			return button;
		}

		// 新增：创建插入控件按钮
		ImButton* CreateInsertWidgetButton(const std::string& cnName, const std::string& registerName, ImTextureID icon)
		{
			ImButton* button = new ImButton("InsertWidgetButton");
			button->bHaveBorder = false;

			ImHorizontalBox* hbox = new ImHorizontalBox("InsertWidgetHBox");
			ImImage* iconImage = new ImImage("WidgetIcon", icon, 24, 24);
			ImTextBlock* text = new ImTextBlock("WidgetText");
			text->SetText(cnName);
			text->SetHorizontalAlignment(ImTextBlock::TextAlignment_Horizontal::Left);

			hbox->AddChildToHorizontalBox(iconImage)->SetIfAutoSize(false);
			hbox->AddChildToHorizontalBox(text);
			hbox->bHaveBackGround = false;

			button->SetContent(hbox);
			button->OnLeftClicked.Add([this, registerName]()
				{
					OnInsertWidgetButtonClicked(registerName);
				});

			return button;
		}

		// 新增：显示分区根菜单
		void ShowSectionRootMenu(const std::string& sectionName, const ImVec2& position)
		{
			m_PopupMenus.CurrentMode = PopupMenuSystem::MenuMode::SectionRoot;
			m_PopupMenus.TargetVarName = sectionName;

			// 根据分区名称激活对应的菜单窗口
			if (sectionName == "BasicVarsSection")
			{
				m_PopupMenus.BasicVarsSectionMenu->SetPopupRect(position);
				m_PopupMenus.BasicVarsSectionMenu->SetActive();
			}
			else if (sectionName == "ObjectVarsSection")
			{
				m_PopupMenus.ObjectVarsSectionMenu->SetPopupRect(position);
				m_PopupMenus.ObjectVarsSectionMenu->SetActive();
			}
			else if (sectionName == "WidgetTreeSection")
			{
				m_PopupMenus.WidgetTreeSectionMenu->SetPopupRect(position);
				m_PopupMenus.WidgetTreeSectionMenu->SetActive();
			}
		}

		// 新增：显示控件根菜单
		void ShowWidgetRootMenu(const std::string& widgetVarName, ImWidget* widget, const ImVec2& position)
		{
			m_PopupMenus.CurrentMode = PopupMenuSystem::MenuMode::WidgetRoot;
			m_PopupMenus.TargetVarName = widgetVarName;
			m_PopupMenus.TargetWidget = widget;

			m_PopupMenus.WidgetRootMenu->SetPopupRect(position);
			m_PopupMenus.WidgetRootMenu->SetActive();
		}

		// 新增：显示控件子菜单
		void ShowWidgetChildMenu(ImWidget* widget, const ImVec2& position)
		{
			m_PopupMenus.CurrentMode = PopupMenuSystem::MenuMode::WidgetChild;
			m_PopupMenus.TargetWidget = widget;

			// 找到目标控件的变量名
			m_PopupMenus.TargetVarName = "";
			auto widgetVarNames = m_TargetClass->GetWidgetVariableNames();
			for (const auto& varName : widgetVarNames)
			{
				ImWidget* varWidget = m_TargetClass->GetWidgetVariable(varName);
				if (widget->IsInTree(varWidget))
				{
					m_PopupMenus.TargetVarName = varName;
					break;
				}
			}

			m_PopupMenus.WidgetChildMenu->SetPopupRect(position);
			m_PopupMenus.WidgetChildMenu->SetActive();
		}

        void OnCreateBasicVariableClicked(const ImGuiWidget::ImBasicVariable::BasicType& type)
        {
            // 关闭当前激活的菜单
            CloseActiveMenu();

            Action_CreateBasicVariable(type);
        }

		// 新增：处理创建变量点击
		void OnCreateObjectVariableClicked(const std::string& objectRegisterName)
		{
			// 关闭当前激活的菜单
			CloseActiveMenu();

            Action_CreateObjectVariable(objectRegisterName);
		}

		// 新增：处理插入控件点击
		void OnInsertWidgetButtonClicked(const std::string& registerName)
		{
			// 关闭所有菜单
			m_PopupMenus.InsertWidgetMenu->Close();
			CloseActiveMenu();

			// 根据当前模式处理插入逻辑
			switch (m_PopupMenus.CurrentMode)
			{
			case PopupMenuSystem::MenuMode::SectionRoot:
				if (m_PopupMenus.TargetVarName == "WidgetTreeSection")
				{
					// 在控件树根目录新建控件
                    Action_CreateWidgetVariable(registerName);
				}
				break;

			case PopupMenuSystem::MenuMode::WidgetRoot:
				// 在根控件下插入子项
				if (m_PopupMenus.TargetWidget)
				{
                    Action_InsertWidget(
                        m_PopupMenus.TargetVarName,
                        m_PopupMenus.TargetWidget,
                        m_PopupMenus.TargetWidget->GetChildNum(),
                        registerName
                    );
				}
				break;

			case PopupMenuSystem::MenuMode::WidgetChild:
				// 在子控件位置插入
				if (!m_PopupMenus.TargetWidget) break;

				ImWidget* parent = m_PopupMenus.TargetWidget->GetParents();
				if (!parent) break;

				int insertIndex = -1;
				for (int i = 0; i < parent->GetChildNum(); i++)
				{
					if (parent->GetChildAt(i) == m_PopupMenus.TargetWidget)
					{
						insertIndex = i;
						break;
					}
				}

				if (insertIndex < 0) break;

				if (m_PopupMenus.InsertMode == PopupMenuSystem::InsertChildMode::InsertNext)
				{
					insertIndex++;
				}
				else if (m_PopupMenus.InsertMode == PopupMenuSystem::InsertChildMode::InsertToThis)
				{
					// 插入到该控件内部
                    Action_InsertWidget(
                        m_PopupMenus.TargetVarName,
                        m_PopupMenus.TargetWidget,
                        m_PopupMenus.TargetWidget->GetChildNum(),
                        registerName
                    );
                    break;
				}
                //插入到该控件父控件的指定位置（与该控件平级）
                Action_InsertWidget(
                    m_PopupMenus.TargetVarName,
                    parent,
                    insertIndex,
                    registerName
                );

                break;
			}
		}

		// 新增：处理删除控件点击
		void OnDeleteWidgetClicked()
		{
			CloseActiveMenu();

			if ((m_PopupMenus.CurrentMode == PopupMenuSystem::MenuMode::WidgetRoot ||
				m_PopupMenus.CurrentMode == PopupMenuSystem::MenuMode::WidgetChild) &&
				!m_PopupMenus.TargetVarName.empty())
			{
                Action_DeleteWidget(m_PopupMenus.TargetVarName, m_PopupMenus.TargetWidget);
			}
		}

		// 新增：关闭当前激活的菜单
		void CloseActiveMenu()
		{
			switch (m_PopupMenus.CurrentMode)
			{
			case PopupMenuSystem::MenuMode::SectionRoot:
				if (m_PopupMenus.TargetVarName == "BasicVarsSection")
					m_PopupMenus.BasicVarsSectionMenu->Close();
				else if (m_PopupMenus.TargetVarName == "ObjectVarsSection")
					m_PopupMenus.ObjectVarsSectionMenu->Close();
				else if (m_PopupMenus.TargetVarName == "WidgetTreeSection")
					m_PopupMenus.WidgetTreeSectionMenu->Close();
				break;

			case PopupMenuSystem::MenuMode::WidgetRoot:
				m_PopupMenus.WidgetRootMenu->Close();
				break;

			case PopupMenuSystem::MenuMode::WidgetChild:
				m_PopupMenus.WidgetChildMenu->Close();
				break;
			}

			m_PopupMenus.InsertWidgetMenu->Close();
		}

//----------------动作及事件----------------------

        // Action系统初始化
        void ActionInit()
        {
            ResetAction();
            ResetEvent();

            // 监听文件重命名事件
            AddSequentialProcessor(Action::ProjectView::RENAME_FILE, [this](const std::string& OldFullPath, const std::string& NewFullPath)
                {
                    if (m_EditedFileFullPath == OldFullPath)
                    {
                        m_EditedFileFullPath = NewFullPath;
                        ResetAction();
                        ResetEvent();
                    }
                });


        }
        void ResetAction() 
        {
            for (auto& id : m_FileActions)
            {
                RemoveProcessor(id);
            }

            m_FileActions.clear();

            AddSequentialProcessor(m_EditedFileFullPath + Action::WIDGET_SELECTED, [this](const std::string& SelectedWidgetName) 
                {
                    SelectItemByName(SelectedWidgetName);
                });
        };
        
        // 内部Action处理函数
        void Action_SelectItem(const OutlineViewSelectionInfo& selectionInfo)
        {
            if (selectionInfo.VariableType == "Widget")
            {
                if (selectionInfo.IsRootWidget)
                {
                    ExecuteAction(m_EditedFileFullPath + Action::OutlineView::SELECT_VARIABLE, selectionInfo.VariableName);
                }
                ExecuteAction(m_EditedFileFullPath + Action::WIDGET_SELECTED, selectionInfo.VariableName, (ImGuiWidget::ImWidget*)selectionInfo.DataPointer);
            }
            else
            {
                ExecuteAction(m_EditedFileFullPath + Action::OutlineView::SELECT_VARIABLE, selectionInfo.VariableName);
            }
            
        }

        void Action_CreateBasicVariable(const ImGuiWidget::ImBasicVariable::BasicType& typeName)
        {
            ExecuteAction(m_EditedFileFullPath + Action::OutlineView::CREATE_BASIC_VARIABLE, typeName);
        }

        void Action_CreateObjectVariable(const std::string& objectRegisterName)
        {
            ExecuteAction(m_EditedFileFullPath + Action::OutlineView::CREATE_OBJECT_VARIABLE, objectRegisterName);
        }

        void Action_CreateWidgetVariable(const std::string& widgetRegisterName)
        {
            ExecuteAction(m_EditedFileFullPath + Action::OutlineView::CREATE_WIDGET_VARIABLE, widgetRegisterName);
        }

        void Action_InsertWidget(const std::string& OperatorVarName, ImGuiWidget::ImWidget* target, int insertIndex, const std::string& widgetRegisterName)
        {
            ExecuteAction(m_EditedFileFullPath + Action::OutlineView::INSERT_WIDGET, OperatorVarName, target, widgetRegisterName, insertIndex);
        }
        void Action_DeleteWidget(const std::string& widgetRootVarName, ImGuiWidget::ImWidget* target)
        {
            ExecuteAction(m_EditedFileFullPath + Action::OutlineView::DELETE_WIDGET, widgetRootVarName, target);
        }

        void ResetEvent() 
        {
            for (auto& id : m_FileEvents)
            {
                Unsubscribe(id);
            }

            m_FileEvents.clear();

            m_FileEvents.push_back(Subscribe(m_EditedFileFullPath + Events::OutlineView::UPDATE_BASIC_VARIABLE_SECTION, [this]()
                {
                    HandelUpdateBasicVariableSection();
                }));

            m_FileEvents.push_back(Subscribe(m_EditedFileFullPath + Events::OutlineView::UPDATE_OBJECT_VARIABLE_SECTION, [this]()
                {
                    HandelUpdateObjectVariableSection();
                }));

            m_FileEvents.push_back(Subscribe(m_EditedFileFullPath + Events::OutlineView::UPDATE_WIDGET_VARIABLE_SECTION, [this]()
                {
                    HandelUpdateWidgetVariableSection();
                }));

            m_FileEvents.push_back(Subscribe(m_EditedFileFullPath + Events::OutlineView::VARIABLE_RENAMED,
                [this](const std::string& oldName, const std::string& newName)
                {
                    OutlineViewChangeInfo changeInfo(OutlineViewChangeType::VariableRenamed);
                    changeInfo.VariableName = newName;
                    changeInfo.OldName = oldName;
                    changeInfo.NewName = newName;
                    if (m_TargetClass->GetWidgetVariable(newName))
                    {
                        changeInfo.VariableType = "Widget";
                    }
                    else if (m_TargetClass->GetBasicVariable(newName))
                    {
                        changeInfo.VariableType = "ObjectVariable";
                    }
                    else
                    {
                        changeInfo.VariableType = "BasicVariable";
                    }
                    
                    HandleVariableRenamed(changeInfo);
                }
            ));

            // 订阅子控件增量更新事件
            m_FileEvents.push_back(Subscribe(m_EditedFileFullPath + Events::OutlineView::WIDGET_CHILD_ADDED,
                [this](const std::string& parentVarName, const std::string& childWidgetName)
                {
                    // 获取父控件
                    ImWidget* parentWidget = m_TargetClass->GetWidgetVariable(parentVarName);
                    if (!parentWidget) return;

                    // 找到添加的子控件
                    ImWidget* childWidget = nullptr;
                    int childCount = parentWidget->GetChildNum();
                    for (int i = 0; i < childCount; i++)
                    {
                        ImWidget* child = parentWidget->GetChildAt(i);
                        if (child && child->GetWidgetName() == childWidgetName)
                        {
                            childWidget = child;
                            break;
                        }
                    }

                    if (!childWidget) return;

                    OutlineViewChangeInfo changeInfo(OutlineViewChangeType::WidgetChildAdded);
                    changeInfo.ParentVarName = parentVarName;
                    changeInfo.ParentWidget = parentWidget;
                    changeInfo.ChangedWidget = childWidget;
                    HandleWidgetChildAdded(changeInfo);
                }
            ));

            m_FileEvents.push_back(Subscribe(m_EditedFileFullPath + Events::OutlineView::WIDGET_CHILD_REMOVED,
                [this](const std::string& parentVarName, const std::string& childWidgetName)
                {
                    ImWidget* parentWidget = m_TargetClass->GetWidgetVariable(parentVarName);
                    if (!parentWidget) return;

                    OutlineViewChangeInfo changeInfo(OutlineViewChangeType::WidgetChildRemoved);
                    changeInfo.ParentVarName = parentVarName;
                    changeInfo.ParentWidget = parentWidget;
                    // 注意：被删除的控件指针可能无效，这里我们只传递名称
                    changeInfo.VariableName = childWidgetName;
                    HandleWidgetChildRemoved(changeInfo);
                }
            ));

            m_FileEvents.push_back(Subscribe(m_EditedFileFullPath + Events::OutlineView::WIDGET_CHILD_RENAMED,
                [this](const std::string& parentVarName, const std::string& oldName, const std::string& newName)
                {
                    ImWidget* parentWidget = m_TargetClass->GetWidgetVariable(parentVarName);
                    if (!parentWidget) return;

                    // 找到重命名的子控件
                    ImWidget* childWidget = nullptr;
                    int childCount = parentWidget->GetChildNum();
                    for (int i = 0; i < childCount; i++)
                    {
                        ImWidget* child = parentWidget->GetChildAt(i);
                        if (child && child->GetWidgetName() == newName)
                        {
                            childWidget = child;
                            break;
                        }
                    }

                    OutlineViewChangeInfo changeInfo(OutlineViewChangeType::WidgetChildRenamed);
                    changeInfo.ParentVarName = parentVarName;
                    changeInfo.ParentWidget = parentWidget;
                    changeInfo.ChangedWidget = childWidget;
                    changeInfo.OldName = oldName;
                    changeInfo.NewName = newName;
                    HandleWidgetChildRenamed(changeInfo);
                }
            ));

        };
	 };
}