# ImWidget 控件库

ImWidget 是一个基于 Dear ImGui 的高级 C++ UI 控件库，提供完整的控件体系、事件系统、窗口管理和可视化编辑支持。本库是 ImDesigner 可视化设计器的基础控件库。

## 项目结构

```
ImWidget/
├── Include/                 # 头文件
│   ├── Application/        # 应用程序框架
│   ├── ImComboWidget/      # 组合控件
│   ├── ImEvent/            # 事件系统
│   ├── ImTools/            # 工具类
│   ├── ImWidget/           # 控件库核心（40+控件）
│   └── ImWindows/          # 窗口管理
├── Source/                 # 源文件实现
├── Extern/                 # 外部依赖
│   ├── imgui-master/       # ImGui核心库
│   ├── nlohmann_json/      # JSON序列化
│   └── stb_image/          # 图像处理
└── build/                  # 构建目录
```

## 1. 控件系统

### 1.1 继承体系

控件系统采用多级继承结构：

```
ImObject (属性系统基类)
    ↓
ImWidget (控件基类)
    ↓
ImPanelWidget (容器控件基类)
    ↓
具体控件类 (ImButton, ImTextBlock, ImCanvasPanel等)
```

### 1.2 ImObject - 属性系统基类

`ImObject` 是所有控件的基类，集成了运行时属性系统（ROP - Runtime Object Property）。提供类型安全的属性访问和管理功能。

**核心特性：**
- 支持多种属性类型：Color, Float, Bool, Int, String, Vec2, Struct, StringArray, Enum
- 路径式属性访问（如 `"child/subchild/property"`）
- 类型安全的 getter/setter 模板方法

**关键方法：**
```cpp
template<typename T>
T* GetPropertyPtr(const std::string& name);  // 获取属性指针

template<typename T>
bool SetPropertyValue(const std::string& name, const T& value);  // 设置属性值

ImProperty GetPathProperty(const std::string& path);  // 通过路径获取属性
```

### 1.3 ImWidget - 控件基类

`ImWidget` 是所有 UI 控件的抽象基类，提供位置、大小、可见性、父子关系等基础功能。

**核心成员：**
- `Position`, `Size` - 位置和尺寸
- `bVisible` - 可见性
- `bFocusable`, `bHasFocus` - 焦点控制
- `bHoverable`, `bHovered` - 悬停状态
- `m_Parents` - 父控件指针
- `m_Slot` - 所属的布局槽

**关键方法：**
```cpp
virtual void Render();  // 渲染控件
virtual ImWidget* ChildHitTest(ImVec2 Pos);  // 命中测试
virtual void HandleEvent(ImEvent* event);  // 事件处理
virtual ImWidget* CopyWidget();  // 拷贝控件

// 父子关系管理
virtual ImSlot* AddChild(ImWidget* child, ImVec2 RelativePosition = ImVec2(FLT_MIN, FLT_MIN));
virtual bool RemoveChild(ImWidget* child, bool bDeleteOld = false);
virtual int GetChildNum();
```

### 1.4 ImPanelWidget - 容器控件基类

`ImPanelWidget` 是支持子控件的容器基类，管理子控件的布局和渲染。

**核心特性：**
- 子控件通过 `ImSlot` 进行管理
- 自动布局标记（`bLayOutDirty`）
- 背景和边框渲染支持

**关键方法：**
```cpp
virtual ImSlot* CreateSlot(ImWidget* Content) = 0;  // 创建布局槽
virtual void Relayout();  // 重新布局
void RenderChild();  // 渲染所有子控件
```

## 2. 事件系统

事件系统提供完整的输入事件处理机制，支持冒泡、捕获和事件分发。

### 2.1 ImEvent - 事件基类

`ImEvent` 是所有事件的基类，提供事件类型、阶段、目标等通用信息。

**事件类型（ImEventType）：**
- 鼠标事件：`MouseDown`, `MouseUp`, `MouseMove`, `MouseClick`, `MouseDoubleClick`
- 悬停事件：`HoverIn`, `Hover`, `HoverOut`
- 键盘事件：`KeyDown`, `KeyUp`, `KeyPress`
- 焦点事件：`FocusIn`, `FocusOut`
- 拖拽事件：`DragStart`, `DragUpdate`, `DragEnd`, `DragEnter`, `DragOver`, `DragLeave`, `Drop`

