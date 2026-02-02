#pragma once

// 命令大类枚举
enum class CommandCategory
{
    ClassProperty = 0,      // 1. 类基础字符属性设置
    VariableOperation,      // 2. 增删变量
    ObjectPropertyEdit,     // 3. ImObject变量属性路径编辑
    WidgetPropertyEdit,     // 4. 控件树属性路径编辑
    SlotPropertyEdit,        // 5. 控件树节点slot属性路径编辑
    ChildWidgetOperation,   //6. 控件树子项插入删除操作
    RenameOperation         //7.重命名操作
};