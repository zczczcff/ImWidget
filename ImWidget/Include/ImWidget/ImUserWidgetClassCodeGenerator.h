#pragma once
#include <sstream>
#include <iomanip>
#include <unordered_set>
#include <fstream>
#include <regex>
#include <set>
#include <algorithm>
#include <iostream>
#include "ImUserWidgetClass.h"
#include "ImWidgetCodeGenerator.h"
#include "ImUserWidgetSerializer.h"

namespace ImGuiWidget
{
    class ImUserWidgetClassCodeGenerator
    {
    private:
        // 代码生成上下文
        struct ClassGenContext
        {
            std::ofstream& oss;          // 输出流
            int indentLevel = 0;              // 当前缩进级别
            std::string className;           // 生成的类名
            std::string namespaceName;       // 命名空间
            std::set<std::string> usedHeaders; // 使用的头文件列表
            bool inNamespace = false;        // 是否在命名空间中

            std::string indentStr() const
            {
                return std::string(indentLevel * 4, ' ');
            }

            // 写入一行代码
            void writeLine(const std::string& line)
            {
                oss << indentStr() << line << "\n";
            }

            // 写入空行
            void writeEmptyLine()
            {
                oss << "\n";
            }

            // 增加缩进
            void increaseIndent() { indentLevel++; }

            // 减少缩进
            void decreaseIndent() { indentLevel = std::max(0, indentLevel - 1); }
        };

        // 自动生成标记
        static const std::string AUTO_GEN_BEGIN;
        static const std::string AUTO_GEN_END;


        // 字符串转代码格式
        static std::string StringToCode(const std::string& str)
        {
            // 检查是否包含中文字符
            bool hasChinese = std::any_of(str.begin(), str.end(),
                [](char c) { return static_cast<unsigned char>(c) >= 0x80; });

            if (hasChinese)
            {
                return "u8\"" + str + "\"";
            }
            return "\"" + str + "\"";
        }

        // 类型对应头文件
        static std::string TypeToHeader(const std::string& typeName)
        {
            std::string header = typeName;
            size_t pos = header.find_last_of("::");
            if (pos != std::string::npos)
            {
                header = header.substr(pos + 1);
            }
            return header + ".h";
        }

        // BasicType转C++类型字符串
        static std::string BasicTypeToCppType(ImBasicVariable::BasicType type)
        {
            switch (type)
            {
            case ImBasicVariable::BasicType::Int: return "int";
            case ImBasicVariable::BasicType::Float: return "float";
            case ImBasicVariable::BasicType::Bool: return "bool";
            case ImBasicVariable::BasicType::String: return "std::string";
            case ImBasicVariable::BasicType::Color: return "ImU32";
            default: return "int";
            }
        }

        // 获取基本变量类型默认值
        static std::string GetBasicVariableTypeDefault(ImBasicVariable::BasicType type)
        {
            switch (type)
            {
            case ImBasicVariable::BasicType::Int: return "0";
            case ImBasicVariable::BasicType::Float: return "0.0f";
            case ImBasicVariable::BasicType::Bool: return "false";
            case ImBasicVariable::BasicType::String: return "\"\"";
            case ImBasicVariable::BasicType::Color: return "IM_COL32(255, 255, 255, 255)";
            default: return "0";
            }
        }

        // 生成基本变量成员声明
        static void GenerateBasicVariableMemberDeclarations(const std::vector<std::string>& basicVars,
            const ImUserWidgetClass& widgetClass,
            ClassGenContext& context)
        {
            if (basicVars.empty()) return;

            context.writeLine("//===Auto Gen Begin=== (Basic Variable Members)");
            context.writeLine("// 基本变量");

            for (const auto& varName : basicVars)
            {
                ImBasicVariable* var = widgetClass.GetBasicVariable(varName);
                if (!var) continue;

                std::string typeStr = BasicTypeToCppType(var->GetBasicType());
                std::string defaultValue = GetBasicVariableTypeDefault(var->GetBasicType());

                context.writeLine(typeStr + " m_" + varName + " = " + defaultValue + ";");
            }

            context.writeLine("//===Auto Gen End=== (Basic Variable Members)");
            context.writeEmptyLine();
        }

