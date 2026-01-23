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
#include "ImWidgetCodeGenerator.h"  // 重用现有的代码生成工具
#include "ImUserWidgetSerializer.h"

namespace ImGuiWidget
{
    // 用户控件类代码生成器
    class ImUserWidgetClassCodeGenerator
    {
    private:
        // 代码生成上下文
        struct ClassGenContext
        {
            std::ofstream& oss;          // 输出流
            int indentLevel = 0;              // 当前缩进级别
            std::string className;            // 生成的类名
            std::set<std::string> usedHeaders; // 已使用的头文件

            std::string indentStr() const
            {
                return std::string(indentLevel * 4, ' ');
            }
        };

        // 检查字符串是否包含中文
        static bool ContainsChinese(const std::string& str)
        {
            for (char c : str)
            {
                if (static_cast<unsigned char>(c) >= 0x80)
                {
                    return true;
                }
            }
            return false;
        }

        // 为字符串生成带u8前缀的代码
        static std::string StringToCode(const std::string& str)
        {
            if (ContainsChinese(str))
            {
                return "u8\"" + str + "\"";
            }
            return "\"" + str + "\"";
        }

        // 获取类型对应的头文件名
        static std::string TypeToHeader(const std::string& typeName)
        {
            // 移除可能的命名空间前缀
            std::string header = typeName;
            size_t pos = header.find_last_of("::");
            if (pos != std::string::npos)
            {
                header = header.substr(pos + 1);
            }
            return header + ".h";
        }

        // 生成基本变量的初始化代码
        static void GenerateBasicVariableCode(ImBasicVariable* var,
            const std::string& varName,
            ClassGenContext& context)
        {
            if (!var) return;

            auto properties = var->GetProperties();
            if (properties.empty()) return;

            // 基本变量只有一个属性
            auto prop = *properties.begin();
            void* valuePtr = var->GetValuePtr();
            if (!valuePtr) return;

            std::string valueCode;
            switch (var->GetBasicType())
            {
            case ImBasicVariable::BasicType::Int:
                valueCode = std::to_string(*static_cast<int*>(valuePtr));
                break;
            case ImBasicVariable::BasicType::Float:
                valueCode = std::to_string(*static_cast<float*>(valuePtr)) + "f";
                break;
            case ImBasicVariable::BasicType::Bool:
                valueCode = *static_cast<bool*>(valuePtr) ? "true" : "false";
                break;
            case ImBasicVariable::BasicType::String:
                valueCode = StringToCode(*static_cast<std::string*>(valuePtr));
                break;
            case ImBasicVariable::BasicType::Color:
                valueCode = ColorToCode(*static_cast<ImU32*>(valuePtr));
                break;
            }

            context.oss << context.indentStr()
                << varName << " = new ImBasicVariable(\"" << var->GetName()
                << "\", ImBasicVariable::BasicType::"
                << BasicTypeToString(var->GetBasicType())
                << ");\n";

            context.oss << context.indentStr()
                << varName << "->SetPropertyValue<"
                << PropertyTypeToCppTypeString(prop.type)
                << ">(\"" << prop.name << "\", " << valueCode << ");\n\n";
        }

        // 生成ImObject变量的初始化代码
        static void GenerateObjectVariableCode(ImObject* obj,
            const std::string& varName,
            ClassGenContext& context)
        {
            if (!obj) return;

            std::string typeName = obj->GetRegisterTypeName();
            context.usedHeaders.insert(TypeToHeader(typeName));

            context.oss << context.indentStr()
                << varName << " = ImObjectFactory::GetInstance().CreateObject(\""
                << typeName << "\");\n";

            // 生成属性设置代码
            auto properties = obj->GetProperties();
            for (const auto& prop : properties)
            {
                void* valuePtr = prop.getter();
                if (!valuePtr) continue;

                if (prop.type == PropertyType::Struct)
                {
                    ImObject* nestedStruct = static_cast<ImObject*>(valuePtr);
                    std::string nestedAccessor = varName + "_" + prop.name;

                    context.oss << context.indentStr()
                        << "ImObject* " << nestedAccessor
                        << " = " << varName << "->GetPropertyPtr<ImObject>(\""
                        << prop.name << "\");\n";

                    // 递归生成嵌套结构体属性
                    GenerateObjectVariableCode(nestedStruct, nestedAccessor, context);
                }
                else
                {
                    std::string valueCode = ValueToCode(prop.type, valuePtr);
                    context.oss << context.indentStr()
                        << varName << "->SetPropertyValue<"
                        << PropertyTypeToCppTypeString(prop.type)
                        << ">(\"" << prop.name << "\", " << valueCode << ");\n";
                }
            }
            context.oss << "\n";
        }

