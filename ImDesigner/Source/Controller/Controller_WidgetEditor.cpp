#include "Controller/Controller_WidgetEditor.h"

Controller_WidgetEditor::Controller_WidgetEditor(UI_WidgetTreeView* In_UI_WidgetTreeView, UI_WidgetEditor* In_UI_WidgetEditor, UI_FileDetail* In_UI_FileDetail, Model_WidgetEditor* In_Model_WidgetEditor)
	:m_UI_WidgetTreeView(In_UI_WidgetTreeView),
	m_UI_WidgetEditor(In_UI_WidgetEditor),
	m_UI_FileDetail(In_UI_FileDetail),
	m_Model_WidgetEditor(In_Model_WidgetEditor)
{

}
