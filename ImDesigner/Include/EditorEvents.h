#pragma once
#include "EditorGlobal.h"


namespace Events
{
	extern const KeyStringType REGISTER_LOG_UPDATE_FUN;//注册日志更新委托

	namespace MainUI
	{
		extern const KeyStringType SET_UNDOREDO_STATE;//设置UndoRedo状态
		extern const KeyStringType UI_FILE_OPENED;//UI文件被打开
	}

	namespace ProjectView
	{
		extern const KeyStringType ACTIVATE_FILE_RENAME;//激活指定文件的重命名
		extern const KeyStringType EXPAND_TO_FILE;//将树状视图展开至指定文件
		extern const KeyStringType SCROLL_TO_FILE_WITH_DELAY;//将指定文件滚动至可见范围内
		extern const KeyStringType UPDATE_PROJECT_VIEW;//更新项目视图
	}

	namespace OutlineView
	{
		extern const KeyStringType UPDATE_BASIC_VARIABLE_SECTION;//刷新基本变量区
		extern const KeyStringType UPDATE_OBJECT_VARIABLE_SECTION;//刷新基本变量区
		extern const KeyStringType UPDATE_WIDGET_VARIABLE_SECTION;//刷新基本变量区
		extern const KeyStringType VARIABLE_RENAMED;// 变量重命名
		extern const KeyStringType WIDGET_CHILD_ADDED;// 子控件添加
		extern const KeyStringType WIDGET_CHILD_REMOVED;// 子控件删除
		extern const KeyStringType WIDGET_CHILD_RENAMED;// 子控件重命名
	}

	namespace UIFileView
	{
		extern const KeyStringType UPDATE_WIDGETTREE_VIEW;//更新控件树
	}

	namespace DetailView
	{
		extern const KeyStringType UPDATE_PROPERTY_DISPLAY;//更新属性显示
	}
}