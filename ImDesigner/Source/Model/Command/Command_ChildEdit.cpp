#include "Model/Command/Command_ChildEdit.h"
#include "ImWidget/ImWidget.h"
#include "ImWidget/ImSlot.h"
#include "Tools/JLog.h"

bool ChildAddCommand::Execute()
{
	if (ImGuiWidget::ImSlot* childslot = Target->InsertChildAt(index, Child))
	{
		childslot->Deserialize(SlotData);
		AddLogLineEx(u8"添加控件：[", Target->GetRegisterTypeName(), "] ",
			Target->GetWidgetName(), u8" <- 添加子控件 <- [",
			Child->GetRegisterTypeName(), "] ", Child->GetWidgetName());
		return true;
	}
	else return false;
}

bool ChildAddCommand::Undo()
{
	if (ImGuiWidget::ImSlot* childslot= Target->GetSlotAt(index))
	{
		SlotData = childslot->Serialize();
		Target->RemoveChildAt(index, false);
		AddLogLineEx(u8"撤销添加：[", Target->GetRegisterTypeName(), "] ",
			Target->GetWidgetName(), u8" -> 移除子控件 -> [",
			Child->GetRegisterTypeName(), "] ", Child->GetWidgetName());
		return true;
	}
	else
	{
		SlotData.clear();
		return false;
	}
}

bool ChildRemoveCommand::Execute()
{
	if (ImGuiWidget::ImSlot* childslot = Target->GetSlotAt(index))
	{
		SlotData = childslot->Serialize();
		Target->RemoveChildAt(index, false);
		AddLogLineEx(u8"移除控件：[", Target->GetRegisterTypeName(), "] ",
			Target->GetWidgetName(), u8" -> 移除子控件 -> [",
			Child->GetRegisterTypeName(), "] ", Child->GetWidgetName());
		return true;
	}
	else
	{
		SlotData.clear();
		return false;
	}
}

bool ChildRemoveCommand::Undo()
{
	if (ImGuiWidget::ImSlot* childslot = Target->InsertChildAt(index, Child))
	{
		childslot->Deserialize(SlotData);
		AddLogLineEx(u8"撤销移除：[", Target->GetRegisterTypeName(), "] ",
			Target->GetWidgetName(), u8" <- 恢复子控件 <- [",
			Child->GetRegisterTypeName(), "] ", Child->GetWidgetName());
	}
	else return false;
}
