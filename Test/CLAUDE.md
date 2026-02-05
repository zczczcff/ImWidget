[根目录](../CLAUDE.md) > **Test**

## Test - 单元测试模块

### 模块职责
Test模块是ImWidget项目的单元测试模块，用于验证核心功能的正确性，包括序列化、代码生成、用户控件类等功能。

### 入口与启动
- **入口文件**: `Source/ImUserWidgetClassTest.cpp` - 主测试入口
- **可执行文件**: `ImWidgetTest` - 测试程序

### 对外接口
#### 主要测试函数
- `TestUserWidgetClassSerialization()` - 用户控件类序列化测试

### 关键依赖与配置
- **ImWidget**: 核心UI库依赖
- **测试库**: 可选Catch2（已在CMakeLists.txt中注释）
- **平台支持**:
  - Windows 64-bit
  - Linux ARM
- **编译配置**: CMake构建系统

### 测试用例描述

#### 1. 用户控件类序列化测试 (`TestUserWidgetClassSerialization`)
此测试验证ImUserWidgetClass的完整功能链：

1. **创建ImUserWidgetClass对象**
   - 验证类名创建

2. **添加基本变量**
   - Int类型变量
   - Float类型变量
   - Bool类型变量
   - String类型变量
   - Color类型变量
   - Vec2类型变量

3. **添加控件变量**
   - ImButton控件变量
   - 使用ROP属性系统设置控件属性

4. **添加ImObject变量**
   - 创建通用对象变量
   - 设置对象属性

5. **变量重命名**
   - 验证变量重命名功能

6. **序列化**
   - 使用ImUserWidgetClassSerializer序列化到JSON
   - 验证序列化结果

7. **验证序列化数据**
   - 检查类名
   - 检查默认根控件
   - 检查变量数量和类型

8. **反序列化**
   - 从JSON恢复ImUserWidgetClass对象
   - 验证恢复的正确性

9. **文件操作**
   - 保存到文件
   - 从文件加载
   - 验证文件操作正确性

10. **代码生成**
    - 使用ImUserWidgetClassCodeGenerator生成C++代码
    - 输出.h和.cpp文件

### 测试执行流程

```
main() -> ImInit() -> TestUserWidgetClassSerialization()
         -> 执行所有测试步骤
         -> exit(1)
```

### 测试结果示例
```
=== 测试ImUserWidgetClass序列化功能 ===
1. 创建ImUserWidgetClass: TestWidgetClass
2. 添加基本变量:
  - 添加Int变量: IntVar_0
    设置初始值为: 42
  - 添加Float变量: FloatVar_1
    设置初始值为: 3.14
  - 添加Bool变量: BoolVar_2
    设置初始值为: true
  - 添加String变量: StringVar_3
    设置初始值为: Hello, World!
  - 添加Color变量: ColorVar_4
    设置初始值为: 红色 (ARGB: ff0000ff)
  - 添加Vec2变量: Vec2Var_5
    设置初始值为: (100.0, 200.0)
3. 添加控件变量:
  - 添加ImButton变量: WidgetVar_6
    - 设置按钮的文本为: Click Me
    - 设置为默认根控件
4. 添加ImObject变量:
  - 添加ImObject变量: ObjectVar_7
    - 设置对象主色为蓝色
5. 变量重命名:
  - 重命名变量 IntVar_0 为 RenamedInt
6. 序列化ImUserWidgetClass:
  {
    "ClassName": "TestWidgetClass",
    "DefaultRoot": "WidgetVar_6",
    "Variables": [...]
  }
7. 验证序列化数据:
  - 类名: TestWidgetClass
  - 默认根控件: WidgetVar_6
  - 总变量数: 8
  - 控件变量数: 1
  - 对象变量数: 1
  - 基本变量数: 6
8. 测试反序列化:
  - 反序列化成功!
  - 类名: TestWidgetClass
  - 默认根控件: WidgetVar_6
  - 总变量数: 8
  - 验证Int变量 RenamedInt = 42 (应为42)
  - 验证按钮控件 WidgetVar_6 存在
  - 按钮的文本: Click Me (应为Click Me)
9. 文件操作测试:
  - 保存到文件成功: test_widget_class.json
  - 从文件加载成功
  - 加载的类名: LoadedWidgetClass
  - 删除测试文件
10. .h/.cpp文件生成测试:
  - 成功生成C++代码
  - 头文件: TestGeneratedWidget.h
  - 源文件: TestGeneratedWidget.cpp
=== 测试完成 ===
```

### 测试覆盖范围

| 功能模块 | 测试状态 | 测试文件 |
|---------|---------|---------|
| ImUserWidgetClass创建 | 已测试 | ImUserWidgetClassTest.cpp |
| 基本变量管理 | 已测试 | ImUserWidgetClassTest.cpp |
| 控件变量管理 | 已测试 | ImUserWidgetClassTest.cpp |
| 对象变量管理 | 已测试 | ImUserWidgetClassTest.cpp |
| 变量重命名 | 已测试 | ImUserWidgetClassTest.cpp |
| JSON序列化 | 已测试 | ImUserWidgetClassTest.cpp |
| JSON反序列化 | 已测试 | ImUserWidgetClassTest.cpp |
| 文件保存/加载 | 已测试 | ImUserWidgetClassTest.cpp |
| 代码生成 | 已测试 | ImUserWidgetClassTest.cpp |
| 基础控件渲染 | 未测试 | - |
| 事件系统 | 未测试 | - |
| 命令系统 | 未测试 | - |

### 常见问题 (FAQ)
1. **Q**: 如何运行测试？
   **A**: 编译Test项目后，运行ImWidgetTest可执行文件

2. **Q**: 测试失败时如何调试？
   **A**: 在ImUserWidgetClassTest.cpp中设置断点，使用调试器逐步执行

3. **Q**: 如何添加新的测试用例？
   **A**: 在Source目录下创建新的.cpp文件，并在CMakeLists.txt中添加

### 相关文件清单
```
Source/
└── ImUserWidgetClassTest.cpp  # 主测试文件

cmake/
├── linux-arm-toolchain.cmake
└── win64-toolchain.cmake
```

### 待扩展测试
1. **基础控件测试**: 验证各个基础控件的创建和渲染
2. **事件系统测试**: 验证事件发布和订阅机制
3. **命令系统测试**: 验证撤销/重做功能
4. **文件管理测试**: 验证项目文件的创建、编辑、删除
5. **代码生成测试**: 验证生成的代码可以正确编译和使用

### 变更记录 (Changelog)
### 2024-02-05 v1.2.0
- 首次AI上下文初始化
- 识别序列化测试用例
- 建立测试覆盖清单