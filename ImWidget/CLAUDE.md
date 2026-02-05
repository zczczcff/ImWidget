[根目录](../../CLAUDE.md) > **ImWidget**

## ImWidget核心库

### 模块职责
ImWidget是项目的核心UI框架库，提供基础控件、事件系统、序列化、代码生成等核心功能。基于ImGui库构建，支持跨平台（Windows、Linux ARM）运行。

### 入口与启动
- **入口文件**: `ImWidgetConfig.h` - ImGui配置定义
- **主入口**: `ImMain.cpp` - 平台无关的启动入口，Windows和Linux适配
- **核心实例**: `ImGlobalInstance.cpp` - 全局实例管理

---

## 完整的控件类继承关系

```
ImObject (ROP属性系统基类)
├── ImWidget (所有控件基类)
│   ├── ImPanelWidget (容器控件基类)
│   │   ├── ImHorizontalBox (水平容器)
│   │   ├── ImVerticalBox (垂直容器)
│   │   ├── ImHorizontalSplitter (水平分割器)
│   │   ├── ImVerticalSplitter (垂直分割器)
│   │   ├── ImScrollBox (滚动容器)
│   │   ├── ImCanvasPanel (画布面板)
│   │   ├── ImExpandableBox (可展开容器)
│   │   ├── ImResizableBox (可调整大小容器)
│   │   ├── ImButton (按钮控件)
│   │   ├── ImCheckBox (复选框控件)
│   │   ├── ImComboBox (下拉选择框)
│   │   ├── ImInputText (单行文本输入)
│   │   ├── ImIntInput (整数输入框)
│   │   ├── ImFloatInput (浮点数输入框)
│   │   ├── ImTextBlock (文本块)
│   │   ├── ImMultiLineTextBlock (多行文本块)
│   │   ├── ImImage (图像控件)
│   │   ├── ImSlider (滑块控件)
│   │   ├── ImScrollingTextList (滚动文本列表)
│   │   ├── ImSimpleFigure (简单图形)
│   │   ├── ImBorder (边框控件)
│   │   ├── ImColorPicker (颜色选择器)
│   │   └── ImCurve (曲线图)
│   └── ImUserWidget (用户自定义控件基类)
```

---

## ROP属性系统详细文档

### 属性类型枚举

```cpp
enum class PropertyType {
    Color,       // ImU32 - 颜色值 (ARGB格式)
    Float,       // float - 浮点数
    Bool,        // bool - 布尔值
    Int,         // int - 整数
    String,      // std::string - 字符串
    Vec2,        // ImVec2 - 二维向量
    Struct,      // ImObject* - 嵌套结构体
    StringArray,  // std::vector<std::string> - 字符串数组
    Enum         // std::string - 枚举类型（存储为字符串）
};
```

### 颜色值宏定义

```cpp
// 颜色分量提取（与STB不同，顺序为R,G,B,A，不丢失精度）
#define IM_COLOR_GET_R(col) ((col) & 0xFF)
#define IM_COLOR_GET_G(col) (((col) >> 8) & 0xFF)
#define IM_COLOR_GET_B(col) (((col) >> 16) & 0xFF)
#define IM_COLOR_GET_A(col) (((col) >> 24) & 0xFF)

// 颜色值设置
#define IM_COLOR_SET_RGBA(r, g, b, a) ((r) | ((g) << 8) | ((b) << 16) | ((a) << 24))
```

### 属性注册宏

```cpp
// 基本对象声明
DECLARE_IMOBJECT(ClassName, ParentClassName)
    registrar
        .RegisterProperty(PropertyType::String, "PropertyName", &ClassName::MemberVar, "描述")
        .RegisterOptionalProperty(PropertyType::Int, "EnumProp", &ClassName::EnumVar,
            { "Option1", "Option2", "Option3" }, "枚举描述")
END_DECLARE_IMOBJECT()
```

### 属性访问接口

```cpp
// 获取属性
ROP::Property<PropertyType> prop = object->GetProperty("PropertyName");

// 读取属性值
int intValue = prop.GetValue<int>();
float floatValue = prop.GetValue<float>();
std::string strValue = prop.GetValue<std::string>();
ImU32 colorValue = prop.GetValue<ImU32>();
ImVec2 vecValue = prop.GetValue<ImVec2>();

// 设置属性值
prop.SetValue<int>(42);
prop.SetValue<std::string>("Hello");

// 可选属性（枚举）特殊访问
ROP::OptionalProperty<PropertyType> optProp = object->GetPropertyAsOptional("EnumProp");
std::string optionStr = optProp.GetOptionString();
optProp.SetOptionByString("Option1");
optProp.SetOptionByIndex(0);
```

### 路径属性访问

```cpp
// 访问嵌套属性：path格式 "child1/child2/propertyName"
ImProperty prop = object->GetPathProperty("child1/child2/PropertyName");
float value = prop.GetValue<float>();
```

### 属性系统特性

1. **继承支持**: 子类继承父类的所有属性
2. **属性覆盖**: 子类可以覆盖父类的同名属性
3. **类型安全**: 编译时类型检查
4. **多态属性**: 同名属性在不同类中可具有不同类型
5. **可选属性**: 支持枚举类型选项列表
6. **描述信息**: 每个属性可附带描述信息
7. **类名限定**: 使用 "ClassName::PropertyName" 格式区分同名属性

