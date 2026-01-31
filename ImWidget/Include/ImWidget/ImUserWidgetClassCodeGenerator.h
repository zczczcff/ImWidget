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

namespace ImGuiWidget
{
    class ImUserWidgetClassCodeGenerator
    {
    private:
        // 代码生成上下文
        struct ClassGenContext
        {
            std::ostream& oss;          // 输出流
            int indentLevel = 0;        // 当前缩进级别
            std::string className;      // 生成的类名
            std::string namespaceName;  // 命名空间
            std::set<std::string> usedHeaders; // 使用的头文件列表
            bool inNamespace = false;   // 是否在命名空间中

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

        // ================ 从ImWidgetCodeGenerator.h复制过来的辅助函数 ================

        // 辅助函数：将ImU32颜色转换为IM_COL32宏
        static std::string ColorToCode(ImU32 color)
        {
            int r = IM_COLOR_GET_R(color);
            int g = IM_COLOR_GET_G(color);
            int b = IM_COLOR_GET_B(color);
            int a = IM_COLOR_GET_A(color);

            std::ostringstream oss;
            oss << "IM_COL32(" << r << ", " << g << ", " << b << ", " << a << ")";
            return oss.str();
        }

        // 辅助函数：将ImVec2转换为代码
        static std::string Vec2ToCode(const ImVec2& vec)
        {
            std::ostringstream oss;
            oss << "ImVec2(" << std::to_string(vec.x) << "f, " << std::to_string(vec.y) << "f)";
            return oss.str();
        }

        // 定义中文检测函数
        static bool NeedsU8Prefix(const std::string& str)
        {
            for (char c : str)
            {
                if (static_cast<unsigned char>(c) >= 0x80)
                {
                    return true; // 检测到非ASCII字符
                }
            }
            return false;
        }

        // 辅助函数：将值转换为字符串表示
        static std::string ValueToCode(PropertyType type, void* valuePtr)
        {
            if (!valuePtr) return "";

            switch (type)
            {
            case PropertyType::Color:
                return ColorToCode(*static_cast<ImU32*>(valuePtr));

            case PropertyType::Float:
                return std::to_string(*static_cast<float*>(valuePtr)) + "f";

            case PropertyType::Bool:
                return (*static_cast<bool*>(valuePtr)) ? "true" : "false";

            case PropertyType::Int:
                return std::to_string(*static_cast<int*>(valuePtr));

            case PropertyType::String: {
                std::string str = *static_cast<std::string*>(valuePtr);
                return NeedsU8Prefix(str) ?
                    "u8\"" + str + "\"" :  // 含中文时添加u8前缀
                    "\"" + str + "\"";     // 不含中文保持原样
            }

            case PropertyType::Vec2:
                return Vec2ToCode(*static_cast<ImVec2*>(valuePtr));

            case PropertyType::Struct:
                return "nullptr";  // 结构体特殊处理

            case PropertyType::StringArray: {
                std::vector<std::string>* vec = static_cast<std::vector<std::string>*>(valuePtr);
                std::ostringstream oss;
                oss << "{";
                for (size_t i = 0; i < vec->size(); ++i)
                {
                    if (i > 0) oss << ", ";
                    if (NeedsU8Prefix((*vec)[i]))
                    {
                        oss << "u8\"" << (*vec)[i] << "\"";
                    }
                    else
                    {
                        oss << "\"" << (*vec)[i] << "\"";
                    }
                }
                oss << "}";
                return oss.str();
            }

            case PropertyType::Enum: {
                std::vector<std::string>* vec = static_cast<std::vector<std::string>*>(valuePtr);
                if (vec && !vec->empty())
                {
                    std::string selected = vec->back();
                    return NeedsU8Prefix(selected) ?
                        "u8\"" + selected + "\"" :  // 含中文时添加u8前缀
                        "\"" + selected + "\"";     // 不含中文保持原样
                }
                return "\"\""; // 空字符串作为默认
            }

            default:
                return "/* Unknown type */";
            }
        }

        // ================ 新的ROP属性辅助函数 ================

