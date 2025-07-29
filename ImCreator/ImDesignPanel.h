#pragma once
#include "ImWidget/ImUserWidget.h"
#include "ImWidget/ImCanvasPanel.h"
#include "ImWidget/ImTextBlock.h"
#include "ImWidget/ImResizableBox.h"
#include "ImWidget/ImDesignPanel.h"


#include "ExampleWidgetInfor.h"

class DesiginPanel : public ImGuiWidget::ImUserWidget
{
private:
	ImGuiWidget::ImDesignPanel* m_MainPanel;
protected:
	virtual void OnDragOn(ImGuiWidget::ImDragHandle* OriginalHandle) override
	{
		ImVec2 Pos = ImGui::GetMousePos();
		ExampleWidgetDragHandle* Target_ExampleWidgetDragHandle = dynamic_cast<ExampleWidgetDragHandle*>(OriginalHandle);

		if (!Target_ExampleWidgetDragHandle) return;

		switch (Target_ExampleWidgetDragHandle->widgettype)
		{
		case WidgetType::ImButton:
		{
			Handle_DragOnButton(Pos);
			break;
		}

		default:
			break;
		}


	}

	void Handle_DragOnButton(ImVec2 MousePos)
	{
		static int count = 0;
		
		ImGuiWidget::ImButton* button = new ImGuiWidget::ImButton("Button_" + std::to_string(count));
		m_MainPanel->AddChildToCanvasPanel(button)->SetSlotPosAndSize(MousePos - Position, button->GetMinSize());
		count++;
	}
public:
	DesiginPanel(const std::string& WidgetName) :
		ImGuiWidget::ImUserWidget(WidgetName),
		m_MainPanel(new ImGuiWidget::ImDesignPanel(WidgetName+"_MainPanel"))
	{
		SetAllowDragOn(true);
		SetRootWidget(m_MainPanel);
	}
};