---

## 事件系统完整文档

### 事件类型枚举

```cpp
enum class ImEventType {
    // 鼠标事件
    MouseDown,       // 鼠标按下
    MouseUp,         // 鼠标释放
    MouseMove,       // 鼠标移动
    MouseEnter,      // 鼠标进入
    MouseLeave,      // 鼠标离开
    MouseWheel,      // 鼠标滚轮
    MouseClick,      // 鼠标点击
    MouseDoubleClick,// 鼠标双击

    // 悬停事件
    HoverIn,         // 悬停进入
    Hover,           // 悬停持续时
    HoverOut,        // 悬停退出

    // 键盘事件
    KeyDown,         // 键盘按下
    KeyUp,           // 键盘释放
    KeyPress,        // 键盘按键

    // 文本输入事件
    TextInput,       // 文本输入

    // 焦点事件
    FocusIn,         // 获得焦点
    FocusOut,        // 失去焦点
    FocusRequest,    // 焦点请求

    // 拖拽事件
    DragStart,       // 拖拽开始（来自拖拽源控件）
    DragUpdate,      // 拖拽更新（来自拖拽源控件）
    DragEnd,         // 拖拽结束（来自拖拽源控件）
    DragEnter,       // 拖拽进入（来自拖拽目标控件）
    DragOver,        // 拖拽悬停（来自拖拽目标控件）
    DragLeave,       // 拖拽离开（来自拖拽目标控件）
    Drop,            // 拖拽放置（来自拖拽目标控件）

    // 手势事件（预留）
    GestureStart,
    GestureUpdate,
    GestureEnd,

    // 自定义事件
    Custom
};
```

---

## 控件详细文档

### ImCheckBox - 复选框控件

**继承关系**: `ImWidget -> ImCheckBox`

**特性**: 带有自定义样式的复选框，支持多种状态颜色

#### 关键API

```cpp
// 状态管理
void SetChecked(bool checked);
bool IsChecked() const;

// 样式设置
void SetBoxColor(ImU32 color);
void SetCheckColor(ImU32 color);
void SetBackgroundColor(ImU32 color);
void SetHoveredBoxColor(ImU32 color);
void SetPressedBoxColor(ImU32 color);
void SetBoxRounding(float rounding);
void SetBorderThickness(float thickness);

// 回调函数
void SetOnToggled(std::function<void(bool)> callback);
void SetOnHoverBegin(std::function<void(void)> callback);
void SetOnHoverEnd(std::function<void(void)> callback);

// 状态查询
bool IsHovered() const;
bool IsPressed() const;
```

#### 属性列表（ROP）
- `Checked` (Bool) - 是否选中
- `BoxColor` (Color) - 复选框边框颜色
- `CheckColor` (Color) - 选中标记颜色
- `BackgroundColor` (Color) - 背景颜色
- `HoveredBoxColor` (Color) - 悬停时复选框颜色
- `PressedBoxColor` (Color) - 按下时复选框颜色
- `BoxRounding` (Float) - 边框圆角
- `BorderThickness` (Float) - 边框粗细

---

### ImComboBox - 下拉选择框

**继承关系**: `ImWidget -> ImComboBox`

**特性**: 完整的下拉选择框，支持自定义样式、多选项管理、自动调整位置

#### 关键API

```cpp
// 选项管理
void SetItems(const std::vector<std::string>& items);
void AddItem(const std::string& item);
void ClearItems();

// 选择管理
void SetSelectedIndex(int index);
void SetSelectedItem(const std::string& selectedItem);
int GetSelectedIndex() const;
std::string GetSelectedText() const;

// 回调函数
void SetOnSelectionChanged(std::function<void(int)> callback);
void SetOnPopupOpened(std::function<void(void)> callback);
void SetOnPopupClosed(std::function<void(void)> callback);

// 样式设置
void SetNormalStyle(const ButtonStateStyle& style);
void SetHoveredStyle(const ButtonStateStyle& style);
void SetPressedStyle(const ButtonStateStyle& style);
void SetPopupBackgroundColor(ImU32 color);
void SetHighlightColor(ImU32 color);
void SetTextColor(ImU32 color);
void SetPopupRounding(float rounding);
void SetItemHeight(float height);
```

#### 属性列表（ROP）
- `Items` (StringArray) - 选项列表
- `SelectedIndex` (Int) - 选中项索引
- `ArrowSize` (Float) - 下拉箭头大小
- `ItemHeight` (Float) - 选项项高度
- `PopupRounding` (Float) - 下拉窗口圆角
- `PopupPadding` (Vec2) - 下拉窗口内边距
- `PopupBgColor` (Color) - 下拉窗口背景颜色
- `HighlightColor` (Color) - 高亮颜色
- `TextColor` (Color) - 文本颜色
- `NormalStyle` (Struct) - 正常状态样式
- `HoveredStyle` (Struct) - 悬停状态样式
- `PressedStyle` (Struct) - 按下状态样式
- `DropdownStyle` (Struct) - 下拉状态样式

---

### ImTextBlock - 文本块

