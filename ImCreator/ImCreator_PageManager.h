#pragma once
#include "ImWidget/ImUserWidget.h"

#include "ImWindows/ImPageManager.h"
#include "ImDesignPanel.h"
#include "ImWidget/ImWidgetSerializer.h"

class ImCreatorUIPageManager :public ImGuiWidget::ImUserWidget
{
private:
	ImWindows::ImPageManager* m_RootPageManager;
	std::map<std::string, DesiginPanel*> AllUIDesiginPages;
	std::function<void(ImWidget*)> OnWidgetSelected;
	std::function<void(ImWidget*)> OnDragWidgetOn;
public:
	ImCreatorUIPageManager():ImGuiWidget::ImUserWidget("ImCreatorUIPageManager")
	{
		m_RootPageManager = new ImWindows::ImPageManager("ImCreatorUIPageManager_RootPageManager");
		SetRootWidget(m_RootPageManager);
	}

	bool AddEditedPage(const std::string& file)
	{
		auto it = AllUIDesiginPages.find(file);
		if ( it != AllUIDesiginPages.end())
		{
			m_RootPageManager->SetActivePageByContent(it->second);			
			return false;
		}
		DesiginPanel* NewDesiginPanel = new DesiginPanel(file + "_DesiginPanel");
		if (!NewDesiginPanel->InitFromFile(file))
		{
			delete NewDesiginPanel;
			return false;
		}
		NewDesiginPanel->SetOnSelected([this](ImGuiWidget::ImWidget* SelectedWidget) 
			{
				if (OnWidgetSelected)
				{
					OnWidgetSelected(SelectedWidget);
				}
			});
		NewDesiginPanel->SetOnDragWidgetOn([this](ImGuiWidget::ImWidget* NewWidget) {OnDragWidgetOn(NewWidget); });
		AllUIDesiginPages.insert(std::make_pair(file, NewDesiginPanel));
		m_RootPageManager->AddPage(file, NewDesiginPanel);
		return false;
	}

	bool CreateNewUIFile(const std::string& file)
	{
		auto it = AllUIDesiginPages.find(file);
		if (it != AllUIDesiginPages.end())
		{
			return false;
		}
		DesiginPanel* NewDesiginPanel = new DesiginPanel(file + "_DesiginPanel");
		NewDesiginPanel->SetOnSelected([this](ImGuiWidget::ImWidget* SelectedWidget)
			{
				if (OnWidgetSelected)
				{
					OnWidgetSelected(SelectedWidget);
				}
			});
		NewDesiginPanel->SetOnDragWidgetOn([this](ImGuiWidget::ImWidget* NewWidget) {OnDragWidgetOn(NewWidget); });
		AllUIDesiginPages.insert(std::make_pair(file, NewDesiginPanel));
		m_RootPageManager->AddPage(file, NewDesiginPanel);
		return false;
	}

	bool SaveCurrentUIFile()
	{
		ImGuiWidget::ImWidget* ActiveDesiginPanel = m_RootPageManager->GetActivePageContent();
		if (ActiveDesiginPanel)
		{
			std::string FileName;
			for (auto& e : AllUIDesiginPages)
			{
				if (e.second == ActiveDesiginPanel)
				{
					FileName = e.first;
				}
			}
			if (FileName.empty())return false;
			ImGuiWidget::ImWidget* ActiveRootWidget = ((DesiginPanel*)ActiveDesiginPanel)->GetRootContent();

			ImGuiWidget::SaveWidgetTreeToFile(ActiveRootWidget, FileName);
		}
		return true;
	}

	void SetOnWidgetSelected(std::function<void(ImWidget*)>callback)
	{
		OnWidgetSelected = callback;
	}
	ImGuiWidget::ImWidget* GetCurrentMainPanel() 
	{
		return m_RootPageManager->GetActivePageContent();
	}

	ImGuiWidget::ImWidget* GetMainPanelByName(const std::string& Name)
	{
		auto it = AllUIDesiginPages.find(Name);
		if (it != AllUIDesiginPages.end())
		{
			return it->second->GetMainPanel();
		}
	}
	void SetOnWidgetDragedOn(std::function<void(ImWidget*)> callback)
	{
		OnDragWidgetOn = callback;
	}
};