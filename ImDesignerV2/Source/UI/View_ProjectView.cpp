#include "UI/View_ProjectView.h"
#include <EditorKit/StatePath.h>
void UI_ProjectView::Init()
{
	WatchStringRelative("CurrentProjectFolder", [this](const std::string& folder) { RebuildView(); });
}

void UI_ProjectView::RebuildView()
{
	std::string CurrentFolder = (*m_StateSystem)[m_Path + "/" + "CurrentProjectFolder"].GetStringValue();

}

UI_ProjectView::UI_ProjectView(const std::string& name, StatePath* stateSystem, const std::string& Path)
	:ImGuiWidget::ImUserWidget(name),
	EditorObjectBase(stateSystem,Path)
{

}