**继承关系**: `ImWidget -> ImTextBlock`

**特性**: 支持水平/垂直对齐、可变字体大小的文本显示控件

#### 关键API

```cpp
// 文本内容
void SetText(const std::string& text);

// 文本样式
void SetTextColor(ImU32 color);

// 对齐方式
enum class TextAlignment_Horizontal { Left, Center, Right };
enum class TextAlignment_Vertical { Top, Center, Bottom };

void SetHorizontalAlignment(const TextAlignment_Horizontal& alignment);
void SetVerticalAlignment(const TextAlignment_Vertical& alignment);

// 字体大小
// 通过 ImApplication::GetFont(TextSize) 获取字体
```

#### 属性列表（ROP）
- `Text` (String) - 文本内容
- `TextFontSize` (Int) - 字体大小
- `TextColor` (Color) - 文本颜色
- `HorizontalAlignment` (Enum) - 水平对齐方式
- `VerticalAlignment` (Enum) - 垂直对齐方式

---

### ImImage - 图像控件

**继承关系**: `ImWidget -> ImImage`

**特性**: 支持文件加载、纹理ID设置、保持宽高比

#### 关键API

```cpp
// 纹理设置
void SetTextureID(ImTextureID TextureID, int newOriginalWidth = -1, int newOriginalHeight = -1);

// 文件加载
bool LoadImageFromFile(const std::string& FilePath);

// 样式设置
void SetAlpha(float alpha);
void SetTintcolor(ImU32 color);

// 宽高比控制
void SetMaintainAspectRatio(bool maintain);
bool GetMaintainAspectRatio() const;

// 计算
ImVec2 CalculateAspectRatioSize(const ImVec2& availableSize) const;
void SetOriginalSize(int Width, int Height);
```

#### 属性列表（ROP）
- `MaintainAspectRatio` (Bool) - 是否保持宽高比
- `TintColor` (Color) - 着色颜色

---

### ImSlider - 滑块控件

**继承关系**: `ImWidget -> ImSlider`

**特性**: 支持水平/垂直方向、自定义句柄样式、幂函数曲线、反向滑动

#### ImSliderStyle - 滑块样式结构

```cpp
struct ImSliderStyle : public ImObject {
    ImU32 background;       // 背景颜色
    ImU32 filled;           // 填充颜色
    ImU32 handle;           // 句柄颜色
    ImU32 handle_border;     // 句柄边框颜色
    ImU32 text;             // 文本颜色
    ImU32 handle_hovered;   // 句柄悬停颜色
    ImU32 handle_active;     // 句柄激活颜色

    // 预设主题
    void SetLightTheme();    // 浅色主题
    void SetDarkTheme();     // 深色主题
};
```

#### 关键API

```cpp
// 值管理
void SetValue(float value);
float v;              // 当前值
float v_Min;         // 最小值
float v_Max;         // 最大值

// 句柄类型
enum class SliderHandleType { SLIDER_HANDLE_CIRCLE, SLIDER_HANDLE_RECT };
SliderHandleType handle_type;

// 显示控制
std::string format;    // 值显示格式（如 "%.1f"）
bool show_value;      // 是否显示数值
bool show_handle;     // 是否显示句柄
bool draggable;       // 是否可拖拽

// 尺寸控制
float handle_ratio;       // 句柄大小比例
float handlewidth_ratio; // 句柄宽度/高度比例
bool bReverse;           // 是否反向滑动

// 幂函数
float power;             // 幂函数指数（1.0 = 线性）
```

#### 属性列表（ROP）

**ImSlider**: 值范围管理
- `MinValue` (Float) - 最小值
- `MaxValue` (Float) - 最大值
- `CurrentValue` (Float) - 当前值
- `HandleType` (Enum) - 句柄类型
- `Style` (Struct) - 样式对象（ImSliderStyle）
- `ShowValue` (Bool) - 是否显示数值
- `ValueFormat` (String) - 值显示格式
- `ShowHandle` (Bool) - 是否显示句柄
- `Draggable` (Bool) - 是否可拖拽
- `HandleRatio` (Float) - 句柄大小比例
- `HandleWidthRatio` (Float) - 句柄宽高比例
- `Reverse` (Bool) - 是否反向滑动
- `Power` (Float) - 幂函数指数

**ImSliderStyle**: 颜色样式
- `BackgroundColor` (Color) - 背景颜色
- `FilledColor` (Color) - 填充颜色
- `HandleColor` (Color) - 句柄颜色
- `HandleBorderColor` (Color) - 句柄边框颜色
- `TextColor` (Color) - 文本颜色
- `HandleHoveredColor` (Color) - 句柄悬停颜色
- `HandleActiveColor` (Color) - 句柄激活颜色

---

### ImHorizontalBox - 水平容器

**继承关系**: `ImPanelWidget -> ImHorizontalBox`

**特性**: 水平布局容器，支持固定大小和比例大小的子控件

#### ImHorizontalBoxSlot - 水平布局插槽

```cpp
class ImHorizontalBoxSlot : public ImPaddingSlot {
    float SizeRatio = 1.f;  // 宽度比例
};
```

#### 关键API

