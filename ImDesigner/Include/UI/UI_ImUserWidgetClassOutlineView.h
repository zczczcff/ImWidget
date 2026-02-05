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
	std::string ItemName;  // 完整路径（用于控件路径格式）

	OutlineViewSelectionInfo()
		: VariableName("")
		, VariableType("")
		, IsRootWidget(false)
		, IsChildWidget(false)
		, DataPointer(nullptr)
		, WidgetRegisterTypeName("")
		, ItemButton(nullptr)
		, ItemName("")
	{
	}

	OutlineViewSelectionInfo(const std::string& varName, const std::string& varType,
		bool isRoot, bool isChild, void* dataPtr, ImGuiWidget::ImButton* ItemButton, const std::string& widgetType = "", const std::string& itemName = "")
		: VariableName(varName)
		, VariableType(varType)
		, IsRootWidget(isRoot)
		, IsChildWidget(isChild)
		, DataPointer(dataPtr)
		, WidgetRegisterTypeName(widgetType)
		, ItemButton(ItemButton)
		, ItemName(itemName.empty() ? varName : itemName)
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
	ImGuiWidget::ImWidget* ParentWidget;          // 父控件指针（对于子控件操作）
	ImGuiWidget::ImWidget* ChangedWidget;         // 变更的控件（对于子控件操作）

	OutlineViewChangeInfo(OutlineViewChangeType type) : ChangeType(type) {}
};

// 大纲视图控件（支持增量更新）
class UI_ImUserWidgetClassOutlineView : public ImGuiWidget::ImUserWidget, public EditorEventObject
{
private:
	ImGuiWidget::ImUserWidgetClass* m_TargetClass;

	// UI控件
	ImGuiWidget::ImScrollBox* m_ScrollBox;
	ImGuiWidget::ImExpandableBox* m_BasicVarsSection;
	ImGuiWidget::ImExpandableBox* m_ObjectVarsSection;
	ImGuiWidget::ImExpandableBox* m_WidgetTreeSection;
	ImGuiWidget::ImVerticalBox* m_MainContainer;

	// 分区内容容器
	ImGuiWidget::ImVerticalBox* m_BasicVarsContainer;
	ImGuiWidget::ImVerticalBox* m_ObjectVarsContainer;
	ImGuiWidget::ImVerticalBox* m_WidgetTreeContainer;

	// 当前选择
	OutlineViewSelectionInfo m_CurrentSelection;

	// 防止循环触发标志
	bool m_IsProcessingAction;

	// 控件树缓存
	//std::unordered_set<std::string> m_RootWidgetNames;

	// 样式颜色
	ImU32 m_SelectedBgColor = IM_COL32(65, 105, 225, 255);
	ImU32 m_HoverBgColor = IM_COL32(70, 130, 180, 255);
	ImU32 m_NormalBgColor = IM_COL32(240, 240, 240, 255);
	ImU32 m_SectionHeaderColor = IM_COL32(80, 80, 80, 255);
	ImU32 m_TextColor = IM_COL32(0, 0, 0, 255);
	ImU32 m_TypeTextColor = IM_COL32(180, 180, 180, 255);

	// 缓存映射
	std::unordered_map<std::string, OutlineViewSelectionInfo> ItemName_To_SelectionInfo;
	std::unordered_map<std::string, ImGuiWidget::ImButton*> VariableName_To_Button;      // 变量名到按钮的映射
	std::unordered_map<std::string, ImGuiWidget::ImExpandableBox*> WidgetPath_To_Expander; // 控件路径到展开框的映射
	std::unordered_map<std::string, ImGuiWidget::ImTextBlock*> WidgetName_To_CountText;   // 控件名到计数文本的映射
	std::unordered_map<std::string, ImGuiWidget::ImHorizontalBox*> WidgetName_To_ContentContainer; // 控件名到内容容器的映射
	std::unordered_map<std::string, bool> WidgetName_HadChildren;                     // 控件名到是否有子控件的记录
	std::unordered_map<ImGuiWidget::ImExpandableBox*, std::string> Expander_To_WidgetPath; // 展开框到控件路径的映射
	std::unordered_map<std::string, ImGuiWidget::ImVerticalBox*> WidgetPath_To_Container; // 控件路径到容器的映射

	// 存储展开状态
	std::unordered_map<std::string, bool> m_ExpandedStateMap;

	// 弹出菜单系统
	struct PopupMenuSystem
	{
		ImGuiWidget::ImWindow* BasicVarsSectionMenu = nullptr;
		ImGuiWidget::ImWindow* ObjectVarsSectionMenu = nullptr;
		ImGuiWidget::ImWindow* WidgetTreeSectionMenu = nullptr;
		ImGuiWidget::ImWindow* WidgetRootMenu = nullptr;
		ImGuiWidget::ImWindow* WidgetChildMenu = nullptr;
		ImGuiWidget::ImWindow* InsertWidgetMenu = nullptr;
		ImGuiWidget::ImWindow* VariableMenu = nullptr;  // 新增：变量右键菜单

		ImGuiWidget::ImVerticalBox* BasicVarsMenuContent = nullptr;
		ImGuiWidget::ImVerticalBox* ObjectVarsMenuContent = nullptr;
		ImGuiWidget::ImVerticalBox* WidgetTreeMenuContent = nullptr;
		ImGuiWidget::ImVerticalBox* WidgetRootMenuContent = nullptr;
		ImGuiWidget::ImVerticalBox* WidgetChildMenuContent = nullptr;
		ImGuiWidget::ImVerticalBox* InsertWidgetMenuContent = nullptr;
		ImGuiWidget::ImVerticalBox* VariableMenuContent = nullptr;  // 新增：变量菜单内容

		enum class MenuMode
		{
			None,
			SectionRoot,
			WidgetRoot,
			WidgetChild,
			Variable  // 新增：变量模式
		};