        // 生成基本变量初始化代码
        static void GenerateBasicVariableInitCode(const std::vector<std::string>& basicVars,
            const ImUserWidgetClass& widgetClass,
            ClassGenContext& context)
        {
            if (basicVars.empty()) return;

            context.writeLine("//===Auto Gen Begin=== (Basic Variables Init)");
            context.writeLine("// 初始化基本变量");

            for (const auto& varName : basicVars)
            {
                ImBasicVariable* var = widgetClass.GetBasicVariable(varName);
                if (!var) continue;

                auto props = var->GetProperties();
                if (props.empty()) continue;

                auto prop = *props.begin();
                void* valuePtr = var->GetValuePtr();
                if (!valuePtr) continue;

                std::string valueCode = ValueToCode(prop.type, valuePtr);
                context.writeLine("m_" + varName + " = " + valueCode + ";");
            }

            context.writeLine("//===Auto Gen End=== (Basic Variables Init)");
            context.writeEmptyLine();
        }

        // 替换文件中标记区域的内容（改进版本）
        static bool ReplaceMarkedRegionInFile(const std::string& filePath,
            const std::string& marker,
            const std::string& newContent)
        {
            // 首先读取整个文件内容
            std::ifstream file(filePath);
            if (!file.is_open())
            {
                std::cerr << "无法打开文件: " << filePath << std::endl;
                return false;
            }

            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string content = buffer.str();
            file.close();

            // 正则表达式匹配模式
            std::string beginPattern = "//===Auto Gen Begin=== \\(" + marker + "\\)";
            std::string endPattern = "//===Auto Gen End=== \\(" + marker + "\\)";

            // 构建完整的匹配模式
            std::string fullPattern = beginPattern + "[\\s\\S]*?" + endPattern;

            std::regex pattern(fullPattern);

            // 构建替换内容
            std::string replacement = beginPattern + "\n" + newContent + "\n" + endPattern;

            // 执行替换
            std::string newFileContent;
            try
            {
                newFileContent = std::regex_replace(content, pattern, replacement);
            }
            catch (const std::regex_error& e)
            {
                std::cerr << "正则表达式错误: " << e.what() << std::endl;
                return false;
            }

            // 如果替换没有发生（标记不存在），在文件末尾添加新内容
            if (newFileContent == content)
            {
                std::cerr << "警告: 未找到标记区域 '" << marker << "'，将在文件末尾添加内容" << std::endl;
                newFileContent = content + "\n" + replacement + "\n";
            }

            // 写回文件
            std::ofstream outFile(filePath);
            if (!outFile.is_open())
            {
                std::cerr << "无法写入文件: " << filePath << std::endl;
                return false;
            }

            outFile << newFileContent;
            outFile.close();

            return true;
        }

        // 生成头文件（改进版本）
        static bool GenerateHeaderFile(const ImUserWidgetClass& widgetClass,
            const std::string& className,
            const std::string& headerOutputPath)
        {
            // 检查文件是否存在
            bool fileExists = std::ifstream(headerOutputPath).good();

            if (!fileExists)
            {
                // 生成全新头文件
                return GenerateNewHeaderFile(widgetClass, className, headerOutputPath);
            }
            else
            {
                // 更新已存在的头文件
                return UpdateExistingHeaderFile(widgetClass, className, headerOutputPath);
            }
        }

