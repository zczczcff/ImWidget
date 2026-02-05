[根目录](../../CLAUDE.md) > **ImDesigner**

## ImDesigner - UI设计器

### 模块职责
ImDesigner是一个功能完整的UI设计器，支持可视化编辑、代码生成、命令撤销/重做等高级功能。采用MVC架构，提供项目管理、控件编辑、属性编辑等全方位的UI设计能力。

### 入口与启动
- **入口文件**: `Include/UI/MainUI.h` - 主界面类
- **主入口**: `Source/ImDesigner_main.cpp` - 应用程序入口，初始化MVC组件
- **全局实例**:
  - `MainUI* global_MainUI` - 主界面
  - `Model_MainModel* global_MainModel` - 主数据模型
  - `Controller_MainController* global_MainController` - 主控制器

### 对外接口

#### UI视图层 (UI/)
- `MainUI` - 主界面容器
- `UI_ProjectView` - 项目文件视图
- `UI_WidgetEditor` - 控件编辑器
- `UI_DetailView` - 属性详情视图
- `UI_WidgetTreeView` - 控件树形视图
- `UI_ImUserWidgetClassOutlineView` - 用户控件类大纲视图
- `UI_UserWidgetClassView` - 用户控件类视图
- `EditorViewBase` - 编辑器基类
- `IconManager` - 图标管理器

#### 模型层 (Model/)
- `Model_MainModel` - 主数据模型
- `Model_UserWidgetClassEditor` - 用户控件类编辑模型
- `Model_UIFile` - UI文件模型
- `Projectconfig` - 项目配置

#### 控制器层 (Controller/)
- `Controller_MainController` - 主控制器
- `Controller_WidgetEditor` - 控件编辑控制器

#### 命令系统 (Model/Command/)
- `ImDesignerCommandBase` - 命令基类
- `ImDesignerCommandManager` - 命令管理器
- `ImDesignerCommandType` - 命令类型定义
- `ImDesignerCommandData` - 命令数据
- `Command_WidgetVarPropertyEdit` - 控件变量属性编辑命令
- `Command_WidgetSlotPropertyEdit` - 控件槽属性编辑命令
- `Command_ObjectVarPropertyEdit` - 对象变量属性编辑命令
- `Command_ClassStringPropertyEdit` - 类字符串属性编辑命令
- `Command_VarRename` - 变量重命名命令
- `Command_ChildWidgetOperation` - 子控件操作命令
- `Command_VariableOperation` - 变量操作命令

#### 资源层 (Resource/)
- 完整的控件资源定义（Button, TextBlock, Slider等）

#### 工具层 (Tools/)
- `JAsyncLog` - 异步日志工具
- `JLog` - 日志工具
- `ProjectFileManager` - 项目文件管理器

#### 编辑器系统
- `EditorEventObject` - 编辑器事件对象基类
- `EditorEventInterface` - 编辑器事件接口
- `EditorGlobal.h/.cpp` - 编辑器全局变量和函数
- `EditorGlobalInterface` - 编辑器全局接口
- `EditorAction` - 编辑器动作定义
- `EditorEvents` - 编辑器事件定义

### 关键依赖与配置
- **ImWidget**: 核心UI库依赖
- **EditorKit**: 编辑器工具包依赖
- **平台支持**:
  - Windows 64-bit: DirectX 11后端
  - Linux ARM: OpenGL ES 2.0后端
- **编译配置**: CMake构建系统，支持跨平台编译
- **配置文件**: ProjectConfig.json - 项目配置文件

### 数据模型

#### MVC架构
- **Model**: 管理UI文件、编辑状态、命令历史
- **View**: 提供可视化界面，包括主界面、编辑器、属性面板等
- **Controller**: 处理用户输入，协调Model和View

#### 核心数据结构
```cpp
// 编辑的UI文件
struct EditedUIFile {
    std::string FileFullPath;
    ImUserWidgetClass* EditedFile;
    Model_ImUserWidgetClassEditor* model_editor;
};
```

