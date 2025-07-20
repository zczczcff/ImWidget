#pragma once
#include "ImWidget/ImUserWidget.h"
#include "ImWidget/ImCanvasPanel.h"
#include "ImWidget/ImTextBlock.h"
#include "ImWidget/ImResizableBox.h"
#include "ImWidget/ImDesignPanel.h"
class DesiginPanel : public ImGuiWidget::ImUserWidget
{
private:
	ImGuiWidget::ImDesignPanel* m_MainPanel;
protected:
	virtual void OnDragOn(ImGuiWidget::ImDragHandle* OriginalHandle) override
	{
		ImVec2 Pos = ImGui::GetMousePos();
		ImGuiWidget::ImTextBlock* testblock = new ImGuiWidget::ImTextBlock("testblock");
		//ImGuiWidget::ImResizableBox* testBox = new ImGuiWidget::ImResizableBox("testresizableBox");
		//testBox->SetContent(testblock);
		testblock->SetText("test");
		m_MainPanel->AddChildToCanvasPanel(testblock)->SetSlotPosAndSize(Pos - m_MainPanel->GetPosition(), ImVec2(80, 20));
		//testBox->SetOnResizing(
		//	[this,testBox](ImVec2 Pos,ImVec2 Size) 
		//	{
		//		for (int i = 0; i < m_MainPanel->GetChildNum(); i++)
		//		{
		//			ImGuiWidget::ImCanvasPanelSlot* slottemp = (ImGuiWidget::ImCanvasPanelSlot*)(m_MainPanel->GetSlot(i));
		//			if (slottemp->GetContent() == testBox)
		//			{
		//				slottemp->RelativePosition = Pos - m_MainPanel->GetPosition();
		//				slottemp->SlotSize = Size;
		//				break;
		//			}
		//		}
		//	}
		//);
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