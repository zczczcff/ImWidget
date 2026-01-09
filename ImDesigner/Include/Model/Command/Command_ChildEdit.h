#pragma once
#include "CommandBase.h"

namespace ImGuiWidget
{
    class ImWidget;
    class ImPanelWidget;
}

class ChildEditCommand :public EditCommand
{
protected:
    ImGuiWidget::ImWidget* Target;
    std::vector<uint8_t> SlotData;
    int index;
    //ImGuiWidget::ImWidget* Child;
public:
    ChildEditCommand(ImGuiWidget::ImWidget* Target, int index):
        Target(Target),
        index(index)
    {
        m_type = EditCommandType::ChildChange;
    }
    virtual std::string GetDescription() const override
    {
        return m_Description;
    }
    virtual bool CanMergeWith(const EditCommand* other) const override
    {
        return false;
    }
};

class ChildAddCommand :public ChildEditCommand
{
private:
	ImGuiWidget::ImWidget* Child;
public:
	ChildAddCommand(ImGuiWidget::ImWidget* Target, ImGuiWidget::ImWidget* Child, int index) :
		ChildEditCommand(Target, index),
		Child(Child)
	{

    }

    virtual bool Execute() override;

    virtual bool Undo() override;
};

class ChildRemoveCommand :public ChildEditCommand
{
private:
    ImGuiWidget::ImWidget* Child;
public:
    ChildRemoveCommand(ImGuiWidget::ImWidget* Target, ImGuiWidget::ImWidget* Child, int index) :
        ChildEditCommand(Target, index),
        Child(Child)
    {

    }

    virtual bool Execute() override;

    virtual bool Undo() override;
};