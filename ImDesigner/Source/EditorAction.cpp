#include "EditorAction.h"

namespace Action
{
	// Action键定义
	namespace MainUI
	{
		const KeyStringType UI_FILE_SELECTED = "UIFileSelected";
		const KeyStringType EDITOR_PAGE_CLOSED = "EditorPageClosed";
		const KeyStringType EDITOR_PAGE_SELECTED = "EditorPageSelected";
		const KeyStringType REQUEST_UNDO = "RequestUndo";
		const KeyStringType REQUEST_REDO = "RequestRedo";
	}

	namespace ProjectView
	{
		const KeyStringType UI_FILE_SELECTED = "UIFileSelected";
		const KeyStringType CREATE_NEW_FILE = "CreateNewFile";//请求创建新文件
		const KeyStringType RENAME_FILE = "RenameFile";//请求重命名文件
	}
}
