#include "Application/ImApplication.h"
#include "ImWidget/ImGlobalInstance.h"
#include "ImWidget/ImBasicWidgetDeclaration.h"
#include <iostream>
#include "ImUserWidgetClass.h"
#include "ImUserWidgetSerializer.h"
#include "ImUserWidgetClassCodeGenerator.h"
#include "ImWidgetFactory.h"
#include "ImObjectFactory.h"

using namespace ImGuiWidget;
using json = nlohmann::ordered_json;

void TestUserWidgetClassSerialization()
{
    std::cout << "=== 测试ImUserWidgetClass序列化功能 ===" << std::endl;

    // 1. 创建ImUserWidgetClass对象
    ImUserWidgetClass widgetClass("TestWidgetClass");
    std::cout << "1. 创建ImUserWidgetClass: " << widgetClass.GetClassName() << std::endl;

    // 2. 添加基本变量
    std::cout << "\n2. 添加基本变量:" << std::endl;

    std::string intVarName;
    if (widgetClass.AddBasicVariable(PropertyType::Int, intVarName))
    {
        std::cout << "  - 添加Int变量: " << intVarName << std::endl;

        // 设置初始值
        auto* intVar = widgetClass.GetBasicVariable(intVarName);
        if (intVar)
        {
            // 使用新的ImWidgetClassVariable_Basic接口
            intVar->SetValue<int>(42);
            int value = intVar->GetValue<int>();
            std::cout << "    设置值为: " << value << std::endl;
        }
    }

    std::string floatVarName;
    if (widgetClass.AddBasicVariable(PropertyType::Float, floatVarName))
    {
        std::cout << "  - 添加Float变量: " << floatVarName << std::endl;

        auto* floatVar = widgetClass.GetBasicVariable(floatVarName);
        if (floatVar)
        {
            floatVar->SetValue<float>(3.14f);
            float value = floatVar->GetValue<float>();
            std::cout << "    设置值为: " << value << std::endl;
        }
    }

    std::string boolVarName;
    if (widgetClass.AddBasicVariable(PropertyType::Bool, boolVarName))
    {
        std::cout << "  - 添加Bool变量: " << boolVarName << std::endl;

        auto* boolVar = widgetClass.GetBasicVariable(boolVarName);
        if (boolVar)
        {
            boolVar->SetValue<bool>(true);
            bool value = boolVar->GetValue<bool>();
            std::cout << "    设置值为: " << (value ? "true" : "false") << std::endl;
        }
    }

    std::string stringVarName;
    if (widgetClass.AddBasicVariable(PropertyType::String, stringVarName))
    {
        std::cout << "  - 添加String变量: " << stringVarName << std::endl;

        auto* stringVar = widgetClass.GetBasicVariable(stringVarName);
        if (stringVar)
        {
            stringVar->SetValue<std::string>("Hello, World!");
            std::string value = stringVar->GetValue<std::string>();
            std::cout << "    设置值为: " << value << std::endl;
        }
    }

    std::string colorVarName;
    if (widgetClass.AddBasicVariable(PropertyType::Color, colorVarName))
    {
        std::cout << "  - 添加Color变量: " << colorVarName << std::endl;

        auto* colorVar = widgetClass.GetBasicVariable(colorVarName);
        if (colorVar)
        {
            colorVar->SetValue<ImU32>(IM_COL32(255, 0, 0, 255)); // 红色
            ImU32 value = colorVar->GetValue<ImU32>();
            std::cout << "    设置值为: 红色 (ARGB: " << std::hex << value << std::dec << ")" << std::endl;
        }
    }

    std::string vec2VarName;
    if (widgetClass.AddBasicVariable(PropertyType::Vec2, vec2VarName))
    {
        std::cout << "  - 添加Vec2变量: " << vec2VarName << std::endl;

        auto* vec2Var = widgetClass.GetBasicVariable(vec2VarName);
        if (vec2Var)
        {
            vec2Var->SetValue<ImVec2>(ImVec2(100.0f, 200.0f));
            ImVec2 value = vec2Var->GetValue<ImVec2>();
            std::cout << "    设置值为: (" << value.x << ", " << value.y << ")" << std::endl;
        }
    }

    // 3. 添加控件树变量
    std::cout << "\n3. 添加控件树变量:" << std::endl;

    std::string buttonVarName;
    if (widgetClass.AddWidgetVariable("ImButton", buttonVarName))
    {
        std::cout << "  - 添加ImButton变量: " << buttonVarName << std::endl;

        // 获取按钮控件
        ImWidget* buttonWidget = widgetClass.GetWidgetVariable(buttonVarName);
        if (buttonWidget)
        {
            // 设置按钮位置和大小
            buttonWidget->SetPosition(ImVec2(10, 10));
            buttonWidget->SetSize(ImVec2(100, 40));

            // 设置按钮文本属性（使用ROP属性系统）
            std::string textVarName = "Text";
            std::string textValue = "Click Me";

            // 使用ROP属性系统设置文本
            bool success = buttonWidget->SetPropertyValue<std::string>("Text", textValue);
            if (success)
            {
                std::cout << "    - 设置按钮文本为: " << textValue << std::endl;
            }
            else
            {
                std::cout << "    - 无法设置按钮文本，可能属性名不正确" << std::endl;
            }

            // 设置为默认根控件
            widgetClass.SetDefaultRootVariable(buttonVarName);
            std::cout << "    - 设置为默认根控件" << std::endl;
        }
    }

    // 4. 添加ImObject变量
    std::cout << "\n4. 添加ImObject变量:" << std::endl;

    std::string colorStyleVarName;
    if (widgetClass.AddObjectVariable("ImObject", colorStyleVarName))  // 假设存在ImColorStyle类
    {
        std::cout << "  - 添加ImObject变量: " << colorStyleVarName << std::endl;

        // 获取对象
        ImObject* colorStyle = widgetClass.GetObjectVariable(colorStyleVarName);
        if (colorStyle)
        {
            // 使用ROP属性系统设置对象属性
            // 示例：设置颜色属性
            bool success = colorStyle->SetPropertyValue<ImU32>("PrimaryColor", IM_COL32(0, 0, 255, 255));  // 蓝色
            if (success)
            {
                std::cout << "    - 设置主颜色为蓝色" << std::endl;
            }
        }
    }

    // 5. 测试变量重命名
    std::cout << "\n5. 测试变量重命名:" << std::endl;

    if (!intVarName.empty())
    {
        std::string newIntVarName = "RenamedInt";
        if (widgetClass.RenameVariable(intVarName, newIntVarName))
        {
            std::cout << "  - 重命名 " << intVarName << " 为 " << newIntVarName << std::endl;
            intVarName = newIntVarName;
        }
        else
        {
            std::cout << "  - 重命名失败，名称可能已被使用" << std::endl;
        }
    }

    // 6. 序列化
    std::cout << "\n6. 序列化ImUserWidgetClass:" << std::endl;

    // 使用序列化器进行序列化
    json serialized = ImUserWidgetClassSerializer::SerializeUserWidgetClass(widgetClass);

    // 打印序列化结果
    std::string serializedStr = serialized.dump(2); // 使用2个空格缩进
    std::cout << serializedStr << std::endl;

    // 7. 验证序列化结果
    std::cout << "\n7. 验证序列化结果:" << std::endl;

    if (serialized.contains("ClassName"))
    {
        std::cout << "  - 类名: " << serialized["ClassName"].get<std::string>() << std::endl;
    }

    if (serialized.contains("DefaultRoot"))
    {
        std::string defaultRoot = serialized["DefaultRoot"].get<std::string>();
        std::cout << "  - 默认根控件: " << defaultRoot << std::endl;
    }

    if (serialized.contains("Variables") && serialized["Variables"].is_array())
    {
        std::cout << "  - 总变量数量: " << serialized["Variables"].size() << std::endl;

        // 按类型统计变量
        int widgetCount = 0;
        int objectCount = 0;
        int basicCount = 0;

        for (const auto& varJson : serialized["Variables"])
        {
            if (varJson.contains("VariableType"))
            {
                int type = varJson["VariableType"].get<int>();
                switch (static_cast<WidgetClassVariableType>(type))
                {
                case WidgetClassVariableType::Widget:
                    widgetCount++;
                    break;
                case WidgetClassVariableType::Object:
                    objectCount++;
                    break;
                case WidgetClassVariableType::Basic:
                    basicCount++;
                    break;
                }
            }
        }

        std::cout << "  - 控件变量: " << widgetCount << std::endl;
        std::cout << "  - 对象变量: " << objectCount << std::endl;
        std::cout << "  - 基本变量: " << basicCount << std::endl;
    }

    // 8. 测试反序列化
    std::cout << "\n8. 测试反序列化:" << std::endl;

    // 创建一个新的ImUserWidgetClass用于反序列化测试
    ImUserWidgetClass deserializedClass("DeserializedTest");

    if (ImUserWidgetClassSerializer::DeserializeUserWidgetClass(deserializedClass, serialized))
    {
        std::cout << "  - 反序列化成功!" << std::endl;
        std::cout << "  - 类名: " << deserializedClass.GetClassName() << std::endl;
        std::cout << "  - 默认根控件: " << deserializedClass.GetDefaultRootVariableName() << std::endl;

        // 验证变量数量
        auto allVarNames = deserializedClass.GetAllVariableNames();
        std::cout << "  - 总变量数量: " << allVarNames.size() << std::endl;

        // 验证基本变量值
        if (!intVarName.empty())
        {
            auto* intVar = deserializedClass.GetBasicVariable(intVarName);
            if (intVar)
            {
                int value = intVar->GetValue<int>();
                std::cout << "  - 验证Int变量 " << intVarName << " = " << value << " (应为42)" << std::endl;
            }
        }

        // 验证控件变量
        if (!buttonVarName.empty())
        {
            ImWidget* buttonWidget = deserializedClass.GetWidgetVariable(buttonVarName);
            if (buttonWidget)
            {
                std::cout << "  - 验证按钮控件 " << buttonVarName << " 存在" << std::endl;

                // 验证按钮文本
                auto textProp = buttonWidget->GetProperty("Text");
                if (textProp.IsValid())
                {
                    std::string textValue = textProp.GetValue<std::string>();
                    std::cout << "  - 按钮文本: " << textValue << " (应为Click Me)" << std::endl;
                }
            }
        }
    }
    else
    {
        std::cout << "  - 反序列化失败!" << std::endl;
    }

    // 9. 文件操作测试
    std::cout << "\n9. 文件操作测试:" << std::endl;

    std::string testFile = "test_widget_class.json";

    // 保存到文件
    if (ImUserWidgetClassSerializer::SaveToFile(widgetClass, testFile))
    {
        std::cout << "  - 保存到文件成功: " << testFile << std::endl;

        // 从文件加载
        ImUserWidgetClass loadedClass("LoadedWidgetClass");
        if (ImUserWidgetClassSerializer::LoadFromFile(loadedClass, testFile))
        {
            std::cout << "  - 从文件加载成功" << std::endl;
            std::cout << "  - 加载的类名: " << loadedClass.GetClassName() << std::endl;
        }
        else
        {
            std::cout << "  - 从文件加载失败" << std::endl;
        }

        // 删除测试文件
        std::remove(testFile.c_str());
        std::cout << "  - 删除测试文件" << std::endl;
    }
    else
    {
        std::cout << "  - 保存到文件失败" << std::endl;
    }

    // 10. .h/.cpp类文件输出测试
    std::cout << "\n10. .h/.cpp类文件输出测试:" << std::endl;

    // 使用代码生成器
    if (ImUserWidgetClassCodeGenerator::ExportUserWidgetClassToFiles(
        widgetClass,
        "TestGeneratedWidget",
        "TestGeneratedWidget.h",
        "TestGeneratedWidget.cpp"))
    {
        std::cout << "  - 成功生成C++类文件" << std::endl;
        std::cout << "  - 头文件: TestGeneratedWidget.h" << std::endl;
        std::cout << "  - 源文件: TestGeneratedWidget.cpp" << std::endl;
    }
    else
    {
        std::cout << "  - 生成C++类文件失败" << std::endl;
    }

    std::cout << "\n=== 测试完成 ===" << std::endl;
}

// 注册基础控件
#include "ImWidget/ImBasicWidgetDeclaration.h"

ImGuiWidget::ImWidget* ImInit()
{
    // 注册基础控件
    ImGuiWidget::RegisterBaseWidget();

    // 运行测试
    TestUserWidgetClassSerialization();

    exit(1);

    // 返回nullptr表示不使用控件
    return nullptr;
}

void ImTick()
{
    // 每帧调用的函数
    // 可以添加GUI渲染或其他每帧逻辑
}