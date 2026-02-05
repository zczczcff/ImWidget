[根目录](../../CLAUDE.md) > **ImDesigner**

## ImDesigner - UI设计器

### 模块职责
ImDesigner是一个功能完整的UI设计器，支持可视化编辑、代码生成、命令撤销/重做等高级功能。采用事件驱动架构，通过事件总线实现各组件间的松耦合通信，提供项目管理、控件编辑、属性编辑等全方位的UI设计能力。

### 模块统计
- **头文件**: 61个
- **源文件**: 46个
- **代码行数**: 约48,000+行
- **主要语言**: C++17

---

## 架构设计

### 事件驱动架构图

```
┌─────────────────────────────────────────────────────────────┐
│                    事件驱动架构                              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌──────────────────────────────────────────────────────┐  │
│  │         ActionSystem<KeyStringType>                   │  │
│  │         (全局事件总线)                                 │  │
│  └──────────────────────────────────────────────────────┘  │
│                          │                                  │
│         ┌────────────────┼────────────────┐                │
│         ▼                ▼                ▼                │
│  ┌──────────────┐ ┌──────────────┐ ┌──────────────┐      │
│  │  Model层     │ │   View层     │ │  全局服务     │      │
│  │ (数据管理)    │ │  (UI组件)    │ │  (项目管理)   │      │
│  └──────────────┘ └──────────────┘ └──────────────┘      │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 组件继承关系图

```
EditorEventObject (事件对象基类)
    │
    ├─── Model_MainModel (主数据模型)
    │
    ├─── Model_UserWidgetClassEditor (编辑器模型)
    │
    ├─── MainUI : ImUserWidget (主界面)
    │
    ├─── UI_WidgetEditor : ImUserWidget (控件编辑器)
    │
    ├─── UI_DetailView : ImUserWidget (属性详情视图)
    │
    ├─── UI_ImUserWidgetClassOutlineView : ImUserWidget (大纲视图)
    │
    └─── UI_ProjectView : ImUserWidget (项目文件视图)


CommandBase<CommandDataType> (EditorKit)
    │
    └─── ImUserWidgetClassCommandBase
          │
          ├─── WidgetPropertyEditCommandBase<T>
          │     └─── 具体属性编辑命令
          │
          ├─── InsertChildByPathCommand
          │
          ├─── InsertChildByJsonCommand
          │
          ├─── RemoveChildByPathCommand
          │
          └─── 其他命令类型
```

---

## 命令系统完整文档

### 命令系统架构

```
┌─────────────────────────────────────────────────────────────┐
│                      命令系统架构                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌──────────────────────────────────────────────────────┐  │
│  │        ImDesingnerCommandManager                      │  │
│  │        (命令管理器)                                    │  │
│  └───────────────────┬──────────────────────────────────┘  │
│                      │                                      │
│         ┌────────────┴────────────┐                        │
│         ▼                         ▼                        │
│  ┌─────────────┐          ┌─────────────┐                 │
│  │  Command    │          │   Command   │                 │
│  │  History    │          │   Stack     │                 │
│  └─────────────┘          └─────────────┘                 │
│         │                         │                         │
│         └────────────┬────────────┘                        │
│                      ▼                                      │
│  ┌──────────────────────────────────────────────────┐     │
│  │           ImUserWidgetClassCommandBase            │     │
│  │           (命令基类)                               │     │
│  └──────────────────────────────────────────────────┘     │
│                      │                                      │
│     ┌────────────────┼────────────────┐                   │
│     ▼                ▼                ▼                   │
│ ┌────────┐      ┌─────────┐     ┌──────────┐             │
│ │ Widget │      │Variable │     │ Property │             │
│ │ Command│      │ Command │     │ Command  │             │
│ └────────┘      └─────────┘     └──────────┘             │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 核心命令类

#### ImUserWidgetClassCommandBase - 命令基类

**文件**: `Include/Model/Command/ImDesignerCommandBase.h`

所有编辑命令的基类，继承自EditorKit的CommandBase。

