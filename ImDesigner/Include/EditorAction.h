#pragma once
#include <EditorKit/StaticString.h>
#include "EditorGlobal.h"
// 编辑器所有Action键定义
namespace Action
{
	//MainUI动作
	namespace MainUI
	{
		extern const KeyStringType UI_FILE_SELECTED;//UI文件被选中（点击）
		extern const KeyStringType EDITOR_PAGE_CLOSED;//编辑页面被关闭
		extern const KeyStringType EDITOR_PAGE_SELECTED;//编辑页面被选中（通过Tab按钮）
		extern const KeyStringType _REQUEST_UPDATE_UNDOREDO_STATE;//请求更新UndoRedo状态
	}

	namespace ProjectView
	{
		extern const KeyStringType UI_FILE_SELECTED;//UI文件被选中（点击）
		extern const KeyStringType CREATE_NEW_FILE;//请求创建新文件
		extern const KeyStringType RENAME_FILE;//请求重命名文件
	}

	namespace OutlineView
	{
		extern const KeyStringType SELECT_VARIABLE;          // 选中变量
		extern const KeyStringType SELECT_WIDGET;            // 选中控件
		extern const KeyStringType CREATE_BASIC_VARIABLE;          // 创建基本变量
		extern const KeyStringType CREATE_OBJECT_VARIABLE;          // 创建Object变量
		extern const KeyStringType CREATE_WIDGET_VARIABLE;          // 创建控件树变量
		extern const KeyStringType DELETE_VARIABLE;          // 删除变量
		extern const KeyStringType INSERT_WIDGET;            // 插入控件
		extern const KeyStringType DELETE_WIDGET;            // 删除控件
		extern const KeyStringType COPY_VARIABLE;			//复制变量
	}

	namespace UIFileView
	{
		extern const KeyStringType REQUEST_DELETE_WIDGET;//请求删除控件

		extern const KeyStringType REQUEST_INSERT_WIDGET;//请求插入控件
	}

	namespace DetailView
	{
		extern const KeyStringType _REQUEST_EDIT_PROPERTY;//请求编辑object属性
	}

	extern const KeyStringType _REQUEST_UNDO;//请求撤销
	extern const KeyStringType _REQUEST_REDO;//请求撤销
	extern const KeyStringType WIDGET_SELECTED;//控件被选中（点击）
}

