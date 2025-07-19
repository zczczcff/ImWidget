#pragma once
#include "ImWidget/ImUserWidget.h"
#include "ImWidget/ImCanvasPanel.h"
#include "ImWidget/ImTextBlock.h"
class DesiginPanel : public ImGuiWidget::ImUserWidget
{
private:
	ImGuiWidget::ImCanvasPanel* m_MainPanel;
protected:
	virtual void OnDragOn(ImGuiWidget::ImDragHandle* OriginalHandle) override
	{
		ImVec2 Pos = ImGui::GetMousePos();
		ImGuiWidget::ImTextBlock* testblock = new ImGuiWidget::ImTextBlock("testblock");
		testblock->SetText("test");
		m_MainPanel->AddChildToCanvasPanel(testblock)->SetSlotPosAndSize(Pos - m_MainPanel->GetPosition(), ImVec2(80, 20));
	}
public:
	DesiginPanel(const std::string& WidgetName) :
		ImGuiWidget::ImUserWidget(WidgetName),
		m_MainPanel(new ImGuiWidget::ImCanvasPanel(WidgetName+"_MainPanel"))
	{
		SetAllowDragOn(true);
		SetRootWidget(m_MainPanel);
	}
};