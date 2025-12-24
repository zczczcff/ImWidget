#pragma once

#include <imgui.h>

namespace ImGuiWidget
{
    // 事件类型枚举
    enum class ImEventType {
        // 鼠标事件
        MouseDown,
        MouseUp,
        MouseMove,
        MouseEnter,
        MouseLeave,
        MouseWheel,
        MouseClick,
        MouseDoubleClick,
        MouseDragStart,
        MouseDrag,
        MouseDragEnd,

        // 键盘事件
        KeyDown,
        KeyUp,
        KeyPress,

        // 焦点事件
        FocusIn,
        FocusOut,

        // 拖拽事件
        DragEnter,
        DragOver,
        DragLeave,
        Drop,

        // 手势事件（预留）
        GestureStart,
        GestureUpdate,
        GestureEnd,

        // 自定义事件
        Custom
    };

    // 事件阶段
    enum class ImEventPhase {
        Capture,  // 捕获阶段（从根到目标）
        Target,   // 目标阶段
        Bubble    // 冒泡阶段（从目标到根）
    };

    // 修饰键状态
    struct ImModifierKeys {
        bool ctrl : 1;
        bool shift : 1;
        bool alt : 1;
        bool super : 1;  // Windows键或Command键

        ImModifierKeys() : ctrl(false), shift(false), alt(false), super(false) {}

        static ImModifierKeys GetCurrent() {
            ImModifierKeys mods;
            ImGuiIO& io = ImGui::GetIO();
            mods.ctrl = io.KeyCtrl;
            mods.shift = io.KeyShift;
            mods.alt = io.KeyAlt;
            mods.super = io.KeySuper;
            return mods;
        }
    };
}