```cpp
class ImUserWidgetClassCommandBase : public CommandBase<CommandDataType>
{
protected:
    // 执行命令
    virtual ActionResult Execute(CommandDataType& data) override;

    // 撤销命令
    virtual ActionResult Undo(CommandDataType& data) override;

    // 重做命令
    virtual ActionResult Redo(CommandDataType& data) override;

    // 命令合并（用于连续编辑同一属性）
    virtual bool CanMerge(const CommandBase* other) const override;
    virtual void Merge(const CommandBase* other) override;
};
```

#### WidgetPropertyEditCommandBase - 控件属性编辑命令

**文件**: `Include/Model/Command/Command_WidgetVarPropertyEdit.h`

模板化的属性编辑命令，支持命令合并。

```cpp
template<typename T, PropertyType Type>
class WidgetPropertyEditCommandBase : public ImUserWidgetClassCommandBase
{
protected:
    std::string m_WidgetPath;     // 控件路径
    std::string m_PropertyName;   // 属性名
    T m_OldValue;                 // 旧值
    T m_NewValue;                 // 新值

public:
    WidgetPropertyEditCommandBase(
        const std::string& widgetPath,
        const std::string& propertyName,
        const T& oldValue,
        const T& newValue
    );
};
```

#### 子控件操作命令

**文件**: `Include/Model/Command/Command_ChildWidgetOperation.h`

##### InsertChildByPathCommand - 插入子控件

```cpp
class InsertChildByPathCommand : public ImUserWidgetClassCommandBase
{
private:
    std::string m_ParentPath;       // 父控件路径
    int m_InsertIndex;              // 插入位置
    std::string m_ChildWidgetJson;  // 子控件JSON

public:
    InsertChildByPathCommand(
        const std::string& parentPath,
        int insertIndex,
        const std::string& childWidgetJson
    );
};
```

##### InsertChildByJsonCommand - 通过JSON插入

```cpp
class InsertChildByJsonCommand : public ImUserWidgetClassCommandBase
{
private:
    std::string m_ParentPath;
    std::string m_JsonPath;        // JSON中的路径
    std::string m_ChildWidgetJson;

public:
    InsertChildByJsonCommand(
        const std::string& parentPath,
        const std::string& jsonPath,
        const std::string& childWidgetJson
    );
};
```

##### RemoveChildByPathCommand - 删除子控件

```cpp
class RemoveChildByPathCommand : public ImUserWidgetClassCommandBase
{
private:
    std::string m_WidgetPath;
    int m_ChildIndex;
    std::string m_RemovedWidgetJson;  // 删除的控件JSON

public:
    RemoveChildByPathCommand(
        const std::string& widgetPath,
        int childIndex
    );
};
```

#### 变量操作命令

**文件**: `Include/Model/Command/Command_VariableOperation.h`

```cpp
// 添加变量命令
class AddVariableCommand : public ImUserWidgetClassCommandBase;

// 删除变量命令
class RemoveVariableCommand : public ImUserWidgetClassCommandBase;

// 重命名变量命令
class RenameVariableCommand : public ImUserWidgetClassCommandBase;
```

#### 其他命令类型

| 命令类 | 文件 | 功能 |
|--------|------|------|
| `Command_WidgetSlotPropertyEdit` | Command_WidgetSlotPropertyEdit.h | 槽属性编辑 |
| `Command_ObjectVarPropertyEdit` | Command_ObjectVarPropertyEdit.h | Object变量编辑 |
| `Command_ClassStringPropertyEdit` | Command_ClassStringPropertyEdit.h | 类字符串编辑 |
| `Command_VarRename` | Command_VarRename.h | 变量重命名 |

---

## 事件系统完整文档

### 事件系统架构

基于EditorKit框架的ActionSystem实现事件驱动架构。