        // 生成全新头文件
        static bool GenerateNewHeaderFile(const ImUserWidgetClass& widgetClass,
            const std::string& className,
            const std::string& headerOutputPath)
        {
            std::ofstream headerFile(headerOutputPath);
            if (!headerFile.is_open()) return false;

            // 写入UTF-8 BOM
            const unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
            headerFile.write(reinterpret_cast<const char*>(bom), sizeof(bom));

            ClassGenContext context{ headerFile, 0, className, widgetClass.GetNamespace() };

            // 生成头文件内容
            context.writeLine("#pragma once");
            context.writeEmptyLine();
            context.writeLine("// 自动生成的自定义控件类 - " + context.className);
            context.writeLine("// 注意：此文件为自动生成，请勿手动编辑");
            context.writeEmptyLine();

            // 包含必要头文件
            context.writeLine("#include \"ImUserWidget.h\"");
            context.writeLine("#include \"ImObjectFactory.h\"");
            context.writeLine("#include \"ImWidgetFactory.h\"");
            context.writeEmptyLine();

            // 开始命名空间
            if (!context.namespaceName.empty())
            {
                context.writeLine("namespace " + context.namespaceName);
                context.writeLine("{");
                context.increaseIndent();
            }

            // 类声明开始
            context.writeLine("class " + context.className + " : public " + widgetClass.GetBaseClass());
            context.writeLine("{");
            context.writeLine("public:");
            context.increaseIndent();

            // 构造函数
            context.writeLine(context.className + "(const std::string& name);");
            context.writeLine("virtual ~" + context.className + "();");
            context.writeEmptyLine();

            context.decreaseIndent();
            context.writeLine("protected:");
            context.increaseIndent();

            context.writeLine("void InitializeVariables();");
            context.writeEmptyLine();

            // 生成成员变量
            GenerateHeaderMembers(widgetClass, context);

            context.decreaseIndent();
            context.writeLine("};");

            // 结束命名空间
            if (!context.namespaceName.empty())
            {
                context.decreaseIndent();
                context.writeLine("} // namespace " + context.namespaceName);
            }

            headerFile.close();
            return true;
        }

        // 生成头文件成员变量声明
        static void GenerateHeaderMembers(const ImUserWidgetClass& widgetClass, ClassGenContext& context)
        {
            auto widgetVars = widgetClass.GetWidgetVariableNames();
            auto objectVars = widgetClass.GetObjectVariableNames();
            auto basicVars = widgetClass.GetBasicVariableNames();

            // 收集所有控件（包括子控件）
            std::vector<std::pair<std::string, ImWidget*>> allWidgets;

            // 添加直接控件变量
            for (const auto& varName : widgetVars)
            {
                ImWidget* widget = widgetClass.GetWidgetVariable(varName);
                if (widget)
                {
                    allWidgets.push_back({ varName, widget });
                    // 递归收集子控件
                    CollectChildWidgets(widget, varName, allWidgets);
                }
            }

            if (allWidgets.empty() && objectVars.empty() && basicVars.empty()) return;

            context.writeLine("//===Auto Gen Begin=== (Member Variables)");

            // 生成所有控件变量声明（包括子控件）
            if (!allWidgets.empty())
            {
                context.writeLine("// 控件变量");
                for (const auto& [varName, widget] : allWidgets)
                {
                    std::string typeName = widget->GetRegisterTypeName();
                    // 提取类型名（去掉命名空间）
                    size_t pos = typeName.find_last_of("::");
                    if (pos != std::string::npos)
                    {
                        typeName = typeName.substr(pos + 1);
                    }
                    context.writeLine(typeName + "* m_" + varName + " = nullptr;");
                }
                context.writeEmptyLine();
            }

            // 生成ImObject变量声明
            if (!objectVars.empty())
            {
                context.writeLine("// ImObject变量");
                for (const auto& varName : objectVars)
                {
                    ImObject* obj = widgetClass.GetObjectVariable(varName);
                    if (obj)
                    {
                        std::string typeName = obj->GetRegisterTypeName();
                        size_t pos = typeName.find_last_of("::");
                        if (pos != std::string::npos)
                        {
                            typeName = typeName.substr(pos + 1);
                        }
                        context.writeLine(typeName + "* m_" + varName + " = nullptr;");
                    }
                }
                context.writeEmptyLine();
            }

            // 生成基本变量声明
            if (!basicVars.empty())
            {
                context.writeLine("// 基本变量");
                for (const auto& varName : basicVars)
                {
                    ImBasicVariable* var = widgetClass.GetBasicVariable(varName);
                    if (var)
                    {
                        std::string typeStr = BasicTypeToCppType(var->GetBasicType());
                        context.writeLine(typeStr + " m_" + varName + " = " +
                            GetBasicVariableTypeDefault(var->GetBasicType()) + ";");
                    }
                }
            }

            context.writeLine("//===Auto Gen End=== (Member Variables)");
        }

