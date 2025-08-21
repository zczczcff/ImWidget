#pragma once
#include "ImWidget/ImUserWidget.h"

#include "ImWindows/ImPageManager.h"
#include "ImDesignPanel.h"
#include "ImWidget/ImWidgetSerializer.h"
#include "ImWidget/ImWidgetCodeGenerator.h"

class ImCreatorUIPageManager :public ImGuiWidget::ImUserWidget
{
private:
	struct PageInfor
	{
		DesiginPanel* PanelWidget;
		std::string OutPutOffset;
	};
private:
	ImWindows::ImPageManager* m_RootPageManager;
	std::map<std::string, PageInfor> AllUIDesiginPages;
public:
	std::function<void(ImWidget*)> OnWidgetSelected;
	std::function<void(ImWidget*)> OnDragWidgetOn;
	std::function<void(const std::string&)> OnPageSwitched;
	std::function<void(ImWidget*)> OnCopyWidget;
	std::function<void()> OnPasteWidget;
private:
	void HandleClosePage(const std::string& Title)
	{
		auto it = AllUIDesiginPages.find(Title);
		if (it != AllUIDesiginPages.end())
		{
			delete it->second.PanelWidget;
			AllUIDesiginPages.erase(it);
		}
	}
public:
	ImCreatorUIPageManager():ImGuiWidget::ImUserWidget("ImCreatorUIPageManager")
	{
		m_RootPageManager = new ImWindows::ImPageManager("ImCreatorUIPageManager_RootPageManager");
		SetRootWidget(m_RootPageManager);
		m_RootPageManager->SetOnPageClosed([this](const std::string& Title) 
			{
				HandleClosePage(Title);
			});
	}

	bool AddEditedPage(const std::string& FileName,const std::string& FilePath,const std::string& OutPutOffset)
	{
		auto it = AllUIDesiginPages.find(FileName);
		if ( it != AllUIDesiginPages.end())
		{
			m_RootPageManager->SetActivePageByContent(it->second.PanelWidget);			
			return false;
		}
		DesiginPanel* NewDesiginPanel = new DesiginPanel(FileName + "_DesiginPanel");
		if (!NewDesiginPanel->InitFromFile(FilePath))
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

		NewDesiginPanel->SetOnCopyWidget([this](ImGuiWidget::ImWidget* CopyWidget) 
			{
				if (OnCopyWidget)
				{
					OnCopyWidget(CopyWidget);
				}
			});

		NewDesiginPanel->SetOnPasteWidget([this]() 
			{
				if (OnPasteWidget)
				{
					OnPasteWidget();
				}
			});

		PageInfor NewPage = { NewDesiginPanel ,OutPutOffset };
		AllUIDesiginPages.insert(std::make_pair(FileName, NewPage));
		m_RootPageManager->AddPage(FileName, NewDesiginPanel);
		return true;
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

		PageInfor NewPage = { NewDesiginPanel ,""};
		AllUIDesiginPages.insert(std::make_pair(file, NewPage));
		m_RootPageManager->AddPage(file, NewDesiginPanel);
		return false;
	}

	bool SaveCurrentUIFile(const std::string& OutPutFolder)
	{
		ImGuiWidget::ImWidget* ActiveDesiginPanel = m_RootPageManager->GetActivePageContent();
		if (ActiveDesiginPanel)
		{
			std::string FileName;
			PageInfor CurrentInfor;
			for (auto& e : AllUIDesiginPages)
			{
				if (e.second.PanelWidget == ActiveDesiginPanel)
				{
					FileName = e.first;
					CurrentInfor = e.second;
				}
			}
			if (FileName.empty())return false;
			ImGuiWidget::ImWidget* ActiveRootWidget = ((DesiginPanel*)ActiveDesiginPanel)->GetRootContent();
			std::string OutPutPath = OutPutFolder;
			if (!CurrentInfor.OutPutOffset.empty())
			{
				OutPutPath.append("/");
				OutPutPath.append(CurrentInfor.OutPutOffset);
				OutPutPath.append("/");
			}
			OutPutPath.append(FileName);
			OutPutPath.append(".imui");
			ImGuiWidget::SaveWidgetTreeToFile(ActiveRootWidget, OutPutPath);
		}
		return true;
	}

	bool GenerateCode(const std::string& CppoutputDirectory, const std::string& HeaderoutputDirectory)
	{
		ImGuiWidget::ImWidget* ActiveDesiginPanel = m_RootPageManager->GetActivePageContent();
		if (ActiveDesiginPanel)
		{
			std::string FileName;
			PageInfor CurrentInfor;
			for (auto& e : AllUIDesiginPages)
			{
				if (e.second.PanelWidget == ActiveDesiginPanel)
				{
					FileName = e.first;
					CurrentInfor = e.second;
				}
			}
			if (FileName.empty())return false;
			ImGuiWidget::ImWidget* ActiveRootWidget = ((DesiginPanel*)ActiveDesiginPanel)->GetRootContent();

			std::string CppOutPutDir = CppoutputDirectory;
			std::string HeaderOutPutDir = HeaderoutputDirectory;
			if (!CurrentInfor.OutPutOffset.empty())
			{
				CppOutPutDir.append("/");
				CppOutPutDir.append(CurrentInfor.OutPutOffset);
				HeaderOutPutDir.append("/");
				HeaderOutPutDir.append(CurrentInfor.OutPutOffset);

			}
			if (!FileUtil::createDirectory(CppOutPutDir) || !FileUtil::createDirectory(HeaderOutPutDir))
			{
				return false;
			}

			return ImGuiWidget::ExportUserWidgetToFiles(ActiveRootWidget, FileName, CppOutPutDir, HeaderOutPutDir);
		}
		return true;
	}

	void SetOnWidgetSelected(std::function<void(ImWidget*)>callback)
	{
		OnWidgetSelected = callback;
	}
	ImGuiWidget::ImDesignPanel* GetCurrentMainPanel()
	{
		if (auto CurrentDesignPanel = dynamic_cast<DesiginPanel*>(m_RootPageManager->GetActivePageContent()))
		{
			return CurrentDesignPanel->GetMainPanel();
		}
		else
		{
			return nullptr;
		}
	}

	ImGuiWidget::ImDesignPanel* GetMainPanelByName(const std::string& Name)
	{
		auto it = AllUIDesiginPages.find(Name);
		if (it != AllUIDesiginPages.end())
		{
			return it->second.PanelWidget->GetMainPanel();
		}
		return nullptr;
	}

	void SetActivePage(const std::string& Title)
	{
		m_RootPageManager->SetActivePage(Title);
	}

	void SetOnWidgetDragedOn(std::function<void(ImWidget*)> callback)
	{
		OnDragWidgetOn = callback;
	}

	void SetOnPageSwitched(std::function<void(const std::string&)> callback) { m_RootPageManager->SetOnActivePageChanged(callback); }
};