#include "EditorEvents.h"

namespace Events
{
	const KeyStringType REGISTER_LOG_UPDATE_FUN = "RegisterLogUpdateFun";//注册日志更新委托

	namespace MainUI
	{
		const KeyStringType SET_UNDOREDO_STATE = "SetUndoRedoState";//设置UndoRedo状态
		const KeyStringType UI_FILE_OPENED = "UIFIleOpened";//UI文件被打开
	}

	namespace ProjectView
	{
		const KeyStringType ACTIVATE_FILE_RENAME = "ActivateFileRename";//激活指定文件的重命名
		const KeyStringType EXPAND_TO_FILE = "ExpandToFile";//将树状视图展开至指定文件
		const KeyStringType SCROLL_TO_FILE_WITH_DELAY = "ScrollToFileWithDelay";//将指定文件滚动至可见范围内
		const KeyStringType UPDATE_PROJECT_VIEW = "UpdateProjectView";//更新项目视图
	}
}