        // 递归收集所有子控件
        static void CollectChildWidgets(ImWidget* parentWidget, const std::string& parentVarName,
            std::vector<std::pair<std::string, ImWidget*>>& allWidgets)
        {
            if (!parentWidget) return;

            // 检查是否是面板控件
            ImPanelWidget* panelWidget = dynamic_cast<ImPanelWidget*>(parentWidget);
            if (!panelWidget) return;

            int childCount = panelWidget->GetChildNum();
            for (int i = 0; i < childCount; i++)
            {
                ImWidget* childWidget = panelWidget->GetChildAt(i);
                if (!childWidget) continue;

                // 使用控件本身的名称作为变量名
                std::string childVarName = childWidget->GetWidgetName();
                if (childVarName.empty())
                {
                    childVarName = parentVarName + "_Child" + std::to_string(i);
                }

                allWidgets.push_back({ childVarName, childWidget });

                // 递归收集子控件的子控件
                CollectChildWidgets(childWidget, childVarName, allWidgets);
            }
        }

        // 更新已存在的头文件（改进版本）
        static bool UpdateExistingHeaderFile(const ImUserWidgetClass& widgetClass,
            const std::string& className,
            const std::string& filePath)
        {
            // 生成新的成员变量内容
            std::ostringstream memberStream;
            std::ofstream oss;
            ClassGenContext memberContext{ oss, 1, className, widgetClass.GetNamespace() };
            GenerateHeaderMembers(widgetClass, memberContext);

            memberStream << oss.rdbuf();

            std::string memberVarsContent = memberStream.str();

            // 替换标记区域
            return ReplaceMarkedRegionInFile(filePath, "Member Variables", memberVarsContent);
        }

        // 生成源文件（改进版本）
        static bool GenerateSourceFile(const ImUserWidgetClass& widgetClass,
            const std::string& className,
            const std::string& sourceOutputPath)
        {
            // 检查文件是否存在
            bool fileExists = std::ifstream(sourceOutputPath).good();

            if (!fileExists)
            {
                // 生成全新源文件
                return GenerateNewSourceFile(widgetClass, className, sourceOutputPath);
            }
            else
            {
                // 更新已存在的源文件
                return UpdateExistingSourceFile(widgetClass, className, sourceOutputPath);
            }
        }

