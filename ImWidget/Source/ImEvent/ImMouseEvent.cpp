#include "ImEvent/ImMouseEvent.h"
#include "ImWidget/ImWidget.h"

namespace ImGuiWidget
{
    ImMouseEvent::ImMouseEvent(ImEventType type, ImMouseButton button, int clickCount, bool bubbles)
        : ImEvent(type, bubbles), m_button(button), m_clickCount(clickCount),
        m_position(0, 0), m_localPosition(0, 0), m_delta(0, 0) {}

    // ×ø±ê×ª»»

    ImVec2 ImMouseEvent::GetPositionRelativeTo(ImWidget* widget) const {
        if (!widget) return m_position;
        return ImVec2(m_position.x - widget->GetPosition().x,
            m_position.y - widget->GetPosition().y);
    }

    ImMouseEvent* ImMouseEvent::Clone() const {
        auto clone = new ImMouseEvent(*this);
        return clone;
    }
}