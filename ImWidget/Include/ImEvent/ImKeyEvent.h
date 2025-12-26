#pragma once
#include "ImEvent/ImEvent.h"

namespace ImGuiWidget
{
    // 键盘事件基类
    class ImKeyEvent : public ImEvent
    {
    private:
        int m_keyCode;
        int m_scancode;
        ImModifierKeys m_modifiers;
        char m_character;  // 对于KeyPress事件，表示输入的字符
        bool m_repeat;     // 是否为重复按键

    public:
        ImKeyEvent(ImEventType type, int keyCode, int scancode = 0, char character = 0, bool repeat = false)
            : ImEvent(type), m_keyCode(keyCode), m_scancode(scancode), m_character(character),
            m_repeat(repeat), m_modifiers(ImModifierKeys::GetCurrent())
        {
        }

        int GetKeyCode() const { return m_keyCode; }
        int GetScancode() const { return m_scancode; }
        char GetCharacter() const { return m_character; }
        ImModifierKeys GetModifiers() const { return m_modifiers; }
        bool IsRepeat() const { return m_repeat; }

        // 添加SetModifiers函数
        void SetModifiers(const ImModifierKeys& mods) { m_modifiers = mods; }

        // 设置重复状态
        void SetRepeat(bool repeat) { m_repeat = repeat; }

        // 设置字符（用于KeyPress事件）
        void SetCharacter(char character) { m_character = character; }

        // 常用键检查
        bool IsEscape() const { return m_keyCode == ImGuiKey_Escape; }
        bool IsEnter() const { return m_keyCode == ImGuiKey_Enter || m_keyCode == ImGuiKey_KeypadEnter; }
        bool IsTab() const { return m_keyCode == ImGuiKey_Tab; }
        bool IsBackspace() const { return m_keyCode == ImGuiKey_Backspace; }
        bool IsDelete() const { return m_keyCode == ImGuiKey_Delete; }
        bool IsSpace() const { return m_keyCode == ImGuiKey_Space; }
        bool IsArrowKey() const
        {
            return m_keyCode == ImGuiKey_LeftArrow || m_keyCode == ImGuiKey_RightArrow ||
                m_keyCode == ImGuiKey_UpArrow || m_keyCode == ImGuiKey_DownArrow;
        }
        bool IsHome() const { return m_keyCode == ImGuiKey_Home; }
        bool IsEnd() const { return m_keyCode == ImGuiKey_End; }
        bool IsPageUp() const { return m_keyCode == ImGuiKey_PageUp; }
        bool IsPageDown() const { return m_keyCode == ImGuiKey_PageDown; }

        // 检查是否按下特定修饰键组合
        bool IsCtrlPressed() const { return m_modifiers.ctrl; }
        bool IsShiftPressed() const { return m_modifiers.shift; }
        bool IsAltPressed() const { return m_modifiers.alt; }
        bool IsSuperPressed() const { return m_modifiers.super; }

        // 检查特定组合键
        bool IsCtrlOnly() const { return m_modifiers.ctrl && !m_modifiers.shift && !m_modifiers.alt && !m_modifiers.super; }
        bool IsShiftOnly() const { return !m_modifiers.ctrl && m_modifiers.shift && !m_modifiers.alt && !m_modifiers.super; }
        bool IsAltOnly() const { return !m_modifiers.ctrl && !m_modifiers.shift && m_modifiers.alt && !m_modifiers.super; }
        bool IsCtrlShift() const { return m_modifiers.ctrl && m_modifiers.shift && !m_modifiers.alt && !m_modifiers.super; }

        virtual ImKeyEvent* Clone() const override
        {
            return new ImKeyEvent(*this);
        }
    };

    class ImKeyDownEvent : public ImKeyEvent
    {
    public:
        ImKeyDownEvent(int keyCode, int scancode = 0, bool repeat = false)
            : ImKeyEvent(ImEventType::KeyDown, keyCode, scancode, 0, repeat)
        {
        }

        virtual ImKeyDownEvent* Clone() const override
        {
            return new ImKeyDownEvent(*this);
        }
    };

    class ImKeyUpEvent : public ImKeyEvent
    {
    public:
        ImKeyUpEvent(int keyCode, int scancode = 0)
            : ImKeyEvent(ImEventType::KeyUp, keyCode, scancode)
        {
        }

        virtual ImKeyUpEvent* Clone() const override
        {
            return new ImKeyUpEvent(*this);
        }
    };

    class ImKeyPressEvent : public ImKeyEvent
    {
    public:
        ImKeyPressEvent(char character)
            : ImKeyEvent(ImEventType::KeyPress, 0, 0, character)
        {
        }

        virtual ImKeyPressEvent* Clone() const override
        {
            return new ImKeyPressEvent(*this);
        }
    };
}