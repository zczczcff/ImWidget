#pragma once
#include "ImEvent/ImEvent.h"
#include <string>
namespace ImGuiWidget
{
    // 文本输入事件 - 专门处理字符输入
    class ImTextInputEvent : public ImEvent
    {
    private:
        std::string m_text;          // 输入的文本内容
        ImModifierKeys m_modifiers;  // 修饰键状态
        bool m_isComposing;          // 是否在输入法组合中

    public:
        ImTextInputEvent(const std::string& text, bool isComposing = false)
            : ImEvent(ImEventType::TextInput), m_text(text), m_isComposing(isComposing),
            m_modifiers(ImModifierKeys::GetCurrent())
        {
        }

        // 获取输入的文本
        const std::string& GetText() const { return m_text; }

        // 获取文本长度
        size_t GetLength() const { return m_text.length(); }

        // 是否为空文本
        bool IsEmpty() const { return m_text.empty(); }

        // 是否在输入法组合中
        bool IsComposing() const { return m_isComposing; }

        // 获取修饰键状态
        ImModifierKeys GetModifiers() const { return m_modifiers; }

        // 设置修饰键
        void SetModifiers(const ImModifierKeys& mods) { m_modifiers = mods; }

        // 设置组合状态
        void SetComposing(bool composing) { m_isComposing = composing; }

        // 文本操作辅助方法
        void AppendText(const std::string& additionalText) { m_text += additionalText; }
        void ClearText() { m_text.clear(); }

        virtual ImTextInputEvent* Clone() const override
        {
            return new ImTextInputEvent(*this);
        }
    };

}