```cpp
// 添加子控件
ImHorizontalBoxSlot* AddChildToHorizontalBox(ImWidget* child);

// 插槽属性
// SizeRatio: 子控件宽度比例（仅对非固定大小的子控件有效）
```

---

### ImScrollBox - 滚动容器

**继承关系**: `ImPanelWidget -> ImScrollBox`

**特性**: 支持水平和垂直滚动、自定义滚动条样式、滚动到指定控件

#### 关键API

```cpp
// 内容管理
ImSlot* SetContent(ImWidget* content, bool DeleteOld = true);

// 滚动位置控制
void SetScrollPosition(const ImVec2& position);
ImVec2 GetScrollPosition() const;

// 滚动控制
void EnableHorizontalScroll(bool enable);
void EnableVerticalScroll(bool enable);
void ShowHorizontalScrollbar(bool show);
void ShowVerticalScrollbar(bool show);

// 滚动条样式
void SetScrollbarThickness(float thickness);
void SetScrollbarColors(ImU32 bg, ImU32 grab, ImU32 grabHovered, ImU32 grabActive);

// 信息获取
ImVec2 GetContentSize() const;

// 滚动到控件
void ScrollToWidget(ImWidget* target, const ImVec2& Offset = ImVec2(0, 0));
```

#### 属性列表（ROP）

**滚动条样式**:
- `ScrollbarThickness` (Float) - 滚动条粗细
- `ScrollbarBgColor` (Color) - 滚动条背景颜色
- `ScrollbarGrabColor` (Color) - 滚动条滑块颜色
- `ScrollbarGrabHoveredColor` (Color) - 滚动条滑块悬停颜色
- `ScrollbarGrabActiveColor` (Color) - 滚动条滑块激活颜色

**滚动控制**:
- `HorizontalScrollEnabled` (Bool) - 水平滚动是否启用
- `VerticalScrollEnabled` (Bool) - 垂直滚动是否启用
- `ShowHorizontalScrollbar` (Bool) - 是否显示水平滚动条
- `ShowVerticalScrollbar` (Bool) - 是否显示垂直滚动条

---

### ImSimpleFigure - 简单图形控件

**继承关系**: `ImWidget -> ImSimpleFigure`

**特性**: 支持三角形、矩形、圆形三种基本图形

#### 关键API

```cpp
// 图形类型
enum class FigureType { TRIANGLE, SQUARE, CIRCLE };

// 样式设置
void SetFillColor(ImU32 color);

// 大小控制
float m_SizeRatio;  // 图形大小比例 (0.0~1.0)
float m_MinSize;     // 最小尺寸
```

#### 属性列表（ROP）
- `Type` (Enum) - 图形类型（Triangle/Square/Circle）
- `SizeRatio` (Float) - 图形大小比例 (0.0~1.0)
- `MinSize` (Float) - 最小尺寸
- `FillColor` (Color) - 填充颜色
- `HasBorder` (Bool) - 是否显示边框
- `BorderColor` (Color) - 边框颜色
- `BorderThickness` (Float) - 边框粗细

---

### ImBorder - 边框控件

**继承关系**: `ImPanelWidget -> ImBorder`

**特性**: 单一内容的边框容器，支持可选边框显示

#### 关键API

```cpp
// 内容管理
void SetContent(ImWidget* Content, bool DeleteOld = true);
ImWidget* GetContent();
```

---

### ImResizableBox - 可调整大小容器

**继承关系**: `ImPanelWidget -> ImResizableBox`

**特性**: 9个控制点（4个角+4个边中点+中心点）的可调整大小容器

#### 关键API

```cpp
// 控制点样式
enum ControlPoint {
    TopLeft, TopCenter, TopRight,
    MidRight, BottomRight, BottomCenter,
    BottomLeft, MidLeft, Center, COUNT
};

void SetControlPointRadius(float radius);
void SetControlPointColor(ImU32 color);

// 边框样式
void SetBoxBorderColor(ImU32 color);
void SetBoxBorderThickness(float thickness);

// 回调函数
void SetOnResizing(std::function<void(ImVec2, ImVec2)> callback);
void SetOnResized(std::function<void(ImVec2, ImVec2)> callback);
void SetOnBeginPreResing(std::function<void(ImVec2, ImVec2)> callback);

// 外部调整
void OuterReSize(ImVec2 Pos, ImVec2 size, bool bTriggleCallBack = true);

// 插槽
ImResizableBoxSlot* SetContent(ImWidget* Child);
```

---

### ImVerticalBox - 垂直容器

**继承关系**: `ImPanelWidget -> ImVerticalBox`

**特性**: 垂直布局容器，支持固定大小和比例大小的子控件

#### ImVerticalBoxSlot - 垂直布局插槽

```cpp
class ImVerticalBoxSlot : public ImPaddingSlot {
    float SizeRatio = 1.f;  // 高度比例
};
```

#### 关键API

```cpp
// 添加子控件
ImVerticalBoxSlot* AddChildToVerticalBox(ImWidget* child);

// 插槽属性
// SizeRatio: 子控件高度比例（仅对非固定大小的子控件有效）
```

---

### ImHorizontalSplitter - 水平分割器

**继承关系**: `ImPanelWidget -> ImHorizontalSplitter`

