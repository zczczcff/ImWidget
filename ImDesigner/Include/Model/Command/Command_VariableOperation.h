// Command_VariableOperation.h - 基于新ImUserWidgetClass的统一变量操作命令
#pragma once

#include "ImDesignerCommandBase.h"
#include "ImWidget/ImUserWidgetClass.h"
#include "ImUserWidgetSerializer.h"
#include <nlohmann/json.hpp>

// VariableOperation子类型
enum class VariableOperationSubType
{
    CreateNewVariable = 0,    // 创建新变量（统一接口）
    RemoveVariable,           // 删除变量（统一接口）
    PasteVariable,            // 粘贴变量（通过json对象）
    PasteObjectVariable,      // 粘贴ImObject的Json对象为ImObject变量
    PasteWidgetVariable       // 粘贴ImWidget的Json对象为控件树变量
};

class VariableOperationCommandBase : public ImUserWidgetClassCommandBase
{
protected:
    std::string m_VariableName;  // 执行后记录的变量名
    ImGuiWidget::WidgetClassVariableType m_VariableType;  // 变量类型

public:
    VariableOperationCommandBase(ImGuiWidget::ImUserWidgetClass* target,
        VariableOperationSubType subType)
        : ImUserWidgetClassCommandBase(target,
            CommandDataType(CommandCategory::VariableOperation,
                static_cast<int>(subType)))
    {
    }

    virtual ~VariableOperationCommandBase() = default;

    // 变量操作通常不可合并
    virtual bool CanMergeWith(const CommandBase<CommandDataType>* other) const override
    {
        return false;
    }

    // 获取执行后生成的变量名
    std::string GetVariableName() const { return m_VariableName; }

    // 获取变量类型
    ImGuiWidget::WidgetClassVariableType GetVariableType() const { return m_VariableType; }
};

// ============================================================================
// 创建新变量命令
// ============================================================================
class CreateNewVariableCommand : public VariableOperationCommandBase
{
private:
    ImGuiWidget::WidgetClassVariableType m_Type;
    std::string m_SpecificType;  // 具体类型名称

public:
    CreateNewVariableCommand(ImGuiWidget::ImUserWidgetClass* target,
        ImGuiWidget::WidgetClassVariableType type,
        const std::string& specificType)
        : VariableOperationCommandBase(target,
            VariableOperationSubType::CreateNewVariable)
        , m_Type(type)
        , m_SpecificType(specificType)
    {
    }

    virtual bool Execute() override
    {
        if (!m_TargetClass)
            return false;

        // 执行添加操作，变量名由ImUserWidgetClass生成
        bool success = m_TargetClass->AddVariable(m_Type, m_SpecificType, m_VariableName);

        if (success)
        {
            m_VariableType = m_Type;
        }

        return success;
    }

    virtual bool Undo() override
    {
        if (!m_TargetClass)
            return false;

        return m_TargetClass->RemoveVariable(m_VariableName);
    }

    virtual std::string GetDescription() const override
    {
        std::string typeStr;
        switch (m_Type)
        {
        case ImGuiWidget::WidgetClassVariableType::Widget:
            typeStr = "控件树变量";
            break;
        case ImGuiWidget::WidgetClassVariableType::Object:
            typeStr = "ImObject变量";
            break;
        case ImGuiWidget::WidgetClassVariableType::Basic:
            typeStr = "基本变量";
            break;
        }

        return "创建" + typeStr + ": " + m_VariableName + " (类型: " + m_SpecificType + ")";
    }
};

// ============================================================================
// 删除变量命令
// ============================================================================
class RemoveVariableCommand : public VariableOperationCommandBase
{
private:
    ImGuiWidget::WidgetClassVariableType m_Type;
    std::string m_SpecificType;      // 具体类型名称
    nlohmann::json m_SerializedData; // 序列化后的变量数据（用于恢复）

public:
    RemoveVariableCommand(ImGuiWidget::ImUserWidgetClass* target,
        const std::string& variableName)
        : VariableOperationCommandBase(target,
            VariableOperationSubType::RemoveVariable)
    {
        m_VariableName = variableName;
    }