```
┌─────────────────────────────────────────────────────────────┐
│                      事件系统架构                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌──────────────────────────────────────────────────────┐  │
│  │         ActionSystem<KeyStringType>                   │  │
│  │         (全局动作系统)                                 │  │
│  └───────────────────┬──────────────────────────────────┘  │
│                      │                                      │
│         ┌────────────┴────────────┐                        │
│         ▼                         ▼                        │
│  ┌──────────────┐          ┌──────────────┐              │
│  │   Validators │          │  Processors  │              │
│  │   (验证器)    │          │  (处理器)     │              │
│  └──────────────┘          └──────────────┘              │
│         │                         │                         │
│         └────────────┬────────────┘                        │
│                      ▼                                      │
│           ┌──────────────────────┐                        │
│           │  EditorEventObject   │                        │
│           │  (事件对象基类)       │                        │
│           └──────────────────────┘                        │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### EditorEventObject - 事件对象基类

**文件**: `Include/EditorEventObject.h`

所有Model和UI类的基类，提供事件发布/订阅功能。

```cpp
class EditorEventObject
{
protected:
    ActionSystem<KeyStringType>* m_ActionSystem;

public:
    // 发布事件
    template<typename... Args>
    ActionResult Publish(const KeyStringType& eventName, Args&&... args);

    // 订阅事件
    template<typename Callable>
    EditorEventID Subscribe(const KeyStringType& eventName, Callable&& handler);

    // 取消订阅
    void Unsubscribe(const EditorEventID& id);

    // 添加顺序处理器
    template<typename Callable>
    ActionHandle<KeyStringType> AddSequentialProcessor(
        const KeyStringType& eventName,
        Callable&& handler
    );

    // 添加验证器
    template<typename Callable>
    ActionHandle<KeyStringType> AddValidator(
        const KeyStringType& eventName,
        Callable&& handler
    );
};
```

### 动作定义

**文件**: `Include/EditorAction.h`

```cpp
namespace Action {
    // 主界面动作
    namespace MainUI {
        ACTION(OpenFile);           // 打开文件
        ACTION(SaveFile);           // 保存文件
        ACTION(CloseFile);          // 关闭文件
        ACTION(GenerateCode);       // 生成代码
    }

    // 项目视图动作
    namespace ProjectView {
        ACTION(CreateFile);         // 创建文件
        ACTION(DeleteFile);         // 删除文件
        ACTION(RenameFile);         // 重命名文件
        ACTION(RefreshTree);        // 刷新文件树
    }

    // 大纲视图动作
    namespace OutlineView {
        ACTION(SelectWidget);       // 选择控件
        ACTION(ExpandSection);      // 展开分区
        ACTION(CollapseSection);    // 折叠分区
        ACTION(RefreshView);        // 刷新视图
    }

    // 详情视图动作
    namespace DetailView {
        ACTION(EditProperty);       // 编辑属性
        ACTION(RefreshProperties);  // 刷新属性
    }

    // 控件选中动作（双参数签名）
    ACTION(WIDGET_SELECTED);        // (widgetVarName, widget*)
}
```

### 事件定义

**文件**: `Include/EditorEvents.h`

```cpp
namespace Events {
    // 主界面事件
    namespace MainUI {
        EVENT(FileOpened);          // 文件已打开
        EVENT(FileSaved);           // 文件已保存
        EVENT(FileClosed);          // 文件已关闭
    }

    // 项目视图事件
    namespace ProjectView {
        EVENT(FileSelected);        // 文件被选中
        EVENT(FileCreated);         // 文件已创建
        EVENT(FileDeleted);         // 文件已删除
    }

    // 大纲视图事件
    namespace OutlineView {
        EVENT(WidgetSelected);      // 控件被选中
        EVENT(SectionExpanded);     // 分区已展开
        EVENT(SectionCollapsed);    // 分区已折叠
    }
}
```

---

## 核心类详细文档

### Model_MainModel - 主数据模型

**文件**: `Include/Model/Model_MainModel.h`

管理项目配置、文件编辑状态的全局数据模型。

```cpp
class Model_MainModel : public EditorEventObject
{
public:
    // 编辑的UI文件结构
    struct EditedUIFile {
        std::string FileFullPath;               // 文件完整路径
        ImUserWidgetClass* EditedFile;         // 编辑的文件对象
        Model_ImUserWidgetClassEditor* model_editor;  // 编辑器模型
    };

