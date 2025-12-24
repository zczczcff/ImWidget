#pragma once
#include "ImEvent/ImEvent.h"
#include <memory>

namespace ImGuiWidget
{
    class ImDragEvent : public ImEvent {
    private:
        void* m_dragResource;
        ImVec2 m_dragPosition;
        ImVec2 m_dragDelta;
        std::string m_dragType;
        ImModifierKeys m_mods;
    public:
        ImDragEvent(ImEventType type, void* resource = nullptr,
            const ImVec2& position = ImVec2(0, 0),
            const std::string& dragType = "")
            : ImEvent(type), m_dragResource(resource),
            m_dragPosition(position), m_dragType(dragType) {}

        // 获取拖拽数据
        void* GetDragHandle() const { return m_dragResource; }
        ImVec2 GetPosition() const { return m_dragPosition; }
        ImVec2 GetDelta() const { return m_dragDelta; }
        const std::string& GetDragType() const { return m_dragType; }

        // 设置拖拽数据
        void SetDragData(void* resource, const std::string& type = "") {
            m_dragResource = resource;
            m_dragType = type;
        }
        void SetPosition(const ImVec2& pos) { m_dragPosition = pos; }
        void SetDelta(const ImVec2& delta) { m_dragDelta = delta; }
        void SetModifiers(const ImModifierKeys& mods) { m_mods = mods; }
        template<typename T>
        T* GetDragData() const {
            return static_cast<T*>(m_dragResource);
        }

        virtual ImDragEvent* Clone() const override {
            return new ImDragEvent(*this);
        }
    };

    // 专门的拖拽开始事件
    class ImDragStartEvent : public ImDragEvent {
    public:
        ImDragStartEvent(void* resource = nullptr, const ImVec2& position = ImVec2(0, 0))
            : ImDragEvent(ImEventType::MouseDragStart, resource, position) {}

        virtual ImDragStartEvent* Clone() const override {
            return new ImDragStartEvent(*this);
        }
    };

    // 拖拽进行事件
    class ImDragUpdateEvent : public ImDragEvent {
    public:
        ImDragUpdateEvent(void* resource = nullptr, const ImVec2& position = ImVec2(0, 0))
            : ImDragEvent(ImEventType::MouseDrag, resource, position) {}

        virtual ImDragUpdateEvent* Clone() const override {
            return new ImDragUpdateEvent(*this);
        }
    };

    // 拖拽结束事件
    class ImDragEndEvent : public ImDragEvent {
    public:
        ImDragEndEvent(void* resource = nullptr, const ImVec2& position = ImVec2(0, 0))
            : ImDragEvent(ImEventType::MouseDragEnd, resource, position) {}

        virtual ImDragEndEvent* Clone() const override {
            return new ImDragEndEvent(*this);
        }
    };
}