    virtual bool Execute() override
    {
        if (!m_TargetClass)
            return false;

        // 获取变量
        ImGuiWidget::ImWidgetClassVariable* var = m_TargetClass->GetVariable(m_VariableName);
        if (!var)
            return false;

        m_Type = var->GetType();
        m_SpecificType = var->GetTypeString();

        // 序列化变量数据 - 使用公共函数
        m_SerializedData = ImGuiWidget::ImUserWidgetClassSerializer::SerializeVariable(var);

        // 执行删除
        return m_TargetClass->RemoveVariable(m_VariableName);
    }

    virtual bool Undo() override
    {
        if (!m_TargetClass)
            return false;

        // 从序列化数据重建变量 - 使用公共函数
        ImGuiWidget::ImWidgetClassVariable* var =
            ImGuiWidget::ImUserWidgetClassSerializer::CreateVariableFromJson(m_SerializedData);

        if (!var)
            return false;

        // 根据变量类型添加到目标类
        bool success = false;
        switch (var->GetType())
        {
        case ImGuiWidget::WidgetClassVariableType::Widget:
        {
            auto widgetVar = var->As<ImGuiWidget::ImWidgetClassVariable_Widget>();
            if (widgetVar)
            {
                // 转移控件所有权
                ImGuiWidget::ImWidget* widget = widgetVar->GetWidget();
                widgetVar->SetWidget(nullptr, false);  // 不删除控件
                success = m_TargetClass->SetWidgetVariableDirect(m_VariableName, widget);
            }
            break;
        }
        case ImGuiWidget::WidgetClassVariableType::Object:
        {
            auto objectVar = var->As<ImGuiWidget::ImWidgetClassVariable_Object>();
            if (objectVar)
            {
                // 转移对象所有权
                ImGuiWidget::ImObject* obj = objectVar->GetObject();
                objectVar->SetObject(nullptr, false);  // 不删除对象
                success = m_TargetClass->SetObjectVariableDirect(m_VariableName, obj);
            }
            break;
        }
        case ImGuiWidget::WidgetClassVariableType::Basic:
        {
            auto basicVar = var->As<ImGuiWidget::ImWidgetClassVariable_Basic>();
            if (basicVar)
            {
                // 转移基本变量所有权
                success = m_TargetClass->SetBasicVariableDirect(m_VariableName, basicVar);
                // 注意：SetBasicVariableDirect会接管所有权，所以var指针不应再被删除
                var = nullptr;  // 防止后面的delete
            }
            break;
        }
        }

        // 清理临时变量对象
        if (var)
        {
            delete var;
        }

        return success;
    }

    virtual std::string GetDescription() const override
    {
        std::string typeStr;
        switch (m_Type)
        {
        case ImGuiWidget::WidgetClassVariableType::Widget:
            typeStr = "控件树变量";
            break;
        case ImGuiWidget::WidgetClassVariableType::Object:
            typeStr = "ImObject变量";
            break;
        case ImGuiWidget::WidgetClassVariableType::Basic:
            typeStr = "基本变量";
            break;
        }

        return "删除" + typeStr + ": " + m_VariableName;
    }
};

// ============================================================================
// 粘贴变量命令（通过ImWidgetClassVariable序列化得到的json对象）
// ============================================================================
class PasteVariableCommand : public VariableOperationCommandBase
{
private:
    nlohmann::json m_SerializedData;      // 序列化后的变量数据
    std::string m_OriginalVariableName;   // 原始变量名（用于生成新名称）
    bool m_KeepOriginalName;              // 是否保持原始名称

public:
    PasteVariableCommand(ImGuiWidget::ImUserWidgetClass* target,
        const nlohmann::json& serializedData,
        bool keepOriginalName = false)
        : VariableOperationCommandBase(target,
            VariableOperationSubType::PasteVariable)
        , m_SerializedData(serializedData)
        , m_KeepOriginalName(keepOriginalName)
    {
        // 从序列化数据中提取原始变量名
        if (m_SerializedData.contains("Name"))
        {
            m_OriginalVariableName = m_SerializedData["Name"].get<std::string>();
        }
    }