        // 辅助函数：将ROP属性值转换为代码
        static std::string ROPValueToCode(const ROP::Property<PropertyType>& prop)
        {
            if (!prop.IsValid()) return "";

            try
            {
                switch (prop.GetType())
                {
                case PropertyType::Color:
                    return ColorToCode(prop.GetValue<ImU32>());

                case PropertyType::Float:
                    return std::to_string(prop.GetValue<float>()) + "f";

                case PropertyType::Bool:
                    return prop.GetValue<bool>() ? "true" : "false";

                case PropertyType::Int:
                    return std::to_string(prop.GetValue<int>());

                case PropertyType::String: {
                    std::string str = prop.GetValue<std::string>();
                    return NeedsU8Prefix(str) ?
                        "u8\"" + str + "\"" : "\"" + str + "\"";
                }

                case PropertyType::Vec2:
                    return Vec2ToCode(prop.GetValue<ImVec2>());

                case PropertyType::Struct:
                    return "nullptr";  // 结构体特殊处理

                case PropertyType::StringArray: {
                    std::vector<std::string> vec = prop.GetValue<std::vector<std::string>>();
                    std::ostringstream oss;
                    oss << "{";
                    for (size_t i = 0; i < vec.size(); ++i)
                    {
                        if (i > 0) oss << ", ";
                        if (NeedsU8Prefix(vec[i]))
                        {
                            oss << "u8\"" + vec[i] << "\"";
                        }
                        else
                        {
                            oss << "\"" + vec[i] << "\"";
                        }
                    }
                    oss << "}";
                    return oss.str();
                }

                case PropertyType::Enum: {
                    // 处理枚举属性 - 通过OptionalProperty获取当前选项
                    auto optionalProp = prop.GetObject()->ToOptionalProperty(prop);
                    if (optionalProp.IsValid())
                    {
                        std::string selected = optionalProp.GetOptionString();
                        return NeedsU8Prefix(selected) ?
                            "u8\"" + selected + "\"" : "\"" + selected + "\"";
                    }
                    return "\"\"";
                }

                default:
                    return "/* Unknown type */";
                }
            }
            catch (const std::exception& e)
            {
                std::cerr << "Error converting ROP property value: " << e.what() << std::endl;
                return "";
            }
        }

        // 辅助函数：将PropertyType转换为类型名称字符串
        static std::string PropertyTypeToCppTypeString(PropertyType type)
        {
            switch (type)
            {
            case PropertyType::Color: return "ImU32";
            case PropertyType::Float: return "float";
            case PropertyType::Bool: return "bool";
            case PropertyType::Int: return "int";
            case PropertyType::String: return "std::string";
            case PropertyType::Vec2: return "ImVec2";
            case PropertyType::Struct: return "ImObject";
            case PropertyType::StringArray: return "std::vector<std::string>";
            case PropertyType::Enum: return "std::string";
            default: return "UnknownType";
            }
        }

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

        // ================ 新的ROP属性生成函数 ================

