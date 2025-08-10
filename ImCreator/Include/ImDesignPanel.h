#pragma once
#include "ImWidget/ImBasicWidgetList.h"

#include "ImWidget/ImWidgetSerializer.h"
#include "ExampleWidgetInfor.h"

class DesiginPanel : public ImGuiWidget::ImUserWidget
{
private:
	ImGuiWidget::ImDesignPanel* m_MainPanel;

	std::function<void(ImWidget*)> OnDragWidgetOn;
protected:
	virtual void OnDragOn(ImGuiWidget::ImDragHandle* OriginalHandle) override
	{
		static int count = 0;
		ImVec2 MousePos = ImGui::GetMousePos();
		ExampleWidgetDragHandle* Target_ExampleWidgetDragHandle = dynamic_cast<ExampleWidgetDragHandle*>(OriginalHandle);

		if (!Target_ExampleWidgetDragHandle) return;

		ImGuiWidget::ImWidget* NewWidget = nullptr;

		switch (Target_ExampleWidgetDragHandle->widgettype)
		{
		case WidgetType::ImButton:
		{
			NewWidget = new ImGuiWidget::ImButton("Button_" + std::to_string(count));
			break;
		}
		case WidgetType::ImTextBlock:
		{
			NewWidget = new ImGuiWidget::ImTextBlock("TextBlock_" + std::to_string(count));
			break;
		}
		case WidgetType::ImCheckBox:
		{
			NewWidget = new ImGuiWidget::ImCheckBox("ImCheckBox");
			break;
		}
		case WidgetType::ImImage:
		{
			NewWidget = new ImGuiWidget::ImImage("Image_" + std::to_string(count));
			break;
		}
		case WidgetType::ImCanvasPanel:
		{
			NewWidget = new ImGuiWidget::ImCanvasPanel("CanvasPanel_" + std::to_string(count));
			break;
		}
		case WidgetType::ImHorizontalBox:
		{
			NewWidget = new ImGuiWidget::ImHorizontalBox("HorizontalBox_" + std::to_string(count));
			break;
		}
		case WidgetType::ImVerticalBox:
		{
			NewWidget = new ImGuiWidget::ImVerticalBox("VerticalBox_" + std::to_string(count));
			break;
		}
		case WidgetType::ImComboBox:
		{
			NewWidget = new ImGuiWidget::ImComboBox("ComboBox_" + std::to_string(count));
			break;
		}
		case WidgetType::ImVerticalSplitter:
		{
			NewWidget = new ImGuiWidget::ImVerticalSplitter("VerticalSplitter");
			break;
		}
		case WidgetType::ImHorizontalSplitter:
		{
			NewWidget = new ImGuiWidget::ImHorizontalSplitter("ImHorizontalSplitter");
			break;
		}
		case WidgetType::ImExpandableBox:
		{
			NewWidget = new ImGuiWidget::ImExpandableBox("ImExpandableBox");
			break;
		}
		case WidgetType::ImInputText:
		{
			NewWidget = new ImGuiWidget::ImInputText("ImInputText");
			break;
		}
		case WidgetType::ImIntInput:
		{
			NewWidget = new ImGuiWidget::ImIntInput("ImIntInput");
			break;
		}
		case WidgetType::ImFloatInput:
		{
			NewWidget = new ImGuiWidget::ImFloatInput("ImFloatInput");
			break;
		}
		case WidgetType::ImMultiLineTextBlock:
		{
			NewWidget = new ImGuiWidget::ImMultiLineTextBlock("ImMultiLineTextBlock");
			break;
		}
		case WidgetType::ImScrollBox:
		{
			NewWidget = new ImGuiWidget::ImScrollBox("ImScrollBox");
			break;
		}
		case WidgetType::ImScrollingTextList:
		{
			NewWidget = new ImGuiWidget::ImScrollingTextList("ImScrollingTextList");
			break;
		}
		case WidgetType::ImSlider:
		{
			NewWidget = new ImGuiWidget::ImSlider("ImSlider");
			break;
		}
		default:
			break;
		}
		if (NewWidget)
		{
			auto widget = m_MainPanel->ChildHitTest(MousePos);
			ImGuiWidget::ImPanelWidget* Panel = dynamic_cast<ImGuiWidget::ImPanelWidget*>(widget);
			if (Panel)
			{
				if (!Panel->AddChild(NewWidget, MousePos - Panel->GetPosition()))
				{
					delete NewWidget;
				}
				else
				{
					if (OnDragWidgetOn)
					{
						OnDragWidgetOn(NewWidget);
					}
				}
			}
			if (OnDragWidgetOn)
			{
				OnDragWidgetOn(NewWidget);
			}


			count++;
		}

	}

	void Handle_DragOnButton(ImVec2 MousePos)
	{
		static int count = 0;

		ImGuiWidget::ImButton* button = new ImGuiWidget::ImButton("Button_" + std::to_string(count));
		//m_MainPanel->AddChildToCanvasPanel(button)->SetSlotPosAndSize(MousePos - Position, button->GetMinSize());
		auto widget = m_MainPanel->ChildHitTest(MousePos);
		ImGuiWidget::ImPanelWidget* Panel = dynamic_cast<ImGuiWidget::ImPanelWidget*>(widget);
		if (Panel)
		{
			Panel->AddChild(button);
		}
		count++;
	}
	void Handle_DragOnTextBlock(ImVec2 MousePos)
	{
		static int count = 0;

	}
public:
	DesiginPanel(const std::string& WidgetName) :
		ImGuiWidget::ImUserWidget(WidgetName),
		m_MainPanel(new ImGuiWidget::ImDesignPanel(WidgetName + "_MainPanel"))
	{
		SetAllowDragOn(true);
		SetRootWidget(m_MainPanel);
	}

	void SetOnSelected(std::function<void(ImWidget*)> callback) { m_MainPanel->SetOnSelected(callback); }
	void SetOnDragWidgetOn(std::function<void(ImWidget*)>callback) { OnDragWidgetOn = callback; }

	bool InitFromFile(const std::string& file)
	{
		if (m_MainPanel->GetChildAt(0))return false;
		ImGuiWidget::ImWidget* NewRoot = ImGuiWidget::LoadWidgetTreeFromFile(file);
		if (!NewRoot)return false;
		m_MainPanel->AddChild(NewRoot);
	}

	ImGuiWidget::ImDesignPanel* GetMainPanel() { return m_MainPanel; }
	ImGuiWidget::ImWidget* GetRootContent()
	{
		return m_MainPanel->GetChildAt(0);
	}
};