**事件阶段（ImEventPhase）：**
- `Capture` - 捕获阶段（从根到目标）
- `Target` - 目标阶段
- `Bubble` - 冒泡阶段（从目标到根）

### 2.2 ImEventSystem - 事件系统

`ImEventSystem` 管理事件的收集、分发和处理，维护焦点和悬停状态。

**核心功能：**
- 从 ImGui IO 收集鼠标、键盘、拖拽事件
- 双点击检测和悬停时间计算
- 焦点管理（Tab 键导航）
- 拖拽操作支持

**关键方法：**
```cpp
void CollectMouseEvent();  // 收集鼠标事件
void CollectKeyEvent();    // 收集键盘事件
void DispatchEvents();     // 分发事件队列
void SetFocus(ImWidget* widget);  // 设置焦点控件
```

### 2.3 事件处理流程

1. **事件收集**：每帧从 ImGui 收集原始输入事件
2. **事件创建**：创建对应类型的 `ImEvent` 对象
3. **命中测试**：通过 `HitTest()` 确定事件目标
4. **事件分发**：按照捕获→目标→冒泡的顺序分发事件
5. **事件处理**：控件重写 `HandleEventInternal()` 处理事件

## 3. 窗口系统

### 3.1 ImWindow - 窗口类

`ImWindow` 表示一个可渲染的窗口，包含根控件和窗口属性。

**窗口属性：**
- `m_title` - 窗口标题
- `m_size`, `m_position` - 窗口尺寸和位置
- `bIsOpen`, `bIsActive` - 打开和激活状态
- `bIsMovable`, `bIsResizable` - 可移动和可调整大小
- `bHasTitleBar`, `bHasBackground` - 标题栏和背景
- `bIsPopup`, `bIsSubMenu` - 弹出窗口和子菜单

**窗口关系：**
- `m_parentWindow` - 父窗口
- `m_childWindows` - 子窗口列表
- 支持窗口层级和递归关闭

**关键方法：**
```cpp
void Render();  // 渲染窗口及其根控件
void Close();   // 关闭窗口
void Open();    // 打开窗口
bool ContainsPoint(const ImVec2& point) const;  // 判断点是否在窗口内
```

### 3.2 窗口管理

窗口管理器（`ImWindowManager`）负责窗口的生命周期、渲染顺序和事件分发。

## 4. 基本控件类

### 4.1 ImButton - 按钮控件

`ImButton` 是支持多种状态的按钮控件，继承自 `ImPanelWidget`。

**状态样式：**
- `m_NormalStyle` - 正常状态
- `m_HoveredStyle` - 悬停状态
- `m_PressedStyle` - 按下状态
- `m_FocusedStyle` - 焦点状态

**事件回调：**
- `OnLeftClicked`, `OnRightClicked` - 左右键点击
- `OnDoubleClicked` - 双点击
- `OnMouseHoverIn`, `OnMouseHover`, `OnMouseHoverOut` - 悬停事件

**使用方法：**
```cpp
ImButton* button = new ImButton("MyButton");
button->SetTooltipText("点击我");
button->OnLeftClicked.Add([](){
    // 处理点击事件
});
```

### 4.2 其他常用控件

#### ImTextBlock - 文本显示控件
- 支持水平对齐（左、中、右）和垂直对齐（上、中、下）
- 可设置字体大小、文本颜色
- 自动计算文本尺寸，支持换行
- 属性：`Text`（字符串）、`TextFontSize`（整数）、`TextColor`（颜色）、对齐方式

#### ImInputText - 文本输入框
- 支持三种输入模式：文本模式、整数模式、小数模式
- 完整的文本选择、复制、粘贴、剪切功能
- 光标闪烁、文本滚动、输入验证
- 支持负号、小数点位数的配置
- 事件：`OnTextChanged`、`OnTextCommit`
- 属性：文本颜色、背景色、边框、选择背景色、输入模式等

#### ImCanvasPanel - 画布面板（自由布局）
- 绝对定位布局容器，子控件使用相对坐标（0-1范围）
- 每个子控件有独立的 `ImCanvasPanelSlot`，包含 `RelativePosition` 和 `SlotSize`
- 支持百分比定位和尺寸
- 自动重新布局，背景和边框渲染
- 属性：继承自 `ImPanelWidget` 的背景和边框属性

#### 布局容器类

##### ImVerticalBox - 垂直布局容器