#### 命令系统
- 支持撤销/重做
- 命令类型：属性编辑、变量操作、控件操作等
- 命令管理器：统一管理所有编辑操作

### 测试与质量
- 暂无独立测试目录
- 通过命令系统实现撤销/重做功能
- 使用异步日志记录编辑操作

### 常见问题 (FAQ)
1. **Q**: 如何撤销编辑操作？
   **A**: 点击界面上的撤销按钮或使用快捷键，命令系统会自动回滚操作

2. **Q**: 如何重命名文件？
   **A**: 使用项目视图中的重命名功能，系统会自动更新所有相关引用

3. **Q**: 如何生成代码？
   **A**: 使用主界面的生成按钮，系统会根据当前编辑的UI文件生成对应的C++代码

### 相关文件清单
```
Include/
├── UI/                      # 视图层
│   ├── MainUI.h
│   ├── UI_ProjectView.h
│   ├── UI_WidgetEditor.h
│   ├── UI_DetailView.h
│   ├── UI_WidgetTreeView.h
│   ├── UI_ImUserWidgetClassOutlineView.h
│   ├── UI_UserWidgetClassView.h
│   ├── EditorViewBase.h
│   └── IconManager.h
├── Model/                   # 模型层
│   ├── Model_MainModel.h
│   ├── Model_UserWidgetClassEditor.h
│   ├── Model_UIFile.h
│   ├── Projectconfig.h
│   └── Command/             # 命令系统
│       ├── ImDesignerCommandBase.h
│       ├── ImDesignerCommandManager.h
│       ├── Command_WidgetVarPropertyEdit.h
│       ├── Command_WidgetSlotPropertyEdit.h
│       ├── Command_ObjectVarPropertyEdit.h
│       ├── Command_ClassStringPropertyEdit.h
│       ├── Command_VarRename.h
│       ├── Command_ChildWidgetOperation.h
│       └── Command_VariableOperation.h
├── Controller/              # 控制器层
│   ├── Controller_MainController.h
│   └── Controller_WidgetEditor.h
├── Resource/               # 资源层
│   ├── Button.h/.cpp
│   ├── TextBlock.h/.cpp
│   ├── Slider.h/.cpp
│   ├── ... (所有控件资源)
├── Tools/                  # 工具层
│   ├── JAsyncLog.h/.cpp
│   ├── JLog.h/.cpp
│   └── ProjectFileManager.h
├── EditorGlobal.h          # 编辑器全局定义
├── EditorGlobalInterface.h
├── EditorAction.h
├── EditorEvents.h
└── EditorEventObject.h     # 编辑器事件对象基类

Source/
├── ImDesigner_main.cpp     # 主入口
├── EditorGlobal.cpp
├── EditorGlobalInterface.cpp
├── EditorAction.cpp
├── EditorEvents.cpp
├── UI/                     # 视图层实现
├── Model/                  # 模型层实现
├── Controller/             # 控制器层实现
├── Resource/               # 资源层实现
├── Tools/                  # 工具层实现
└── UI_Init/                # UI初始化代码
```

### 主要功能流程

#### 1. 文件编辑流程
```
打开文件 -> 加载ImUserWidgetClass -> 创建编辑器页面
      -> 创建大纲视图 -> 创建详情视图 -> 显示编辑界面
```

#### 2. 控件编辑流程
```
选择控件 -> 显示属性 -> 编辑属性 -> 创建编辑命令
      -> 执行命令 -> 更新视图 -> 记录到命令历史
```

#### 3. 代码生成流程
```
编辑完成 -> 保存文件 -> 生成C++代码
      -> 输出.h和.cpp文件
```

### 变更记录 (Changelog)
### 2024-02-05 v1.2.0
- 首次AI上下文初始化
- 识别MVC架构和命令系统
- 建立完整的文件清单和功能描述