    virtual bool Execute() override
    {
        if (!m_TargetClass)
            return false;

        // 从序列化数据创建变量 - 使用公共函数
        ImGuiWidget::ImWidgetClassVariable* var =
            ImGuiWidget::ImUserWidgetClassSerializer::CreateVariableFromJson(m_SerializedData);

        if (!var)
            return false;

        // 确定新变量名
        std::string newName;
        if (m_KeepOriginalName)
        {
            // 尝试使用原始名称，如果冲突则生成新名称
            newName = GenerateUniqueVariableName(m_OriginalVariableName);
        }
        else
        {
            // 基于原始名称生成新名称（添加_Copy后缀）
            std::string baseName = m_OriginalVariableName + "_Copy";
            newName = GenerateUniqueVariableName(baseName);
        }

        // 更新变量名
        var->SetName(newName);

        // 对于控件树变量，还需要更新控件名称
        if (var->GetType() == ImGuiWidget::WidgetClassVariableType::Widget)
        {
            auto widgetVar = var->As<ImGuiWidget::ImWidgetClassVariable_Widget>();
            if (widgetVar && widgetVar->GetWidget())
            {
                widgetVar->GetWidget()->SetWidgetName(newName);
            }
        }

        // 根据变量类型添加到目标类
        bool success = false;
        switch (var->GetType())
        {
        case ImGuiWidget::WidgetClassVariableType::Widget:
        {
            auto widgetVar = var->As<ImGuiWidget::ImWidgetClassVariable_Widget>();
            if (widgetVar)
            {
                // 转移控件所有权
                ImGuiWidget::ImWidget* widget = widgetVar->GetWidget();
                widgetVar->SetWidget(nullptr, false);  // 不删除控件
                success = m_TargetClass->SetWidgetVariableDirect(newName, widget);
                m_VariableName = newName;
                m_VariableType = ImGuiWidget::WidgetClassVariableType::Widget;
            }
            break;
        }
        case ImGuiWidget::WidgetClassVariableType::Object:
        {
            auto objectVar = var->As<ImGuiWidget::ImWidgetClassVariable_Object>();
            if (objectVar)
            {
                // 转移对象所有权
                ImGuiWidget::ImObject* obj = objectVar->GetObject();
                objectVar->SetObject(nullptr, false);  // 不删除对象
                success = m_TargetClass->SetObjectVariableDirect(newName, obj);
                m_VariableName = newName;
                m_VariableType = ImGuiWidget::WidgetClassVariableType::Object;
            }
            break;
        }
        case ImGuiWidget::WidgetClassVariableType::Basic:
        {
            auto basicVar = var->As<ImGuiWidget::ImWidgetClassVariable_Basic>();
            if (basicVar)
            {
                // 转移基本变量所有权
                success = m_TargetClass->SetBasicVariableDirect(newName, basicVar);
                m_VariableName = newName;
                m_VariableType = ImGuiWidget::WidgetClassVariableType::Basic;
                // SetBasicVariableDirect会接管所有权，所以var指针不应再被删除
                var = nullptr;  // 防止后面的delete
            }
            break;
        }
        }

        // 清理临时变量对象
        if (var)
        {
            delete var;
        }

        return success;
    }

    virtual bool Undo() override
    {
        if (!m_TargetClass)
            return false;

        return m_TargetClass->RemoveVariable(m_VariableName);
    }

