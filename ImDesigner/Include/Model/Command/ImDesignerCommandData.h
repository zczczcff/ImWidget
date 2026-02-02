#pragma once
#include "ImDesignerCommandType.h"

// CommandBase使用的DataType数据结构
struct CommandDataType
{
    CommandCategory category;
    int subType;  // 使用int存储子类型，由具体命令类转换为对应枚举
    std::string ValueInfor;

    CommandDataType(CommandCategory cat = CommandCategory::ClassProperty,
        int sub = 0,const std::string& ValueInfor = "")
        : category(cat), subType(sub),ValueInfor(ValueInfor)
    {
    }

    // 用于合并判断
    bool operator==(const CommandDataType& other) const
    {
        return category == other.category && subType == other.subType && ValueInfor == ValueInfor;
    }
};