        // 生成ROP属性设置代码（处理重名属性）
        static void GenerateROPPropertySetCode(ROP::Property<PropertyType>& prop,
            const std::string& objectVarName,
            ClassGenContext& context)
        {
            if (!prop.IsValid()) return;

            std::string propName = prop.GetName();
            std::string className = prop.GetClassName();

            // 生成唯一属性键（处理重名属性）
            std::string propertyKey = propName;
            if (!className.empty() && className != prop.GetObject()->GetClassName())
            {
                // 如果是父类的属性，使用"类名::属性名"格式
                propertyKey = className + "::" + propName;
            }

            if (prop.GetType() == PropertyType::Enum)
            {
                // 处理枚举属性 - 通过OptionalProperty设置
                context.writeLine("{");
                context.increaseIndent();
                context.writeLine("auto optionalProp = " + objectVarName +
                    "->GetPropertyAsOptional(\"" + propName + "\", \"" + className + "\");");
                context.writeLine("if (optionalProp.IsValid())");
                context.writeLine("{");
                context.increaseIndent();

                auto optionalProp = prop.GetObject()->ToOptionalProperty(prop);
                std::string optionStr = optionalProp.GetOptionString();
                std::string optionCode = NeedsU8Prefix(optionStr) ?
                    "u8\"" + optionStr + "\"" :
                    "\"" + optionStr + "\"";

                context.writeLine("optionalProp.SetOptionByString(" + optionCode + ");");
                context.decreaseIndent();
                context.writeLine("}");
                context.decreaseIndent();
                context.writeLine("}");
            }
            else if (prop.GetType() == PropertyType::Struct)
            {
                // ================ 改进：完善结构体递归处理 ================
                context.writeLine("{");
                context.increaseIndent();
                context.writeLine(u8"// 处理嵌套结构体: " + propName);
                std::string nestedVarName = objectVarName + "_" + propName;
                // 获取嵌套对象
                context.writeLine("ImObject* "+ nestedVarName +" = " + objectVarName +
                    "->GetProperty(\"" + propName + "\", \"" + className + "\").GetPointer<ImObject>();");
                context.writeLine("if ("+ nestedVarName+ ")");
                context.writeLine("{");
                context.increaseIndent();

                // 递归处理嵌套结构体的属性
                
                context.writeLine(u8"// 递归设置嵌套结构体属性");
                ImObject* nestedObj = prop.GetPointer<ImObject>();
                GenerateROPObjectProperties(nestedObj, nestedVarName, context, true);

                context.decreaseIndent();
                context.writeLine("}");
                context.decreaseIndent();
                context.writeLine("}");
            }
            else
            {
                // 基本类型属性
                std::string valueCode = ROPValueToCode(prop);
                std::string typeStr = PropertyTypeToCppTypeString(prop.GetType());

                // 使用正确的类名和属性名
                if (!className.empty() && className != prop.GetObject()->GetClassName())
                {
                    // 父类的属性，需要指定类名
                    context.writeLine(objectVarName + "->GetProperty(\"" + propName + "\", \"" +
                        className + "\").SetValue<" + typeStr + ">(" + valueCode + ");");
                }
                else
                {
                    // 自身属性，可以直接使用属性名
                    context.writeLine(objectVarName + "->SetPropertyValue<" + typeStr +
                        ">(\"" + propName + "\", " + valueCode + ");");
                }
            }
        }

