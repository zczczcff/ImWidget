#pragma once
#include "ImEvent/ImEvent.h"
namespace ImGuiWidget
{
    // 键盘事件
    class ImKeyEvent : public ImEvent {
    private:
        int m_keyCode;
        int m_scancode;
        ImModifierKeys m_modifiers;
        char m_character;  // 对于KeyPress事件，表示输入的字符

    public:
        ImKeyEvent(ImEventType type, int keyCode, int scancode = 0, char character = 0)
            : ImEvent(type), m_keyCode(keyCode), m_scancode(scancode), m_character(character),
            m_modifiers(ImModifierKeys::GetCurrent()) {}

        int GetKeyCode() const { return m_keyCode; }
        int GetScancode() const { return m_scancode; }
        char GetCharacter() const { return m_character; }
        ImModifierKeys GetModifiers() const { return m_modifiers; }

        // 常用键检查
        bool IsEscape() const { return m_keyCode == ImGuiKey_Escape; }
        bool IsEnter() const { return m_keyCode == ImGuiKey_Enter || m_keyCode == ImGuiKey_KeypadEnter; }
        bool IsTab() const { return m_keyCode == ImGuiKey_Tab; }
        bool IsBackspace() const { return m_keyCode == ImGuiKey_Backspace; }
        bool IsDelete() const { return m_keyCode == ImGuiKey_Delete; }

        virtual ImKeyEvent* Clone() const override {
            return new ImKeyEvent(*this);
        }
    };

    class ImKeyDownEvent : public ImKeyEvent {
    public:
        ImKeyDownEvent(int keyCode, int scancode = 0)
            : ImKeyEvent(ImEventType::KeyDown, keyCode, scancode) {}

        virtual ImKeyDownEvent* Clone() const override {
            return new ImKeyDownEvent(*this);
        }
    };

    class ImKeyUpEvent : public ImKeyEvent {
    public:
        ImKeyUpEvent(int keyCode, int scancode = 0)
            : ImKeyEvent(ImEventType::KeyUp, keyCode, scancode) {}

        virtual ImKeyUpEvent* Clone() const override {
            return new ImKeyUpEvent(*this);
        }
    };

    class ImKeyPressEvent : public ImKeyEvent {
    public:
        ImKeyPressEvent(char character)
            : ImKeyEvent(ImEventType::KeyPress, 0, 0, character) {}

        virtual ImKeyPressEvent* Clone() const override {
            return new ImKeyPressEvent(*this);
        }
    };
}