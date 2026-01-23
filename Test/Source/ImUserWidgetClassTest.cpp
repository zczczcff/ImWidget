#include "Application/ImApplication.h"
#include "ImWidget/ImGlobalInstance.h"
#include "ImWidget/ImBasicWidgetDeclaration.h"
#include <iostream>
#include "ImUserWidgetClass.h"
#include "ImUserWidgetSerializer.h"
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
    if (widgetClass.AddBasicVariable(ImBasicVariable::BasicType::Int, "Data", intVarName))
    {
        std::cout << "  - 添加Int变量: " << intVarName << std::endl;

        // 设置初始值
        auto* intVar = widgetClass.GetBasicVariable(intVarName);
        if (intVar)
        {
            auto props = intVar->GetProperties();
            for (auto& prop : props)
            {
                if (prop.name == intVarName)
                {
                    ((ImGuiWidget::PropertyInfo)prop).SetIntValue(42);
                    std::cout << "    设置值为: " << prop.GetIntValue() << std::endl;
                }
            }
        }
    }

    std::string floatVarName;
    if (widgetClass.AddBasicVariable(ImBasicVariable::BasicType::Float, "Data", floatVarName))
    {
        std::cout << "  - 添加Float变量: " << floatVarName << std::endl;

        auto* floatVar = widgetClass.GetBasicVariable(floatVarName);
        if (floatVar)
        {
            auto props = floatVar->GetProperties();
            for (auto& prop : props)
            {
                if (prop.name == floatVarName)
                {
                    ((ImGuiWidget::PropertyInfo)prop).SetFloatValue(3.14f);
                    std::cout << "    设置值为: " << prop.GetFloatValue() << std::endl;
                }
            }
        }
    }

    std::string boolVarName;
    if (widgetClass.AddBasicVariable(ImBasicVariable::BasicType::Bool, "Data", boolVarName))
    {
        std::cout << "  - 添加Bool变量: " << boolVarName << std::endl;

        auto* boolVar = widgetClass.GetBasicVariable(boolVarName);
        if (boolVar)
        {
            auto props = boolVar->GetProperties();
            for (auto& prop : props)
            {
                if (prop.name == boolVarName)
                {
                    ((ImGuiWidget::PropertyInfo)prop).SetBoolValue(true);
                    std::cout << "    设置值为: " << (prop.GetBoolValue() ? "true" : "false") << std::endl;
                }
            }
        }
    }

    std::string stringVarName;
    if (widgetClass.AddBasicVariable(ImBasicVariable::BasicType::String, "Data", stringVarName))
    {
        std::cout << "  - 添加String变量: " << stringVarName << std::endl;

        auto* stringVar = widgetClass.GetBasicVariable(stringVarName);
        if (stringVar)
        {
            auto props = stringVar->GetProperties();
            for (auto& prop : props)
            {
                if (prop.name == stringVarName)
                {
                    ((ImGuiWidget::PropertyInfo)prop).SetStringValue("Hello, World!");
                    std::cout << "    设置值为: " << prop.GetStringValue() << std::endl;
                }
            }
        }
    }

    std::string colorVarName;
    if (widgetClass.AddBasicVariable(ImBasicVariable::BasicType::Color, "Style", colorVarName))
    {
        std::cout << "  - 添加Color变量: " << colorVarName << std::endl;

        auto* colorVar = widgetClass.GetBasicVariable(colorVarName);
        if (colorVar)
        {
            auto props = colorVar->GetProperties();
            for (auto& prop : props)
            {
                if (prop.name == colorVarName)
                {
                    ((ImGuiWidget::PropertyInfo)prop).SetColorValue(IM_COL32(255, 0, 0, 255)); // 红色
                    std::cout << "    设置值为: 红色 (ARGB: " << std::hex << prop.GetColorValue() << std::dec << ")" << std::endl;
                }
            }
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

            // 创建并添加TextBlock作为按钮内容
            ImWidget* textBlock = ImWidgetFactory::GetInstance().CreateWidget("ImTextBlock", "ButtonText");
            if (textBlock)
            {
                // 设置TextBlock文本
                auto textProps = textBlock->GetProperties();
                for (auto& prop : textProps)
                {
                    if (prop.name == "Text")
                    {
                        ((ImGuiWidget::PropertyInfo)prop).SetStringValue("Click Me");
                        break;
                    }
                }

                // 将TextBlock添加到按钮中
                if (buttonWidget->GetAllowMaxChildNum() > 0)
                {
                    buttonWidget->AddChild(textBlock);
                    std::cout << "    - 添加ImTextBlock作为按钮内容" << std::endl;
                }
                else
                {
                    delete textBlock;
                }
            }

            // 设置为默认根控件
            widgetClass.SetDefaultRootVariable(buttonVarName);
            std::cout << "    - 设置为默认根控件" << std::endl;
        }
    }

    // 4. 序列化
    std::cout << "\n4. 序列化ImUserWidgetClass:" << std::endl;

    json serialized = SerializeUserWidgetClass(widgetClass);

    // 打印序列化结果
    std::string serializedStr = serialized.dump(2); // 使用2个空格缩进
    std::cout << serializedStr << std::endl;

    // 5. 可选：验证序列化结果的基本结构
    std::cout << "\n5. 验证序列化结果:" << std::endl;

    if (serialized.contains("ClassName"))
    {
        std::cout << "  - 类名: " << serialized["ClassName"].get<std::string>() << std::endl;
    }

    if (serialized.contains("DefaultRoot"))
    {
        std::string defaultRoot = serialized["DefaultRoot"].get<std::string>();
        std::cout << "  - 默认根控件: " << defaultRoot << std::endl;
    }

    if (serialized.contains("BasicVariables") && serialized["BasicVariables"].is_array())
    {
        std::cout << "  - 基本变量数量: " << serialized["BasicVariables"].size() << std::endl;
    }

    if (serialized.contains("WidgetVariables") && serialized["WidgetVariables"].is_array())
    {
        std::cout << "  - 控件树变量数量: " << serialized["WidgetVariables"].size() << std::endl;

        // 显示控件树变量的详细信息
        for (const auto& widgetJson : serialized["WidgetVariables"])
        {
            if (widgetJson.contains("Name") && widgetJson.contains("Type"))
            {
                std::cout << "    * " << widgetJson["Name"].get<std::string>()
                    << " (" << widgetJson["Type"].get<std::string>() << ")" << std::endl;
            }
        }
    }

    // 6. 可选：测试反序列化（可选步骤）
    std::cout << "\n6. 测试反序列化（可选）:" << std::endl;

    // 创建一个新的ImUserWidgetClass用于反序列化测试
    ImUserWidgetClass deserializedClass("DeserializedTest");

    if (DeserializeUserWidgetClass(deserializedClass, serialized))
    {
        std::cout << "  - 反序列化成功!" << std::endl;
        std::cout << "  - 类名: " << deserializedClass.GetClassName() << std::endl;
        std::cout << "  - 默认根控件: " << deserializedClass.GetDefaultRootVariableName() << std::endl;
        std::cout << "  - 基本变量数量: " << deserializedClass.GetBasicVariableNames().size() << std::endl;
        std::cout << "  - 控件树变量数量: " << deserializedClass.GetWidgetVariableNames().size() << std::endl;
    }
    else
    {
        std::cout << "  - 反序列化失败!" << std::endl;
    }

    // 7. .h.cpp类文件输出测试

    deserializedClass.ExportToCppFiles("test", "test.h", "test.cpp");

    std::cout << "\n=== 测试完成 ===" << std::endl;
}

#include "ImWidget/ImBasicWidgetDeclaration.h"

ImGuiWidget::ImWidget* ImInit()
{
    ImGuiWidget::RegisterBaseWidget();
    TestUserWidgetClassSerialization();

    exit(1);
    while(1){}

    return nullptr;
}
void ImTick(){}