        // 生成ROP对象的所有属性（处理重名）
        static void GenerateROPObjectProperties(ImObject* obj, const std::string& varName,
            ClassGenContext& context, bool isNested = false)
        {
            if (!obj) return;

            // 获取所有属性（按顺序）
            auto allProps = obj->GetAllPropertiesOrdered();

            // 统计属性名出现次数，确定是否需要类名前缀
            std::unordered_map<std::string, int> nameCount;
            for (const auto& prop : allProps)
            {
                nameCount[prop.GetName()]++;
            }

            // 对于嵌套结构体，输出调试信息
            if (isNested)
            {
                context.writeLine(u8"// 开始设置嵌套对象 " + varName + u8" 的属性");
            }

            for (auto& prop : allProps)
            {
                std::string propName = prop.GetName();
                std::string className = prop.GetClassName();

                // 判断是否使用类名前缀
                bool useClassName = (nameCount[propName] > 1);

                if (useClassName)
                {
                    // 使用带类名的属性键
                    GenerateROPPropertySetCode(prop, varName, context);
                }
                else
                {
                    // 属性名唯一，直接使用属性名
                    if (prop.GetType() == PropertyType::Enum)
                    {
                        // 枚举属性需要特殊处理
                        context.writeLine("{");
                        context.increaseIndent();
                        context.writeLine("auto optionalProp = " + varName +
                            "->GetPropertyAsOptional(\"" + propName + "\");");
                        context.writeLine("if (optionalProp.IsValid())");
                        context.writeLine("{");
                        context.increaseIndent();

                        auto optionalProp = prop.GetObject()->ToOptionalProperty(prop);
                        std::string optionStr = optionalProp.GetOptionString();
                        std::string optionCode = NeedsU8Prefix(optionStr) ?
                            "u8\"" + optionStr + "\"" :
                            "\"" + optionStr + "\"";

                        context.writeLine("optionalProp.SetOptionByString(" + optionCode + ");");
                        context.decreaseIndent();
                        context.writeLine("}");
                        context.decreaseIndent();
                        context.writeLine("}");
                    }
                    else if (prop.GetType() == PropertyType::Struct)
                    {
                        // ================ 改进：处理唯一属性名但类型为Struct的情况 ================
                        context.writeLine("{");
                        context.increaseIndent();
                        context.writeLine(u8"// 处理嵌套结构体: " + propName);
                        std::string nestedVarName = varName + "_" + propName;
                        // 获取嵌套对象
                        context.writeLine("ImObject* "+ nestedVarName +" = " + varName +
                            "->GetProperty(\"" + propName + "\").GetPointer<ImObject>();");
                        context.writeLine("if ("+ nestedVarName+")");
                        context.writeLine("{");
                        context.increaseIndent();

                        // 递归处理嵌套结构体的属性
                        context.writeLine(u8"// 递归设置嵌套结构体属性");
                        ImObject* nestedObj = prop.GetPointer<ImObject>();
                        GenerateROPObjectProperties(nestedObj, nestedVarName, context, true);

                        context.decreaseIndent();
                        context.writeLine("}");
                        context.decreaseIndent();
                        context.writeLine("}");
                    }
                    else
                    {
                        // 基本类型
                        std::string valueCode = ROPValueToCode(prop);
                        std::string typeStr = PropertyTypeToCppTypeString(prop.GetType());
                        context.writeLine(varName + "->SetPropertyValue<" + typeStr +
                            ">(\"" + propName + "\", " + valueCode + ");");
                    }
                }
            }
        }

        // ================ 改进：基本变量直接获取值 ================

        // 生成基本变量初始化代码（直接获取值，不使用ROP）
        static void GenerateBasicVariableInitCode(const std::vector<std::string>& basicVars,
            const ImUserWidgetClass& widgetClass,
            ClassGenContext& context)
        {
            if (basicVars.empty()) return;

            context.writeLine("//===Auto Gen Begin=== (Basic Variables Init)");

            for (const auto& varName : basicVars)
            {
                ImBasicVariable* var = widgetClass.GetBasicVariable(varName);
                if (!var) continue;

                // 直接通过GetValuePtr获取值，不使用ROP
                void* valuePtr = var->GetValuePtr();
                if (!valuePtr) continue;

                switch (var->GetBasicType())
                {
                case ImBasicVariable::BasicType::Int:
                    context.writeLine(varName + " = " + std::to_string(*static_cast<int*>(valuePtr)) + ";");
                    break;
                case ImBasicVariable::BasicType::Float:
                    context.writeLine(varName + " = " + std::to_string(*static_cast<float*>(valuePtr)) + "f;");
                    break;
                case ImBasicVariable::BasicType::Bool:
                    context.writeLine(varName + " = " + (*static_cast<bool*>(valuePtr) ? "true" : "false") + ";");
                    break;
                case ImBasicVariable::BasicType::String: {
                    std::string str = *static_cast<std::string*>(valuePtr);
                    context.writeLine(varName + " = " + (NeedsU8Prefix(str) ? "u8\"" + str + "\"" : "\"" + str + "\"") + ";");
                    break;
                }
                case ImBasicVariable::BasicType::Color: {
                    ImU32 color = *static_cast<ImU32*>(valuePtr);
                    context.writeLine(varName + " = " + ColorToCode(color) + ";");
                    break;
                }
                }
            }

            context.writeLine("//===Auto Gen End=== (Basic Variables Init)");
            context.writeEmptyLine();
        }

