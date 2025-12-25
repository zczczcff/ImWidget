#pragma once
#include "ImEvent/ImEvent.h"

namespace ImGuiWidget
{
    class ImFocusEvent : public ImEvent {
    private:
        ImWidget* m_relatedTarget;  // 相关目标控件
        ImFocusReason m_reason;     // 焦点变化原因

    public:
        ImFocusEvent(ImEventType type, ImWidget* relatedTarget = nullptr,
            ImFocusReason reason = ImFocusReason::Unknown)
            : ImEvent(type, true, false),
            m_relatedTarget(relatedTarget),
            m_reason(reason) {}

        ImWidget* GetRelatedTarget() const { return m_relatedTarget; }
        ImFocusReason GetReason() const { return m_reason; }

        virtual ImFocusEvent* Clone() const override {
            return new ImFocusEvent(*this);
        }
    };

    class ImFocusInEvent : public ImFocusEvent {
    public:
        ImFocusInEvent(ImWidget* relatedTarget = nullptr,
            ImFocusReason reason = ImFocusReason::Unknown)
            : ImFocusEvent(ImEventType::FocusIn, relatedTarget, reason) {}

        virtual ImFocusInEvent* Clone() const override {
            return new ImFocusInEvent(*this);
        }
    };

    class ImFocusOutEvent : public ImFocusEvent {
    public:
        ImFocusOutEvent(ImWidget* relatedTarget = nullptr,
            ImFocusReason reason = ImFocusReason::Unknown)
            : ImFocusEvent(ImEventType::FocusOut, relatedTarget, reason) {}

        virtual ImFocusOutEvent* Clone() const override {
            return new ImFocusOutEvent(*this);
        }
    };

    // 焦点请求事件
    class ImFocusRequestEvent : public ImEvent {
    private:
        ImWidget* m_requestedWidget;
        ImFocusReason m_reason;

    public:
        ImFocusRequestEvent(ImWidget* widget, ImFocusReason reason = ImFocusReason::User)
            : ImEvent(ImEventType::FocusRequest, false, false),
            m_requestedWidget(widget),
            m_reason(reason) {}

        ImWidget* GetRequestedWidget() const { return m_requestedWidget; }
        ImFocusReason GetReason() const { return m_reason; }

        virtual ImFocusRequestEvent* Clone() const override {
            return new ImFocusRequestEvent(*this);
        }
    };
}