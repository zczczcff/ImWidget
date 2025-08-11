#pragma once
#include <sstream>
#include <iomanip>
#include <unordered_set>
#include "ImWidget.h"
#include "ImWidgetProperty.h"
#include "ImPanelWidget.h"
#include <fstream>
#include <regex>

namespace ImGuiWidget
{
    struct CodeGenContext
    {
        std::ostringstream& oss;  // 输出流
        int indentLevel = 0;      // 当前缩进级别
        int widgetCounter = 0;    // 控件计数器（用于生成唯一变量名）
        std::unordered_map<ImWidget*, std::string> widgetVarMap; // 控件到变量名的映射

        std::string indentStr() const {
            return std::string(indentLevel * 4, ' ');
        }

        std::string generateVarName(ImWidget* widget) {
            if (widgetVarMap.find(widget) != widgetVarMap.end()) {
                return widgetVarMap[widget];
            }

            std::string varName = widget->GetWidgetName();
            widgetVarMap[widget] = varName;
            return varName;
        }
    };

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
        oss << "ImVec2(" << std::to_string(vec.x) << "f, " << std::to_string(vec.y) << "f)";
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
        case PropertyType::StringArray: 
        {
            std::vector<std::string>* vec = static_cast<std::vector<std::string>*>(valuePtr);
            std::ostringstream oss;
            oss << "{";
            for (size_t i = 0; i < vec->size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << (*vec)[i] << "\"";
            }
            oss << "}";
            return oss.str();
        }
        case PropertyType::Enum: 
        {
            std::vector<std::string>* vec = static_cast<std::vector<std::string>*>(valuePtr);
            if (vec && !vec->empty()) {
                // 取最后一个元素（当前选中的值）
                return "\"" + vec->back() + "\"";
            }
            return "\"\""; // 空字符串作为默认
            break;
        }
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

    template<typename T>
    void GeneratePropertiesCode(T* obj, const std::string& accessor, CodeGenContext& context)
    {
        auto properties = obj->GetProperties();
        for (const auto& prop : properties) {
            void* valuePtr = prop.getter();
            if (!valuePtr) continue;

            if (prop.type == PropertyType::Struct) 
            {
                PropertyStruct* nestedStruct = static_cast<PropertyStruct*>(valuePtr);
                std::string nestedAccessor = accessor + "_" + prop.name;
                context.oss << context.indentStr() << "PropertyStruct* " << nestedAccessor
                    << " = " << accessor << "->GetPropertyPtr<PropertyStruct>(" << prop.name
                    << ");\n";
                GeneratePropertiesCode(nestedStruct, nestedAccessor, context);
            }
            else 
            {
                std::string valueCode = ValueToCode(prop.type, valuePtr);
                context.oss << context.indentStr()
                    << accessor << "->SetPropertyValue<"
                    << PropertyTypeToString(prop.type) << ">(\""
                    << prop.name << "\", " << valueCode << ");\n";
            }
        }
    }


    void GenerateWidgetTreeCode(ImWidget* widget, CodeGenContext& context) {
        if (!widget) return;

        // 生成变量名
        std::string varName = context.generateVarName(widget);

        // 生成控件创建代码（使用完全限定名）
        context.oss << context.indentStr()
            << "ImGuiWidget::" << widget->GetRegisterTypeName() << "* " << varName
            << " = new ImGuiWidget::" << widget->GetRegisterTypeName()
            << "(\"" << widget->GetWidgetName() << "\");\n";

        // 生成属性设置代码（使用完全限定名）
        auto properties = widget->GetProperties();
        for (const auto& prop : properties) {
            void* valuePtr = prop.getter();
            if (!valuePtr) continue;

            if (prop.type == PropertyType::Struct) {
                PropertyStruct* nestedStruct = static_cast<PropertyStruct*>(valuePtr);
                std::string nestedAccessor = varName + "_" + prop.name;
                context.oss << context.indentStr()
                    << "ImGuiWidget::PropertyStruct* " << nestedAccessor
                    << " = " << varName << "->GetPropertyPtr<ImGuiWidget::PropertyStruct>(\""
                    << prop.name << "\");\n";
                GeneratePropertiesCode(nestedStruct, nestedAccessor, context);
            }
            else {
                std::string valueCode = ValueToCode(prop.type, valuePtr);
                context.oss << context.indentStr()
                    << varName << "->SetPropertyValue<"
                    << PropertyTypeToString(prop.type) << ">(\""
                    << prop.name << "\", " << valueCode << ");\n";
            }
        }

        // 处理容器控件及其子控件（使用完全限定名）
        if (auto panel = dynamic_cast<ImPanelWidget*>(widget)) {
            int slotCount = panel->GetSlotNum();
            for (int i = 0; i < slotCount; i++) {
                ImSlot* slot = panel->GetSlot(i);
                ImWidget* child = slot ? slot->GetContent() : nullptr;

                if (child) {
                    // 递归生成子控件代码
                    GenerateWidgetTreeCode(child, context);

                    // 生成添加子控件的代码
                    std::string childVarName = context.generateVarName(child);
                    std::string childSlotName = varName + "_slot" + std::to_string(i);

                    context.oss << context.indentStr()
                        << "ImGuiWidget::ImSlot* " << childSlotName << " = "
                        << varName << "->AddChild(" << childVarName << ");\n";

                    // 生成slot属性设置代码
                    auto slotProps = slot->GetProperties();
                    for (const auto& prop : slotProps) {
                        void* valuePtr = prop.getter();
                        if (!valuePtr) continue;

                        std::string valueCode = ValueToCode(prop.type, valuePtr);
                        context.oss << context.indentStr()
                            << childSlotName << "->SetPropertyValue<"
                            << PropertyTypeToString(prop.type) << ">(\""
                            << prop.name << "\", " << valueCode << ");\n";
                    }
                }
            }
        }
    }