**功能**：垂直排列子控件，支持固定尺寸和比例分配两种布局模式。

**继承**：`ImPanelWidget`

**构造函数**：
```cpp
ImVerticalBox(const std::string& WidgetName);
```

**公共方法**：
- `ImVerticalBoxSlot* AddChildToVerticalBox(ImWidget* child)` - 添加子控件并返回垂直布局槽
- `virtual void Render() override` - 渲染容器和子控件
- `virtual ImVec2 GetMinSize() override` - 计算最小尺寸
- `virtual ImWidget* CopyWidget() override` - 拷贝控件
- `virtual std::string GetRegisterTypeName() override` - 返回注册类型名"ImVerticalBox"

**布局槽类**：`ImVerticalBoxSlot`（继承自`ImPaddingSlot`）
- **属性**：
  - `SizeRatio`（浮点数）：尺寸比例，用于自动尺寸分配的控件
  - 继承的`Padding`属性：内边距设置
- **方法**：
  - `ImVerticalBoxSlot(ImWidget* Content, ImWidget* Owner)` - 构造函数
  - `virtual ImSlot* CopySlot() override` - 拷贝布局槽

**布局规则**：
1. **固定尺寸控件**：子控件使用最小尺寸，不参与比例分配
2. **自动尺寸控件**：根据`SizeRatio`比例分配剩余空间
3. **空间不足时**：自动尺寸控件获得0尺寸，固定尺寸控件保持不变

**使用示例**：
```cpp
ImVerticalBox* vbox = new ImVerticalBox("VerticalContainer");
ImVerticalBoxSlot* slot1 = vbox->AddChildToVerticalBox(button1);
slot1->SizeRatio = 1.0f; // 比例分配
slot1->PaddingLeft = 10.0f; // 设置内边距

ImVerticalBoxSlot* slot2 = vbox->AddChildToVerticalBox(button2);
slot2->GetIfAutoSize() = false; // 固定尺寸模式
```

##### ImHorizontalBox - 水平布局容器

**功能**：水平排列子控件，支持固定尺寸和比例分配两种布局模式。

**继承**：`ImPanelWidget`

**构造函数**：
```cpp
ImHorizontalBox(const std::string& WidgetName);
```

**公共方法**：
- `ImHorizontalBoxSlot* AddChildToHorizontalBox(ImWidget* child)` - 添加子控件并返回水平布局槽
- `virtual void Render() override` - 渲染容器和子控件
- `virtual ImVec2 GetMinSize() override` - 计算最小尺寸
- `virtual ImWidget* CopyWidget() override` - 拷贝控件
- `virtual std::string GetRegisterTypeName() override` - 返回注册类型名"ImHorizontalBox"

**布局槽类**：`ImHorizontalBoxSlot`（继承自`ImPaddingSlot`）
- **属性**：
  - `SizeRatio`（浮点数）：尺寸比例，用于自动尺寸分配的控件
  - 继承的`Padding`属性：内边距设置
- **方法**：
  - `ImHorizontalBoxSlot(ImWidget* Content, ImWidget* Owner)` - 构造函数
  - `virtual ImSlot* CopySlot() override` - 拷贝布局槽

**布局规则**：
1. **固定尺寸控件**：子控件使用最小尺寸，不参与比例分配
2. **自动尺寸控件**：根据`SizeRatio`比例分配剩余空间
3. **空间不足时**：自动尺寸控件获得0尺寸，固定尺寸控件保持不变

**使用示例**：
```cpp
ImHorizontalBox* hbox = new ImHorizontalBox("HorizontalContainer");
ImHorizontalBoxSlot* slot1 = hbox->AddChildToHorizontalBox(button1);
slot1->SizeRatio = 2.0f; // 占据更多空间
slot1->PaddingTop = 5.0f;

ImHorizontalBoxSlot* slot2 = hbox->AddChildToHorizontalBox(button2);
slot2->GetIfAutoSize() = false; // 固定尺寸
```

##### ImVerticalSplitter - 垂直分割器

**功能**：垂直分割容器，支持拖拽分割条动态调整各部分高度比例，包含最小尺寸约束和交互式调整。

**继承**：`ImPanelWidget`

**构造函数**：
```cpp
ImVerticalSplitter(const std::string& WidgetName);
```