        // 替换文件中标记区域的内容
        static bool ReplaceMarkedRegionInFile(const std::string& filePath,
            const std::string& marker,
            const std::string& newContent)
        {
            // 读取文件内容
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

            // 构建正则表达式模式
            std::string beginPattern = "//===Auto Gen Begin=== \\(" + marker + "\\)";
            std::string endPattern = "//===Auto Gen End=== \\(" + marker + "\\)\n";
            std::string beginline = "//===Auto Gen Begin=== (" + marker + ")";
            std::string endline = "//===Auto Gen End=== (" + marker + ")";

            // 匹配完整模式
            std::string fullPattern = beginPattern + "[\\s\\S]*?" + endPattern;

            std::regex pattern(fullPattern);

            // 查找标记区域的缩进
            std::string indent = "";
            std::smatch match;
            if (std::regex_search(content, match, std::regex(beginPattern + "\\s*\\n(\\s*)")))
            {
                indent = match[1]; // 获取缩进字符串
            }

            // 对替换内容进行缩进处理
            std::stringstream indentedContent;
            std::stringstream originalContent(newContent);
            std::string line;

            while (std::getline(originalContent, line))
            {
                // 跳过空行
                if (line.empty())
                {
                    indentedContent << "\n";
                    continue;
                }
                if (line == beginline)
                {
                    indentedContent << line << "\n";
                    continue;
                }
                if (line == endline)
                {
                    // 无需处理特殊标记
                }
                // 对非空行添加缩进
                indentedContent << indent << line << "\n";
            }

            std::string replacement = indentedContent.str();

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

            // 写入文件
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

        // 递归收集所有子控件
        static void CollectChildWidgets(ImWidget* parentWidget, const std::string& parentVarName,
            std::vector<std::pair<std::string, ImWidget*>>& allWidgets)
        {
            if (!parentWidget) return;

            // 检查是否是容器控件
            try
            {
                int childCount = parentWidget->GetChildNum();
                for (int i = 0; i < childCount; i++)
                {
                    ImWidget* childWidget = parentWidget->GetChildAt(i);
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
            catch (...)
            {
                // 控件不支持子控件，跳过
            }
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
                context.writeLine(u8"// 控件变量");
                for (const auto& [varName, widget] : allWidgets)
                {
                    std::string typeName = widget->GetRegisterTypeName();
                    // 提取类型名（去掉命名空间）
                    size_t pos = typeName.find_last_of("::");
                    if (pos != std::string::npos)
                    {
                        typeName = typeName.substr(pos + 1);
                    }
                    context.writeLine(typeName + "* " + varName + " = nullptr;");
                }
                context.writeEmptyLine();
            }

            // 生成ImObject变量声明
            if (!objectVars.empty())
            {
                context.writeLine(u8"// ImObject变量");
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
                        context.writeLine(typeName + "* " + varName + " = nullptr;");
                    }
                }
                context.writeEmptyLine();
            }

            // 生成基本变量声明
            if (!basicVars.empty())
            {
                context.writeLine(u8"// 基本变量");
                for (const auto& varName : basicVars)
                {
                    ImBasicVariable* var = widgetClass.GetBasicVariable(varName);
                    if (var)
                    {
                        std::string typeStr = BasicTypeToCppType(var->GetBasicType());
                        context.writeLine(typeStr + " " + varName + " = " +
                            GetBasicVariableTypeDefault(var->GetBasicType()) + ";");
                    }
                }
            }

            context.writeLine("//===Auto Gen End=== (Member Variables)");
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
            context.writeLine(u8"// 自动生成的自定义控件类 - " + context.className);
            context.writeLine(u8"// 注意：此文件为自动生成，请勿手动编辑");
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

        // 更新已存在的头文件
        static bool UpdateExistingHeaderFile(const ImUserWidgetClass& widgetClass,
            const std::string& className,
            const std::string& filePath)
        {
            // 生成成员变量代码片段，缩进级别设为0，让标记区域自行处理缩进
            std::ostringstream memberStream;
            ClassGenContext memberContext{ memberStream, 0, className, widgetClass.GetNamespace() };
            GenerateHeaderMembers(widgetClass, memberContext);

            std::string memberVarsContent = memberStream.str();

            // 替换文件中的标记区域
            return ReplaceMarkedRegionInFile(filePath, "Member Variables", memberVarsContent);
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

            context.writeLine(u8"// 释放控件");
            for (const auto& varName : widgetVars)
            {
                context.writeLine("if (" + varName + ") delete " + varName + ";");
            }

            context.writeEmptyLine();
            context.writeLine(u8"// 释放对象");
            for (const auto& varName : objectVars)
            {
                context.writeLine("if (" + varName + ") delete " + varName + ";");
            }
        }

        // 生成InitializeVariables函数代码（使用ROP系统）
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

            // ================ 修改：基本变量直接获取值，不使用ROP ================
            if (!basicVars.empty())
            {
                context.writeLine(u8"// 初始化基本变量（直接获取值）");
                GenerateBasicVariableInitCode(basicVars, widgetClass, context);
            }

            // 生成ImObject变量初始化（使用ROP系统）
            if (!objectVars.empty())
            {
                context.writeLine(u8"// 初始化ImObject变量（使用ROP系统）");
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

                    // 创建对象
                    context.writeLine(varName + " = new " + typeName + "();");

                    // 使用ROP系统设置属性
                    context.writeLine(u8"// 设置 " + varName + u8" 的属性");
                    GenerateROPObjectProperties(obj, varName, context);
                }
                context.writeEmptyLine();
            }