**特性**: 支持拖拽调整比例的水平分割器，带自定义样式

#### ImHorizontalSplitterStyle - 分割器样式

```cpp
struct ImHorizontalSplitterStyle : public ImObject {
    float BarWidth = 4.0f;        // 分隔条宽度
    ImU32 Color;                  // 普通颜色
    ImU32 HoveredColor;           // 悬停颜色
    ImU32 ActiveColor;            // 活动颜色
    float Rounding = 0.0f;        // 圆角半径
};
```

#### ImHorizontalSplitterSlot - 分割器插槽

```cpp
class ImHorizontalSplitterSlot : public ImPaddingSlot {
    float Ratio = 1.0f;    // 宽度比例
    float MinSize = 30.0f; // 最小宽度
};
```

#### 关键API

```cpp
// 设置样式
void SetSplitterStyle(const ImHorizontalSplitterStyle& style);

// 添加分割部分
ImHorizontalSplitterSlot* AddPart(ImWidget* widget, float ratio = 1.0f);

// 设置最小尺寸
void SetMinSize(int index, float minSize);
```

---

### ImVerticalSplitter - 垂直分割器

**继承关系**: `ImPanelWidget -> ImVerticalSplitter`

**特性**: 支持拖拽调整比例的垂直分割器，与水平分割器类似

#### ImVerticalSplitterStyle - 分割器样式

```cpp
struct ImVerticalSplitterStyle : public ImObject {
    float BarHeight = 4.0f;       // 分隔条高度
    ImU32 Color;                  // 普通颜色
    ImU32 HoveredColor;           // 悬停颜色
    ImU32 ActiveColor;            // 活动颜色
    float Rounding = 0.0f;        // 圆角半径
};
```

#### 关键API

```cpp
// 设置样式
void SetSplitterStyle(const ImVerticalSplitterStyle& style);

// 添加分割部分
ImVerticalSplitterSlot* AddPart(ImWidget* widget, float ratio = 1.0f);

// 设置最小尺寸
void SetMinSize(int index, float minSize);
```

---

### ImCanvasPanel - 画布面板

**继承关系**: `ImPanelWidget -> ImCanvasPanel`

**特性**: 支持绝对位置布局的画布容器

#### ImCanvasPanelSlot - 画布插槽

```cpp
class ImCanvasPanelSlot : public ImSlot {
    ImVec2 RelativePosition;  // 相对位置 (0.0-1.0)
    ImVec2 SlotSize;          // 插槽大小
};
```

#### 关键API

```cpp
// 添加子控件
ImCanvasPanelSlot* AddChildToCanvasPanel(ImWidget* child);

// 布局特性
// 子控件使用相对位置 (0.0-1.0) 和相对大小进行布局
// Position = ParentPosition + RelativePosition * ParentSize
// Size = SlotSize * ParentSize
```

---

### ImExpandableBox - 可展开容器

**继承关系**: `ImPanelWidget -> ImExpandableBox`

**特性**: 带折叠按钮的可展开容器，支持头部和体部分离

#### 关键API

```cpp
// 设置头部和主体
ImPaddingSlot* SetHead(ImWidget* HeadWidget);
ImPaddingSlot* SetBody(ImWidget* BodyWidget);

// 展开状态
void SetExpandedState(bool newstate);
bool GetIfExpanded();

// 样式设置
void SetHoveredTriangleColor(ImU32 color);
bool IsHovered() const;

// 回调函数
void SetOnExpandedStateChanged(std::function<void(bool)> callback);
void SetOnHoverBegin(std::function<void(void)> callback);
void SetOnHoverEnd(std::function<void(void)> callback);
```

#### 属性列表（ROP）
- `IsExpanded` (Bool) - 是否展开
- `TriangleSize` (Float) - 三角形大小
- `HeadPad` (Float) - 头部内边距
- `BodyPad` (Float) - 体内边距
- `TriangleColor` (Color) - 三角形颜色
- `HoveredTriangleColor` (Color) - 悬停时三角形颜色

---

### ImButton - 按钮控件

**继承关系**: `ImPanelWidget -> ImButton`

**特性**: 支持多种状态样式、工具提示、焦点状态的按钮

#### ButtonStateStyle - 按钮状态样式

```cpp
struct ButtonStateStyle : public ImObject {
    ImU32 BackgroundColor = IM_COL32(0, 0, 0, 0);
    float Rounding = 0.0f;
    bool HasBorder = false;
    float BorderThickness = 1.0f;
    ImU32 BorderColor = IM_COL32(0, 0, 0, 0);
};
```

#### 关键API

```cpp
// 内容管理
void SetContent(ImWidget* child, bool bDeleteOld = true);
ImWidget* GetContent();
ImPaddingSlot* GetContentSlot();

// 工具提示
void SetTooltipText(const std::string& text);

// 回调函数
void SetOnPressed(std::function<void(void)> callback);
void SetOnReleased(std::function<void(void)> callback);
void SetOnHoverBegin(std::function<void(void)> callback);
void SetOnFocusGained(std::function<void(void)> callback);
void SetOnFocusLost(std::function<void(void)> callback);

// 委托（多播）
ImMulticastDelegate<> OnRightClicked;
ImMulticastDelegate<> OnLeftClicked;
ImMulticastDelegate<> OnDoubleClicked;
ImMulticastDelegate<> OnMouseHoverIn;
ImMulticastDelegate<> OnMouseHover;
ImMulticastDelegate<> OnMouseHoverOut;

// 状态样式
void SetNormalStyle(const ButtonStateStyle& style);
void SetHoveredStyle(const ButtonStateStyle& style);
void SetPressedStyle(const ButtonStateStyle& style);
void SetFocusedStyle(const ButtonStateStyle& style);

// 状态查询
bool IsHovered() const;
bool IsPressed() const;
```