    virtual std::string GetDescription() const override
    {
        std::string typeStr;
        switch (m_VariableType)
        {
        case ImGuiWidget::WidgetClassVariableType::Widget:
            typeStr = "控件树变量";
            break;
        case ImGuiWidget::WidgetClassVariableType::Object:
            typeStr = "ImObject变量";
            break;
        case ImGuiWidget::WidgetClassVariableType::Basic:
            typeStr = "基本变量";
            break;
        default:
            typeStr = "变量";
            break;
        }

        return "粘贴" + typeStr + ": " + m_VariableName;
    }

private:
    // 生成唯一变量名
    std::string GenerateUniqueVariableName(const std::string& baseName) const
    {
        if (!m_TargetClass)
            return baseName;

        // 检查所有变量名
        auto allVariables = m_TargetClass->GetAllVariableNames();
        std::unordered_set<std::string> existingNames(allVariables.begin(), allVariables.end());

        std::string name = baseName;
        int counter = 1;

        while (existingNames.find(name) != existingNames.end())
        {
            name = baseName + "_" + std::to_string(counter);
            counter++;
        }

        return name;
    }
};

// ============================================================================
// 粘贴ImObject的Json对象为ImObject变量
// ============================================================================
class PasteObjectVariableCommand : public VariableOperationCommandBase
{
private:
    nlohmann::json m_ObjectJson;        // ImObject的Json对象（通过SerializeImObject得到）
    std::string m_SuggestedName;        // 建议的变量名
    bool m_KeepSuggestedName;           // 是否保持建议名称

public:
    PasteObjectVariableCommand(ImGuiWidget::ImUserWidgetClass* target,
        const nlohmann::json& objectJson,
        const std::string& suggestedName = "",
        bool keepSuggestedName = false)
        : VariableOperationCommandBase(target,
            VariableOperationSubType::PasteObjectVariable)
        , m_ObjectJson(objectJson)
        , m_SuggestedName(suggestedName)
        , m_KeepSuggestedName(keepSuggestedName)
    {
        // 验证Json对象格式
        if (!objectJson.contains("Type") || !objectJson.contains("Properties"))
        {
            std::cerr << "Error: Invalid ImObject JSON format" << std::endl;
        }
    }

    virtual bool Execute() override
    {
        if (!m_TargetClass || !m_ObjectJson.contains("Type"))
            return false;

        // 从Json创建ImObject - 使用新的公共函数
        ImGuiWidget::ImObject* obj =
            ImGuiWidget::ImUserWidgetClassSerializer::CreateImObjectFromJson(m_ObjectJson);

        if (!obj)
            return false;

        // 确定变量名
        std::string varName;
        if (m_KeepSuggestedName && !m_SuggestedName.empty())
        {
            varName = GenerateUniqueVariableName(m_SuggestedName);
        }
        else
        {
            // 使用对象类型作为基础名称
            std::string typeName = m_ObjectJson["Type"].get<std::string>();
            std::string baseName = typeName;
            size_t pos = baseName.find_last_of("::");
            if (pos != std::string::npos)
                baseName = baseName.substr(pos + 1);

            baseName += "_Copy";
            varName = GenerateUniqueVariableName(baseName);
        }

        // 添加到目标类
        bool success = m_TargetClass->SetObjectVariableDirect(varName, obj);

        if (success)
        {
            m_VariableName = varName;
            m_VariableType = ImGuiWidget::WidgetClassVariableType::Object;
        }
        else
        {
            delete obj;
        }

        return success;
    }

    virtual bool Undo() override
    {
        if (!m_TargetClass)
            return false;

        return m_TargetClass->RemoveVariable(m_VariableName);
    }

    virtual std::string GetDescription() const override
    {
        std::string typeName = "Unknown";
        if (m_ObjectJson.contains("Type"))
        {
            typeName = m_ObjectJson["Type"].get<std::string>();
        }

        return "粘贴ImObject变量: " + m_VariableName + " (类型: " + typeName + ")";
    }

private:
    // 生成唯一变量名
    std::string GenerateUniqueVariableName(const std::string& baseName) const
    {
        if (!m_TargetClass)
            return baseName;

        // 检查所有变量名
        auto allVariables = m_TargetClass->GetAllVariableNames();
        std::unordered_set<std::string> existingNames(allVariables.begin(), allVariables.end());

        std::string name = baseName;
        int counter = 1;

        while (existingNames.find(name) != existingNames.end())
        {
            name = baseName + "_" + std::to_string(counter);
            counter++;
        }

        return name;
    }
};

