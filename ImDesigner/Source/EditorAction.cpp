#include "EditorAction.h"

namespace Action
{
	// Action键定义
	namespace MainUI
	{
		const KeyStringType UI_FILE_SELECTED = "UIFileSelected";
		const KeyStringType EDITOR_PAGE_CLOSED = "EditorPageClosed";
		const KeyStringType EDITOR_PAGE_SELECTED = "EditorPageSelected";
		const KeyStringType _REQUEST_UPDATE_UNDOREDO_STATE;//请求更新UndoRedo状态
	}

	namespace ProjectView
	{
		const KeyStringType UI_FILE_SELECTED = "UIFileSelected";
		const KeyStringType CREATE_NEW_FILE = "CreateNewFile";//请求创建新文件
		const KeyStringType RENAME_FILE = "RenameFile";//请求重命名文件
	}

	namespace UIFileView
	{
		const KeyStringType REQUEST_DELETE_WIDGET = "RequestDeleteWidget";//请求删除控件
		
		const KeyStringType REQUEST_INSERT_WIDGET = "RequestInsertWidget";//请求插入控件
	}

	namespace DetailView
	{
		const KeyStringType _REQUEST_EDIT_PROPERTY="_RequestEditProperty";//请求编辑object属性
	}
	const KeyStringType _REQUEST_UNDO = "_RequestUndo";//请求撤销
	const KeyStringType _REQUEST_REDO = "_RequestRedo";//请求撤销
	const KeyStringType WIDGET_SELECTED = "WidgetSelected";//控件被选中（点击）
}