#### 属性列表（ROP）
- `TooltipText` (String) - 工具提示文本
- `OriginalMinSize` (Vec2) - 原始最小尺寸
- `NormalStyle` (Struct) - 正常状态样式
- `HoveredStyle` (Struct) - 悬停状态样式
- `PressedStyle` (Struct) - 按下状态样式
- `FocusedStyle` (Struct) - 焦点状态样式

---

### ImInputText - 文本输入控件

**继承关系**: `ImWidget -> ImInputText`

**特性**: 支持多种输入模式、文本验证、选择、复制粘贴的文本输入控件

#### 输入模式枚举

```cpp
enum class ImInputTextMode {
    Text,       // 普通文本模式
    Integer,    // 只允许输入整数
    Decimal     // 只允许输入小数
};
```

#### 关键API

```cpp
// 文本操作
void SetText(const std::string& text);
const std::string& GetText() const;

// 样式设置
void SetTextColor(ImU32 color);
void SetBackgroundColor(ImU32 color);
void SetBorderColor(ImU32 color);
void SetSelectionColor(ImU32 color);
void SetSelectionTextColor(ImU32 color);
void SetBorderThickness(float thickness);
void SetRounding(float rounding);

// 输入模式
void SetInputMode(const ImInputTextMode& mode);
void SetAllowNegative(bool allow);
void SetMaxIntegerDigits(int digits);
void SetMaxDecimalDigits(int digits);

// 验证
void SetValidationCallback(std::function<bool(const std::string&)> callback);
bool IsValidInput(const std::string& text) const;
bool IsCurrentTextValid() const;

// 回调函数
void SetOnTextChanged(std::function<void(const std::string&)> callback);
ImMulticastDelegate<const std::string&> OnTextCommit;
```

#### 属性列表（ROP）
- `Text` (String) - 文本内容
- `TextColor` (Color) - 文本颜色
- `BackgroundColor` (Color) - 背景颜色
- `BorderColor` (Color) - 边框颜色
- `SelectionColor` (Color) - 选中背景颜色
- `SelectionTextColor` (Color) - 选中文本颜色
- `BorderThickness` (Float) - 边框粗细
- `Rounding` (Float) - 圆角半径
- `InputMode` (Enum) - 输入模式（文本/整数/小数）
- `AllowNegative` (Bool) - 允许负数
- `MaxIntegerDigits` (Int) - 最大整数位数
- `MaxDecimalDigits` (Int) - 最大小数位数

---

### ImSingleLineInputText - 单行输入文本

**继承关系**: `ImWidget -> ImSingleLineInputText`

**特性**: 简化的单行文本输入控件，使用ImGui原生InputTextEx

#### 关键API

```cpp
// 基于ImGui的简化输入控件
// 使用固定128字节缓冲区
// 支持自定义背景颜色
```

---

### ImIntInput - 整数输入框

**继承关系**: `ImInputText -> ImIntInput`

**特性**: 专门用于整数输入的控件，支持范围限制和验证

#### 关键API

```cpp
// 范围设置
void SetRange(int minVal, int maxVal);

// 值操作
int GetValue() const;
void SetValue(int value);

// 回调函数
void SetOnIntValueChanged(std::function<void(int)> callback);
```

#### 属性列表（ROP）
- `MinValue` (Int) - 最小值
- `MaxValue` (Int) - 最大值

---

### ImFloatInput - 浮点数输入框

**继承关系**: `ImInputText -> ImFloatInput`

**特性**: 专门用于浮点数输入的控件，支持小数位控制和范围限制

#### 关键API

```cpp
// 范围设置
void SetRange(float minVal, float maxVal);

// 小数位控制
void SetDecimalPlaces(int places);

// 值操作
float GetValue() const;
void SetValue(float value);

// 回调函数
void SetOnFloatValueChanged(std::function<void(float)> callback);
```

#### 属性列表（ROP）
- `MinValue` (Float) - 最小值
- `MaxValue` (Float) - 最大值
- `DecimalPlaces` (Int) - 小数位数

---

### ImMultiLineTextBlock - 多行文本块

**继承关系**: `ImWidget -> ImMultiLineTextBlock`

**特性**: 支持自动换行、文本选择、全局多控件选择的多行文本显示

#### 关键API

```cpp
// 文本操作
void SetText(const std::string& Text);
void SetTextColor(ImU32 Color);
void SetBackgroundColor(ImU32 Color);
void SetSelectionColor(ImU32 Color);
void SetLineSpacing(float Spacing);

// 选择操作
void SetSelection(int startindex, int endindex);
void ClearSelection();

// 全局选择（跨多个控件）
static void ClearAllSelections();
static void CopyAllSelectedTexts();
```

