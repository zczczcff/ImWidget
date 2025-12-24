#pragma once
#include "ImEvent/ImEvent.h"
namespace ImGuiWidget
{
    class ImDragEvent : public ImEvent {
    private:
        class ImDragHandle* m_dragHandle;
        ImVec2 m_dragPosition;

    public:
        ImDragEvent(ImEventType type, ImDragHandle* handle = nullptr, const ImVec2& position = ImVec2(0, 0))
            : ImEvent(type), m_dragHandle(handle), m_dragPosition(position) {}

        ImDragHandle* GetDragHandle() const { return m_dragHandle; }
        ImVec2 GetPosition() const { return m_dragPosition; }

        template<typename T>
        T* GetDragData() const {
            if (!m_dragHandle) return nullptr;
            // 这里可以根据实际需求返回特定类型的数据
            return dynamic_cast<T*>(m_dragHandle);
        }

        virtual ImDragEvent* Clone() const override {
            return new ImDragEvent(*this);
        }
    };
}