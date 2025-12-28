#include "ImEvent/ImDragObject.h"
#include "ImWidget/ImTextBlock.h"

class DragObject_ExampleWidget :public ImGuiWidget::ImDragObject
{
public:
	std::string WidgetTypeName;
	std::string DisplayText;
	DragObject_ExampleWidget(const std::string& WidgetTypeName, const std::string& DisplayText) :
		WidgetTypeName(WidgetTypeName),
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