            // 生成控件变量初始化（包括子控件，使用ROP系统）
            if (!allWidgets.empty())
            {
                context.writeLine(u8"// 初始化控件变量（使用ROP系统）");

                // 先创建所有控件
                for (const auto& [varName, widget] : allWidgets)
                {
                    std::string typeName = widget->GetRegisterTypeName();
                    size_t pos = typeName.find_last_of("::");
                    if (pos != std::string::npos)
                    {
                        typeName = typeName.substr(pos + 1);
                    }

                    std::string widgetName = widget->GetWidgetName();
                    if (widgetName.empty())
                    {
                        widgetName = varName;
                    }

                    context.writeLine(varName + " = new " + typeName + "(\"" + widgetName + "\");");
                }
                context.writeEmptyLine();

                // 然后设置控件属性（使用ROP系统）
                for (const auto& [varName, widget] : allWidgets)
                {
                    context.writeLine(u8"// 设置 " + varName + u8" 的属性");
                    GenerateROPObjectProperties(widget, varName, context);
                }
                context.writeEmptyLine();

                // 最后建立父子关系
                context.writeLine(u8"// 建立控件树父子关系");
                for (const auto& [varName, widget] : allWidgets)
                {
                    try
                    {
                        int childCount = widget->GetChildNum();
                        if (childCount <= 0) continue;

                        for (int i = 0; i < childCount; i++)
                        {
                            ImWidget* childWidget = widget->GetChildAt(i);
                            if (!childWidget) continue;

                            std::string childVarName = FindWidgetVariableName(childWidget, allWidgets);
                            if (childVarName.empty()) continue;

                            context.writeLine(varName + "->AddChild(" + childVarName + ");");
                        }
                    }
                    catch (...)
                    {
                        // 控件不支持子控件，跳过
                    }
                }
            }

            context.writeLine("//===Auto Gen End=== (InitializeVariables)");
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
            context.writeLine(u8"// 自动生成的自定义控件源文件 - " + context.className);
            context.writeLine(u8"// 注意：此文件为自动生成，请勿手动编辑");
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
                context.writeLine("SetRootWidget(" + defaultRoot + ");");
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

        // 更新已存在的源文件
        static bool UpdateExistingSourceFile(const ImUserWidgetClass& widgetClass,
            const std::string& className,
            const std::string& filePath)
        {
            // 生成InitializeVariables函数体内的代码片段，缩进级别设为0
            std::stringstream initStream;
            ClassGenContext initContext{ initStream, 0, className, widgetClass.GetNamespace() };
            GenerateInitializeVariablesCode(widgetClass, initContext);

            std::string newInitContent = initStream.str();

            // 替换文件中的标记区域
            return ReplaceMarkedRegionInFile(filePath, "InitializeVariables", newInitContent);
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