        // 生成全新源文件
        static bool GenerateNewSourceFile(const ImUserWidgetClass& widgetClass,
            const std::string& className,
            const std::string& sourceOutputPath)
        {
            std::ofstream sourceFile(sourceOutputPath);
            if (!sourceFile.is_open()) return false;

            // 写入UTF-8 BOM
            const unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
            sourceFile.write(reinterpret_cast<const char*>(bom), sizeof(bom));

            ClassGenContext context{ sourceFile, 0, className, widgetClass.GetNamespace() };

            // 生成源文件内容
            context.writeLine("// 自动生成的自定义控件源文件 - " + context.className);
            context.writeLine("// 注意：此文件为自动生成，请勿手动编辑");
            context.writeEmptyLine();

            // 包含头文件
            context.writeLine("#include \"" + context.className + ".h\"");
            context.writeLine("#include \"ImObjectFactory.h\"");
            context.writeLine("#include \"ImWidgetFactory.h\"");
            context.writeEmptyLine();

            // 收集需要的头文件
            std::set<std::string> headers;
            CollectRequiredHeaders(widgetClass, headers);

            for (const auto& header : headers)
            {
                context.writeLine("#include " + header);
            }
            context.writeEmptyLine();

            // 开始命名空间
            if (!context.namespaceName.empty())
            {
                context.writeLine("namespace " + context.namespaceName);
                context.writeLine("{");
                context.increaseIndent();
            }

            // 构造函数
            context.writeLine(context.className + "::" + context.className + "(const std::string& name)");
            context.increaseIndent();
            context.writeLine(": " + widgetClass.GetBaseClass() + "(name)");
            context.decreaseIndent();
            context.writeLine("{");
            context.increaseIndent();

            context.writeLine("InitializeVariables();");

            // 设置根控件
            std::string defaultRoot = widgetClass.GetDefaultRootVariableName();
            if (!defaultRoot.empty())
            {
                context.writeLine("SetRootWidget(m_" + defaultRoot + ");");
            }

            context.decreaseIndent();
            context.writeLine("}");
            context.writeEmptyLine();

            // 析构函数
            context.writeLine(context.className + "::~" + context.className + "()");
            context.writeLine("{");
            context.increaseIndent();

            GenerateDestructorCode(widgetClass, context);

            context.decreaseIndent();
            context.writeLine("}");
            context.writeEmptyLine();

            // InitializeVariables函数
            context.writeLine("void " + context.className + "::InitializeVariables()");
            context.writeLine("{");
            context.increaseIndent();

            GenerateInitializeVariablesCode(widgetClass, context);

            context.decreaseIndent();
            context.writeLine("}");

            // 结束命名空间
            if (!context.namespaceName.empty())
            {
                context.decreaseIndent();
                context.writeLine("} // namespace " + context.namespaceName);
            }

            sourceFile.close();
            return true;
        }

        // 收集需要的头文件
        static void CollectRequiredHeaders(const ImUserWidgetClass& widgetClass, std::set<std::string>& headers)
        {
            // 添加基础头文件
            headers.insert("\"ImWidget.h\"");
            headers.insert("\"ImPanelWidget.h\"");

            // 根据控件变量添加头文件
            auto widgetVars = widgetClass.GetWidgetVariableNames();
            for (const auto& varName : widgetVars)
            {
                ImWidget* widget = widgetClass.GetWidgetVariable(varName);
                if (widget)
                {
                    headers.insert("\"" + widget->GetRegisterTypeName() + ".h\"");
                }
            }
        }

        // 生成析构函数代码
        static void GenerateDestructorCode(const ImUserWidgetClass& widgetClass, ClassGenContext& context)
        {
            auto widgetVars = widgetClass.GetWidgetVariableNames();
            auto objectVars = widgetClass.GetObjectVariableNames();

            context.writeLine("// 释放控件");
            for (const auto& varName : widgetVars)
            {
                context.writeLine("if (m_" + varName + ") delete m_" + varName + ";");
            }

            context.writeEmptyLine();
            context.writeLine("// 释放对象");
            for (const auto& varName : objectVars)
            {
                context.writeLine("if (m_" + varName + ") delete m_" + varName + ";");
            }
        }

