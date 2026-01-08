#include "Model/Command/Command_ChildEdit.h"
#include "ImWidget/ImPanelWidget.h"
#include "Tools/JLog.h"

bool ChildAddCommand::Execute()
{
	if (Target->InsertChildAt(index, Child))
	{
		return true;
	}
	else return false;
}

bool ChildAddCommand::Undo()
{
	if (Target->RemoveChildAt(index, false))
	{
		return true;
	}
	else return false;
}

bool ChildRemoveCommand::Execute()
{
	if (Target->RemoveChildAt(index, false))
	{
		AddLogLineEx(u8"移除控件：[", Target->GetRegisterTypeName(), "] ",
			Target->GetWidgetName(), u8" -> 移除子控件 -> [",
			Child->GetRegisterTypeName(), "] ", Child->GetWidgetName());
		return true;
	}
	else return false;
}

bool ChildRemoveCommand::Undo()
{
	if (Target->InsertChildAt(index, Child))
	{
		AddLogLineEx(u8"撤销移除：[", Target->GetRegisterTypeName(), "] ",
			Target->GetWidgetName(), u8" <- 恢复子控件 <- [",
			Child->GetRegisterTypeName(), "] ", Child->GetWidgetName());
	}
	else return false;
}