---

### ImColorPicker - 颜色选择器

**继承关系**: `ImWidget -> ImColorPicker`

**特性**: 完整的颜色选择器，支持HSV/RGB转换、透明度控制、十六进制输入

#### 关键API

```cpp
// 颜色设置
void SetColor(const ImVec4& color);  // RGBA (0-1)
void SetColor(const ImU32& color);
const ImVec4& GetColor() const;

// HSV操作
void GetHSV(float& h, float& s, float& v) const;
void SetFromHSV(float h, float s, float v);

// 样式设置
void SetSVAreaSize(float size);

// 回调函数
void SetOnColorChanged(std::function<void(const ImU32&)> callback);
```

---

### ImMenuButton - 菜单按钮

**继承关系**: `ImButton -> ImMenuButton`

**特性**: 带下拉菜单的按钮，支持四个方向停靠

#### 停靠方向枚举

```cpp
enum class MenuDockDirection {
    Dock_Up,     // 向上展开
    Dock_Down,   // 向下展开
    Dock_Left,   // 向左展开
    Dock_Right   // 向右展开
};
```

#### 关键API

```cpp
// 设置菜单内容
void SetMenu(ImWidget* MenuWidget);

// 停靠方向
void SetDockDirection(MenuDockDirection direction);

// 菜单窗口大小
void SetMenuWindowSize(const ImVec2& size);
```

---

### ImCurve - 曲线图

**继承关系**: `ImWidget -> ImCurve`

**特性**: 支持多曲线、坐标轴、网格、图例的曲线图控件

#### CurveData - 曲线数据结构

```cpp
struct CurveData {
    std::vector<ImVec2> points;     // 曲线上的点
    ImU32 color;                    // 曲线颜色
    float thickness;                // 曲线粗细
    std::string label;              // 曲线标签
};
```

#### 关键API

```cpp
// 坐标轴设置
void SetAxisRange(float xMin, float xMax, float yMin, float yMax);
void SetOrigin(float x, float y);
void SetAxisNames(const std::string& xName, const std::string& yName);
void SetTickInterval(float xInterval, float yInterval);

// 曲线操作
void AddCurve(const std::vector<ImVec2>& points, ImU32 color, float thickness, const std::string& label = "");
void ClearCurves();

// 样式设置
void SetAxisColor(ImU32 color);
void SetGridColor(ImU32 color);
void SetBackgroundColor(ImU32 color);
void ShowGrid(bool show);
void ShowLabels(bool show);
```

---

### ImScrollingTextList - 滚动文本列表

**继承关系**: `ImWidget -> ImScrollingTextList`

**特性**: 支持滚动、文本选择、自动换行的文本列表控件

#### 关键API

```cpp
// 文本管理
void AddItem(const std::string& text, ImU32 color = IM_COL32(0, 0, 0, 255));
void Clear();
void ModifyItem(size_t index, const std::string& newText, ImU32 newColor);
void RemoveItem(size_t index);

// 样式设置
void SetTextColor(ImU32 color);
void SetItemColor(size_t index, ImU32 color);
void SetAllItemsColor(ImU32 color);
void SetLineSpacing(float spacing);

// 滚动条样式
// 滚动条相关属性通过ROP属性系统配置
```

#### 属性列表（ROP）
- `LineSpacing` (Float) - 行间距
- `ScrollbarThickness` (Float) - 滚动条厚度

---

### ImDesignPanel - 设计器面板

**继承关系**: `ImPanelWidget -> ImDesignPanel`

**特性**: 支持可视化编辑、控件选择、大小调整的设计器面板

#### 关键API

```cpp
// 选择操作
void SetSelectedWidget(ImWidget* SelectedWidget, bool bTriggerCallBack = false);
ImWidget* GetSelectedWidget() const;

// 回调函数
void SetOnSelected(std::function<void(ImWidget*)> callback);
void SetOnUnSelected(std::function<void()> callback);

// 布局特性
// 只允许一个子控件（设计内容）
```

---

## 代码示例

### 使用复选框

```cpp
#include "ImWidget/ImCheckBox.h"

ImCheckBox* checkbox = new ImCheckBox("MyCheckBox");
checkbox->SetPosition(ImVec2(10, 10));
checkbox->SetSize(ImVec2(20, 20));

// 设置回调
checkbox->SetOnToggled([](bool checked) {
    std::cout << "Checkbox state: " << (checked ? "checked" : "unchecked") << std::endl;
});

// 设置样式
checkbox->SetBoxColor(IM_COL32(100, 100, 100, 255));
checkbox->SetCheckColor(IM_COL32(0, 0, 255, 255));
```

### 使用下拉选择框

```cpp
#include "ImWidget/ImComboBox.h"

ImComboBox* combo = new ImComboBox("MyCombo");
combo->SetPosition(ImVec2(10, 10));
combo->SetSize(ImVec2(150, 30));

// 设置选项
combo->SetItems({"Option 1", "Option 2", "Option 3"});

// 设置回调
combo->SetOnSelectionChanged([](int index) {
    std::cout << "Selected index: " << index << std::endl;
});
```

### 使用滑块