    bool ExportWidgetTreeToFile(ImWidget* root, const std::string& filePath, const std::string& functionName)
    {
        std::ofstream outFile(filePath);
        if (!outFile.is_open())
        {
            return false;
        }

        // 生成文件头部
        outFile << "// This file was automatically generated by ImGuiWidget code generator\n";
        outFile << "// Generated at: " << __DATE__ << " " << __TIME__ << "\n\n";
        outFile << "#include \"ImWidget.h\"\n";
        outFile << "#include \"ImButton.h\"\n";
        outFile << "#include \"ImPanelWidget.h\"\n";
        outFile << "#include \"imgui.h\"\n\n";
        outFile << "using namespace ImGuiWidget;\n\n";

        // 生成函数
        outFile << "void " << functionName << "()\n{\n";

        // 生成控件树代码
        std::ostringstream codeStream;
        CodeGenContext context{ codeStream, 1 }; // 缩进级别从1开始
        GenerateWidgetTreeCode(root, context);

        outFile << codeStream.str();
        outFile << "}\n";

        outFile.close();
        return true;
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


    bool ExportUserWidgetToFiles(ImWidget* rootWidget,
        const std::string& widgetName,
        const std::string& CppoutputDirectory,
        const std::string& HeaderoutputDirectory)
    {
        std::string headerdir = HeaderoutputDirectory;
        if (!headerdir.empty() && headerdir.back() != '/' && headerdir.back() != '\\') {
            headerdir += '/';
        }
        // 确保输出目录以分隔符结尾
        std::string cppdir = CppoutputDirectory;
        if (!cppdir.empty() && cppdir.back() != '/' && cppdir.back() != '\\') {
            cppdir += '/';
        }

        // 创建头文件和源文件路径
        std::string headerPath = headerdir + widgetName + ".h";
        std::string sourcePath = cppdir + widgetName + ".cpp";

        // 收集所有控件（包括子控件）
        std::vector<ImWidget*> allWidgets;
        std::map<ImWidget*, std::string> widgetVarMap;
        std::set<std::string> requiredHeaders;

        // 递归收集控件
        std::function<void(ImWidget*)> collectWidgets = [&](ImWidget* widget) {
            if (!widget) return;

            // 生成唯一变量名（使用控件名）
            std::string varName = widget->GetWidgetName();
            // 替换非法字符
            std::replace(varName.begin(), varName.end(), ' ', '_');
            std::replace(varName.begin(), varName.end(), '-', '_');

            widgetVarMap[widget] = varName;
            allWidgets.push_back(widget);

            // 添加所需头文件
            std::string header = widget->GetRegisterTypeName() + ".h";
            requiredHeaders.insert(header);

            // 处理子控件
            if (auto panel = dynamic_cast<ImPanelWidget*>(widget)) {
                int slotCount = panel->GetSlotNum();
                for (int i = 0; i < slotCount; i++) {
                    ImSlot* slot = panel->GetSlot(i);
                    if (slot && slot->GetContent()) {
                        collectWidgets(slot->GetContent());
                    }
                }
            }
        };

        collectWidgets(rootWidget);

        // 1. 处理头文件
        std::string headerContent;
        bool headerExists = false;

        // 读取现有头文件（如果存在）
        std::ifstream headerIn(headerPath);
        if (headerIn.good()) {
            headerExists = true;
            std::ostringstream ss;
            ss << headerIn.rdbuf();
            headerContent = ss.str();
            headerIn.close();
        }

        // 创建或更新头文件
        std::ofstream headerOut(headerPath);
        if (!headerOut.is_open()) return false;

        if (!headerExists) 
        {
            // 创建新头文件
			headerOut << "#pragma once\n"
				<< "#include \"ImUserWidget.h\"\n\n"
                << "#include \"ImBasicWidgetDeclaration.h\"\n\n"
				//<< "namespace ImGuiWidget {\n"
				<< "class " << widgetName << " : public ImGuiWidget::ImUserWidget\n"
                << "{\n"
				<< "public:\n"
				//<< "    " << widgetName << "(const std::string& name);\n"
				<< "    void Init();\n\n"
                << "    " << widgetName << "(const std::string& name): ImGuiWidget::ImUserWidget(name)\n"
                << "{\n"
                << "    Init();\n"
                << "}\n\n"
				<< "protected:\n"
				<< "//----Gen Members Begin----\n"
				<< "    // Auto-generated widget pointers\n";
			for (auto widget : allWidgets)
			{
				std::string typeName = widget->GetRegisterTypeName();
				std::string varName = widgetVarMap[widget];
                headerOut<< "    ImGuiWidget::" + typeName + "* " + varName + ";\n";
			}
            headerOut << "//----Gen Members End----\n"
                << "};\n";
                //<< "}\n";
        }
        else {
            // 更新现有头文件 - 只替换成员区域
            std::regex membersPattern(
                R"(//----Gen Members Begin----[\s\S]*?//----Gen Members End----)"
            );

            std::string newMembers = "//----Gen Members Begin----\n"
                "    // Auto-generated widget pointers\n";

            // 为每个控件生成成员变量声明
            for (auto widget : allWidgets) {
                std::string typeName = widget->GetRegisterTypeName();
                std::string varName = widgetVarMap[widget];

                newMembers += "    ImGuiWidget::" + typeName + "* " + varName + ";\n";
            }

            newMembers += "//----Gen Members End----";

            headerContent = std::regex_replace(
                headerContent,
                membersPattern,
                newMembers
            );

            headerOut << headerContent;
        }
        headerOut.close();

        // 2. 处理源文件
        std::string sourceContent;
        bool sourceExists = false;

        // 读取现有源文件（如果存在）
        std::ifstream sourceIn(sourcePath);
        if (sourceIn.good()) {
            sourceExists = true;
            std::ostringstream ss;
            ss << sourceIn.rdbuf();
            sourceContent = ss.str();
            sourceIn.close();
        }

        // 创建或更新源文件
        std::ofstream sourceOut(sourcePath);
        if (!sourceOut.is_open()) return false;

        // 生成包含区域
        std::string newIncludes = "//----Gen Include Begin----\n"
            "// Auto-generated includes\n";

        // 添加必需的头文件
        newIncludes += "#include \"" + widgetName + ".h\"\n";
        newIncludes += "#include \"ImBasicWidgetList.h\"\n";
        //for (auto& header : requiredHeaders) {
        //    newIncludes += "#include \"" + header + "\"\n";
        //}
        newIncludes += "//----Gen Include End----\n";

        // 生成初始化代码
        std::ostringstream initCode;
        initCode << "//----Gen Code Begin----\n"
            << "// Auto-generated initialization code\n"
            << "void " << widgetName << "::Init()\n"
            << "{\n";

        // 创建控件实例
        for (auto widget : allWidgets) {
            std::string typeName = widget->GetRegisterTypeName();
            std::string varName = widgetVarMap[widget];

            initCode << "    " << varName << " = new ImGuiWidget::" << typeName
                << "(\"" << widget->GetWidgetName() << "\");\n";
        }
        initCode << "\n";

        // 设置控件属性
        for (auto widget : allWidgets) {
            std::string varName = widgetVarMap[widget];
            auto properties = widget->GetProperties();

            for (const auto& prop : properties) {
                void* valuePtr = prop.getter();
                if (!valuePtr) continue;

                if (prop.type == PropertyType::Struct) {
                    PropertyStruct* nestedStruct = static_cast<PropertyStruct*>(valuePtr);
                    std::string nestedAccessor = varName + "_" + prop.name;
                    initCode << "    ImGuiWidget::PropertyStruct* " << nestedAccessor
                        << " = " << varName << "->GetPropertyPtr<ImGuiWidget::PropertyStruct>(\""
                        << prop.name << "\");\n";
                    CodeGenContext context{ initCode, 1 };
                    GeneratePropertiesCode(nestedStruct, nestedAccessor, context);
                }
                else {
                    std::string valueCode = ValueToCode(prop.type, valuePtr);
                    initCode << "    " << varName << "->SetPropertyValue<"
                        << PropertyTypeToString(prop.type) << ">(\""
                        << prop.name << "\", " << valueCode << ");\n";
                }
            }
            initCode << "\n";
        }

        // 建立父子关系并设置slot属性
        std::map<std::pair<ImPanelWidget*, ImSlot*>, std::string> slotVarMap; // 存储slot变量名

        for (auto widget : allWidgets) {
            if (auto panel = dynamic_cast<ImPanelWidget*>(widget)) {
                std::string panelVar = widgetVarMap[panel];

                for (int i = 0; i < panel->GetSlotNum(); i++) {
                    ImSlot* slot = panel->GetSlot(i);
                    if (!slot || !slot->GetContent()) continue;

                    std::string childVar = widgetVarMap[slot->GetContent()];
                    std::string slotVar = panelVar + "_slot" + std::to_string(i);

                    // 保存slot变量名
                    slotVarMap[{panel, slot}] = slotVar;

                    // 添加子控件并获取slot
                    initCode << "    ImGuiWidget::ImSlot* " << slotVar << " = "
                        << panelVar << "->AddChild(" << childVar << ");\n";

                    // 设置slot的属性
                    auto slotProps = slot->GetProperties();
                    for (const auto& prop : slotProps) {
                        void* valuePtr = prop.getter();
                        if (!valuePtr) continue;

                        if (prop.type == PropertyType::Struct) {
                            PropertyStruct* nestedStruct = static_cast<PropertyStruct*>(valuePtr);
                            std::string nestedAccessor = slotVar + "_" + prop.name;
                            initCode << "    ImGuiWidget::PropertyStruct* " << nestedAccessor
                                << " = " << slotVar << "->GetPropertyPtr<ImGuiWidget::PropertyStruct>(\""
                                << prop.name << "\");\n";
                            CodeGenContext context{ initCode, 1 };
                            GeneratePropertiesCode(nestedStruct, nestedAccessor, context);
                        }
                        else {
                            std::string valueCode = ValueToCode(prop.type, valuePtr);
                            initCode << "    " << slotVar << "->SetPropertyValue<"
                                << PropertyTypeToString(prop.type) << ">(\""
                                << prop.name << "\", " << valueCode << ");\n";
                        }
                    }

                    //// 对于ImPaddingSlot特殊处理
                    //if (auto paddingSlot = dynamic_cast<ImPaddingSlot*>(slot)) {
                    //    initCode << "    " << slotVar << "->PaddingTop = " << paddingSlot->PaddingTop << "f;\n";
                    //    initCode << "    " << slotVar << "->PaddingBottom = " << paddingSlot->PaddingBottom << "f;\n";
                    //    initCode << "    " << slotVar << "->PaddingLeft = " << paddingSlot->PaddingLeft << "f;\n";
                    //    initCode << "    " << slotVar << "->PaddingRight = " << paddingSlot->PaddingRight << "f;\n";
                    //}

                    initCode << "\n";
                }
            }
        }

        // 设置根控件
        std::string rootVar = widgetVarMap[rootWidget];
        initCode << "\n    SetRootWidget(" << rootVar << ");\n"
            << "}\n"
            << "//----Gen Code End----\n";

        if (!sourceExists) {
            // 创建新源文件
            sourceOut << newIncludes
                //<< "using namespace ImGuiWidget;\n\n"

                << initCode.str();
        }
        else {
            // 更新现有源文件 - 替换标记区域

            // 处理包含区域
            std::regex includesPattern(
                R"(//----Gen Include Begin----\n[\s\S]*?//----Gen Include End----\n)"
            );
            sourceContent = std::regex_replace(
                sourceContent,
                includesPattern,
                newIncludes
            );

            // 处理代码区域
            std::regex codePattern(
                R"(//----Gen Code Begin----\n[\s\S]*?//----Gen Code End----\n)"
            );
            sourceContent = std::regex_replace(
                sourceContent,
                codePattern,
                initCode.str()
            );

            sourceOut << sourceContent;
        }

        sourceOut.close();
        return true;
    }
}