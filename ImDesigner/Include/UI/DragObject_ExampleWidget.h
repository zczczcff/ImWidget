#include "ImEvent/ImDragObject.h"
#include "ImWidget/ImTextBlock.h"

class DragObject_ExampleWidget :public ImGuiWidget::ImDragObject
{
public:
	std::string WidgetRegisterTypeName;
	std::string DisplayText;
	DragObject_ExampleWidget(const std::string& WidgetRegisterTypeName, const std::string& DisplayText) :
		WidgetRegisterTypeName(WidgetRegisterTypeName),
		DisplayText(DisplayText)
	{
		ImGuiWidget::ImTextBlock* previewtextblock = new ImGuiWidget::ImTextBlock("DragObject_PreviewTextBlock");
		previewtextblock->SetText(DisplayText);
		Preview = previewtextblock;		
	}
	virtual ~DragObject_ExampleWidget()
	{
		delete Preview;
	}
};