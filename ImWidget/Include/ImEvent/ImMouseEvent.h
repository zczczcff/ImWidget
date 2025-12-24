#pragma once
#include "ImEvent/ImEvent.h"
namespace ImGuiWidget
{
    // 鼠标按钮枚举
    enum class ImMouseButton
    {
        Left = 0,
        Right = 1,
        Middle = 2,
        Extra1 = 3,
        Extra2 = 4
    };

    // 鼠标事件基类
    class ImMouseEvent : public ImEvent
    {
    protected:
        ImVec2 m_position;      // 相对于窗口的位置
        ImVec2 m_localPosition; // 相对于目标控件的位置
        ImVec2 m_delta;         // 移动距离
        ImMouseButton m_button;
        int m_clickCount;
        ImModifierKeys m_modifiers;  // 新增：修饰键状态

    public:
        ImMouseEvent(ImEventType type, ImMouseButton button = ImMouseButton::Left,
            int clickCount = 1, bool bubbles = true);

        // 位置相关
        ImVec2 GetPosition() const { return m_position; }
        ImVec2 GetLocalPosition() const { return m_localPosition; }
        ImVec2 GetDelta() const { return m_delta; }
        ImMouseButton GetButton() const { return m_button; }
        int GetClickCount() const { return m_clickCount; }
        ImModifierKeys GetModifiers() const { return m_modifiers; }

        // 设置位置（事件系统内部使用）
        void SetPosition(const ImVec2& pos) { m_position = pos; }
        void SetLocalPosition(const ImVec2& pos) { m_localPosition = pos; }
        void SetDelta(const ImVec2& delta) { m_delta = delta; }
        void SetModifiers(const ImModifierKeys& mods) { m_modifiers = mods; }

        // 坐标转换
        ImVec2 GetPositionRelativeTo(ImWidget* widget) const;

        virtual ImMouseEvent* Clone() const override;
    };

    // 具体的鼠标事件类型
    class ImMouseDownEvent : public ImMouseEvent {
    public:
        ImMouseDownEvent(ImMouseButton button, int clickCount = 1)
            : ImMouseEvent(ImEventType::MouseDown, button, clickCount) {}

        virtual ImMouseDownEvent* Clone() const override {
            return new ImMouseDownEvent(*this);
        }
    };

    class ImMouseUpEvent : public ImMouseEvent {
    public:
        ImMouseUpEvent(ImMouseButton button, int clickCount = 1)
            : ImMouseEvent(ImEventType::MouseUp, button, clickCount) {}

        virtual ImMouseUpEvent* Clone() const override {
            return new ImMouseUpEvent(*this);
        }
    };

    class ImMouseMoveEvent : public ImMouseEvent {
    public:
        ImMouseMoveEvent() : ImMouseEvent(ImEventType::MouseMove) {}

        virtual ImMouseMoveEvent* Clone() const override {
            return new ImMouseMoveEvent(*this);
        }
    };

    class ImMouseWheelEvent : public ImMouseEvent {
    private:
        ImVec2 m_wheelDelta;

    public:
        ImMouseWheelEvent(const ImVec2& wheelDelta)
            : ImMouseEvent(ImEventType::MouseWheel), m_wheelDelta(wheelDelta) {}

        ImVec2 GetWheelDelta() const { return m_wheelDelta; }
        float GetHorizontalDelta() const { return m_wheelDelta.x; }
        float GetVerticalDelta() const { return m_wheelDelta.y; }

        virtual ImMouseWheelEvent* Clone() const override {
            return new ImMouseWheelEvent(*this);
        }
    };

    class ImMouseEnterEvent : public ImMouseEvent {
    public:
        ImMouseEnterEvent() : ImMouseEvent(ImEventType::MouseEnter, ImMouseButton::Left, 1, false) {}

        virtual ImMouseEnterEvent* Clone() const override {
            return new ImMouseEnterEvent(*this);
        }
    };

    class ImMouseLeaveEvent : public ImMouseEvent {
    public:
        ImMouseLeaveEvent() : ImMouseEvent(ImEventType::MouseLeave, ImMouseButton::Left, 1, false) {}

        virtual ImMouseLeaveEvent* Clone() const override {
            return new ImMouseLeaveEvent(*this);
        }
    };

    class ImMouseClickEvent : public ImMouseEvent {
    public:
        ImMouseClickEvent(ImMouseButton button, int clickCount = 1)
            : ImMouseEvent(ImEventType::MouseClick, button, clickCount) {}

        virtual ImMouseClickEvent* Clone() const override {
            return new ImMouseClickEvent(*this);
        }
    };

    class ImMouseDoubleClickEvent : public ImMouseEvent {
    public:
        ImMouseDoubleClickEvent(ImMouseButton button)
            : ImMouseEvent(ImEventType::MouseDoubleClick, button, 2) {}

        virtual ImMouseDoubleClickEvent* Clone() const override {
            return new ImMouseDoubleClickEvent(*this);
        }
    };

    //// 新增：鼠标悬停事件
    //class ImMouseHoverEvent : public ImMouseEvent {
    //private:
    //    float m_hoverTime;  // 悬停时间（秒）

    //public:
    //    ImMouseHoverEvent(float hoverTime = 0.0f)
    //        : ImMouseEvent(ImEventType::MouseHover, ImMouseButton::Left, 1, false), m_hoverTime(hoverTime) {}

    //    float GetHoverTime() const { return m_hoverTime; }

    //    virtual ImMouseHoverEvent* Clone() const override {
    //        return new ImMouseHoverEvent(*this);
    //    }
    //};

    //// 新增：滚轮按下事件
    //class ImMouseWheelClickEvent : public ImMouseEvent {
    //public:
    //    ImMouseWheelClickEvent()
    //        : ImMouseEvent(ImEventType::MouseWheelClick, ImMouseButton::Middle, 1) {}

    //    virtual ImMouseWheelClickEvent* Clone() const override {
    //        return new ImMouseWheelClickEvent(*this);
    //    }
    //};

    //// 新增：滚轮倾斜事件
    //class ImMouseWheelTiltEvent : public ImMouseEvent {
    //private:
    //    ImVec2 m_tiltDelta;

    //public:
    //    ImMouseWheelTiltEvent(const ImVec2& tiltDelta)
    //        : ImMouseEvent(ImEventType::MouseWheelTilt), m_tiltDelta(tiltDelta) {}

    //    ImVec2 GetTiltDelta() const { return m_tiltDelta; }

    //    virtual ImMouseWheelTiltEvent* Clone() const override {
    //        return new ImMouseWheelTiltEvent(*this);
    //    }
    //};
}