        // 生成InitializeVariables函数代码
        static void GenerateInitializeVariablesCode(const ImUserWidgetClass& widgetClass, ClassGenContext& context)
        {
            auto basicVars = widgetClass.GetBasicVariableNames();
            auto objectVars = widgetClass.GetObjectVariableNames();
            auto widgetVars = widgetClass.GetWidgetVariableNames();

            // 收集所有控件（包括子控件）
            std::vector<std::pair<std::string, ImWidget*>> allWidgets;
            for (const auto& varName : widgetVars)
            {
                ImWidget* widget = widgetClass.GetWidgetVariable(varName);
                if (widget)
                {
                    allWidgets.push_back({ varName, widget });
                    CollectChildWidgets(widget, varName, allWidgets);
                }
            }

            context.writeLine("//===Auto Gen Begin=== (InitializeVariables)");

            // 生成基本变量初始化
            if (!basicVars.empty())
            {
                context.writeLine("// 初始化基本变量");
                for (const auto& varName : basicVars)
                {
                    ImBasicVariable* var = widgetClass.GetBasicVariable(varName);
                    if (!var) continue;

                    auto props = var->GetProperties();
                    if (props.empty()) continue;

                    auto prop = *props.begin();
                    void* valuePtr = var->GetValuePtr();
                    if (!valuePtr) continue;

                    std::string valueCode = ValueToCode(prop.type, valuePtr);
                    context.writeLine("m_" + varName + " = " + valueCode + ";");
                }
                context.writeEmptyLine();
            }

            // 生成ImObject变量初始化
            if (!objectVars.empty())
            {
                context.writeLine("// 初始化ImObject变量");
                for (const auto& varName : objectVars)
                {
                    ImObject* obj = widgetClass.GetObjectVariable(varName);
                    if (!obj) continue;

                    std::string typeName = obj->GetRegisterTypeName();
                    size_t pos = typeName.find_last_of("::");
                    if (pos != std::string::npos)
                    {
                        typeName = typeName.substr(pos + 1);
                    }

                    // 使用new直接创建对象
                    context.writeLine("m_" + varName + " = new " + typeName + "();");

                    // 设置对象属性
                    auto props = obj->GetProperties();
                    for (const auto& prop : props)
                    {
                        void* valuePtr = prop.getter();
                        if (!valuePtr) continue;

                        if (prop.type == PropertyType::Struct)
                        {
                            // 处理嵌套结构体
                            ImObject* nestedStruct = static_cast<ImObject*>(valuePtr);
                            std::string nestedVarName = varName + "_" + prop.name;

                            context.writeLine("ImObject* " + nestedVarName + " = m_" + varName +
                                "->GetPropertyPtr<ImObject>(\"" + prop.name + "\");");

                            // 递归处理嵌套结构体的属性
                            GenerateNestedObjectProperties(nestedStruct, nestedVarName, context);
                        }
                        else
                        {
                            std::string valueCode = ValueToCode(prop.type, valuePtr);
                            context.writeLine("m_" + varName + "->SetPropertyValue<" +
                                PropertyTypeToCppTypeString(prop.type) + ">(\"" +
                                prop.name + "\", " + valueCode + ");");
                        }
                    }
                }
                context.writeEmptyLine();
            }

            // 生成控件变量初始化（包括子控件）
            if (!allWidgets.empty())
            {
                context.writeLine("// 初始化控件变量");

                // 先创建所有控件
                for (const auto& [varName, widget] : allWidgets)
                {
                    std::string typeName = widget->GetRegisterTypeName();
                    size_t pos = typeName.find_last_of("::");
                    if (pos != std::string::npos)
                    {
                        typeName = typeName.substr(pos + 1);
                    }

                    // 使用new直接创建控件，使用控件本身的名称作为构造参数
                    std::string widgetName = widget->GetWidgetName();
                    if (widgetName.empty())
                    {
                        widgetName = varName; // 如果控件没有名称，使用变量名
                    }

                    context.writeLine("m_" + varName + " = new " + typeName + "(\"" + widgetName + "\");");
                }
                context.writeEmptyLine();

                // 然后设置控件属性
                for (const auto& [varName, widget] : allWidgets)
                {
                    // 设置控件属性
                    auto props = widget->GetProperties();
                    for (const auto& prop : props)
                    {
                        void* valuePtr = prop.getter();
                        if (!valuePtr) continue;

                        if (prop.type == PropertyType::Struct)
                        {
                            ImObject* nestedStruct = static_cast<ImObject*>(valuePtr);
                            std::string nestedVarName = varName + "_" + prop.name;

                            context.writeLine("ImObject* " + nestedVarName + " = m_" + varName +
                                "->GetPropertyPtr<ImObject>(\"" + prop.name + "\");");

                            // 递归处理嵌套结构体
                            GenerateNestedObjectProperties(nestedStruct, nestedVarName, context);
                        }
                        else
                        {
                            std::string valueCode = ValueToCode(prop.type, valuePtr);
                            context.writeLine("m_" + varName + "->SetPropertyValue<" +
                                PropertyTypeToCppTypeString(prop.type) + ">(\"" +
                                prop.name + "\", " + valueCode + ");");
                        }
                    }
                }
                context.writeEmptyLine();

                // 最后建立父子关系
                context.writeLine("// 建立控件树父子关系");
                for (const auto& [varName, widget] : allWidgets)
                {
                    ImPanelWidget* panelWidget = dynamic_cast<ImPanelWidget*>(widget);
                    if (!panelWidget) continue;

                    int childCount = panelWidget->GetChildNum();
                    for (int i = 0; i < childCount; i++)
                    {
                        ImWidget* childWidget = panelWidget->GetChildAt(i);
                        if (!childWidget) continue;

                        // 查找子控件对应的变量名
                        std::string childVarName = FindWidgetVariableName(childWidget, allWidgets);
                        if (childVarName.empty()) continue;

                        // 添加子控件到父控件
                        context.writeLine("m_" + varName + "->AddChild(m_" + childVarName + ");");
                    }
                }
            }

            context.writeLine("//===Auto Gen End=== (InitializeVariables)");
        }