		MenuMode CurrentMode = MenuMode::None;
		std::string TargetVarName;
		std::string TargetVarType;  // 新增：变量类型（BasicVariable/ObjectVariable）
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
	UI_ImUserWidgetClassOutlineView(const std::string& widgetName, ImGuiWidget::ImUserWidgetClass* targetClass, const std::string& editedFileFullPath)
		: ImGuiWidget::ImUserWidget(widgetName)
		, EditorEventObject()
		, m_TargetClass(targetClass)
		, m_EditedFileFullPath(editedFileFullPath)
		, m_BasicVarsContainer(nullptr)
		, m_ObjectVarsContainer(nullptr)
		, m_WidgetTreeContainer(nullptr)
		, m_IsProcessingAction(false)
	{
		BuildUI();
		InitPopupMenus();
		ActionInit();
	}

	virtual ~UI_ImUserWidgetClassOutlineView()
	{
		// 注意：m_ScrollBox等控件会被ImUserWidget自动销毁
	}

	// 获取当前选择信息
	const OutlineViewSelectionInfo& GetCurrentSelection() const { return m_CurrentSelection; }

	// 完全刷新视图
	void RefreshView()
	{
		ClearAllCaches();
		if (m_MainContainer)
		{
			m_MainContainer->RemoveAllChild(true);
			BuildContent();
		}
	}

	// 设置目标类
	void SetTargetClass(ImGuiWidget::ImUserWidgetClass* targetClass)
	{
		m_TargetClass = targetClass;
		ClearAllCaches();
		RefreshView();
	}

	// 设置样式颜色
	void SetSelectedColor(ImU32 color) { m_SelectedBgColor = color; }
	void SetHoverColor(ImU32 color) { m_HoverBgColor = color; }
	void SetNormalColor(ImU32 color) { m_NormalBgColor = color; }

protected:

	// 判断是否为根控件
	bool IsRootWidget(const std::string& widgetName) const
	{
		return m_TargetClass->GetVariable(widgetName);
	}

	// 构建UI
	void BuildUI()
	{
		m_ScrollBox = new ImGuiWidget::ImScrollBox("OutlineScrollBox");
		m_ScrollBox->EnableVerticalScroll(true);
		m_ScrollBox->ShowVerticalScrollbar(true);

		m_MainContainer = new ImGuiWidget::ImVerticalBox("MainContainer");
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
		auto* classInfoBox = new ImGuiWidget::ImVerticalBox("ClassInfoBox");
		classInfoBox->SetBackGroundColor(IM_COL32(45, 45, 45, 255));

		auto* classNameText = new ImGuiWidget::ImTextBlock("ClassNameText");
		classNameText->SetText(u8"类名: " + m_TargetClass->GetClassName());
		classNameText->SetTextColor(m_TextColor);
		classInfoBox->AddChild(classNameText);

		auto* namespaceText = new ImGuiWidget::ImTextBlock("NamespaceText");
		namespaceText->SetText(u8"命名空间: " + m_TargetClass->GetNamespace());
		namespaceText->SetTextColor(m_TextColor);
		classInfoBox->AddChild(namespaceText);

		auto* baseClassText = new ImGuiWidget::ImTextBlock("BaseClassText");
		baseClassText->SetText(u8"基类: " + m_TargetClass->GetBaseClass());
		baseClassText->SetTextColor(m_TextColor);
		classInfoBox->AddChild(baseClassText);

		auto* statsText = new ImGuiWidget::ImTextBlock("StatsText");
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
		m_BasicVarsSection = new ImGuiWidget::ImExpandableBox("BasicVarsSection");
		m_BasicVarsSection->SetHead(CreateSectionHeader(u8"基本变量", "BasicVarsSection"));

		m_BasicVarsContainer = new ImGuiWidget::ImVerticalBox("BasicVarsContainer");
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
			auto* emptyText = new ImGuiWidget::ImTextBlock("BasicVarsEmptyText");
			emptyText->SetText(u8"无基本变量");
			emptyText->SetTextColor(IM_COL32(128, 128, 128, 255));
			m_BasicVarsContainer->AddChild(emptyText);
		}
	}