    // 项目配置
    Projectconfig* GetProjectConfig();

    // 文件管理
    EditedUIFile* GetEditedUIFile(const std::string& filePath);
    void AddEditedUIFile(const std::string& filePath, ImUserWidgetClass* file);
    void RemoveEditedUIFile(const std::string& filePath);

    // 当前选中的文件
    EditedUIFile* GetCurrentEditedFile();
    void SetCurrentEditedFile(const std::string& filePath);
};
```

### Model_UserWidgetClassEditor - 编辑器模型

**文件**: `Include/Model/Model_UserWidgetClassEditor.h`

管理单个UI文件的编辑状态、命令历史。

```cpp
class Model_UserWidgetClassEditor : public EditorEventObject
{
public:
    // 获取编辑的文件
    ImUserWidgetClass* GetEditedFile();

    // 命令管理
    ImDesingnerCommandManager* GetCommandManager();

    // 执行命令
    bool ExecuteCommand(ImUserWidgetClassCommandBase* command);

    // 撤销/重做
    bool Undo();
    bool Redo();

    // 选择状态
    ImWidget* GetSelectedWidget();
    void SetSelectedWidget(ImWidget* widget);

    // 变量管理
    std::vector<ImObjectVar*>& GetVariables();
    void AddVariable(ImObjectVar* var);
    void RemoveVariable(const std::string& varName);
};
```

### MainUI - 主界面

**文件**: `Include/UI/MainUI.h`

主界面容器，管理多个编辑器页面。

```cpp
class MainUI : public ImUserWidget
{
public:
    // 页面管理
    UI_WidgetEditor* GetWidgetEditor(const std::string& filePath);
    void OpenWidgetEditor(const std::string& filePath, ImUserWidgetClass* file);
    void CloseWidgetEditor(const std::string& filePath);

    // 获取子视图
    UI_ProjectView* GetProjectView();
    UI_DetailView* GetDetailView();

    // 菜单动作
    void OnNewFile();
    void OnSaveFile();
    void OnGenerateCode();
};
```

### UI_WidgetEditor - 控件编辑器

**文件**: `Include/UI/UI_WidgetEditor.h`

可视化控件编辑器，支持鼠标点击选择、虚线框高亮显示、快捷键撤销等功能。

```cpp
class UI_WidgetEditor : public ImUserWidget, public EditorEventObject
{
public:
    // 设置选中的控件
    bool SetSelectedWidget(ImGuiWidget::ImWidget* widget);
    bool SetSelectedWidget(const std::string& widgetVarName, ImGuiWidget::ImWidget* widget);

    // 设置当前编辑的控件树
    void SetEditingWidgetTree(const std::string& widgetVarName);
    std::string GetCurrentEditingWidgetVarName() const;

    // 获取/设置目标类
    ImGuiWidget::ImUserWidgetClass* GetTargetClass() const;
    void SetTargetClass(ImGuiWidget::ImUserWidgetClass* targetClass);

private:
    // 鼠标事件处理
    virtual void OnMouseDown(ImGuiWidget::ImMouseDownEvent& e) override;
    virtual void OnKeyDown(ImGuiWidget::ImKeyDownEvent& e) override;

    // 绘制动态虚线框
    void DrawAnimatedDashedRect(
        const ImVec2& min, const ImVec2& max,
        ImU32 color, float thickness,
        float dashLen, float gapLen,
        float& offset, float speed = 1.0f
    );
};
```

**动作订阅**:
- `EditFileFullPath + Action::WIDGET_SELECTED`: 控件选中动作，双参数签名 `(widgetVarName, widget*)`

**动作发布**:
- 鼠标点击控件时发布 `Action::WIDGET_SELECTED`

### UI_ImUserWidgetClassOutlineView - 大纲视图

**文件**: `Include/UI/UI_ImUserWidgetClassOutlineView.h`

显示变量、控件树的分区视图（1943行超大文件）。

```cpp
class UI_ImUserWidgetClassOutlineView : public ImUserWidget
{
public:
    // 分区定义
    enum SectionType {
        Section_BasicVariables,    // 基本变量分区
        Section_ObjectVariables,   // Object变量分区
        Section_WidgetTree,        // 控件树分区
    };