        // 生成控件树变量的初始化代码
        static void GenerateWidgetTreeCode(ImWidget* widget,
            const std::string& varName,
            ClassGenContext& context)
        {
            if (!widget) return;

            // 收集控件类型所需的头文件
            context.usedHeaders.insert(TypeToHeader(widget->GetRegisterTypeName()));

            // 创建控件
            context.oss << context.indentStr()
                << varName << " = ImWidgetFactory::GetInstance().CreateWidget(\""
                << widget->GetRegisterTypeName() << "\", \""
                << widget->GetWidgetName() << "\");\n";

            // 设置控件属性
            auto properties = widget->GetProperties();
            for (const auto& prop : properties)
            {
                void* valuePtr = prop.getter();
                if (!valuePtr) continue;

                if (prop.type == PropertyType::Struct)
                {
                    ImObject* nestedStruct = static_cast<ImObject*>(valuePtr);
                    std::string nestedAccessor = varName + "_" + prop.name;

                    context.oss << context.indentStr()
                        << "ImObject* " << nestedAccessor
                        << " = " << varName << "->GetPropertyPtr<ImObject>(\""
                        << prop.name << "\");\n";

                    // 递归生成嵌套结构体属性
                    GenerateObjectVariableCode(nestedStruct, nestedAccessor, context);
                }
                else
                {
                    std::string valueCode = ValueToCode(prop.type, valuePtr);
                    context.oss << context.indentStr()
                        << varName << "->SetPropertyValue<"
                        << PropertyTypeToCppTypeString(prop.type)
                        << ">(\"" << prop.name << "\", " << valueCode << ");\n";
                }
            }

            // 处理子控件
            if (auto panel = dynamic_cast<ImPanelWidget*>(widget))
            {
                for (int i = 0; i < panel->GetChildNum(); i++)
                {
                    ImSlot* slot = panel->GetSlotAt(i);
                    if (!slot || !slot->GetContent()) continue;

                    ImWidget* child = slot->GetContent();
                    std::string childVarName = varName + "_child" + std::to_string(i);

                    // 递归生成子控件
                    GenerateWidgetTreeCode(child, childVarName, context);

                    // 添加子控件到父控件
                    std::string slotVarName = varName + "_slot" + std::to_string(i);
                    context.oss << context.indentStr()
                        << "ImSlot* " << slotVarName << " = "
                        << varName << "->AddChild(" << childVarName << ");\n";

                    // 设置slot属性
                    auto slotProps = slot->GetProperties();
                    for (const auto& slotProp : slotProps)
                    {
                        void* slotValuePtr = slotProp.getter();
                        if (!slotValuePtr) continue;

                        std::string slotValueCode = ValueToCode(slotProp.type, slotValuePtr);
                        context.oss << context.indentStr()
                            << slotVarName << "->SetPropertyValue<"
                            << PropertyTypeToCppTypeString(slotProp.type)
                            << ">(\"" << slotProp.name << "\", "
                            << slotValueCode << ");\n";
                    }
                    context.oss << "\n";
                }
            }
            context.oss << "\n";
        }

