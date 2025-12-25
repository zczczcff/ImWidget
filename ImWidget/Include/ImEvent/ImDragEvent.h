#pragma once
#include "ImEvent/ImEvent.h"
#include <memory>
#include "ImEvent/ImDragObject.h"

namespace ImGuiWidget
{
    class ImDragEvent : public ImEvent {
    private:
        //void* m_dragResource;
        static ImDragObject* GlobalDragObject;
        ImVec2 m_dragPosition;
        ImVec2 m_dragDelta;
        //std::string m_dragType;
        ImModifierKeys m_mods;
    public:
        ImDragEvent(ImEventType type, void* resource = nullptr,
            const ImVec2& position = ImVec2(0, 0))
            : ImEvent(type), 
            m_dragPosition(position){}

        // 获取拖拽数据
        ImDragObject* GetDragHandle() const { return GlobalDragObject; }
        ImVec2 GetPosition() const { return m_dragPosition; }
        ImVec2 GetDelta() const { return m_dragDelta; }

        // 设置拖拽数据
        void SetDragData(ImDragObject* resource) {
            GlobalDragObject = resource;
        }
        void SetPosition(const ImVec2& pos) { m_dragPosition = pos; }
        void SetDelta(const ImVec2& delta) { m_dragDelta = delta; }
        void SetModifiers(const ImModifierKeys& mods) { m_mods = mods; }

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