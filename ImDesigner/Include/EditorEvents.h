#pragma once
#include "EditorGlobal.h"


namespace Events
{
	namespace MainUI
	{
		extern const KeyStringType SET_UNDOREDO_STATE;//设置UndoRedo状态
		
	}

	namespace ProjectView
	{
		extern const KeyStringType ACTIVATE_FILE_RENAME;//激活指定文件的重命名
		extern const KeyStringType EXPAND_TO_FILE;//将树状视图展开至指定文件
		extern const KeyStringType SCROLL_TO_FILE_WITH_DELAY;//将指定文件滚动至可见范围内
		extern const KeyStringType UPDATE_PROJECT_VIEW;//更新项目视图
	}
}