**样式结构**：`ImVerticalSplitterStyle`（继承自`ImObject`）
- **属性**：
  - `BarHeight`（浮点数）：分割条高度，默认4.0f
  - `Color`（颜色）：正常状态颜色，默认IM_COL32(100, 100, 100, 255)
  - `HoveredColor`（颜色）：悬停状态颜色，默认IM_COL32(120, 120, 120, 255)
  - `ActiveColor`（颜色）：激活（拖拽）状态颜色，默认IM_COL32(150, 150, 150, 255)
  - `Rounding`（浮点数）：分割条圆角半径，默认0.0f

**公共方法**：
- `void SetSplitterStyle(const ImVerticalSplitterStyle& style)` - 设置分割器样式
- `ImVerticalSplitterSlot* AddPart(ImWidget* widget, float ratio = 1.0f)` - 添加子控件并设置初始比例
- `void SetMinSize(int index, float minSize)` - 设置指定索引子控件的最小高度
- `virtual ImSlot* AddChild(ImWidget* Child, ImVec2 RelativePosition = ImVec2(FLT_MIN, FLT_MIN)) override` - 重写添加子控件方法，支持相对位置插入
- `virtual void Render() override` - 渲染分割器、分割条和子控件
- `virtual void Relayout() override` - 重新计算布局
- `virtual ImWidget* ChildHitTest(ImVec2 Pos) override` - 命中测试，分割条事件由分割器自身处理
- `virtual ImWidget* CopyWidget() override` - 拷贝控件
- `virtual std::string GetRegisterTypeName() override` - 返回注册类型名"ImVerticalSplitter"

**布局槽类**：`ImVerticalSplitterSlot`（继承自`ImPaddingSlot`）
- **属性**：
  - `Ratio`（浮点数）：高度比例，用于分配空间，默认1.0f
  - `MinSize`（浮点数）：最小高度，默认30.0f
  - 继承的`Padding`属性：内边距设置
- **方法**：
  - `ImVerticalSplitterSlot(ImWidget* Content, ImWidget* Owner)` - 构造函数
  - `virtual ImSlot* CopySlot() override` - 拷贝布局槽

**布局规则**：
1. **比例分配**：子控件按`Ratio`比例分配可用高度（扣除分割条高度）
2. **最小尺寸约束**：每个部分有`MinSize`属性确保最小高度
3. **动态调整**：拖拽分割条时实时调整相邻两部分的`Ratio`值
4. **空间不足处理**：当总高度小于最小高度总和时，按最小高度比例压缩各部分
5. **交互反馈**：分割条支持悬停、拖拽状态，鼠标光标自动变为`ResizeNS`

**事件处理**：
- 支持完整的拖拽事件：`DragStart`、`DragUpdate`、`DragEnd`
- 鼠标悬停反馈：分割条颜色变化
- 分割条区域优先处理事件，防止事件穿透

**属性**（通过属性系统访问）：
- `SplitterStyle`（结构体）：分割器样式配置
- 继承自`ImPanelWidget`的背景和边框属性

**使用示例**：
```cpp
ImVerticalSplitter* splitter = new ImVerticalSplitter("VerticalSplitter");

// 添加三个部分，比例为2:1:1
ImCanvasPanel* topPanel = new ImCanvasPanel("TopPanel");
ImVerticalSplitterSlot* topSlot = splitter->AddPart(topPanel, 2.0f);
topSlot->MinSize = 50.0f; // 设置最小高度

ImCanvasPanel* middlePanel = new ImCanvasPanel("MiddlePanel");
ImVerticalSplitterSlot* middleSlot = splitter->AddPart(middlePanel, 1.0f);

ImCanvasPanel* bottomPanel = new ImCanvasPanel("BottomPanel");
ImVerticalSplitterSlot* bottomSlot = splitter->AddPart(bottomPanel, 1.0f);

// 自定义分割器样式
ImVerticalSplitterStyle style;
style.BarHeight = 6.0f;
style.Color = IM_COL32(80, 80, 80, 255);
style.HoveredColor = IM_COL32(100, 100, 100, 255);
style.ActiveColor = IM_COL32(120, 120, 120, 255);
style.Rounding = 2.0f;
splitter->SetSplitterStyle(style);
```

##### ImHorizontalSplitter - 水平分割器

**功能**：水平分割容器，支持拖拽分割条动态调整各部分宽度比例，包含最小尺寸约束和交互式调整。

**继承**：`ImPanelWidget`

**构造函数**：
```cpp
ImHorizontalSplitter(const std::string& WidgetName);
```

