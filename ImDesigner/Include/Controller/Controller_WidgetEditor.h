#pragma once
#include "ImWidget/ImBasicWidgetDeclaration.h"
#include "ImTools/ImDelegate.h"
class UI_WidgetTreeView;
class UI_WidgetEditor;
class Model_WidgetEditor;
class UI_DetailView;

class Controller_WidgetEditor
{
private:
	UI_WidgetTreeView* m_UI_WidgetTreeView;
	UI_WidgetEditor* m_UI_WidgetEditor;
	UI_DetailView* m_UI_FileDetail;
	Model_WidgetEditor* m_Model_WidgetEditor;
public:
	ImMulticastDelegate<bool, bool> OnUndoRedoStateChanged;
public:
	Controller_WidgetEditor(
		UI_WidgetTreeView* In_UI_WidgetTreeView,
		UI_WidgetEditor* In_UI_WidgetEditor,
		UI_DetailView* In_UI_FileDetail,
		Model_WidgetEditor* In_Model_WidgetEditor);

	//void SetSelectedWidget(ImGuiWidget::ImWidget* SelectedWidget);
	void RequestUndo();
	void RequestRedo();
	void UpdateUndoRedoState();
};