        // 在控件列表中查找控件对应的变量名
        static std::string FindWidgetVariableName(ImWidget* targetWidget,
            const std::vector<std::pair<std::string, ImWidget*>>& allWidgets)
        {
            for (const auto& [varName, widget] : allWidgets)
            {
                if (widget == targetWidget)
                {
                    return varName;
                }
            }
            return "";
        }

        // 生成嵌套对象属性的递归函数
        static void GenerateNestedObjectProperties(ImObject* nestedObj, const std::string& varName, ClassGenContext& context)
        {
            if (!nestedObj) return;

            auto props = nestedObj->GetProperties();
            for (const auto& prop : props)
            {
                void* valuePtr = prop.getter();
                if (!valuePtr) continue;

                if (prop.type == PropertyType::Struct)
                {
                    // 更深层次的嵌套
                    ImObject* deeperNested = static_cast<ImObject*>(valuePtr);
                    std::string deeperVarName = varName + "_" + prop.name;

                    context.writeLine("ImObject* " + deeperVarName + " = " + varName +
                        "->GetPropertyPtr<ImObject>(\"" + prop.name + "\");");

                    GenerateNestedObjectProperties(deeperNested, deeperVarName, context);
                }
                else
                {
                    std::string valueCode = ValueToCode(prop.type, valuePtr);
                    context.writeLine(varName + "->SetPropertyValue<" +
                        PropertyTypeToCppTypeString(prop.type) + ">(\"" +
                        prop.name + "\", " + valueCode + ");");
                }
            }
        }