**样式结构**：`ImHorizontalSplitterStyle`（继承自`ImObject`）
- **属性**：
  - `BarWidth`（浮点数）：分割条宽度，默认4.0f
  - `Color`（颜色）：正常状态颜色，默认IM_COL32(100, 100, 100, 255)
  - `HoveredColor`（颜色）：悬停状态颜色，默认IM_COL32(120, 120, 120, 255)
  - `ActiveColor`（颜色）：激活（拖拽）状态颜色，默认IM_COL32(150, 150, 150, 255)
  - `Rounding`（浮点数）：分割条圆角半径，默认0.0f

**公共方法**：
- `void SetSplitterStyle(const ImHorizontalSplitterStyle& style)` - 设置分割器样式
- `ImHorizontalSplitterSlot* AddPart(ImWidget* widget, float ratio = 1.0f)` - 添加子控件并设置初始比例
- `void SetMinSize(int index, float minSize)` - 设置指定索引子控件的最小宽度
- `virtual ImSlot* AddChild(ImWidget* Child, ImVec2 RelativePosition = ImVec2(FLT_MIN, FLT_MIN)) override` - 重写添加子控件方法，支持相对位置插入
- `virtual void Render() override` - 渲染分割器、分割条和子控件
- `virtual void Relayout() override` - 重新计算布局
- `virtual ImWidget* ChildHitTest(ImVec2 Pos) override` - 命中测试，分割条事件由分割器自身处理
- `virtual ImWidget* CopyWidget() override` - 拷贝控件
- `virtual std::string GetRegisterTypeName() override` - 返回注册类型名"ImHorizontalSplitter"

**布局槽类**：`ImHorizontalSplitterSlot`（继承自`ImPaddingSlot`）
- **属性**：
  - `Ratio`（浮点数）：宽度比例，用于分配空间，默认1.0f
  - `MinSize`（浮点数）：最小宽度，默认30.0f
  - 继承的`Padding`属性：内边距设置
- **方法**：
  - `ImHorizontalSplitterSlot(ImWidget* Content, ImWidget* Owner)` - 构造函数
  - `virtual ImSlot* CopySlot() override` - 拷贝布局槽

**布局规则**：
1. **比例分配**：子控件按`Ratio`比例分配可用宽度（扣除分割条宽度）
2. **最小尺寸约束**：每个部分有`MinSize`属性确保最小宽度
3. **动态调整**：拖拽分割条时实时调整相邻两部分的`Ratio`值
4. **空间不足处理**：当总宽度小于最小宽度总和时，按最小宽度比例压缩各部分
5. **交互反馈**：分割条支持悬停、拖拽状态，鼠标光标自动变为`ResizeEW`

**事件处理**：
- 支持完整的拖拽事件：`DragStart`、`DragUpdate`、`DragEnd`
- 鼠标悬停反馈：分割条颜色变化
- 分割条区域优先处理事件，防止事件穿透

**属性**（通过属性系统访问）：
- `SplitterStyle`（结构体）：分割器样式配置
- 继承自`ImPanelWidget`的背景和边框属性

**使用示例**：
```cpp
ImHorizontalSplitter* splitter = new ImHorizontalSplitter("HorizontalSplitter");

// 添加三个部分，比例为3:2:1
ImCanvasPanel* leftPanel = new ImCanvasPanel("LeftPanel");
ImHorizontalSplitterSlot* leftSlot = splitter->AddPart(leftPanel, 3.0f);
leftSlot->MinSize = 100.0f; // 设置最小宽度

ImCanvasPanel* middlePanel = new ImCanvasPanel("MiddlePanel");
ImHorizontalSplitterSlot* middleSlot = splitter->AddPart(middlePanel, 2.0f);

ImCanvasPanel* rightPanel = new ImCanvasPanel("RightPanel");
ImHorizontalSplitterSlot* rightSlot = splitter->AddPart(rightPanel, 1.0f);

// 自定义分割器样式
ImHorizontalSplitterStyle style;
style.BarWidth = 8.0f;
style.Color = IM_COL32(80, 80, 80, 255);
style.HoveredColor = IM_COL32(100, 100, 100, 255);
style.ActiveColor = IM_COL32(120, 120, 120, 255);
style.Rounding = 3.0f;
splitter->SetSplitterStyle(style);
```

##### ImScrollBox - 滚动容器

