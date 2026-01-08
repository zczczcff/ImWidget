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
    ImGuiWidget::ImPanelWidget* Target;
    int index;
    //ImGuiWidget::ImWidget* Child;
public:
    ChildEditCommand(ImGuiWidget::ImPanelWidget* Target, int index):
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
	ChildAddCommand(ImGuiWidget::ImPanelWidget* Target, ImGuiWidget::ImWidget* Child, int index) :
		ChildEditCommand(Target, index),
		Child(Child)
	{

    }

    virtual void Execute() override;

    virtual void Undo() override;
};

class ChildRemoveCommand :public ChildEditCommand
{
private:
    ImGuiWidget::ImWidget* Child;
public:
    ChildRemoveCommand(ImGuiWidget::ImPanelWidget* Target, ImGuiWidget::ImWidget* Child, int index) :
        ChildEditCommand(Target, index),
        Child(Child)
    {

    }

    virtual void Execute() override;

    virtual void Undo() override;
};