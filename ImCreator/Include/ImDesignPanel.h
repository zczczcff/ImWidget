#pragma once
#include "ImWidget/ImBasicWidgetList.h"

#include "ImWidget/ImWidgetSerializer.h"
#include "ExampleWidgetInfor.h"

class DesiginPanel : public ImGuiWidget::ImUserWidget
{
private:
	ImGuiWidget::ImDesignPanel* m_MainPanel;

	std::function<void(ImWidget*)> OnDragWidgetOn;
	std::function<void(ImWidget*)> OnCopyWidget;
	std::function<void()> OnPasteWidget;
	std::function<void(ImWidget*)> OnSelectedWidget;
	bool bIsWidgetSlectedAndPanelFocused = false;
protected:
	virtual void OnDragOn(ImGuiWidget::ImDragHandle* OriginalHandle) override
	{
		ImVec2 MousePos = ImGui::GetMousePos();
		ExampleWidgetDragHandle* Target_ExampleWidgetDragHandle = dynamic_cast<ExampleWidgetDragHandle*>(OriginalHandle);

		if (!Target_ExampleWidgetDragHandle) return;

		ImGuiWidget::ImWidget* NewWidget = CreateNewWidget(Target_ExampleWidgetDragHandle->widgettype);
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
		m_MainPanel->SetOnSelected([this](ImWidget* selectedwidget)
			{
				if (OnSelectedWidget)
				{
					OnSelectedWidget(selectedwidget);
				}
				if (selectedwidget)
				{
					bIsWidgetSlectedAndPanelFocused = true;
				}
			});
		m_MainPanel->SetOnUnSelected([this]() 
			{
				bIsWidgetSlectedAndPanelFocused = false;
			});
	}

	void SetOnSelected(std::function<void(ImWidget*)> callback) { OnSelectedWidget = callback; }
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

	virtual void Tick() override
	{
		if (bIsWidgetSlectedAndPanelFocused)
		{
			ImGuiIO& io = ImGui::GetIO();
			if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_C))
			{
				ImGuiWidget::ImWidget* CopyedWidget = m_MainPanel->GetSelectedWidget();
				if (CopyedWidget&&OnCopyWidget)
				{
					OnCopyWidget(CopyedWidget);
				}
			}
		}
	}
};