**功能**：提供水平和垂直滚动功能，支持自定义滚动条样式和鼠标滚轮交互。

**继承**：`ImPanelWidget`

**构造函数**：
```cpp
ImScrollBox(const std::string& WidgetName);
```

**内容管理**：
- `ImSlot* SetContent(ImWidget* content, bool DeleteOld = true)` - 设置/替换滚动内容
- `ImVec2 GetContentSize() const` - 获取内容尺寸
- `ImVec2 GetScrollPosition() const` - 获取当前滚动位置

**滚动控制**：
- `void SetScrollPosition(const ImVec2& position)` - 设置滚动位置
- `void EnableHorizontalScroll(bool enable)` - 启用/禁用水平滚动
- `void EnableVerticalScroll(bool enable)` - 启用/禁用垂直滚动
- `void ScrollToWidget(ImWidget* target, const ImVec2& Offset = ImVec2(0, 0))` - 滚动到指定控件
  - `target`：目标控件（必须在滚动内容内）
  - `Offset`：目标控件与可见区域边界的偏移量

**滚动条配置**：
- `void ShowHorizontalScrollbar(bool show)` - 显示/隐藏水平滚动条
- `void ShowVerticalScrollbar(bool show)` - 显示/隐藏垂直滚动条
- `void SetScrollbarThickness(float thickness)` - 设置滚动条厚度
- `void SetScrollbarColors(ImU32 bg, ImU32 grab, ImU32 grabHovered, ImU32 grabActive)` - 设置滚动条颜色
  - `bg`：背景色
  - `grab`：滑块颜色
  - `grabHovered`：悬停时滑块颜色
  - `grabActive`：按下时滑块颜色

**属性**（通过属性系统访问）：
- `ScrollbarThickness`（浮点数）：滚动条厚度
- `ScrollbarBgColor`（颜色）：滚动条背景色
- `ScrollbarGrabColor`（颜色）：滑块颜色
- `ScrollbarGrabHoveredColor`（颜色）：悬停时滑块颜色
- `ScrollbarGrabActiveColor`（颜色）：按下时滑块颜色
- `HorizontalScrollEnabled`（布尔值）：水平滚动是否启用
- `VerticalScrollEnabled`（布尔值）：垂直滚动是否启用
- `ShowHorizontalScrollbar`（布尔值）：是否显示水平滚动条
- `ShowVerticalScrollbar`（布尔值）：是否显示垂直滚动条

**事件交互**：
- 支持鼠标滚轮滚动
- 支持拖拽滚动条滑块
- 自动悬停效果和鼠标光标变化
- 点击滚动条轨道跳转到相应位置

**使用示例**：
```cpp
ImScrollBox* scroll = new ImScrollBox("ScrollContainer");
scroll->EnableVerticalScroll(true);
scroll->SetScrollbarThickness(12.0f);
scroll->SetScrollbarColors(
    IM_COL32(240, 240, 240, 255),  // 背景色
    IM_COL32(150, 150, 150, 255),  // 滑块颜色
    IM_COL32(180, 180, 180, 255),  // 悬停颜色
    IM_COL32(200, 200, 200, 255)   // 按下颜色
);

ImCanvasPanel* content = new ImCanvasPanel("ScrollContent");
scroll->SetContent(content);

// 滚动到特定位置
scroll->SetScrollPosition(ImVec2(0, 100));

// 滚动到特定控件
scroll->ScrollToWidget(targetWidget, ImVec2(10, 10));
```

##### ImExpandableBox - 可展开/折叠面板

**功能**：提供可展开/折叠的面板，包含标题区和内容区，支持三角形指示器和悬停效果。

**继承**：`ImPanelWidget`

**构造函数**：
```cpp
ImExpandableBox(const std::string& WidgetName);
```

**结构部件**：
- `ImPaddingSlot* SetHead(ImWidget* HeadWidget)` - 设置头部控件
- `ImPaddingSlot* SetBody(ImWidget* BodyWidget)` - 设置身体控件（展开时显示）

**展开状态控制**：
- `bool GetIfExpanded()` - 获取当前展开状态
- `void SetExpandedState(bool newstate)` - 设置展开状态
  - `newstate`：`true`表示展开，`false`表示折叠

**三角形指示器**：
- `void SetHoveredTriangleColor(ImU32 color)` - 设置悬停时三角形颜色
- `bool IsHovered() const` - 获取当前是否悬停在三角形按钮上