```cpp
#include "ImWidget/ImSlider.h"

ImSlider* slider = new ImSlider("MySlider");
slider->SetPosition(ImVec2(10, 10));
slider->SetSize(ImVec2(200, 20));

// 设置值范围
slider->v_Min = 0.0f;
slider->v_Max = 100.0f;
slider->format = "%.1f";

// 设置幂函数（非线性）
slider->power = 2.0f;  // 二次曲线

// 设置样式
slider->m_Style.SetDarkTheme();
```

### 使用图像控件

```cpp
#include "ImWidget/ImImage.h"

// 从文件加载
ImImage* image = new ImImage("MyImage", "path/to/image.png");
image->SetPosition(ImVec2(0, 0));
image->SetSize(ImVec2(200, 200));

// 或使用现有纹理ID
// ImImage* image = new ImImage("MyImage", textureID, width, height);
```

### 使用可调整大小容器

```cpp
#include "ImWidget/ImResizableBox.h"

ImResizableBox* resizable = new ImResizableBox("MyResizableBox");
resizable->SetPosition(ImVec2(50, 50));
resizable->SetSize(ImVec2(200, 150));

// 设置内容
ImTextBlock* content = new ImTextBlock("Content");
content->SetText("Resizable Content");
resizable->SetContent(content);

// 设置回调
resizable->SetOnResized([](ImVec2 pos, ImVec2 size) {
    std::cout << "New size: " << size.x << " x " << size.y << std::endl;
});
```

### 使用滚动容器

```cpp
#include "ImWidget/ImScrollBox.h"

ImScrollBox* scrollBox = new ImScrollBox("MyScrollBox");
scrollBox->SetPosition(ImVec2(10, 10));
scrollBox->SetSize(ImVec2(300, 200));

// 添加大内容
ImVerticalBox* content = new ImVerticalBox("Content");
for (int i = 0; i < 20; i++) {
    ImButton* btn = new ImButton("Button" + std::to_string(i));
    btn->SetSize(ImVec2(280, 30));
    content->AddChild(btn);
}

scrollBox->SetContent(content);

// 滚动到特定控件
ImButton* targetBtn = static_cast<ImButton*>(content->GetChildAt(15));
scrollBox->ScrollToWidget(targetBtn);
```

---

## 变更记录 (Changelog)
#### 2026-02-05 v4.0.0
- 完成剩余16个控件的深度扫描和文档补充
- 新增ImVerticalBox完整文档（垂直布局、比例控制）
- 新增ImHorizontalSplitter完整文档（水平分割、拖拽调整、样式）
- 新增ImVerticalSplitter完整文档（垂直分割、拖拽调整）
- 新增ImCanvasPanel完整文档（绝对位置布局）
- 新增ImExpandableBox完整文档（可展开容器、折叠按钮）
- 新增ImButton完整文档（多状态样式、工具提示、委托回调）
- 新增ImInputText完整文档（输入模式、验证、选择、复制粘贴）
- 新增ImSingleLineInputText完整文档（简化单行输入）
- 新增ImIntInput完整文档（整数输入、范围限制）
- 新增ImFloatInput完整文档（浮点数输入、小数位控制）
- 新增ImMultiLineTextBlock完整文档（自动换行、多控件选择）
- 新增ImColorPicker完整文档（HSV/RGB、透明度、十六进制）
- 新增ImMenuButton完整文档（菜单按钮、四向停靠）
- 新增ImCurve完整文档（多曲线、坐标轴、网格、图例）
- 新增ImScrollingTextList完整文档（滚动文本列表、选择、换行）
- 新增ImDesignPanel完整文档（可视化编辑、选择、调整）
- 覆盖率达到100%（27/27个控件类）

#### 2026-02-05 v3.0.0
- 深度补捞完成剩余控件详细扫描
- 新增ImCheckBox完整文档（状态、样式、API、属性）
- 新增ImComboBox完整文档（选项管理、样式、API、属性）
- 新增ImTextBlock完整文档（对齐、字体、属性）
- 新增ImImage完整文档（纹理、宽高比、API、属性）
- 新增ImSlider完整文档（值范围、句柄类型、幂函数、样式、属性）
- 新增ImHorizontalBox完整文档（水平布局、比例控制）
- 新增ImScrollBox完整文档（滚动控制、样式、API、属性）
- 新增ImSimpleFigure完整文档（图形类型、样式、属性）
- 新增ImBorder完整文档（边框容器、API）
- 新增ImResizableBox完整文档（控制点、样式、回调、API）
- 新增多个控件使用示例
- 覆盖率从87%提升到95%+

#### 2026-02-05 v2.0.0
- 深度补捞阶段C扫描完成
- 新增完整的控件类继承关系图
- 新增ROP属性系统详细文档（属性类型、接口、示例）
- 新增事件系统完整的事件类型和用法
- 新增所有公开API的详细说明
- 新增关键设计模式的识别和说明
- 新增多个代码示例
- 新增ButtonStateStyle样式类文档
- 新增路径属性访问接口文档
- 覆盖率从33%提升到85%+

#### 2024-02-05 v1.2.0
- 首次AI上下文初始化
- 识别核心控件类和系统架构
- 建立完整文件清单
