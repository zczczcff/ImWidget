[根目录](../../CLAUDE.md) > **ImWidget**

## ImWidget核心库

### 模块职责
ImWidget是项目的核心UI框架库，提供基础控件、事件系统、序列化、代码生成等核心功能。基于ImGui库构建，支持跨平台（Windows、Linux ARM）运行。

### 入口与启动
- **入口文件**: `ImWidgetConfig.h` - ImGui配置定义
- **主入口**: `ImMain.cpp` - 平台无关的启动入口，Windows和Linux适配
- **核心实例**: `ImGlobalInstance.cpp` - 全局实例管理

### 对外接口
#### 基础控件类
- `ImButton` - 按钮控件
- `ImCheckBox` - 复选框控件
- `ImComboBox` - 下拉选择框
- `ImInputText` - 单行文本输入
- `ImIntInput` - 整数输入框
- `ImFloatInput` - 浮点数输入框
- `ImTextBlock` - 文本块
- `ImMultiLineTextBlock` - 多行文本块
- `ImImage` - 图像控件

#### 容器控件类
- `ImHorizontalBox` - 水平容器
- `ImVerticalBox` - 垂直容器
- `ImHorizontalSplitter` - 水平分割器
- `ImVerticalSplitter` - 垂直分割器
- `ImScrollBox` - 滚动容器
- `ImCanvasPanel` - 画布面板
- `ImExpandableBox` - 可展开容器

#### 特殊控件类
- `ImSlider` - 滑块控件
- `ImScrollingTextList` - 滚动文本列表
- `ImSimpleFigure` - 简单图形
- `ImCurve` - 曲线图
- `ImBorder` - 边框控件
- `ImColorPicker` - 颜色选择器
- `ImResizeableBox` - 可调整大小容器

#### 核心系统类
- `ImWidget` - 所有控件基类
- `ImPanelWidget` - 容器控件基类
- `ImUserWidget` - 用户自定义控件基类
- `ImObject` - 通用对象基类
- `ImSlot` - 插槽类（用于嵌套布局）

### 关键依赖与配置
- **ImGui**: UI渲染核心，位于`Extern/imgui-master`
- **nlohmann_json**: JSON序列化库，位于`Extern/nlohmann_json`
- **stb_image**: 图像加载库，位于`Extern/stb_image`
- **ROP**: 属性系统库，位于`Extern/ROP`
- **平台支持**:
  - Windows 64-bit: DirectX 11后端
  - Linux ARM: OpenGL ES 2.0后端

### 数据模型
#### 事件系统
- `ImEvent` - 事件基类
- `ImMouseEvent` - 鼠标事件
- `ImKeyEvent` - 键盘事件
- `ImDragEvent` - 拖拽事件
- `ImHoverEvent` - 悬停事件
- `ImFocusEvent` - 焦点事件
- `ImEventSystem` - 事件管理系统

#### 序列化系统
- `ImUserWidgetClass` - 用户控件类定义
- `ImUserWidgetClassVariable` - 控件类变量
- `ImUserWidgetSerializer` - 序列化器
- `ImUserWidgetClassCodeGenerator` - 代码生成器

#### 工厂系统
- `ImWidgetFactory` - 控件工厂
- `ImObjectFactory` - 对象工厂

### 测试与质量
- 暂无独立测试目录，测试集成在Test模块中
- 使用ROP属性系统进行运行时属性访问
- 支持序列化到JSON格式
- 代码生成功能：从用户控件类定义生成C++源文件

### 常见问题 (FAQ)
1. **Q**: 如何创建自定义控件？
   **A**: 继承`ImWidget`类，实现必要的虚函数，通过`ImWidgetFactory`注册

2. **Q**: 如何保存和加载UI布局？
   **A**: 使用`ImUserWidgetSerializer`进行序列化，支持JSON格式

3. **Q**: 如何实现控件间的通信？
   **A**: 使用事件系统，通过`ImEventSystem`发布和订阅事件

### 相关文件清单
```
Source/
├── Application/          # 应用层实现
│   ├── ImApplication.cpp/.h
│   ├── ImWin64Application.cpp/.h
│   ├── ImPiApplication.cpp/.h
│   └── CHFONT.cpp/.h
├── ImEvent/            # 事件系统
│   ├── ImEvent.cpp/.h
│   ├── ImMouseEvent.cpp/.h
│   ├── ImKeyEvent.cpp/.h
│   ├── ImDragEvent.cpp/.h
│   ├── ImHoverEvent.cpp/.h
│   ├── ImFocusEvent.cpp/.h
│   └── ImEventSystem.cpp/.h
├── ImTools/            # 工具类
│   ├── ImWidgetControlBlock.cpp/.h
│   ├── ImWidgetRef.cpp/.h
│   ├── ImDelegate.cpp/.h
│   └── DelayEventQueue.cpp/.h
├── ImWidget/           # 控件实现
│   ├── ImWidget.cpp/.h
│   ├── ImPanelWidget.cpp/.h
│   ├── ImUserWidget.cpp/.h
│   ├── ImButton.cpp/.h
│   ├── ImVerticalBox.cpp/.h
│   ├── ImHorizontalBox.cpp/.h
│   ├── ImTextBlock.cpp/.h
│   ├── ImInputText.cpp/.h
│   └── ... (其他控件实现)
└── ImWindows/          # 窗口管理
    ├── ImWindow.cpp/.h
    ├── ImWindowManager.cpp/.h
    └── ImPageManager.cpp/.h

Include/
├── Application/
├── ImEvent/
├── ImTools/
├── ImWidget/
└── ImWindows/
```

### 变更记录 (Changelog)
### 2024-02-05 v1.2.0
- 首次AI上下文初始化
- 识别核心控件类和系统架构
- 建立完整文件清单