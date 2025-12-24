#pragma once

#include <vector>
#include <memory>
#include "ImEvent/ImEventType.h"
#include "ImEvent/ImEvent.h"


namespace ImGuiWidget
{
    class ImEventSystem {
    private:
        class ImWidget* m_rootWidget;
        std::vector<std::unique_ptr<ImEvent>> m_eventQueue;

        // 状态跟踪
        ImWidget* m_hoveredWidget = nullptr;
        ImWidget* m_focusedWidget = nullptr;
        ImWidget* m_dragSourceWidget = nullptr;
        ImVec2 m_lastMousePos;
        double m_lastClickTime[5] = { 0 };  // 每个按钮的上次点击时间
        ImVec2 m_lastClickPos[5];         // 每个按钮的上次点击位置

        // 双击检测参数
        const double DOUBLE_CLICK_TIME = 0.3;  // 300ms
        const float DOUBLE_CLICK_DISTANCE = 5.0f;  // 5像素

    public:
        ImEventSystem(ImWidget* root);

        // 主处理入口
        void ProcessEvents();

    private:
        // 从ImGui收集事件
        void CollectEventsFromImGui();

        void CollectMouseEvents(ImGuiIO& io);

        void HandleMouseButtonEvent(ImEventType type, int button, const ImVec2& pos);

        int CalculateClickCount(int button, const ImVec2& pos);

        void CollectKeyboardEvents(ImGuiIO& io);

        void CollectFocusEvents(ImGuiIO& io);

        void CollectDragEvents(ImGuiIO& io);

        // 事件分发
        void DispatchEvents();

        ImWidget* FindEventTarget(ImEvent* event);

        ImVec2 CalculateLocalPosition(const ImVec2& globalPos, ImWidget* widget);

        void DispatchEventThroughHierarchy(ImEvent* event, ImWidget* target);

        // HitTest实现（与之前相同）
        ImWidget* HitTest(ImWidget* widget, const ImVec2& point);

        // 工具函数
        static bool ImVec2Equals(const ImVec2& a, const ImVec2& b, float epsilon = 0.001f);

        static float ImVec2Distance(const ImVec2& a, const ImVec2& b);
    };

}