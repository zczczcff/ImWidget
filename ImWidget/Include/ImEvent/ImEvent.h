#pragma once
#include "ImEvent/ImEventType.h"

namespace ImGuiWidget
{
    // 事件基类
    class ImEvent
    {
    protected:
        ImEventType m_type;
        ImEventPhase m_phase;
        class ImWidget* m_target;
        class ImWidget* m_currentTarget;
        bool m_handled;
        bool m_bubbles;
        bool m_cancelable;
        double m_timestamp;

    public:
        ImEvent(ImEventType type, bool bubbles = true, bool cancelable = true)
            : m_type(type), m_phase(ImEventPhase::Target), m_target(nullptr),
            m_currentTarget(nullptr), m_handled(false), m_bubbles(bubbles),
            m_cancelable(cancelable), m_timestamp(ImGui::GetTime()) {}

        virtual ~ImEvent() = default;

        // 基础属性访问
        ImEventType GetType() const { return m_type; }
        ImEventPhase GetPhase() const { return m_phase; }
        ImWidget* GetTarget() const { return m_target; }
        ImWidget* GetCurrentTarget() const { return m_currentTarget; }
        bool IsHandled() const { return m_handled; }
        bool Bubbles() const { return m_bubbles; }
        bool IsCancelable() const { return m_cancelable; }
        double GetTimestamp() const { return m_timestamp; }

        // 设置内部状态（事件系统使用）
        void SetPhase(ImEventPhase phase) { m_phase = phase; }
        void SetTarget(ImWidget* target) { m_target = target; }
        void SetCurrentTarget(ImWidget* target) { m_currentTarget = target; }

        // 事件控制
        void StopPropagation() { m_handled = true; }
        virtual ImEvent* Clone() const = 0;

        // 类型检查
        template<typename T>
        bool Is() const { return dynamic_cast<const T*>(this) != nullptr; }

        template<typename T>
        T* As() { return dynamic_cast<T*>(this); }
    };
}