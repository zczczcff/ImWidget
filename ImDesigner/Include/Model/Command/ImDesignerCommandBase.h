#pragma once
#include<EditorKit/CommandBase.h>
#include "ImDesignerCommandData.h"

namespace ImGuiWidget
{
    class ImUserWidgetClass;
}

// 所有ImUserWidgetClass编辑命令的基类
class ImUserWidgetClassCommandBase : public CommandBase<CommandDataType>
{
protected:
    ImGuiWidget::ImUserWidgetClass* m_TargetClass;

public:
    ImUserWidgetClassCommandBase(ImGuiWidget::ImUserWidgetClass* target,
        const CommandDataType& data = CommandDataType())
        : CommandBase<CommandDataType>(data), m_TargetClass(target)
    {
    }

    virtual ~ImUserWidgetClassCommandBase() = default;

    // 获取目标类的指针
    ImGuiWidget::ImUserWidgetClass* GetTargetClass() const { return m_TargetClass; }

    virtual std::string GetDescription() const override = 0;
};