**事件回调**：
- `void SetOnExpandedStateChanged(std::function<void(bool)> callback)` - 展开状态变化回调
  - 参数：新的展开状态（`true`=展开，`false`=折叠）
- `void SetOnHoverBegin(std::function<void(void)> callback)` - 悬停开始回调
- `void SetOnHoverEnd(std::function<void(void)> callback)` - 悬停结束回调

**属性**（通过属性系统访问）：
- `IsExpanded`（布尔值）：是否展开
- `TriangleSize`（浮点数）：三角形指示器大小
- `HeadPad`（浮点数）：头部内边距（自动计算，通常为2*TriangleSize）
- `BodyPad`（浮点数）：身体区内边距
- `TriangleColor`（颜色）：三角形颜色
- `HoveredTriangleColor`（颜色）：悬停时三角形颜色

**布局特性**：
- **折叠时**：只显示头部控件和三角形指示器
- **展开时**：显示头部和身体控件，身体控件在头部下方
- **三角形位置**：在头部左侧，根据头部高度自动居中
- **点击交互**：点击三角形区域切换展开/折叠状态

**使用示例**：
```cpp
ImExpandableBox* expandBox = new ImExpandableBox("ExpandablePanel");

// 设置头部和身体
ImTextBlock* header = new ImTextBlock("HeaderText");
header->SetText("点击展开");
expandBox->SetHead(header);

ImCanvasPanel* body = new ImCanvasPanel("BodyContent");
// 添加身体内容...
expandBox->SetBody(body);

// 配置回调
expandBox->SetOnExpandedStateChanged([](bool expanded) {
    std::cout << "面板" << (expanded ? "展开" : "折叠") << std::endl;
});

// 设置初始状态
expandBox->SetExpandedState(true);

// 自定义三角形颜色
expandBox->SetHoveredTriangleColor(IM_COL32(100, 100, 200, 255));
```

#### 其他控件
- **ImCheckBox** - 复选框，支持选中状态和事件
- **ImSlider** - 滑动条，数值范围控制
- **ImComboBox** - 下拉框，选项选择
- **ImImage** - 图像显示控件
- **ImColorPicker** - 颜色选择器
- **ImFloatInput** - 浮点数输入框
- **ImIntInput** - 整数输入框
- **ImMenuButton** - 菜单按钮
- **ImMultiLineTextBlock** - 多行文本显示
- **ImResizableBox** - 可调整大小的容器

## 5. 工具类

### 5.1 ImWidgetRef - 控件引用

`ImWidgetRef` 提供安全的控件弱引用，避免悬空指针问题。

### 5.2 ImDelegate - 委托系统

`ImMulticastDelegate<>` 支持多播委托，用于事件回调。

### 5.3 ImSlot - 布局槽

`ImSlot` 表示控件在容器中的位置和布局信息，支持填充、边距等布局属性。

**Slot 类型：**
- `ImSlot` - 布局槽基类，管理控件的位置和尺寸
- `ImCanvasPanelSlot` - 画布面板专用槽，支持相对位置（0-1范围）和绝对尺寸
- `ImPaddingSlot` - 带内边距的布局槽，用于按钮等控件

**功能：**
- 位置和尺寸管理
- 布局应用（`ApplyLayout()`）
- 属性系统集成，支持序列化

## 6. 属性系统

### 6.1 属性注册

控件通过宏声明注册属性，支持设计时编辑：

```cpp
DECLARE_IMOBJECT(ImButton, ImPanelWidget)
registrar
    .RegisterProperty(PropertyType::String, "TooltipText", &ImButton::m_TooltipText, "提示文本")
    .RegisterProperty(PropertyType::Vec2, "OriginalMinSize", &ImButton::OriginalMinSize, "原始最小尺寸");
END_DECLARE_IMOBJECT()
```

### 6.2 属性类型

支持的类型包括：
- 基础类型：`Bool`, `Int`, `Float`, `String`
- 图形类型：`Color`, `Vec2`
- 复合类型：`Struct`, `StringArray`, `Enum`

## 7. 使用方法

### 7.1 创建简单界面