// ============================================================================
// 粘贴ImWidget的Json对象为控件树变量
// ============================================================================
class PasteWidgetVariableCommand : public VariableOperationCommandBase
{
private:
    nlohmann::json m_WidgetJson;        // ImWidget的Json对象（通过SerializeImWidget得到）
    std::string m_SuggestedName;        // 建议的变量名
    bool m_KeepSuggestedName;           // 是否保持建议名称

public:
    PasteWidgetVariableCommand(ImGuiWidget::ImUserWidgetClass* target,
        const nlohmann::json& widgetJson,
        const std::string& suggestedName = "",
        bool keepSuggestedName = false)
        : VariableOperationCommandBase(target,
            VariableOperationSubType::PasteWidgetVariable)
        , m_WidgetJson(widgetJson)
        , m_SuggestedName(suggestedName)
        , m_KeepSuggestedName(keepSuggestedName)
    {
        // 验证Json对象格式
        if (!widgetJson.contains("Type") || !widgetJson.contains("Name"))
        {
            std::cerr << "Error: Invalid ImWidget JSON format" << std::endl;
        }
    }

    virtual bool Execute() override
    {
        if (!m_TargetClass || !m_WidgetJson.contains("Type") || !m_WidgetJson.contains("Name"))
            return false;

        // 从Json创建ImWidget - 使用新的公共函数
        ImGuiWidget::ImWidget* widget =
            ImGuiWidget::ImUserWidgetClassSerializer::CreateImWidgetFromJson(m_WidgetJson);

        if (!widget)
            return false;

        // 确定变量名
        std::string varName;
        if (m_KeepSuggestedName && !m_SuggestedName.empty())
        {
            varName = GenerateUniqueVariableName(m_SuggestedName);
        }
        else
        {
            // 使用控件类型作为基础名称
            std::string typeName = m_WidgetJson["Type"].get<std::string>();
            std::string baseName = typeName;
            size_t pos = baseName.find_last_of("::");
            if (pos != std::string::npos)
                baseName = baseName.substr(pos + 1);

            baseName += "_Copy";
            varName = GenerateUniqueVariableName(baseName);
        }

        // 设置控件名称
        widget->SetWidgetName(varName);

        // 添加到目标类
        bool success = m_TargetClass->SetWidgetVariableDirect(varName, widget);

        if (success)
        {
            m_VariableName = varName;
            m_VariableType = ImGuiWidget::WidgetClassVariableType::Widget;
        }
        else
        {
            delete widget;
        }

        return success;
    }

    virtual bool Undo() override
    {
        if (!m_TargetClass)
            return false;

        return m_TargetClass->RemoveVariable(m_VariableName);
    }

    virtual std::string GetDescription() const override
    {
        std::string typeName = "Unknown";
        if (m_WidgetJson.contains("Type"))
        {
            typeName = m_WidgetJson["Type"].get<std::string>();
        }

        return "粘贴控件树变量: " + m_VariableName + " (类型: " + typeName + ")";
    }

private:
    // 生成唯一变量名
    std::string GenerateUniqueVariableName(const std::string& baseName) const
    {
        if (!m_TargetClass)
            return baseName;

        // 检查所有变量名
        auto allVariables = m_TargetClass->GetAllVariableNames();
        std::unordered_set<std::string> existingNames(allVariables.begin(), allVariables.end());

        std::string name = baseName;
        int counter = 1;

        while (existingNames.find(name) != existingNames.end())
        {
            name = baseName + "_" + std::to_string(counter);
            counter++;
        }

        return name;
    }
};