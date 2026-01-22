#include "UI/UI_UserWidgetClassView.h"
#include "ImWidget/ImBasicWidgetList.h"
#include "UI/IconManager.h"
#include "ImGlobal.h"
#include "UI/DragObject_ExampleWidget.h"

inline UI_UserWidgetClassView::UI_UserWidgetClassView(const std::string& name, ImGuiWidget::ImUserWidgetClass* target)
	:ImUserWidget(name),
	TargetClass(target)
{
	RootVBox = new ImGuiWidget::ImVerticalBox("UserWidgetClassView_RootVBox");
	InitView();
	SetAllowDragOn(true);
}

void UI_UserWidgetClassView::InitView()
{
	TitleButton = new ImGuiWidget::ImButton(TargetClass->GetClassName() + "_TitleButton");
	ImGuiWidget::ImHorizontalBox* Hbox = new ImGuiWidget::ImHorizontalBox(TargetClass->GetClassName() + "_TitleHbox");
	ImGuiWidget::ImTextBlock* text = new ImGuiWidget::ImTextBlock(TargetClass->GetClassName() + "_Titletext");
	ImGuiWidget::ImImage* icon = new ImGuiWidget::ImImage(TargetClass->GetClassName() + "_Titleicon", IconManager::GetInstance()->GetIcon(ImDesignerIcon::UIFile), 24, 24);
	text->SetHorizontalAlignment(ImGuiWidget::ImTextBlock::TextAlignment_Horizontal::Center);
	Hbox->bHaveBorder = false;
	Hbox->AddChildToHorizontalBox(icon)->SetIfAutoSize(false);
	Hbox->AddChildToHorizontalBox(text)->SetIfAutoSize(false);
	TitleButton->SetContent(Hbox);
	RootVBox->AddChildToVerticalBox(TitleButton)->SetIfAutoSize(false);
	TitleButton->OnRightClicked.Add([this]() { On_TitleButtonRightClicked(); });
	for (size_t i = 0; i < TargetClass->GetVariableCount(); i++)
	{
		auto var = TargetClass->GetVariableAt(i);
		AddVarView(var);
	}
}

void UI_UserWidgetClassView::AddVarView(ImGuiWidget::ImUserWidgetClass::variable* var)
{
	if (var->Vtype == ImGuiWidget::ImUserWidgetClass::variableType::widget)
	{
		UI_WidgetTreeView* NewWidgetTreeView = new UI_WidgetTreeView(var->varName + "_WidgetTreeView", var->varName);
		NewWidgetTreeView->SetTargetWidget(var->var.v_widget);

		//NewWidgetTreeView->OnRequestWidgetDeleted.Add
		//([this, varName = var->varName](ImGuiWidget::ImWidget* WidgetToDelete)
		//{
		//	OnRequestWidgetDeleted.Broadcast(WidgetToDelete, varName);
		//});
		//NewWidgetTreeView->OnWidgetSelectedButtonClicked.Add
		//([this, varName = var->varName](ImGuiWidget::ImWidget* WidgetSelected)
		//{
		//	OnWidgetSelectedButtonClicked.Broadcast(WidgetSelected, varName);
		//});
		//NewWidgetTreeView->OnRequestInsertWidget.Add
		//([this, varName = var->varName](ImGuiWidget::ImWidget* PanelWidget, int index, const std::string& InsertWidgetRegisterName)
		//{
		//	OnRequestInsertWidget.Broadcast(PanelWidget, index, InsertWidgetRegisterName, varName);
		//});


		AllVarViews.insert(std::make_pair(var->varName, viewinfor{ var->varName,NewWidgetTreeView }));
		RootVBox->AddChildToVerticalBox(NewWidgetTreeView)->SetIfAutoSize(false);
	}
	else if (var->Vtype == ImGuiWidget::ImUserWidgetClass::variableType::ImObject)
	{
		ImGuiWidget::ImButton* newStructButton = BuildStructViewButton(var->varName);
		AllVarViews.insert(std::make_pair(var->varName, viewinfor{ var->varName,newStructButton }));
		RootVBox->AddChildToVerticalBox(newStructButton)->SetIfAutoSize(false);
	}
}

bool UI_UserWidgetClassView::RemoveVarView(const std::string& varName)
{
	auto it = AllVarViews.find(varName);
	if (it != AllVarViews.end())
	{
		RootVBox->RemoveChild(it->second.varView);
		delete it->second.varView;
		AllVarViews.erase(it);
		return true;
	}
	return false;
}

void UI_UserWidgetClassView::OnDrop(ImGuiWidget::ImDragEvent& e) 
{
	if (auto dragedWidget = std::dynamic_pointer_cast<DragObject_ExampleWidget>(e.GetDragObject()))
	{
		OnRequestAddWidgetTree.Broadcast(dragedWidget->WidgetRegisterTypeName);
	}
}

ImGuiWidget::ImButton* UI_UserWidgetClassView::BuildStructViewButton(const std::string& varName)
{
	ImGuiWidget::ImButton* newStructButton = new ImGuiWidget::ImButton(varName + "_StructButton");
	ImGuiWidget::ImHorizontalBox* Hbox = new ImGuiWidget::ImHorizontalBox(varName + "_Hbox");
	ImGuiWidget::ImTextBlock* text = new ImGuiWidget::ImTextBlock(varName + "_text");
	ImGuiWidget::ImImage* icon = new ImGuiWidget::ImImage(varName + "_icon", IconManager::GetInstance()->GetIcon(ImDesignerIcon::Dialog), 24, 24);
	text->SetHorizontalAlignment(ImGuiWidget::ImTextBlock::TextAlignment_Horizontal::Left);
	Hbox->bHaveBorder = false;
	Hbox->AddChildToHorizontalBox(icon)->SetIfAutoSize(false);
	Hbox->AddChildToHorizontalBox(text)->SetIfAutoSize(false);
	newStructButton->SetContent(newStructButton);
	return newStructButton;
}
void UI_UserWidgetClassView::InitTitleMenu()
{
	auto windowmanager = ImGuiWidget::GetGlobalApp()->GetWindowManager();

	TitleMenuWidget = new ImGuiWidget::ImVerticalBox("TitleMenuWidget_VerticalBox");



}
ImGuiWidget::ImButton* UI_UserWidgetClassView::CreateTitleMenuButton(const std::string& Text)
{
	ImGuiWidget::ImButton* newStructButton = new ImGuiWidget::ImButton(Text + "_StructButton");
	ImGuiWidget::ImTextBlock* text = new ImGuiWidget::ImTextBlock(Text + "_text");
	text->SetHorizontalAlignment(ImGuiWidget::ImTextBlock::TextAlignment_Horizontal::Left);
	newStructButton->SetContent(text);
	return newStructButton;
}
void UI_UserWidgetClassView::On_TitleButtonRightClicked()
{
	//标题右键菜单
}

void UI_UserWidgetClassView::UpdateVarList()
{
	for (size_t i = 0; i < TargetClass->GetVariableCount(); i++)
	{
		if (AllVarViews.find(TargetClass->GetVariableAt(i)->varName) == AllVarViews.end())//新变量
		{
			AddVarView(TargetClass->GetVariableAt(i));
		}
	}

	for (auto SingleVarView : AllVarViews)
	{
		if (!TargetClass->FindVarByName(SingleVarView.first))
		{
			RemoveVarView(SingleVarView.first);
		}
	}

}
