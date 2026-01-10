#ifndef BITICON_ICONS_H
#define BITICON_ICONS_H

#include <cstdint>
#include <cstddef>

// 图标数据结构
struct BitIcon_IconData {
    int width;
    int height;
    const unsigned char* bit_data;  // 每个像素1位，1=不透明，0=透明
    
    // 将图标解码为RGBA格式
    // invert: 是否反转颜色（黑白互换）
    unsigned char* ToRGBA(bool invert = false) const;
    
    // 释放RGBA数据内存
    static void FreeRGBA(unsigned char* data);
};

// Icon enumeration
enum BitIcon_IconIndex {
    BITICON_DIALOG = 0,
    BITICON_REDO = 1,
    BITICON_SINGLE_WIDGET = 2,
    BITICON_UNDO = 3,
    BITICON_WIDGET_TREE = 4,
    BITICON_COUNT
};

// 图标数组
extern const BitIcon_IconData* g_BitIcon_icons[];

// 图标名称数组
extern const char* g_BitIcon_icon_names[];

// 图标数量
extern const size_t g_BitIcon_icon_count;

// 获取所有图标的数量
inline size_t BitIcon_get_icon_count() {
    return g_BitIcon_icon_count;
}

// 通过索引获取图标
inline const BitIcon_IconData* BitIcon_get_icon_by_index(size_t index) {
    if (index >= g_BitIcon_icon_count) {
        return nullptr;
    }
    return g_BitIcon_icons[index];
}

// 通过名称获取图标
//inline const BitIcon_IconData* BitIcon_get_icon_by_name(const char* name) {
//    for (size_t i = 0; i < g_BitIcon_icon_count; i++) {
//        if (std::strcmp(g_BitIcon_icon_names[i], name) == 0) {
//            return g_BitIcon_icons[i];
//        }
//    }
//    return nullptr;
//}

// 通过枚举值获取图标
inline const BitIcon_IconData* BitIcon_get_icon(BitIcon_IconIndex index) {
    if (index >= BITICON_COUNT) {
        return nullptr;
    }
    return g_BitIcon_icons[index];
}

#endif // BITICON_ICONS_H