    // 刷新视图
    void RefreshView();
    void RefreshSection(SectionType section);

    // 展开/折叠
    void ExpandSection(SectionType section);
    void CollapseSection(SectionType section);

    // 选择项
    void SelectWidget(ImWidget* widget);
    void SelectVariable(ImObjectVar* var);
};
```

**动作订阅**:
- `m_EditedFileFullPath + Action::WIDGET_SELECTED`: 控件选中动作

**动作发布**:
- 选中控件时发布 `Action::WIDGET_SELECTED`

**缓存映射**:
- `WidgetPointer_To_ItemName`: 控件指针到项目名的映射，用于响应来自其他视图的选中动作

### UI_DetailView - 属性详情视图

**文件**: `Include/UI/UI_DetailView.h`

基于ROP属性系统的动态属性编辑器。

```cpp
class UI_DetailView : public ImUserWidget
{
public:
    // 设置编辑目标
    void SetTarget(ImObject* obj);
    void SetWidget(ImWidget* widget);
    void SetVariable(ImObjectVar* var);

    // 刷新属性显示
    void RefreshProperties();

    // 属性编辑回调
    void OnPropertyEdited(const std::string& propName, const PropertyValue& newValue);
};
```

**动作订阅**:
- `EditedFileFullPath + Action::WIDGET_SELECTED`: 控件选中动作

---

## 代码示例

### 创建自定义命令

```cpp
// 自定义命令：修改控件尺寸
class ChangeSizeCommand : public ImUserWidgetClassCommandBase
{
private:
    std::string m_WidgetPath;
    ImVec2 m_OldSize;
    ImVec2 m_NewSize;

public:
    ChangeSizeCommand(
        const std::string& widgetPath,
        const ImVec2& oldSize,
        const ImVec2& newSize
    ) : m_WidgetPath(widgetPath), m_OldSize(oldSize), m_NewSize(newSize) {}

    virtual ActionResult Execute(CommandDataType& data) override
    {
        ImWidget* widget = GetWidgetByPath(data, m_WidgetPath);
        if (!widget) return ActionResult::Failure;
        widget->SetSize(m_NewSize);
        return ActionResult::Success;
    }

    virtual ActionResult Undo(CommandDataType& data) override
    {
        ImWidget* widget = GetWidgetByPath(data, m_WidgetPath);
        if (!widget) return ActionResult::Failure;
        widget->SetSize(m_OldSize);
        return ActionResult::Success;
    }
};

// 使用命令
auto cmd = new ChangeSizeCommand("Root/Button1", oldSize, newSize);
model->ExecuteCommand(cmd);
```

### 发布和订阅事件

```cpp
// 发布事件
Publish(Action::MainUI::OpenFile, "example.uix");

// 订阅事件
EditorEventID id = Subscribe(
    Events::MainUI::FileOpened,
    [this](const std::string& filePath) {
        OnFileOpened(filePath);
    }
);

// 取消订阅
Unsubscribe(id);
```

### 控件选中同步

```cpp
// 视图A：发布控件选中动作
void OnWidgetClicked(ImWidget* widget)
{
    ExecuteAction(m_FilePath + Action::WIDGET_SELECTED, m_WidgetVarName, widget);
}

