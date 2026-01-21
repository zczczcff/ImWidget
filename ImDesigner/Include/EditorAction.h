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
		extern const KeyStringType REQUEST_UNDO;//请求撤销
		extern const KeyStringType REQUEST_REDO;//请求重做
	}

	namespace ProjectView
	{
		extern const KeyStringType UI_FILE_SELECTED;//UI文件被选中（点击）
		extern const KeyStringType CREATE_NEW_FILE;//请求创建新文件
		extern const KeyStringType RENAME_FILE;//请求重命名文件
	}

	namespace UIFileView
	{
		extern const KeyStringType REQUEST_DELETE_WIDGET;//请求删除控件
		
		extern const KeyStringType REQUEST_INSERT_WIDGET;//请求插入控件
	}

	extern const KeyStringType WIDGET_SELECTED;//控件被选中（点击）
}