	// ImObject变量分区
	void BuildObjectVariablesSection()
	{
		m_ObjectVarsSection = new ImGuiWidget::ImExpandableBox("ObjectVarsSection");
		m_ObjectVarsSection->SetHead(CreateSectionHeader(u8"ImObject变量", "ObjectVarsSection"));

		m_ObjectVarsContainer = new ImGuiWidget::ImVerticalBox("ObjectVarsContainer");
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
			auto* emptyText = new ImGuiWidget::ImTextBlock("ObjectVarsEmptyText");
			emptyText->SetText(u8"无ImObject变量");
			emptyText->SetTextColor(IM_COL32(128, 128, 128, 255));
			m_ObjectVarsContainer->AddChild(emptyText);
		}
	}

	// 控件树分区
	void BuildWidgetTreeSection()
	{
		m_WidgetTreeSection = new ImGuiWidget::ImExpandableBox("WidgetTreeSection");
		m_WidgetTreeSection->SetHead(CreateSectionHeader(u8"控件树", "WidgetTreeSection"));

		m_WidgetTreeContainer = new ImGuiWidget::ImVerticalBox("WidgetTreeContainer");
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
	void RefreshWidgetTreeContent(ImGuiWidget::ImWidget* startWidget = nullptr, const std::string& startWidgetPath = "")
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
				ImGuiWidget::ImWidget* widget = m_TargetClass->GetWidgetVariable(varName);
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
				auto* emptyText = new ImGuiWidget::ImTextBlock("WidgetTreeEmptyText");
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
	ImGuiWidget::ImWidget* CreateSectionHeader(const std::string& title, const std::string& sectionType)
	{
		auto* headerContainer = new ImGuiWidget::ImHorizontalBox(title + "HeaderContainer");
		auto* headerText = new ImGuiWidget::ImTextBlock(title + "Header");
		headerText->SetText(title);
		headerText->SetTextColor(m_TextColor);
		headerContainer->AddChild(headerText);
		headerContainer->bHaveBackGround = false;

		auto* headerButton = new ImGuiWidget::ImButton(title + "Button");
		headerButton->SetContent(headerContainer);

		headerButton->OnRightClicked.Add([this, sectionType]()
			{
				ImVec2 mousePos = ImGuiWidget::GetMousePos();
				ShowSectionRootMenu(sectionType, mousePos);
			});

		return headerButton;
	}

	// 创建基本变量项
	ImGuiWidget::ImWidget* CreateBasicVariableItem(const std::string& varName)
	{
		ImGuiWidget::ImWidgetClassVariable_Basic* basicVar = m_TargetClass->GetBasicVariable(varName);
		if (!basicVar) return nullptr;

		return CreateVariableItem(varName, "BasicVariable",
			GetBasicVariableTypeName(basicVar->GetValueType()),
			basicVar, IM_COL32(100, 200, 255, 255));
	}

	// 创建Object变量项
	ImGuiWidget::ImWidget* CreateObjectVariableItem(const std::string& varName)
	{
		ImObject* objectVar = m_TargetClass->GetObjectVariable(varName);
		if (!objectVar) return nullptr;

		return CreateVariableItem(varName, "ObjectVariable",
			objectVar->GetRegisterTypeName(),
			objectVar, IM_COL32(200, 100, 255, 255));
	}

	// 通用的变量项创建方法
	ImGuiWidget::ImWidget* CreateVariableItem(const std::string& varName, const std::string& varType,
		const std::string& typeName,
		void* dataPtr, ImU32 iconColor)
	{
		auto* itemButton = new ImGuiWidget::ImButton(varName + "Button");
		auto* contentContainer = new ImGuiWidget::ImHorizontalBox(varName + "Content");

		auto* nameText = new ImGuiWidget::ImTextBlock(varName + "Name");
		nameText->SetText(varName);
		nameText->SetTextColor(m_TextColor);
		nameText->SetHorizontalAlignment(ImGuiWidget::ImTextBlock::TextAlignment_Horizontal::Left);

		auto* typeText = new ImGuiWidget::ImTextBlock(varName + "Type");
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
	ImGuiWidget::ImWidget* CreateWidgetTreeNode(ImGuiWidget::ImWidget* widget, const std::string& widgetName,
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
	ImGuiWidget::ImWidget* CreateExpandableWidgetNode(ImGuiWidget::ImWidget* widget, const std::string& widgetName,
		const std::string& widgetPath, bool isRootLevel)
	{
		auto* expandableBox = new ImGuiWidget::ImExpandableBox(widgetName + "ExpandableBox");

		// 创建头部
		auto* headerWidget = CreateWidgetNodeHeader(widget, widgetName, widgetPath, isRootLevel, true);
		expandableBox->SetHead(headerWidget);

		// 创建身体：包含子控件
		auto* bodyContainer = new ImGuiWidget::ImVerticalBox(widgetName + "BodyContainer");

		// 缓存映射
		WidgetPath_To_Expander[widgetPath] = expandableBox;
		Expander_To_WidgetPath[expandableBox] = widgetPath;
		WidgetPath_To_Container[widgetPath] = bodyContainer;
		VariableName_To_Button[widgetName] = dynamic_cast<ImGuiWidget::ImButton*>(headerWidget->GetChildAt(0));

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
					bodyContainer->AddChildToVerticalBox(childNode)->SetIfAutoSize(false);
				}
			}
		}

		expandableBox->SetBody(bodyContainer);

		return expandableBox;
	}

	// 创建控件节点头部
	ImGuiWidget::ImWidget* CreateWidgetNodeHeader(ImGuiWidget::ImWidget* widget, const std::string& widgetName,
		const std::string& widgetPath, bool isRootLevel, bool expandable)
	{
		auto* headerContainer = new ImGuiWidget::ImHorizontalBox(widgetName + "HeaderContainer");
		auto* widgetButton = CreateWidgetItemButton(widget, widgetName, widgetPath, isRootLevel);
		headerContainer->AddChild(widgetButton);
		return headerContainer;
	}

	// 创建控件项按钮
	ImGuiWidget::ImButton* CreateWidgetItemButton(ImGuiWidget::ImWidget* widget, const std::string& widgetName,
		const std::string& widgetPath, bool isRootLevel)
	{
		auto* itemButton = new ImGuiWidget::ImButton(widgetName + "Button");
		auto* contentContainer = new ImGuiWidget::ImHorizontalBox(widgetName + "Content");

		auto* nameText = new ImGuiWidget::ImTextBlock(widgetName + "Name");
		nameText->SetText(widgetName);
		nameText->SetTextColor(m_TextColor);
		nameText->SetHorizontalAlignment(ImGuiWidget::ImTextBlock::TextAlignment_Horizontal::Left);

		auto* typeText = new ImGuiWidget::ImTextBlock(widgetName + "Type");
		typeText->SetText(" [" + widget->GetRegisterTypeName() + "]");
		typeText->SetTextColor(m_TypeTextColor);
		typeText->SetHorizontalAlignment(ImGuiWidget::ImTextBlock::TextAlignment_Horizontal::Left);

		contentContainer->AddChild(nameText)->SetIfAutoSize(false);
		contentContainer->AddChild(typeText)->SetIfAutoSize(false);

		// 子控件数量提示
		int childCount = widget->GetChildNum();
		ImGuiWidget::ImTextBlock* countText = nullptr;
		if (childCount > 0)
		{
			countText = new ImGuiWidget::ImTextBlock(widgetName + "Count");
			countText->SetText(" (" + std::to_string(childCount) + ")");
			countText->SetTextColor(IM_COL32(150, 150, 200, 255));
			countText->SetHorizontalAlignment(ImGuiWidget::ImTextBlock::TextAlignment_Horizontal::Left);
			contentContainer->AddChild(countText)->SetIfAutoSize(false);
		}

		itemButton->SetContent(contentContainer);
		ConfigureItemButton(itemButton, "Widget", widgetName, widget, widgetPath);

		// 缓存按钮引用
		VariableName_To_Button[widgetName] = itemButton;
		// 缓存内容容器和计数文本（用于更新子控件数量显示）
		WidgetName_To_ContentContainer[widgetName] = contentContainer;
		if (countText)
		{
			WidgetName_To_CountText[widgetName] = countText;
		}
		WidgetName_HadChildren[widgetName] = (childCount > 0);

		return itemButton;
	}

	// 创建简单的控件项（无子控件的情况）
	ImGuiWidget::ImWidget* CreateWidgetItem(ImGuiWidget::ImWidget* widget, const std::string& widgetName,
		const std::string& widgetPath, bool isRootLevel)
	{
		auto* itemContainer = new ImGuiWidget::ImHorizontalBox(widgetName + "ItemContainer");
		auto* widgetButton = CreateWidgetItemButton(widget, widgetName, widgetPath, isRootLevel);
		itemContainer->AddChild(widgetButton);
		return itemContainer;
	}

	// 配置项按钮样式和回调
	void ConfigureItemButton(ImGuiWidget::ImButton* button, const std::string& itemType,
		const std::string& itemName, void* dataPtr, const std::string& itemPath = "")
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
		selectionInfo.ItemName = itemPath.empty() ? itemName : itemPath;

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
			ImGuiWidget::ImWidgetClassVariable_Basic* var = static_cast<ImGuiWidget::ImWidgetClassVariable_Basic*>(dataPtr);
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

		// 使用完整路径作为key（对于控件）或变量名（对于变量）
		std::string itemPathKey = itemPath.empty() ? itemName : itemPath;
		ItemName_To_SelectionInfo[itemPathKey] = selectionInfo;

		button->SetOnPressed([this, itemPathKey]()
			{
				SelectItemByName(itemPathKey);
			});

		if (itemType == "Widget")
		{
			button->OnRightClicked.Add([this, itemType, itemName, dataPtr]()
				{
					ImVec2 mousePos = ImGuiWidget::GetMousePos();
					ImGuiWidget::ImWidget* widget = static_cast<ImGuiWidget::ImWidget*>(dataPtr);
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
		else if (itemType == "BasicVariable" || itemType == "ObjectVariable")
		{
			// 为变量添加右键菜单
			button->OnRightClicked.Add([this, itemType, itemName]()
				{
					ImVec2 mousePos = ImGuiWidget::GetMousePos();
					ShowVariableMenu(itemType, itemName, mousePos);
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

	// 外部设置选中item（通过变量名或控件路径）
	void SelectItemByName(const std::string& ItemPath, bool OutSideSet = false)
	{
		auto it = ItemName_To_SelectionInfo.find(m_CurrentSelection.VariableName);
		if (it != ItemName_To_SelectionInfo.end())
		{
			it->second.ItemButton->GetNormalStyle().BackgroundColor = m_NormalBgColor;
		}

		it = ItemName_To_SelectionInfo.find(ItemPath);
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
							m_ExpandedStateMap[ItemPath] = true;
						}
					}
					current = current->GetParents();
				}
			}
			else
			{
				// 设置标志，防止循环触发
				m_IsProcessingAction = true;
				Action_SelectItem(m_CurrentSelection);
				m_IsProcessingAction = false;
			}
		}
	}

	// 基本变量类型名称
	std::string GetBasicVariableTypeName(ImGuiWidget::PropertyType type) const
	{
		switch (type)
		{
		case ImGuiWidget::PropertyType::Int: return u8"整数";
		case ImGuiWidget::PropertyType::Float: return u8"浮点数";
		case ImGuiWidget::PropertyType::Bool: return u8"布尔值";
		case ImGuiWidget::PropertyType::String: return u8"字符串";
		case ImGuiWidget::PropertyType::Color: return u8"颜色";
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
			ImGuiWidget::ImButton* button = buttonIt->second;
			if (button && button->GetContent())
			{
				ImGuiWidget::ImHorizontalBox* contentContainer = dynamic_cast<ImGuiWidget::ImHorizontalBox*>(button->GetContent());
				if (contentContainer)
				{
					// 查找名称文本块
					for (int i = 0; i < contentContainer->GetChildNum(); i++)
					{
						ImWidget* child = contentContainer->GetChildAt(i);
						if (child->GetWidgetName().find("Name") != std::string::npos)
						{
							ImGuiWidget::ImTextBlock* nameText = dynamic_cast<ImGuiWidget::ImTextBlock*>(child);
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

		return true;
	}

	// 处理子控件添加
	bool HandleWidgetChildAdded(const OutlineViewChangeInfo& changeInfo)
	{
		if (!changeInfo.ParentWidget) return false;

		// 查找父控件的路径
		std::string parentPath = FindWidgetPath(changeInfo.ParentWidget);
		if (parentPath.empty()) return false;

		// 刷新父控件节点，RefreshWidgetTreeNode会处理展开框的创建
		RefreshWidgetTreeNode(changeInfo.ParentWidget, parentPath);

		// 自动展开父控件以显示新添加的子控件
		auto expanderIt = WidgetPath_To_Expander.find(parentPath);
		if (expanderIt != WidgetPath_To_Expander.end())
		{
			ImGuiWidget::ImExpandableBox* expander = expanderIt->second;
			if (expander)
			{
				// 展开控件并更新状态
				expander->SetExpandedState(true);
				// 状态会通过 SetOnExpandedStateChanged 回调自动保存到 m_ExpandedStateMap
			}
		}

		return true;
	}

	// 处理子控件删除
	bool HandleWidgetChildRemoved(const OutlineViewChangeInfo& changeInfo)
	{
		if (!changeInfo.ParentWidget) return false;

		// 查找父控件的路径
		std::string parentPath = FindWidgetPath(changeInfo.ParentWidget);
		if (parentPath.empty()) return false;

		// 刷新父控件节点，RefreshWidgetTreeNode会处理展开框到简单按钮项的转换
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
			ImGuiWidget::ImButton* button = buttonIt->second;
			if (button && button->GetContent())
			{
				ImGuiWidget::ImHorizontalBox* contentContainer = dynamic_cast<ImGuiWidget::ImHorizontalBox*>(button->GetContent());
				if (contentContainer)
				{
					// 查找名称文本块
					for (int i = 0; i < contentContainer->GetChildNum(); i++)
					{
						ImGuiWidget::ImWidget* child = contentContainer->GetChildAt(i);
						if (child->GetWidgetName().find("Name") != std::string::npos)
						{
							ImGuiWidget::ImTextBlock* nameText = dynamic_cast<ImGuiWidget::ImTextBlock*>(child);
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

				// 更新内容容器和计数文本的缓存映射
				auto contentContainerIt = WidgetName_To_ContentContainer.find(changeInfo.OldName);
				if (contentContainerIt != WidgetName_To_ContentContainer.end())
				{
					WidgetName_To_ContentContainer[changeInfo.NewName] = contentContainerIt->second;
					WidgetName_To_ContentContainer.erase(changeInfo.OldName);
				}

				auto countTextIt = WidgetName_To_CountText.find(changeInfo.OldName);
				if (countTextIt != WidgetName_To_CountText.end())
				{
					WidgetName_To_CountText[changeInfo.NewName] = countTextIt->second;
					WidgetName_To_CountText.erase(changeInfo.OldName);
				}

				auto hadChildrenIt = WidgetName_HadChildren.find(changeInfo.OldName);
				if (hadChildrenIt != WidgetName_HadChildren.end())
				{
					WidgetName_HadChildren[changeInfo.NewName] = hadChildrenIt->second;
					WidgetName_HadChildren.erase(changeInfo.OldName);
				}
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
	void RefreshWidgetTreeNode(ImGuiWidget::ImWidget* widget, const std::string& widgetPath)
	{
		// 查找对应的展开框
		auto expanderIt = WidgetPath_To_Expander.find(widgetPath);
		if (expanderIt != WidgetPath_To_Expander.end())
		{
			ImGuiWidget::ImExpandableBox* expander = expanderIt->second;
			auto containerIt = WidgetPath_To_Container.find(widgetPath);
			if (containerIt != WidgetPath_To_Container.end())
			{
				ImGuiWidget::ImVerticalBox* container = containerIt->second;

				// 检查是否有子控件
				int childCount = widget->GetChildNum();

				if (childCount == 0)
				{
					// 没有子控件，需要将展开框转换为简单按钮项
					// 通过刷新父节点来实现
					bool isRootWidget = IsRootWidget(widget->GetWidgetName());
					if (isRootWidget)
					{
						// 根控件：刷新整个控件树
						RefreshWidgetTreeContent();
						return;
					}
					else
					{
						// 非根控件：刷父节点
						ImWidget* parent = widget->GetParents();
						if (parent)
						{
							std::string ParentPath = FindWidgetPath(parent);
							RefreshWidgetTreeNode(parent, ParentPath);
						}
					}
				}
				else
				{
					// 有子控件，正常刷新
					// 保存当前展开状态
					bool wasExpanded = expander->GetIfExpanded();

					// 清空容器
					container->RemoveAllChild(true);

					// 重新添加子控件
					for (int i = 0; i < childCount; i++)
					{
						ImGuiWidget::ImWidget* child = widget->GetChildAt(i);
						if (child)
						{
							auto* childNode = CreateWidgetTreeNode(child, child->GetWidgetName(), widgetPath, false);
							if (childNode)
							{
								container->AddChildToVerticalBox(childNode)->SetIfAutoSize(false);
							}
						}
					}

					// 恢复展开状态
					expander->SetExpandedState(wasExpanded);
				}
			}
		}
		else
		{
			// 没有找到展开框，需要检查控件是否有子控件
			bool hasChildren = (widget->GetChildNum() > 0);
			bool hasExpander = (expanderIt != WidgetPath_To_Expander.end());

			// 如果控件有子控件但没有展开框，需要重新创建节点
			if (hasChildren && !hasExpander)
			{
				// 判断是否是根控件
				bool isRootWidget = IsRootWidget(widget->GetWidgetName());

				if (isRootWidget)
				{
					// 根控件：刷新整个控件树
					RefreshWidgetTreeContent();
					return;
				}
				else
				{
					// 非根控件：父控件刷新
					ImWidget* parent = widget->GetParents();
					if (parent)
					{
						std::string ParentPath = FindWidgetPath(parent);
						RefreshWidgetTreeNode(parent, ParentPath);
					}
				}
			}
			// 如果控件没有子控件，不需要展开框，保持现状即可
		}

		// 更新当前控件的子项数量显示
		UpdateWidgetChildCountDisplay(widget);

		// 更新父控件的子项数量显示
		ImWidget* parent = widget->GetParents();
		if (parent)
		{
			UpdateWidgetChildCountDisplay(parent);
		}
	}

	// 更新控件的子项数量显示
	void UpdateWidgetChildCountDisplay(ImGuiWidget::ImWidget* widget)
	{
		if (!widget) return;

		std::string widgetName = widget->GetWidgetName();
		int childCount = widget->GetChildNum();
		bool hasChildren = (childCount > 0);

		// 获取内容容器
		auto contentIt = WidgetName_To_ContentContainer.find(widgetName);
		if (contentIt == WidgetName_To_ContentContainer.end()) return;

		ImGuiWidget::ImHorizontalBox* contentContainer = contentIt->second;
		if (!contentContainer) return;

		// 检查之前是否有子控件
		auto hadChildrenIt = WidgetName_HadChildren.find(widgetName);
		bool hadChildren = (hadChildrenIt != WidgetName_HadChildren.end()) ? hadChildrenIt->second : false;

		// 如果子控件状态没有变化，只更新数量
		if (hasChildren == hadChildren)
		{
			// 只在有子控件时才更新计数文本
			if (hasChildren)
			{
				auto countTextIt = WidgetName_To_CountText.find(widgetName);
				if (countTextIt != WidgetName_To_CountText.end())
				{
					ImGuiWidget::ImTextBlock* countText = countTextIt->second;
					if (countText)
					{
						countText->SetText(" (" + std::to_string(childCount) + ")");
					}
				}
			}
		}
		else
		{
			// 子控件状态发生变化，需要重建按钮内容
			if (hasChildren && !hadChildren)
			{
				// 从无子控件变为有子控件，添加计数文本
				auto* countText = new ImGuiWidget::ImTextBlock(widgetName + "Count");
				countText->SetText(" (" + std::to_string(childCount) + ")");
				countText->SetTextColor(IM_COL32(150, 150, 200, 255));
				countText->SetHorizontalAlignment(ImGuiWidget::ImTextBlock::TextAlignment_Horizontal::Left);
				contentContainer->AddChild(countText)->SetIfAutoSize(false);
				WidgetName_To_CountText[widgetName] = countText;
			}
			else if (!hasChildren && hadChildren)
			{
				// 从有子控件变为无子控件，移除计数文本
				auto countTextIt = WidgetName_To_CountText.find(widgetName);
				if (countTextIt != WidgetName_To_CountText.end())
				{
					ImGuiWidget::ImTextBlock* countText = countTextIt->second;
					if (countText)
					{
						contentContainer->RemoveChild(countText);
						WidgetName_To_CountText.erase(widgetName);
					}
				}
			}

			// 更新状态记录
			WidgetName_HadChildren[widgetName] = hasChildren;
		}
	}

	// 查找控件的路径
	std::string FindWidgetPath(ImGuiWidget::ImWidget* widget)
	{
		if (!widget) return "";

		// 先检查是否是根控件
		std::string widgetName = widget->GetWidgetName();
		if (IsRootWidget(widgetName))
		{
			return widgetName;
		}

		// 递归查找父控件路径
		ImGuiWidget::ImWidget* parent = widget->GetParents();
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
		WidgetName_To_CountText.clear();
		WidgetName_To_ContentContainer.clear();
		WidgetName_HadChildren.clear();
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

		// 5. 创建控件子菜单
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

		// 7. 创建变量右键菜单
		m_PopupMenus.VariableMenuContent = BuildVariableMenuContent();
		m_PopupMenus.VariableMenu = windowManager->CreatePopupWindow(
			m_PopupMenus.VariableMenuContent->GetMinSize(),
			ImVec2(0, 0),
			m_PopupMenus.VariableMenuContent,
			false
		);
		m_PopupMenus.VariableMenu->Close();
	}

	// 新增：构建基本变量分区菜单内容
	ImGuiWidget::ImVerticalBox* BuildBasicVarsSectionMenuContent()
	{
		ImGuiWidget::ImVerticalBox* content = new ImGuiWidget::ImVerticalBox("BasicVarsMenuContent");

		// 创建各种类型的新建变量按钮
		std::vector<std::pair<std::string, ImGuiWidget::PropertyType>> varTypes = {
			{u8"新建整数", ImGuiWidget::PropertyType::Int},
			{u8"新建浮点数", ImGuiWidget::PropertyType::Float},
			{u8"新建布尔值", ImGuiWidget::PropertyType::Bool},
			{u8"新建字符串", ImGuiWidget::PropertyType::String},
			{u8"新建颜色", ImGuiWidget::PropertyType::Color}
		};

		for (const auto& varType : varTypes)
		{
			ImGuiWidget::ImButton* button = CreateMenuButton(varType.first);
			button->OnLeftClicked.Add([this, varType]()
				{
					OnCreateBasicVariableClicked(varType.second);
				});
			content->AddChildToVerticalBox(button)->SetIfAutoSize(false);
		}

		return content;
	}

	// 新增：构建Object变量分区菜单内容
	ImGuiWidget::ImVerticalBox* BuildObjectVarsSectionMenuContent()
	{
		ImGuiWidget::ImVerticalBox* content = new ImGuiWidget::ImVerticalBox("ObjectVarsMenuContent");

		ImGuiWidget::ImButton* button = CreateMenuButton(u8"新建ImObject");
		button->OnLeftClicked.Add([this]()
			{
				OnCreateObjectVariableClicked("test");//ImObject暂未建立工厂模式
			});
		content->AddChildToVerticalBox(button)->SetIfAutoSize(false);

		return content;
	}

	// 新增：构建控件树分区菜单内容
	ImGuiWidget::ImVerticalBox* BuildWidgetTreeSectionMenuContent()
	{
		ImGuiWidget::ImVerticalBox* content = new ImGuiWidget::ImVerticalBox("WidgetTreeMenuContent");

		ImGuiWidget::ImButton* newWidgetBtn = CreateMenuButton(u8"新建控件", true);

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
	ImGuiWidget::ImVerticalBox* BuildWidgetRootMenuContent()
	{
		ImGuiWidget::ImVerticalBox* content = new ImGuiWidget::ImVerticalBox("WidgetRootMenuContent");

		// 插入子项按钮
		ImGuiWidget::ImButton* insertChildBtn = CreateMenuButton(u8"插入子项", true);
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
		ImGuiWidget::ImButton* deleteBtn = CreateMenuButton(u8"删除控件树变量");
		deleteBtn->OnLeftClicked.Add([this]()
			{
				OnDeleteWidgetClicked();
			});

		content->AddChildToVerticalBox(insertChildBtn)->SetIfAutoSize(false);
		content->AddChildToVerticalBox(deleteBtn)->SetIfAutoSize(false);

		return content;
	}

	// 构建控件子菜单内容
	ImGuiWidget::ImVerticalBox* BuildWidgetChildMenuContent()
	{
		ImGuiWidget::ImVerticalBox* content = new ImGuiWidget::ImVerticalBox("WidgetChildMenuContent");

		// 复制按钮
		ImGuiWidget::ImButton* copyBtn = CreateMenuButton(u8"复制");
		copyBtn->OnLeftClicked.Add([this]()
			{
				m_PopupMenus.WidgetChildMenu->Close();
				// TODO: 实现复制功能
			});

		// 删除按钮
		ImGuiWidget::ImButton* deleteBtn = CreateMenuButton(u8"删除");
		deleteBtn->OnLeftClicked.Add([this]()
			{
				OnDeleteWidgetClicked();
			});

		// 在前一个位置插入按钮
		ImGuiWidget::ImButton* insertPreviousBtn = CreateMenuButton(u8"在前一个位置插入", true);
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
		ImGuiWidget::ImButton* insertToBtn = CreateMenuButton(u8"插入控件", true);
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
		ImGuiWidget::ImButton* insertAfterBtn = CreateMenuButton(u8"在后一个位置插入", true);
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
	ImGuiWidget::ImVerticalBox* BuildInsertWidgetMenuContent()
	{
		ImGuiWidget::ImVerticalBox* content = new ImGuiWidget::ImVerticalBox("InsertWidgetMenuContent");

		for (auto& widgetInfo : BasicWidgetList::GetBasicWidgetList())
		{
			ImGuiWidget::ImButton* button = CreateInsertWidgetButton(
				widgetInfo.CN_DisplayName,
				widgetInfo.RegisterName,
				IconManager::GetInstance()->GetIcon(widgetInfo.IconID)
			);
			content->AddChildToVerticalBox(button)->SetIfAutoSize(false);
		}

		return content;
	}

	// 新增：构建变量右键菜单内容
	ImGuiWidget::ImVerticalBox* BuildVariableMenuContent()
	{
		ImGuiWidget::ImVerticalBox* content = new ImGuiWidget::ImVerticalBox("VariableMenuContent");

		// 删除变量按钮
		ImGuiWidget::ImButton* deleteBtn = CreateMenuButton(u8"删除变量");
		deleteBtn->OnLeftClicked.Add([this]()
			{
				CloseActiveMenu();
				if (!m_PopupMenus.TargetVarName.empty())
				{
					Action_DeleteVariable(m_PopupMenus.TargetVarName);
				}
			});
		content->AddChildToVerticalBox(deleteBtn)->SetIfAutoSize(false);

		return content;
	}

	// 新增：创建菜单按钮
	ImGuiWidget::ImButton* CreateMenuButton(const std::string& text, bool hasSubMenu = false)
	{
		ImGuiWidget::ImButton* button = new ImGuiWidget::ImButton("MenuButton");
		button->bHaveBorder = false;

		ImGuiWidget::ImHorizontalBox* hbox = new ImGuiWidget::ImHorizontalBox("MenuHBox");
		ImGuiWidget::ImTextBlock* textBlock = new ImGuiWidget::ImTextBlock("MenuText");
		textBlock->SetText(text);
		textBlock->SetHorizontalAlignment(ImGuiWidget::ImTextBlock::TextAlignment_Horizontal::Left);

		auto slot = hbox->AddChildToHorizontalBox(textBlock);
		slot->PaddingLeft = 10;
		slot->PaddingRight = 16;
		slot->PaddingBottom = 2;
		slot->PaddingTop = 2;

		if (hasSubMenu)
		{
			ImGuiWidget::ImTextBlock* arrow = new ImGuiWidget::ImTextBlock("MenuArrow");
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
	ImGuiWidget::ImButton* CreateInsertWidgetButton(const std::string& cnName, const std::string& registerName, ImTextureID icon)
	{
		ImGuiWidget::ImButton* button = new ImGuiWidget::ImButton("InsertWidgetButton");
		button->bHaveBorder = false;

		ImGuiWidget::ImHorizontalBox* hbox = new ImGuiWidget::ImHorizontalBox("InsertWidgetHBox");
		ImGuiWidget::ImImage* iconImage = new ImGuiWidget::ImImage("WidgetIcon", icon, 24, 24);
		ImGuiWidget::ImTextBlock* text = new ImGuiWidget::ImTextBlock("WidgetText");
		text->SetText(cnName);
		text->SetHorizontalAlignment(ImGuiWidget::ImTextBlock::TextAlignment_Horizontal::Left);

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

	// 新增：显示变量右键菜单
	void ShowVariableMenu(const std::string& varType, const std::string& varName, const ImVec2& position)
	{
		m_PopupMenus.CurrentMode = PopupMenuSystem::MenuMode::Variable;
		m_PopupMenus.TargetVarType = varType;
		m_PopupMenus.TargetVarName = varName;

		m_PopupMenus.VariableMenu->SetPopupRect(position);
		m_PopupMenus.VariableMenu->SetActive();
	}

	void OnCreateBasicVariableClicked(const ImGuiWidget::PropertyType& type)
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

		if (m_PopupMenus.TargetVarName.empty()) return;

		if (m_PopupMenus.CurrentMode == PopupMenuSystem::MenuMode::WidgetRoot)
		{
			Action_DeleteVariable(m_PopupMenus.TargetVarName);
		}
		else if(m_PopupMenus.CurrentMode == PopupMenuSystem::MenuMode::WidgetChild)
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

		case PopupMenuSystem::MenuMode::Variable:
			m_PopupMenus.VariableMenu->Close();
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

		// 订阅控件选中事件（参数为控件路径或变量名）
		m_FileActions.push_back(AddSequentialProcessor(m_EditedFileFullPath + Action::WIDGET_SELECTED, [this](const std::string& widgetPath)
			{
				// 检查是否正在处理动作，避免循环触发
				if (!m_IsProcessingAction)
				{
					SelectItemByName(widgetPath, true);  // true 表示外部调用，不会再次发送事件
				}
			}));
	}

	// 内部Action处理函数
	void Action_SelectItem(const OutlineViewSelectionInfo& selectionInfo)
	{
		if (selectionInfo.VariableType == "Widget")
		{
			if (selectionInfo.IsRootWidget)
			{
				ExecuteAction(m_EditedFileFullPath + Action::OutlineView::SELECT_VARIABLE, selectionInfo.VariableName);
			}
			// 发布控件选中动作，使用单参数（控件路径）
			ExecuteAction(m_EditedFileFullPath + Action::WIDGET_SELECTED, selectionInfo.ItemName);
		}
		else
		{
			ExecuteAction(m_EditedFileFullPath + Action::OutlineView::SELECT_VARIABLE, selectionInfo.VariableName);
		}

	}

	void Action_CreateBasicVariable(const ImGuiWidget::PropertyType& typeName)
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

	void Action_DeleteVariable(const std::string& varName)
	{
		ExecuteAction(m_EditedFileFullPath + Action::OutlineView::DELETE_VARIABLE, varName);
	}

	void Action_InsertWidget(const std::string& OperatorVarName, ImGuiWidget::ImWidget* target, int insertIndex, const std::string& widgetRegisterName)
	{
		std::string targetPath = m_TargetClass->GetWidgetVariable(OperatorVarName)->BuildPathTo(target);
		ExecuteAction(m_EditedFileFullPath + Action::OutlineView::INSERT_WIDGET, OperatorVarName, targetPath, widgetRegisterName, insertIndex);
	}
	void Action_DeleteWidget(const std::string& widgetRootVarName, ImGuiWidget::ImWidget* target)
	{
		std::string targetPath = m_TargetClass->GetWidgetVariable(widgetRootVarName)->BuildPathTo(target);
		ExecuteAction(m_EditedFileFullPath + Action::OutlineView::DELETE_WIDGET, widgetRootVarName, targetPath);
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
			[this](const std::string& widgetTreeVarName, const std::string& parentWidgetPath, const std::string& childIdentifier)
			{
				// 获取根控件
				ImWidget* rootWidget = m_TargetClass->GetWidgetVariable(widgetTreeVarName);
				if (!rootWidget) return;

				// 找到父控件
				ImWidget* parentWidget = nullptr;
				if (parentWidgetPath.empty() || parentWidgetPath == ".")
				{
					parentWidget = rootWidget;
				}
				else
				{
					parentWidget = rootWidget->FindChildByPath(parentWidgetPath);
				}

				if (!parentWidget) return;

				// 找到添加的子控件
				ImWidget* childWidget = nullptr;
				int childCount = parentWidget->GetChildNum();

				// 先尝试精确匹配名称
				for (int i = 0; i < childCount; i++)
				{
					ImWidget* child = parentWidget->GetChildAt(i);
					if (child && child->GetWidgetName() == childIdentifier)
					{
						childWidget = child;
						break;
					}
				}

				// 如果精确匹配失败，尝试查找名称包含标识符的子控件
				if (!childWidget)
				{
					for (int i = 0; i < childCount; i++)
					{
						ImWidget* child = parentWidget->GetChildAt(i);
						if (child && child->GetWidgetName().find(childIdentifier) != std::string::npos)
						{
							childWidget = child;
							break;
						}
					}
				}

				// 如果还是找不到，选择最后一个子控件（假设是刚刚添加的）
				if (!childWidget && childCount > 0)
				{
					childWidget = parentWidget->GetChildAt(childCount - 1);
				}

				if (!childWidget) return;

				OutlineViewChangeInfo changeInfo(OutlineViewChangeType::WidgetChildAdded);
				changeInfo.ParentVarName = widgetTreeVarName;
				changeInfo.ParentWidget = parentWidget;
				changeInfo.ChangedWidget = childWidget;
				HandleWidgetChildAdded(changeInfo);
			}
		));

		m_FileEvents.push_back(Subscribe(m_EditedFileFullPath + Events::OutlineView::WIDGET_CHILD_REMOVED,
			[this](const std::string& widgetTreeVarName, const std::string& childWidgetPath)
			{
				// 获取根控件
				ImWidget* rootWidget = m_TargetClass->GetWidgetVariable(widgetTreeVarName);
				if (!rootWidget) return;

				// 解析路径，获取父路径和子控件名称
				std::string parentPath, childName;
				size_t lastSlash = childWidgetPath.find_last_of('/');
				if (lastSlash == std::string::npos)
				{
					// 没有斜杠，表示子控件是根控件的直接子控件
					parentPath = "";
					childName = childWidgetPath;
				}
				else
				{
					parentPath = childWidgetPath.substr(0, lastSlash);
					childName = childWidgetPath.substr(lastSlash + 1);
				}

				// 找到父控件
				ImWidget* parentWidget = nullptr;
				if (parentPath.empty())
				{
					parentWidget = rootWidget;
				}
				else
				{
					parentWidget = rootWidget->FindChildByPath(parentPath);
				}

				if (!parentWidget) return;

				OutlineViewChangeInfo changeInfo(OutlineViewChangeType::WidgetChildRemoved);
				changeInfo.ParentVarName = widgetTreeVarName;
				changeInfo.ParentWidget = parentWidget;
				// 注意：被删除的控件指针可能无效，这里我们只传递名称
				changeInfo.VariableName = childName;
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