// 视图B：订阅控件选中动作
void ResetAction()
{
    AddSequentialProcessor(m_FilePath + Action::WIDGET_SELECTED,
        [this](const std::string& widgetVarName, ImWidget* selectedWidget)
        {
            SetSelectedWidget(widgetVarName, selectedWidget);
        });
}
```

### 创建自定义视图

```cpp
class MyCustomView : public ImUserWidget, public EditorEventObject
{
protected:
    virtual void Render() override
    {
        // 渲染UI
        if (ImGui::Button("Click Me"))
        {
            Publish(Action::MyView::ButtonClicked);
        }
    }

public:
    MyCustomView(const std::string& name) : ImUserWidget(name)
    {
        // 订阅事件
        Subscribe(Events::MainUI::FileOpened,
            [this](const std::string& path) {
                HandleFileOpened(path);
            });
    }
};
```

---

## 入口与启动

### 入口文件
- **主入口**: `Source/ImDesigner_main.cpp` - 应用程序入口
- **主界面**: `Include/UI/MainUI.h` - 主界面类

### 全局实例
```cpp
extern MainUI* global_MainUI;                    // 主界面
extern Model_MainModel* global_MainModel;        // 主数据模型
```

### 初始化流程

```cpp
// ImDesigner_main.cpp
ImGuiWidget::ImWidget* ImInit()
{
    // 1. 注册基础控件
    ImGuiWidget::RegisterBaseWidget();

    // 2. 创建全局组件
    global_MainModel = new Model_MainModel();
    global_MainUI = new MainUI("ImDesigner_MainUI");

    // 3. 事件总线已通过 EditorEventObject 基类自动连接

    return global_MainUI;
}

void ImTick()
{
    global_MainModel->Tick();
}
```

---

## 关键依赖与配置

### 依赖模块
- **ImWidget**: 核心UI库依赖
- **EditorKit**: 编辑器工具包依赖

### 平台支持
- Windows 64-bit: DirectX 11后端
- Linux ARM: OpenGL ES 2.0后端

### 编译配置
- CMake构建系统，支持跨平台编译
- 配置文件: ProjectConfig.json - 项目配置文件

---

## 关键设计模式

### 1. Command Pattern（命令模式）
- **应用**: 撤销/重做系统
- **优势**: 操作封装为对象，支持命令队列，易于扩展

### 2. Observer Pattern（观察者模式）
- **应用**: 事件总线系统
- **优势**: 松耦合通信，一对多通知

### 3. Template Method Pattern（模板方法模式）
- **应用**: 命令基类
- **优势**: 统一执行流程，子类定制细节

### 4. Event Bus Pattern（事件总线模式）
- **应用**: 全局组件通信
- **优势**: 完全解耦Model和View，无需Controller层

---

## 工具层

### JLog / JAsyncLog - 日志工具

**文件**: `Include/Tools/JLog.h`, `Include/Tools/JAsyncLog.h`

```cpp
// 同步日志
JLog::Log("Message");
JLog::Log("Formatted: %s", message);

// 异步日志
JAsyncLog::Log("Async message");
JAsyncLog::Flush();  // 刷新异步日志
```

### ProjectFileManager - 项目文件管理器

**文件**: `Include/Tools/ProjectFileManager.h`

```cpp
// 文件操作
bool CreateFile(const std::string& path);
bool DeleteFile(const std::string& path);
bool RenameFile(const std::string& oldPath, const std::string& newPath);

