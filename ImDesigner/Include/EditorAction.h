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

}

