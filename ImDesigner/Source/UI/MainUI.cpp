#include "UI/MainUI.h"
#include "ImBasicWidgetList.h"
#include "UI/Widget_ExampleWidgetButton.h"
#include "UI/Widget_PageTag.h"
#include "ImComboWidget/ImPageManager.h"

void MainUI::Init2()
{
	ImButton_Project->SetFocusable(false);
	ImButton_Save->SetFocusable(false);
	ImButton_Generate->SetFocusable(false);

	Widget_PageTag* testtag = new Widget_PageTag("testpagetag");
	ImHorizontalBox_PageTag->AddChildToHorizontalBox(testtag)->SetIfAutoSize(false);

	Widget_ExampleWidgetButton* Example_Button = new Widget_ExampleWidgetButton("Example_Button", u8"°´Å¥", "ImButton");
	ImVerticalBox_WidgetList->AddChildToVerticalBox(Example_Button)->SetIfAutoSize(false);


	ImPageManager_LeftPart = new ImGuiWidget::ImPageManager("ImPageManager_LeftPart");
	ImBorder_LeftTab->SetContent(ImPageManager_LeftPart, false);

	ImGuiWidget::ImButton* testbutton = new ImGuiWidget::ImButton("testbutton");
	ImPageManager_LeftPart->AddPage("test", testbutton);
}
