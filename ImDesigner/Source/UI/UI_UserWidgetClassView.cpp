#include "UI/UI_UserWidgetClassView.h"
#include "ImWidget/ImBasicWidgetList.h"
#include "UI/IconManager.h"
#include "ImGlobal.h"

inline UI_UserWidgetClassView::UI_UserWidgetClassView(const std::string& name, ImGuiWidget::ImUserWidgetClass* target)
	:ImUserWidget(name),
	TargetClass(target)
{
	RootVBox = new ImGuiWidget::ImVerticalBox("UserWidgetClassView_RootVBox");
	InitView();
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
		if (var->Vtype == ImGuiWidget::ImUserWidgetClass::variableType::widget)
		{
			UI_WidgetTreeView* NewWidgetTreeView = new UI_WidgetTreeView(var->varName + "_WidgetTreeView");
			NewWidgetTreeView->SetTargetWidget(var->var.v_widget);
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
	//±ÍÃ‚”“º¸≤Àµ•
}