```cpp
// 创建窗口
ImWindow* window = new ImWindow("示例窗口", ImVec2(400, 300), ImVec2(100, 100), "main", windowManager);

// 创建根控件
ImCanvasPanel* root = new ImCanvasPanel("RootPanel");
window->SetRootWidget(root);

// 添加按钮
ImButton* button = new ImButton("TestButton");
button->SetPosition(ImVec2(50, 50));
button->SetSize(ImVec2(100, 40));
button->SetTooltipText("测试按钮");
root->AddChild(button);

// 处理按钮点击
button->OnLeftClicked.Add([](){
    std::cout << "按钮被点击！" << std::endl;
});
```

### 7.2 事件处理

```cpp
class MyWidget : public ImWidget {
protected:
    virtual void HandleEventInternal(ImEvent* event) override {
        if (event->Is<ImMouseClickEvent>()) {
            auto* clickEvent = event->As<ImMouseClickEvent>();
            if (clickEvent->GetButton() == ImMouseButton::Left) {
                // 处理左键点击
                event->StopPropagation(); // 阻止事件冒泡
            }
        }
    }
};
```

### 7.3 自定义控件

```cpp
class MyCustomWidget : public ImPanelWidget {
public:
    MyCustomWidget(const std::string& name) : ImPanelWidget(name) {
        // 初始化代码
    }

    virtual void Render() override {
        ImPanelWidget::Render(); // 渲染背景和子控件

        // 自定义渲染逻辑
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        window->DrawList->AddText(Position, IM_COL32(255, 255, 255, 255), "自定义控件");
    }

    virtual ImSlot* CreateSlot(ImWidget* Content) override {
        return new ImPaddingSlot(Content, this);
    }

    virtual ImWidget* CopyWidget() override {
        return new MyCustomWidget(*this);
    }

    DECLARE_IMOBJECT(MyCustomWidget, ImPanelWidget)
    // 属性注册
    END_DECLARE_IMOBJECT()
};
```

## 8. 构建与集成

### 8.1 依赖项

- **Dear ImGui** - 图形用户界面库
- **nlohmann/json** - JSON 序列化
- **stb_image** - 图像加载

### 8.2 构建步骤

```bash
# Windows (使用 CMake)
mkdir build
cd build
cmake ..
cmake --build . --config Release

# Linux ARM (树莓派)
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchains/raspberry-pi.cmake ..
make
```

### 8.3 集成到项目

1. 将 ImWidget 作为子模块添加到项目中
2. 在 CMakeLists.txt 中添加 `add_subdirectory(ImWidget)`
3. 链接 `ImWidget` 库
4. 包含必要的头文件

## 9. 设计理念

### 9.1 组合优于继承

控件通过组合方式构建复杂界面，而非深度继承层次。

### 9.2 属性驱动设计

所有控件属性可通过运行时属性系统访问和修改，支持序列化和可视化编辑。

### 9.3 事件冒泡机制

事件遵循捕获→目标→冒泡的流程，简化事件处理逻辑。

### 9.4 弱引用管理

使用 `ImWidgetRef` 避免内存泄漏和悬空指针问题。

## 10. 相关项目

- **ImDesigner** - 基于 ImWidget 的可视化 UI 设计器
- **Test/** - 测试项目和示例

---

## 附录：控件类列表

| 控件类 | 描述 | 继承自 |
|--------|------|--------|
| `ImButton` | 按钮控件 | `ImPanelWidget` |
| `ImTextBlock` | 文本显示 | `ImWidget` |
| `ImInputText` | 文本输入 | `ImWidget` |
| `ImCheckBox` | 复选框 | `ImWidget` |
| `ImSlider` | 滑动条 | `ImWidget` |
| `ImComboBox` | 下拉框 | `ImWidget` |
| `ImCanvasPanel` | 画布面板 | `ImPanelWidget` |
| `ImVerticalBox` | 垂直布局 | `ImPanelWidget` |
| `ImHorizontalBox` | 水平布局 | `ImPanelWidget` |
| `ImScrollBox` | 滚动容器 | `ImPanelWidget` |
| `ImVerticalSplitter` | 垂直分割器 | `ImPanelWidget` |
| `ImHorizontalSplitter` | 水平分割器 | `ImPanelWidget` |
| `ImExpandableBox` | 可展开面板 | `ImPanelWidget` |
| `ImImage` | 图像显示 | `ImWidget` |
| `ImColorPicker` | 颜色选择器 | `ImWidget` |

完整的控件列表请查看 `Include/ImWidget/` 目录。

---

*文档最后更新：2026年2月*
*更多信息请参考代码注释和示例项目*