        // 生成类的头文件
        static bool GenerateHeaderFile(const ImUserWidgetClass& widgetClass,
            const std::string& className,
            const std::string& headerOutputPath)
        {
            std::ofstream headerFile(headerOutputPath);
            if (!headerFile.is_open()) return false;

            // 写入UTF-8 BOM
            const unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
            headerFile.write(reinterpret_cast<const char*>(bom), sizeof(bom));

            // 生成头文件保护宏
            std::string guardMacro = className;
            std::transform(guardMacro.begin(), guardMacro.end(), guardMacro.begin(), ::toupper);
            guardMacro += "_H";

            headerFile << "#pragma once\n\n";
            headerFile << "#include \"ImUserWidget.h\"\n";
            headerFile << "#include \"ImObject.h\"\n";
            headerFile << "#include \"ImBasicVariable.h\"\n\n";
            headerFile << "namespace ImGuiWidget\n{\n\n";

            // 类声明
            headerFile << "class " << className << " : public ImUserWidget\n";
            headerFile << "{\n";
            headerFile << "public:\n";
            headerFile << "    " << className << "(const std::string& name);\n";
            headerFile << "    virtual ~" << className << "();\n\n";
            headerFile << "    void InitializeVariables();\n\n";

            // 变量访问器
            headerFile << "    // 变量访问器\n";

            // 控件树变量
            auto widgetVars = widgetClass.GetWidgetVariableNames();
            for (const auto& varName : widgetVars)
            {
                headerFile << "    ImWidget* Get" << varName << "() const { return m_"
                    << varName << "; }\n";
            }

            // ImObject变量
            auto objectVars = widgetClass.GetObjectVariableNames();
            for (const auto& varName : objectVars)
            {
                headerFile << "    ImObject* Get" << varName << "() const { return m_"
                    << varName << "; }\n";
            }

            // 基本变量
            auto basicVars = widgetClass.GetBasicVariableNames();
            for (const auto& varName : basicVars)
            {
                headerFile << "    ImBasicVariable* Get" << varName << "() const { return m_"
                    << varName << "; }\n";
            }

            headerFile << "\nprotected:\n";
            headerFile << "    // 控件树变量\n";
            for (const auto& varName : widgetVars)
            {
                headerFile << "    ImWidget* m_" << varName << " = nullptr;\n";
            }

            headerFile << "\n    // ImObject变量\n";
            for (const auto& varName : objectVars)
            {
                headerFile << "    ImObject* m_" << varName << " = nullptr;\n";
            }

            headerFile << "\n    // 基本变量\n";
            for (const auto& varName : basicVars)
            {
                headerFile << "    ImBasicVariable* m_" << varName << " = nullptr;\n";
            }

            headerFile << "};\n\n";
            headerFile << "} // namespace ImGuiWidget\n";

            headerFile.close();
            return true;
        }

        // 生成类的源文件
        static bool GenerateSourceFile(const ImUserWidgetClass& widgetClass,
            const std::string& className,
            const std::string& sourceOutputPath)
        {
            std::ofstream sourceFile(sourceOutputPath);
            if (!sourceFile.is_open()) return false;

            // 写入UTF-8 BOM
            const unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
            sourceFile.write(reinterpret_cast<const char*>(bom), sizeof(bom));

            // 收集所有需要的头文件
            std::set<std::string> allHeaders;
            allHeaders.insert("\"" + className + ".h\"");
            allHeaders.insert("\"ImObjectFactory.h\"");
            allHeaders.insert("\"ImWidgetFactory.h\"");
            allHeaders.insert("\"ImUserWidgetClass.h\"");

            // 从控件树收集头文件
            auto widgetVars = widgetClass.GetWidgetVariableNames();
            for (const auto& varName : widgetVars)
            {
                ImWidget* widget = widgetClass.GetWidgetVariable(varName);
                if (widget)
                {
                    std::function<void(ImWidget*)> collectHeaders =
                        [&](ImWidget* w)
                    {
                        if (!w) return;
                        allHeaders.insert(TypeToHeader(w->GetRegisterTypeName()));

                        if (auto panel = dynamic_cast<ImPanelWidget*>(w))
                        {
                            for (int i = 0; i < panel->GetChildNum(); i++)
                            {
                                ImSlot* slot = panel->GetSlotAt(i);
                                if (slot && slot->GetContent())
                                {
                                    collectHeaders(slot->GetContent());
                                }
                            }
                        }
                    };

                    collectHeaders(widget);
                }
            }

            // 从ImObject变量收集头文件
            auto objectVars = widgetClass.GetObjectVariableNames();
            for (const auto& varName : objectVars)
            {
                ImObject* obj = widgetClass.GetObjectVariable(varName);
                if (obj)
                {
                    allHeaders.insert(TypeToHeader(obj->GetRegisterTypeName()));
                }
            }

            auto basicVars = widgetClass.GetBasicVariableNames();

            // 写入头文件包含
            sourceFile << "// Auto-generated file - DO NOT EDIT\n\n";
            for (const auto& header : allHeaders)
            {
                sourceFile << "#include " << header << "\n";
            }
            sourceFile << "\n";

            // 开始命名空间
            sourceFile << "namespace ImGuiWidget\n{\n\n";

            // 构造函数
            sourceFile << className << "::" << className << "(const std::string& name)\n";
            sourceFile << "    : ImUserWidget(name)\n";
            sourceFile << "{\n";
            sourceFile << "    InitializeVariables();\n";

            // 设置根控件
            std::string defaultRoot = widgetClass.GetDefaultRootVariableName();
            if (!defaultRoot.empty())
            {
                sourceFile << "    SetRootWidget(m_" << defaultRoot << ");\n";
            }

            sourceFile << "}\n\n";

            // 析构函数
            sourceFile << className << "::~" << className << "()\n";
            sourceFile << "{\n";
            for (const auto& varName : widgetVars)
            {
                sourceFile << "    if (m_" << varName << ") delete m_" << varName << ";\n";
            }
            for (const auto& varName : objectVars)
            {
                sourceFile << "    if (m_" << varName << ") delete m_" << varName << ";\n";
            }
            //for (const auto& varName : basicVars)
            //{
            //    sourceFile << "    if (m_" << varName << ") delete m_" << varName << ";\n";
            //}
            sourceFile << "}\n\n";

            // InitializeVariables函数
            sourceFile << "void " << className << "::InitializeVariables()\n";
            sourceFile << "{\n";

            ClassGenContext context{ sourceFile, 1, className, allHeaders};

            // 生成基本变量初始化代码
            if (!basicVars.empty())
            {
                sourceFile << "    // 初始化基本变量\n";
                for (const auto& varName : basicVars)
                {
                    ImBasicVariable* var = widgetClass.GetBasicVariable(varName);
                    GenerateBasicVariableCode(var, "m_" + varName, context);
                }
            }

            // 生成ImObject变量初始化代码
            if (!objectVars.empty())
            {
                sourceFile << "    // 初始化ImObject变量\n";
                for (const auto& varName : objectVars)
                {
                    ImObject* obj = widgetClass.GetObjectVariable(varName);
                    GenerateObjectVariableCode(obj, "m_" + varName, context);
                }
            }

            // 生成控件树变量初始化代码
            if (!widgetVars.empty())
            {
                sourceFile << "    // 初始化控件树变量\n";
                for (const auto& varName : widgetVars)
                {
                    ImWidget* widget = widgetClass.GetWidgetVariable(varName);
                    GenerateWidgetTreeCode(widget, "m_" + varName, context);
                }
            }

            sourceFile << "}\n\n";
            sourceFile << "} // namespace ImGuiWidget\n";

            sourceFile.close();
            return true;
        }

