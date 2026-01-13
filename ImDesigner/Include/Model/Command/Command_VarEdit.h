#pragma once
#include "CommandBase.h"
#include "ImWidget/ImUserWidgetClass.h"
namespace ImGuiWidget
{
    class ImUserWidgetClass;
}
class VarRenameCommand :public EditCommand
{
protected:
    ImGuiWidget::ImUserWidgetClass* Target;
    std::string OldName;
    std::string NewName;
    std::function<void(const std::string&, const std::string&)> Updater;
public:
    VarRenameCommand(ImGuiWidget::ImUserWidgetClass* Target, const std::string& OldName, const std::string& NewName, std::function<void(const std::string&, const std::string&)> Updater)
        :Target(Target),
        OldName(OldName),
        NewName(NewName),
        Updater(Updater)
    {
        m_type = EditCommandType::RenameVar;
    }

    virtual bool Execute() override
    {
        if (Target->RenameVar(OldName, NewName))
        {
            Updater(OldName, NewName);
            return true;
        }
        else
        {
            return false;
        }
    }

    virtual bool Undo() override
    {
        if (Target->RenameVar(NewName, OldName))
        {
            Updater(NewName, OldName);
            return true;
        }
        else
        {
            return false;
        }
    }
};