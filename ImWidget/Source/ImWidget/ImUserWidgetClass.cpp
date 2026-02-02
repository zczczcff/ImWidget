#include "ImWidget/ImUserWidgetClass.h"
#include "ImWidget/ImWidgetFactory.h"
#include "ImWidget/ImObjectFactory.h"
#include "ImWidget/ImUserWidgetClassCodeGenerator.h"
#include "ImWidget/ImUserWidgetSerializer.h"

bool  ImGuiWidget::ImUserWidgetClass::InitFormJson(const nlohmann::json& FromJson)
{
    return ImUserWidgetClassSerializer::DeserializeUserWidgetClass(*this, FromJson);
}

bool ImGuiWidget::ImUserWidgetClass::InitFromFile(const std::string& FilePath)
{
    std::ifstream file(FilePath);
    if (!file.is_open()) return false;

    nlohmann::ordered_json j = nlohmann::ordered_json::parse(file);

    return InitFormJson(j);
}

ImGuiWidget::ImWidget* ImGuiWidget::ImUserWidgetClass::InsertChildWidget(const std::string& widgetVarName, const std::string& parentWidgetPath, const nlohmann::json& WidgetJson, int index)
{
    if (ImGuiWidget::ImWidget* NewWidget = ImUserWidgetClassSerializer::CreateWidgetFromJson(WidgetJson))
    {
        if (ImGuiWidget::ImWidget* root = GetWidgetVariable(widgetVarName))
        {
            if (ImGuiWidget::ImWidget* Parent = FindWidgetByPath(root, parentWidgetPath))
            {
                if (InsertChildWidget(widgetVarName, Parent, NewWidget, index))
                {
                    return NewWidget;
                }
            }
        }

        delete NewWidget;
        return nullptr;
    }
    else
    {
        return nullptr;
    }
}

bool ImGuiWidget::ImUserWidgetClass::ExportToCppFiles(const std::string& className, const std::string& headerOutputPath, const std::string& sourceOutputPath) const
{
    return ImUserWidgetClassCodeGenerator::ExportUserWidgetClassToFiles(
        *this, className, headerOutputPath, sourceOutputPath);
}

bool ImGuiWidget::ImUserWidgetClass::ExportToJsonFile(const std::string& jsonFileOutputPath)
{
    nlohmann::json j = ToJson();
    if (j.empty()) return false;

    try
    {
        std::ofstream file(jsonFileOutputPath);
        if (file.is_open())
        {
            file << j.dump(4); // 使用4空格缩进
            return true;
        }
    }
    catch (...)
    {
        // 异常处理
    }
    return false;
}

nlohmann::json ImGuiWidget::ImUserWidgetClass::ToJson()
{
    return ImUserWidgetClassSerializer::SerializeUserWidgetClass(*this);
}

// 导出为单头文件
