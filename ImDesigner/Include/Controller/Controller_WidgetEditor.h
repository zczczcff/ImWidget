#pragma once
class UI_WidgetTreeView;
class UI_WidgetEditor;
class Model_WidgetEditor;
class UI_FileDetail;

class Controller_WidgetEditor
{
private:
	UI_WidgetTreeView* m_UI_WidgetTreeView;
	UI_WidgetEditor* m_UI_WidgetEditor;
	UI_FileDetail* m_UI_FileDetail;
	Model_WidgetEditor* m_Model_WidgetEditor;
public:
	Controller_WidgetEditor(
		UI_WidgetTreeView* In_UI_WidgetTreeView,
		UI_WidgetEditor* In_UI_WidgetEditor,
		UI_FileDetail* In_UI_FileDetail,
		Model_WidgetEditor* In_Model_WidgetEditor);
};