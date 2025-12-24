#pragma once
#include "ImEvent/ImEvent.h"
namespace ImGuiWidget
{
    class ImFocusEvent : public ImEvent {
    private:
        ImWidget* m_relatedTarget;  // 对于FocusIn，是失去焦点的控件；对于FocusOut，是获得焦点的控件

    public:
        ImFocusEvent(ImEventType type, ImWidget* relatedTarget = nullptr)
            : ImEvent(type, false, false), m_relatedTarget(relatedTarget) {}

        ImWidget* GetRelatedTarget() const { return m_relatedTarget; }

        virtual ImFocusEvent* Clone() const override {
            return new ImFocusEvent(*this);
        }
    };

    class ImFocusInEvent : public ImFocusEvent {
    public:
        ImFocusInEvent(ImWidget* relatedTarget = nullptr)
            : ImFocusEvent(ImEventType::FocusIn, relatedTarget) {}

        virtual ImFocusInEvent* Clone() const override {
            return new ImFocusInEvent(*this);
        }
    };

    class ImFocusOutEvent : public ImFocusEvent {
    public:
        ImFocusOutEvent(ImWidget* relatedTarget = nullptr)
            : ImFocusEvent(ImEventType::FocusOut, relatedTarget) {}

        virtual ImFocusOutEvent* Clone() const override {
            return new ImFocusOutEvent(*this);
        }
    };
}