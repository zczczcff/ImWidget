[根目录](../../CLAUDE.md) > **ImCreator**

## ImCreator - UI创建器

### 模块职责
ImCreator是一个基于ImWidget库的UI创建工具，提供组件库、文件管理、项目管理等功能，支持通过代码方式创建和编辑UI界面。

### 入口与启动
- **入口文件**: `Include/ImBasicWidgetDeclaration.h` - 基础控件声明
- **主入口**: `Src/ImCreator_main.cpp` - 应用程序入口和主界面初始化
- **全局应用类**: `MyApp` - 主应用逻辑容器

### 对外接口
#### UI组件
- `WidgetTreeView` - 控件树形视图，显示可用控件
- `ImDesignPanel` - 设计面板，用于编辑控件属性
- `DetailList` - 属性列表，显示和编辑选中控件的属性
- `ImCreator_PageManager` - 页面管理器，管理多个UI页面

#### 文件管理
- `FileUtil` - 文件工具，支持文件扫描、路径处理
- `ImCreator_Projectconfig` - 项目配置管理
- `ExampleWidget` - 示例控件包装器

#### 事件系统
- `EventBus` - 简单的事件总线实现
- 支持全局事件订阅和发布

### 关键依赖与配置
- **ImWidget**: 核心UI库依赖
- **平台支持**:
  - Windows 64-bit: DirectX 11后端
  - Linux ARM: OpenGL ES 2.0后端
- **编译配置**: CMake构建系统，支持跨平台编译

### 数据模型
#### 应用架构
- 采用MVC模式（Model-View-Controller）
- `MyApp`作为主容器，管理所有UI元素
- 页面管理器支持多页面切换
- 控件树支持层级嵌套和选择

#### 主要类结构
- `MyApp`: 主应用类，包含所有UI组件
- `ImCreatorUIPageManager`: 页面管理器
- `WidgetTreeView`: 控件树视图
- `DetailList`: 属性编辑列表
- `ExampleWidget`: 示例控件包装

### 测试与质量
- 暂无独立测试目录
- 通过事件系统实现组件间通信
- 使用文件工具进行项目文件管理

### 常见问题 (FAQ)
1. **Q**: 如何创建新的UI文件？
   **A**: 通过"Project"菜单的"NewUI"选项，输入文件名即可创建.imui文件

2. **Q**: 如何保存UI设计？
   **A**: 使用"Project"菜单的"Save"选项，将设计保存到指定文件夹

3. **Q**: 如何生成C++代码？
   **A**: 使用"Project"菜单的"Gen Code"选项，会生成对应的.h和.cpp文件

### 相关文件清单
```
Include/
├── ExampleWidgetInfor.h
├── FileUtil.h
├── ImCreator_PageManager.h
├── ImCreator_Projectconfig.h
├── ImDesignPanel.h
├── ImExampleWidget.h
├── WidgetTreeView.h
└── EventBus.h

Src/
└── ImCreator_main.cpp  # 主入口和初始化逻辑

cmake/
├── linux-arm-toolchain.cmake
└── win64-toolchain.cmake
```

### 主要功能流程
1. **初始化**:
   - 创建主布局容器
   - 初始化控件树和属性面板
   - 加载现有UI文件
   - 设置事件回调

2. **UI编辑**:
   - 从控件树选择控件
   - 在属性面板编辑控件属性
   - 在设计面板预览效果

3. **文件操作**:
   - 扫描指定目录的.imui文件
   - 支持多文件切换编辑
   - 保存UI设计到JSON格式

4. **代码生成**:
   - 从UI设计生成C++类代码
   - 生成头文件和实现文件

### 变更记录 (Changelog)
### 2024-02-05 v1.2.0
- 首次AI上下文初始化
- 识别UI创建器的主要功能和组件
- 建立文件清单和架构描述