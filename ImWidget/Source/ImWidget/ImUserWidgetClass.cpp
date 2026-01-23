#include "ImWidget/ImUserWidgetClass.h"
#include "ImWidget/ImWidgetFactory.h"
#include "ImWidget/ImObjectFactory.h"
#include "ImWidget/ImUserWidgetClassCodeGenerator.h"

bool ImGuiWidget::ImUserWidgetClass::ExportToCppFiles(const std::string& className, const std::string& headerOutputPath, const std::string& sourceOutputPath) const
{
    return ImUserWidgetClassCodeGenerator::ExportUserWidgetClassToFiles(
        *this, className, headerOutputPath, sourceOutputPath);
}

// 导出为单头文件

bool ImGuiWidget::ImUserWidgetClass::ExportToSingleHeader(const std::string& className, const std::string& outputPath) const
{
    return ImUserWidgetClassCodeGenerator::ExportUserWidgetClassToSingleHeader(
        *this, className, outputPath);
}
