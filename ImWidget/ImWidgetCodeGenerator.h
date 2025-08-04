#include <sstream>
#include <iomanip>
#include <unordered_set>
#include "ImWidget.h"
#include "ImWidgetProperty.h"
#include <fstream>

namespace ImGuiWidget
{
    // 辅助函数：将ImU32颜色转换为IM_COL32宏
    std::string ColorToCode(ImU32 color) {
        int r = IM_COLOR_GET_R(color);
        int g = IM_COLOR_GET_G(color);
        int b = IM_COLOR_GET_B(color);
        int a = IM_COLOR_GET_A(color);

        std::ostringstream oss;
        oss << "IM_COL32(" << r << ", " << g << ", " << b << ", " << a << ")";
        return oss.str();
    }

    // 辅助函数：将ImVec2转换为代码
    std::string Vec2ToCode(const ImVec2& vec) {
        std::ostringstream oss;
        oss << "ImVec2(" << vec.x << "f, " << vec.y << "f)";
        return oss.str();
    }

    // 辅助函数：将值转换为字符串表示
    std::string ValueToCode(PropertyType type, void* valuePtr) {
        switch (type) {
        case PropertyType::Color:
            return ColorToCode(*static_cast<ImU32*>(valuePtr));

        case PropertyType::Float:
            return std::to_string(*static_cast<float*>(valuePtr)) + "f";

        case PropertyType::Bool:
            return (*static_cast<bool*>(valuePtr)) ? "true" : "false";

        case PropertyType::Int:
            return std::to_string(*static_cast<int*>(valuePtr));

        case PropertyType::String:
            return "\"" + *static_cast<std::string*>(valuePtr) + "\"";

        case PropertyType::Vec2:
            return Vec2ToCode(*static_cast<ImVec2*>(valuePtr));

        case PropertyType::Struct:
            // 结构体需要特殊处理
            return "/* Struct properties require nested initialization */";

        default:
            return "/* Unknown type */";
        }
    }

    // 辅助函数：将PropertyType转换为类型名称字符串
    std::string PropertyTypeToString(PropertyType type) {
        switch (type) {
        case PropertyType::Color: return "ImU32";
        case PropertyType::Float: return "float";
        case PropertyType::Bool: return "bool";
        case PropertyType::Int: return "int";
        case PropertyType::String: return "std::string";
        case PropertyType::Vec2: return "ImVec2";
        case PropertyType::Struct: return "PropertyStruct";
        default: return "UnknownType";
        }
    }

    // 递归生成结构体属性的初始化代码
    std::string GenerateStructPropertiesCode(PropertyStruct* propStruct,
        const std::string& structAccessor,
        int indentLevel)
    {
        if (!propStruct) return "";

        std::ostringstream oss;
        std::string indent(indentLevel * 4, ' '); // 每级缩进4个空格

        auto properties = propStruct->GetProperties();
        for (const auto& prop : properties) {
            void* valuePtr = prop.getter();
            if (!valuePtr) continue;

            std::string valueCode;
            if (prop.type == PropertyType::Struct) {
                // 嵌套结构体递归处理
                PropertyStruct* nestedStruct = static_cast<PropertyStruct*>(valuePtr);
                std::string nestedAccessor = structAccessor + "." + prop.name;
                oss << GenerateStructPropertiesCode(nestedStruct, nestedAccessor, indentLevel + 1);
            }
            else {
                valueCode = ValueToCode(prop.type, valuePtr);

                oss << indent << structAccessor
                    << ".SetPropertyValue<" << PropertyTypeToString(prop.type) << ">(\""
                    << prop.name << "\", " << valueCode << ");\n";
            }
        }

        return oss.str();
    }



    // 主函数：生成控件的初始化代码
    std::string GenerateWidgetInitializationCode(ImWidget* widget,
        const std::string& varName)
    {
        if (!widget) return "";

        std::ostringstream oss;
        oss << "// Initialization code for " << varName << " ("
            << typeid(*widget).name() << ")\n";

        // 获取控件所有属性
        auto properties = widget->GetProperties();

        for (const auto& prop : properties) {
            void* valuePtr = prop.getter();
            if (!valuePtr) continue;

            if (prop.type == PropertyType::Struct) {
                // 结构体属性需要特殊处理
                PropertyStruct* propStruct = static_cast<PropertyStruct*>(valuePtr);
                std::string structAccessor = varName + ".Get" + prop.name + "()";

                oss << "// Initialize " << prop.name << " properties\n";
                oss << GenerateStructPropertiesCode(propStruct, structAccessor, 1);
            }
            else {
                // 基本类型属性
                std::string valueCode = ValueToCode(prop.type, valuePtr);

                oss << varName << ".SetPropertyValue<" << PropertyTypeToString(prop.type)
                    << ">(\"" << prop.name << "\", " << valueCode << ");\n";
            }
        }

        return oss.str();
    }



    bool ExportWidgetInitializationCodeToFile(ImWidget* widget,
        const std::string& filePath,
        const std::string& varName)
    {
        // 生成初始化代码
        std::string code = GenerateWidgetInitializationCode(widget, varName);

        // 如果生成失败，返回错误
        if (code.empty()) {
            return false;
        }

        // 尝试打开文件
        std::ofstream outFile(filePath);
        if (!outFile.is_open()) {
            return false;
        }

        // 添加文件头注释
        outFile << "// This file was automatically generated by ImGuiWidget code generator\n";
        outFile << "// Generated at: " << __DATE__ << " " << __TIME__ << "\n\n";

        // 添加必要的包含指令
        outFile << "#include \"ImWidget.h\"\n";
        outFile << "#include \"ImButton.h\" // Include specific widget headers\n";
        outFile << "#include \"imgui.h\"\n\n";

        // 添加命名空间
        outFile << "using namespace ImGuiWidget;\n\n";

        // 添加函数框架
        outFile << "void Setup" << varName << "()\n";
        outFile << "{\n";
        outFile << "    // Create the widget\n";
        outFile << "    ImButton* " << varName << " = new ImButton(\"" << varName << "\");\n\n";

        // 添加生成的初始化代码（每行缩进一级）
        std::istringstream codeStream(code);
        std::string line;
        while (std::getline(codeStream, line)) {
            // 跳过空行
            if (line.empty()) continue;

            // 添加缩进
            outFile << "    " << line << "\n";
        }

        // 添加函数结尾
        outFile << "\n    // Additional setup can be added here\n";
        outFile << "}\n";

        // 关闭文件
        outFile.close();

        return true;
    }

}