        // 生成子控件初始化和父子关系设置
        static void GenerateChildWidgetsInitialization(ImWidget* parentWidget, const std::string& parentVarName, ClassGenContext& context)
        {
            // 检查是否是面板控件（可以包含子控件）
            ImPanelWidget* panelWidget = dynamic_cast<ImPanelWidget*>(parentWidget);
            if (!panelWidget) return;

            int childCount = panelWidget->GetChildNum();
            if (childCount == 0) return;

            context.writeLine("// 设置 " + parentVarName + " 的子控件");

            for (int i = 0; i < childCount; i++)
            {
                ImSlot* slot = panelWidget->GetSlotAt(i);
                if (!slot || !slot->GetContent()) continue;

                ImWidget* childWidget = slot->GetContent();
                std::string childVarName = parentVarName + "_Child" + std::to_string(i);

                // 创建子控件
                context.writeLine("ImWidget* " + childVarName + " = ImWidgetFactory::GetInstance().CreateWidget(\"" +
                    childWidget->GetRegisterTypeName() + "\", \"" +
                    childWidget->GetWidgetName() + "\");");

                // 设置子控件属性
                auto childProps = childWidget->GetProperties();
                for (const auto& prop : childProps)
                {
                    void* valuePtr = prop.getter();
                    if (!valuePtr) continue;

                    std::string valueCode = ValueToCode(prop.type, valuePtr);
                    context.writeLine(childVarName + "->SetPropertyValue<" +
                        PropertyTypeToCppTypeString(prop.type) + ">(\"" +
                        prop.name + "\", " + valueCode + ");");
                }

                // 添加子控件到父控件
                context.writeLine("ImSlot* " + childVarName + "_Slot = " +
                    parentVarName + "->AddChild(" + childVarName + ");");

                // 设置slot属性
                auto slotProps = slot->GetProperties();
                for (const auto& slotProp : slotProps)
                {
                    void* slotValuePtr = slotProp.getter();
                    if (!slotValuePtr) continue;

                    std::string slotValueCode = ValueToCode(slotProp.type, slotValuePtr);
                    context.writeLine(childVarName + "_Slot->SetPropertyValue<" +
                        PropertyTypeToCppTypeString(slotProp.type) + ">(\"" +
                        slotProp.name + "\", " + slotValueCode + ");");
                }

                // 递归处理子控件的子控件
                GenerateChildWidgetsInitialization(childWidget, childVarName, context);
            }
            context.writeEmptyLine();
        }

        // 更新已存在的源文件（改进版本）
        static bool UpdateExistingSourceFile(const ImUserWidgetClass& widgetClass,
            const std::string& className,
            const std::string& filePath)
        {
            // 生成新的InitializeVariables内容
            std::ofstream initStream;
            ClassGenContext initContext{ initStream, 1, className, widgetClass.GetNamespace() };
            GenerateInitializeVariablesCode(widgetClass, initContext);

            std::stringstream buffer;
            buffer << initStream.rdbuf();
            std::string newInitContent = buffer.str();

            // 替换标记区域
            return ReplaceMarkedRegionInFile(filePath, "InitializeVariables", newInitContent);
        }

    public:
        // 导出用户控件类到C++文件（主入口函数）
        static bool ExportUserWidgetClassToFiles(const ImUserWidgetClass& widgetClass,
            const std::string& className,
            const std::string& headerOutputPath,
            const std::string& sourceOutputPath)
        {
            if (className.empty())
            {
                std::cerr << "错误: 类名为空\n";
                return false;
            }

            // 生成头文件
            if (!GenerateHeaderFile(widgetClass, className, headerOutputPath))
            {
                std::cerr << "生成头文件失败: " << headerOutputPath << "\n";
                return false;
            }

            // 生成源文件
            if (!GenerateSourceFile(widgetClass, className, sourceOutputPath))
            {
                std::cerr << "生成源文件失败: " << sourceOutputPath << "\n";
                return false;
            }

            std::cout << "成功生成用户控件类: " << className << std::endl;
            std::cout << "头文件: " << headerOutputPath << std::endl;
            std::cout << "源文件: " << sourceOutputPath << std::endl;

            return true;
        }
    };

    // 静态常量定义
    const std::string ImUserWidgetClassCodeGenerator::AUTO_GEN_BEGIN = "//===Auto Gen Begin===";
    const std::string ImUserWidgetClassCodeGenerator::AUTO_GEN_END = "//===Auto Gen End===";
}