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
    class ImUserWidgetClassOutlineView : public ImUserWidget, public EditorGlobalInterface
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

        // 新增：弹出菜单系统
        struct PopupMenuSystem
        {
            // 主菜单窗口
            ImWindow* BasicVarsSectionMenu = nullptr;
            ImWindow* ObjectVarsSectionMenu = nullptr;
            ImWindow* WidgetTreeSectionMenu = nullptr;
            ImWindow* WidgetRootMenu = nullptr;
            ImWindow* WidgetChildMenu = nullptr;

            // 插入控件子菜单窗口（二级菜单）
            ImWindow* InsertWidgetMenu = nullptr;

            // 菜单内容容器
            ImVerticalBox* BasicVarsMenuContent = nullptr;
            ImVerticalBox* ObjectVarsMenuContent = nullptr;
            ImVerticalBox* WidgetTreeMenuContent = nullptr;
            ImVerticalBox* WidgetRootMenuContent = nullptr;
            ImVerticalBox* WidgetChildMenuContent = nullptr;
            ImVerticalBox* InsertWidgetMenuContent = nullptr;

            // 当前激活的菜单模式
            enum class MenuMode
            {
                None,
                SectionRoot,      // 分区根菜单
                WidgetRoot,       // 控件根菜单
                WidgetChild       // 控件子菜单
            };

            MenuMode CurrentMode = MenuMode::None;
            std::string TargetVarName;
            ImWidget* TargetWidget = nullptr;

            // 控件插入模式
            enum class InsertChildMode
            {
                InsertToThis,
                InsertPrevious,
                InsertNext
            };

            InsertChildMode InsertMode = InsertChildMode::InsertToThis;
        };

        PopupMenuSystem m_PopupMenus;

        // 回调函数
        //std::function<void(const std::string& parentVarName, int insertIndex, const std::string& widgetRegisterName)> m_InsertWidgetCallback;
        //std::function<void(const std::string& widgetVarName)> m_DeleteWidgetCallback;
        //std::function<void(const std::string& sectionType, const std::string& varType)> m_CreateVariableCallback;

        // 添加：编辑文件路径
        std::string m_EditedFileFullPath;

        // 添加：Action和Event容器
        std::vector<ActionID> m_FileActions;
        std::vector<EventID> m_FileEvents;

    public:
        ImUserWidgetClassOutlineView(const std::string& widgetName, ImUserWidgetClass* targetClass, const std::string& editedFileFullPath)
            : ImUserWidget(widgetName)
            , EditorGlobalInterface()  // 基类初始化
            , m_TargetClass(targetClass)
            , m_EditedFileFullPath(editedFileFullPath)
        {
            BuildRootWidgetCache();
            BuildUI();
            InitPopupMenus();
            ActionInit();  // 初始化Action系统
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
            m_BasicVarsSection->SetHead(CreateSectionHeader(u8"基本变量", "BasicVarsSection"));

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
            m_ObjectVarsSection->SetHead(CreateSectionHeader(u8"ImObject变量", "ObjectVarsSection"));

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
            m_WidgetTreeSection->SetHead(CreateSectionHeader(u8"控件树", "WidgetTreeSection"));

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
        ImWidget* CreateSectionHeader(const std::string& title,const std::string& sectionType)
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

			headerButton->OnRightClicked.Add([this, sectionType]()
				{
					ImVec2 mousePos = ImGuiWidget::GetMousePos();
					ShowSectionRootMenu(sectionType, mousePos);
				});

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

            if (itemType == "Widget")
            {
                button->OnRightClicked.Add([this, itemType, itemName, dataPtr]()
                    {
                        ImVec2 mousePos = ImGuiWidget::GetMousePos();
                        ImWidget* widget = static_cast<ImWidget*>(dataPtr);
                        bool isRootWidget = IsRootWidget(itemName);

                        if (isRootWidget)
                        {
                            // 根控件菜单
                            ShowWidgetRootMenu(itemName, widget, mousePos);
                        }
                        else
                        {
                            // 子控件菜单
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
                else//非外部选中，执行动作
                {
                    Action_SelectVar(m_CurrentSelection);
                }
			}
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
			std::vector<std::pair<std::string, std::string>> varTypes = {
				{u8"新建整数", "int"},
				{u8"新建浮点数", "float"},
				{u8"新建布尔值", "bool"},
				{u8"新建字符串", "string"},
				{u8"新建颜色", "color"}
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
				if (IsWidgetInTree(varWidget, widget))
				{
					m_PopupMenus.TargetVarName = varName;
					break;
				}
			}

			m_PopupMenus.WidgetChildMenu->SetPopupRect(position);
			m_PopupMenus.WidgetChildMenu->SetActive();
		}

		// 新增：检查控件是否在指定控件树中
		bool IsWidgetInTree(ImWidget* root, ImWidget* target)
		{
			if (!root || !target) return false;
			if (root == target) return true;

			for (int i = 0; i < root->GetChildNum(); i++)
			{
				if (IsWidgetInTree(root->GetChildAt(i), target))
					return true;
			}

			return false;
		}

        void OnCreateBasicVariableClicked(const std::string& typeName)
        {
            // 关闭当前激活的菜单
            CloseActiveMenu();

            Action_CreateBasicVariable(typeName);
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
                        m_PopupMenus.TargetWidget,
                        m_PopupMenus.TargetWidget->GetChildNum(),
                        registerName
                    );
                    break;
				}
                //插入到该控件父控件的指定位置（与该控件平级）
                Action_InsertWidget(
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

//----------------动作----------------------

        // Action系统初始化
        void ActionInit()
        {
            ResetAction();
            ResetEvent();

            // 监听文件重命名事件（如果需要）
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
        void ResetAction() {};
        void ResetEvent() {};
        // 内部Action处理函数
        void Action_SelectVar(const OutlineViewSelectionInfo& selectionInfo)
        {
            AddLogLine("Action_SelectVar");
        }

        void Action_CreateBasicVariable(const std::string& typeName)
        {
            AddLogLine("Action_CreateBasicVariable");
        }

        void Action_CreateObjectVariable(const std::string& objectRegisterName)
        {
            AddLogLine("Action_CreateObjectVariable");
        }

        void Action_CreateWidgetVariable(const std::string& widgetRegisterName)
        {
            AddLogLine("Action_CreateWidgetVariable");
        }

        void Action_InsertWidget(ImGuiWidget::ImWidget* target, int insertIndex, const std::string& widgetRegisterName)
        {
            AddLogLine("Action_InsertWidget");
        }
        void Action_DeleteWidget(const std::string& widgetRootVarName, ImGuiWidget::ImWidget* target)
        {
            AddLogLine("Action_DeleteWidget");
        }

	 };
}