#include "Model/Command/Command_ChildEdit.h"
#include "ImWidget/ImPanelWidget.h"
#include "Tools/JLog.h"

void ChildAddCommand::Execute()
{
	Target->InsertChildAt(index, Child);
}

void ChildAddCommand::Undo()
{
	Target->RemoveChildAt(index, false);
}

void ChildRemoveCommand::Execute()
{
	Target->RemoveChildAt(index, false);
	AddLogLineEx(u8"移除控件：[", Target->GetRegisterTypeName(), "] ",
		Target->GetWidgetName(), u8" -> 移除子控件 -> [",
		Child->GetRegisterTypeName(), "] ",Child->GetWidgetName());
}

void ChildRemoveCommand::Undo()
{
	Target->InsertChildAt(index, Child);
	AddLogLineEx(u8"撤销移除：[", Target->GetRegisterTypeName(), "] ",
		Target->GetWidgetName(), u8" <- 恢复子控件 <- [",
		Child->GetRegisterTypeName(), "] ", Child->GetWidgetName());
}