    public:
        // 将ImUserWidgetClass导出为C++类文件
        static bool ExportUserWidgetClassToFiles(const ImUserWidgetClass& widgetClass,
            const std::string& className,
            const std::string& headerOutputPath,
            const std::string& sourceOutputPath)
        {
            // 检查类名是否有效
            if (className.empty())
            {
                std::cerr << "Error: Class name cannot be empty\n";
                return false;
            }

            // 生成头文件
            if (!GenerateHeaderFile(widgetClass, className, headerOutputPath))
            {
                std::cerr << "Error: Failed to generate header file\n";
                return false;
            }

            // 生成源文件
            if (!GenerateSourceFile(widgetClass, className, sourceOutputPath))
            {
                std::cerr << "Error: Failed to generate source file\n";
                return false;
            }

            return true;
        }

        // 将ImUserWidgetClass导出为单头文件（适合小项目）
        static bool ExportUserWidgetClassToSingleHeader(const ImUserWidgetClass& widgetClass,
            const std::string& className,
            const std::string& outputPath)
        {
            std::ofstream file(outputPath);
            if (!file.is_open()) return false;

            // 写入UTF-8 BOM
            const unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
            file.write(reinterpret_cast<const char*>(bom), sizeof(bom));

            // 生成头文件保护宏
            std::string guardMacro = className;
            std::transform(guardMacro.begin(), guardMacro.end(), guardMacro.begin(), ::toupper);
            guardMacro += "_H";

            file << "#pragma once\n\n";
            file << "#include \"ImUserWidget.h\"\n";
            file << "#include \"ImObject.h\"\n";
            file << "#include \"ImBasicVariable.h\"\n";
            file << "#include \"ImObjectFactory.h\"\n";
            file << "#include \"ImWidgetFactory.h\"\n";
            file << "#include \"ImUserWidgetClass.h\"\n\n";

            // 收集所有需要的额外头文件
            std::set<std::string> allHeaders;

            auto widgetVars = widgetClass.GetWidgetVariableNames();
            for (const auto& varName : widgetVars)
            {
                ImWidget* widget = widgetClass.GetWidgetVariable(varName);
                if (widget)
                {
                    std::function<void(ImWidget*)> collectHeaders =
                        [&](ImWidget* w)
                    {
                        if (!w) return;
                        allHeaders.insert(TypeToHeader(w->GetRegisterTypeName()));

                        if (auto panel = dynamic_cast<ImPanelWidget*>(w))
                        {
                            for (int i = 0; i < panel->GetChildNum(); i++)
                            {
                                ImSlot* slot = panel->GetSlotAt(i);
                                if (slot && slot->GetContent())
                                {
                                    collectHeaders(slot->GetContent());
                                }
                            }
                        }
                    };

                    collectHeaders(widget);
                }
            }

            // 写入额外头文件
            for (const auto& header : allHeaders)
            {
                file << "#include \"" << header << "\"\n";
            }
            file << "\n";

            file << "namespace ImGuiWidget\n{\n\n";

            // 类声明
            file << "class " << className << " : public ImUserWidget\n";
            file << "{\n";
            file << "public:\n";
            file << "    " << className << "(const std::string& name)\n";
            file << "        : ImUserWidget(name)\n";
            file << "    {\n";
            file << "        InitializeVariables();\n";

            // 设置根控件
            std::string defaultRoot = widgetClass.GetDefaultRootVariableName();
            if (!defaultRoot.empty())
            {
                file << "        SetRootWidget(m_" << defaultRoot << ");\n";
            }
            file << "    }\n\n";

            file << "    virtual ~" << className << "()\n";
            file << "    {\n";
            for (const auto& varName : widgetVars)
            {
                file << "        if (m_" << varName << ") delete m_" << varName << ";\n";
            }
            auto objectVars = widgetClass.GetObjectVariableNames();
            for (const auto& varName : objectVars)
            {
                file << "        if (m_" << varName << ") delete m_" << varName << ";\n";
            }
            auto basicVars = widgetClass.GetBasicVariableNames();
            for (const auto& varName : basicVars)
            {
                file << "        if (m_" << varName << ") delete m_" << varName << ";\n";
            }
            file << "    }\n\n";

            // 变量访问器
            file << "    // 变量访问器\n";
            for (const auto& varName : widgetVars)
            {
                file << "    ImWidget* Get" << varName << "() const { return m_"
                    << varName << "; }\n";
            }
            for (const auto& varName : objectVars)
            {
                file << "    ImObject* Get" << varName << "() const { return m_"
                    << varName << "; }\n";
            }
            for (const auto& varName : basicVars)
            {
                file << "    ImBasicVariable* Get" << varName << "() const { return m_"
                    << varName << "; }\n";
            }

            file << "\nprivate:\n";
            file << "    void InitializeVariables()\n";
            file << "    {\n";

            // 生成初始化代码（简化版本，完整版需要大量代码）
            if (!basicVars.empty())
            {
                file << "        // 初始化基本变量\n";
                for (const auto& varName : basicVars)
                {
                    ImBasicVariable* var = widgetClass.GetBasicVariable(varName);
                    if (var)
                    {
                        auto props = var->GetProperties();
                        if (!props.empty())
                        {
                            auto prop = *props.begin();
                            void* valuePtr = var->GetValuePtr();
                            if (valuePtr)
                            {
                                std::string valueCode = ValueToCode(prop.type, valuePtr);
                                file << "        m_" << varName << " = new ImBasicVariable(\""
                                    << var->GetName() << "\", ImBasicVariable::BasicType::"
                                    << BasicTypeToString(var->GetBasicType()) << ");\n";
                                file << "        m_" << varName << "->SetPropertyValue<"
                                    << PropertyTypeToCppTypeString(prop.type) << ">(\""
                                    << prop.name << "\", " << valueCode << ");\n";
                            }
                        }
                    }
                }
            }

            // 注：完整实现需要更多代码，这里只提供基本框架

            file << "    }\n\n";

            // 成员变量
            file << "protected:\n";
            file << "    // 控件树变量\n";
            for (const auto& varName : widgetVars)
            {
                file << "    ImWidget* m_" << varName << " = nullptr;\n";
            }
            file << "\n    // ImObject变量\n";
            for (const auto& varName : objectVars)
            {
                file << "    ImObject* m_" << varName << " = nullptr;\n";
            }
            file << "\n    // 基本变量\n";
            for (const auto& varName : basicVars)
            {
                file << "    ImBasicVariable* m_" << varName << " = nullptr;\n";
            }

            file << "};\n\n";
            file << "} // namespace ImGuiWidget\n";

            file.close();
            return true;
        }
    };
}