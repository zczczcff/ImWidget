#pragma once
#include<EditorKit/CommandBase.h>
#include "ImDesignerCommandData.h"
#include "EditorEventInterface.h"

namespace ImGuiWidget
{
    class ImUserWidgetClass;
}

// 所有ImUserWidgetClass编辑命令的基类
class ImUserWidgetClassCommandBase : public CommandBase<CommandDataType> , public EditorEventInterface
{
protected:
    ImGuiWidget::ImUserWidgetClass* m_TargetClass;
    class Model_ImUserWidgetClassEditor* m_Model;
public:
    ImUserWidgetClassCommandBase(ImGuiWidget::ImUserWidgetClass* target,
        Model_ImUserWidgetClassEditor* Model,
        const CommandDataType& data = CommandDataType())
        : CommandBase<CommandDataType>(data), m_TargetClass(target),m_Model(Model)
    {
    }

    virtual ~ImUserWidgetClassCommandBase() = default;

    // 获取目标类的指针
    ImGuiWidget::ImUserWidgetClass* GetTargetClass() const { return m_TargetClass; }

    virtual std::string GetDescription() const override = 0;
};