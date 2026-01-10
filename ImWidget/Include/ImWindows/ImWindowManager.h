// ImWindowManager.h
#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include "ImWindow.h"
#include <imgui.h>
#include <imgui_internal.h>

namespace ImGuiWidget
{
    class ImWindowManager
    {
    private:
        std::vector<ImWindow*> m_windows;
        ImWindow* m_activeWindow = nullptr;
        ImWindow* m_mainWindow = nullptr;
        int m_nextWindowId = 1;
        class ImEventSystem* m_EventSystem = nullptr;

        // 弹出窗口管理
        std::vector<ImWindow*> m_popupStack; // 弹出窗口栈，用于管理多级菜单

        class ImWidget* ToolTipWidget = nullptr;
        ImVec2 ToolTipPos;
        ImVec2 ToolTipSize;
        float ToolTipPadding = 4.f;
    public:
        ImWindowManager();
        ~ImWindowManager();

        // 创建新窗口
        ImWindow* CreateImWindow(const std::string& title, const ImVec2& size, const ImVec2& pos);

        // 创建弹出窗口（支持父子关系）
        ImWindow* CreatePopupWindow(const ImVec2& size, const ImVec2& pos, ImWidget* RootWidget,
            bool ControlRootWidget, ImWindow* parentPopup = nullptr);

        // 创建子菜单弹出窗口
        ImWindow* CreateSubMenuPopup(const ImVec2& size, const ImVec2& pos, ImWidget* RootWidget,
            bool ControlRootWidget, ImWindow* parentPopup);
        
        // 关闭窗口
        void CloseWindow(ImWindow* window);

        void OpenWindow(ImWindow* window);

        void CloseAllPopups();

        // 设置主窗口（特殊的全屏窗口）
        void SetMainWindow(ImWindow* window);
        ImWindow* GetMainWindow() const { return m_mainWindow; }

        // 获取活动窗口
        ImWindow* GetActiveWindow() const { return m_activeWindow; }

        // 设置活动窗口
        void SetActiveWindow(ImWindow* window);

        void SetWindowInactive(ImWindow* window);

        void SetPopupWindowRect(ImWindow* window, const ImVec2& Min, const ImVec2& Max);

        // 获取所有窗口
        const std::vector<ImWindow*>& GetWindows() const { return m_windows; }

        // 根据ID查找窗口
        ImWindow* FindWindowById(const std::string& id);

        // 渲染所有窗口
        void Render();

        // 处理窗口事件
        void ProcessEvents();

        // 窗口排序（Z-order管理）
        void BringWindowToFront(ImWindow* window);
        void SendWindowToBack(ImWindow* window);

        void CleanupClosedWindows();

        void UpdateActiveWindowFromInput();

        // 获取窗口数量
        int GetWindowCount() const { return static_cast<int>(m_windows.size()); }

        ImWindow* WindowHitTest(const ImVec2& Pos);
        void PushPopupWindow(ImWindow* popup);
        void PopPopupWindow(ImWindow* popup);
        ImWindow* GetTopPopupWindow() const;
    private:

        // 弹出窗口管理
        bool HasOpenPopups() const { return !m_popupStack.empty(); }

        // 检查点是否在任何弹出窗口内
        bool IsPointInAnyPopup(const ImVec2& point) const;

        // 更新弹出窗口状态
        void UpdatePopupState();

        // 递归查找窗口
        ImWindow* FindWindowRecursive(ImWindow* start, const std::string& id) const;

        //自适应弹窗位置计算
        ImRect CalculateAdaptivePopupPosition(
            const ImRect& OriginalRect,
            bool HasParent = false,
            const ImRect& ParentsRect = ImRect(-1, -1, -1, -1)
        );
    public:
        ImEventSystem* GetEventSystem() { return m_EventSystem; }
    };
}