// 项目操作
bool LoadProject(const std::string& projectPath);
bool SaveProject();
```

---

## 常见问题 (FAQ)

### Q1: 如何撤销编辑操作？
**A**: 点击界面上的撤销按钮或使用快捷键Ctrl+Z，命令系统会自动回滚操作。

### Q2: 如何创建自定义命令？
**A**: 继承`ImUserWidgetClassCommandBase`，实现Execute、Undo、Redo方法。

### Q3: 如何订阅编辑器事件？
**A**: 使用`EditorEventObject`的Subscribe方法订阅感兴趣的事件。

### Q4: 如何生成代码？
**A**: 使用主界面的生成按钮，系统会根据当前编辑的UI文件生成对应的C++代码。

### Q5: 为什么没有Controller层？
**A**: ImDesigner采用事件驱动架构，所有Model和View之间的通信都通过全局事件总线实现，无需中间的Controller层，实现了更松耦合的设计。

---

## 文件清单

### 视图层 (UI/)
```
Include/UI/
├── MainUI.h                           # 主界面
├── UI_ProjectView.h                   # 项目文件视图
├── UI_WidgetEditor.h                   # 控件编辑器（可视化编辑）
├── UI_DetailView.h                     # 属性详情视图
├── UI_ImUserWidgetClassOutlineView.h  # 用户控件类大纲视图
├── EditorViewBase.h                    # 编辑器基类
├── IconManager.h                       # 图标管理器
├── Widget_PageTag.h                    # 页面标签
├── DragObject_ExampleWidget.h          # 拖拽对象示例
└── Widget_ExampleWidgetButton.h        # 示例按钮控件
```

### 模型层 (Model/)
```
Include/Model/
├── Model_MainModel.h                   # 主数据模型
├── Model_UserWidgetClassEditor.h       # 用户控件类编辑模型
├── Model_UIFile.h                       # UI文件模型
├── Projectconfig.h                      # 项目配置
└── Command/                             # 命令系统
    ├── ImDesignerCommandBase.h          # 命令基类
    ├── ImDesignerCommandManager.h       # 命令管理器
    ├── ImDesignerCommandData.h          # 命令数据
    ├── ImDesignerCommandType.h          # 命令类型
    ├── Command_WidgetVarPropertyEdit.h  # 控件属性编辑
    ├── Command_WidgetSlotPropertyEdit.h # 槽属性编辑
    ├── Command_ObjectVarPropertyEdit.h  # 对象属性编辑
    ├── Command_ClassStringPropertyEdit.h # 类字符串编辑
    ├── Command_VarRename.h               # 变量重命名
    ├── Command_ChildWidgetOperation.h   # 子控件操作
    └── Command_VariableOperation.h      # 变量操作
```

### 资源层 (Resource/)
```
Include/Resource/
├── Button.h/.cpp                        # 按钮资源
├── TextBlock.h/.cpp                     # 文本块资源
├── Slider.h/.cpp                        # 滑块资源
├── CheckBox.h/.cpp                       # 复选框资源
├── ComboBox.h/.cpp                       # 下拉框资源
├── InputText.h/.cpp                      # 输入框资源
├── HorizontalBox.h/.cpp                  # 水平容器资源
├── VerticalBox.h/.cpp                    # 垂直容器资源
├── CanvasPanel.h/.cpp                    # 画布资源
├── ScrollBox.h/.cpp                      # 滚动容器资源
└── ... (30+ 控件资源)
```

### 工具层 (Tools/)
```
Include/Tools/
├── JLog.h/.cpp                          # 日志工具
├── JAsyncLog.h/.cpp                      # 异步日志工具
└── ProjectFileManager.h                  # 项目文件管理器
```

### 全局系统
```
Include/
├── EditorGlobal.h                        # 编辑器全局变量
├── EditorGlobalInterface.h               # 编辑器全局接口
├── EditorAction.h                         # 动作定义
├── EditorEvents.h                         # 事件定义
├── EditorEventObject.h                    # 事件对象基类
└── EditorEventInterface.h                 # 事件接口
```

---

## 变更记录 (Changelog)

### 2026-02-05 v2.3.0
- 弃用 UI_UserWidgetClassView（已删除文件）
- 更新模块统计：头文件61个，源文件46个
- 更新文件清单

### 2026-02-05 v2.2.0
- 移除 Controller 层，完全采用事件驱动架构
- 弃用 UI_WidgetTreeView（已删除文件）
- 统一控件选中动作为双参数签名 `(widgetVarName, widget*)`
- 新增 UI_WidgetEditor 核心类文档
- 更新架构设计图，移除Controller层和TreeView组件
- 新增事件总线模式说明

### 2026-02-05 v2.0.0
- 完成深度模块扫描和架构分析
- 新增完整的命令系统文档
- 新增事件系统详细说明
- 新增核心类API文档
- 新增代码示例和使用指南
- 新增设计模式说明
- 新增完整的文件清单
- 文档覆盖率提升至95%+

### 2024-02-05 v1.2.0
- 首次AI上下文初始化
- 识别MVC架构和命令系统
- 建立完整的文件清单和功能描述
