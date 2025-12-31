// ImWindowManager.h
#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include "ImWindow.h"
#include <imgui.h>

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
        // 模态窗口管理
        std::vector<ImWindow*> m_modalStack; // 模态窗口栈
        bool m_hasActiveModal = false;
    public:
        ImWindowManager();
        ~ImWindowManager();

        // 创建新窗口
        ImWindow* CreateImWindow(const std::string& title, const ImVec2& size, const ImVec2& pos);

        ImWindow* CreatePopupWindow(const ImVec2& size, const ImVec2& pos,ImWidget* RootWidget,bool ControlRootWidget);
        
        // 创建模态窗口
        ImWindow* CreateModalWindow(const std::string& title, const ImVec2& size, const ImVec2& pos, ImWidget* rootWidget = nullptr, bool controlRootWidget = false);
        
        // 关闭窗口
        void CloseWindow(ImWindow* window);

        // 设置主窗口（特殊的全屏窗口）
        void SetMainWindow(ImWindow* window);
        ImWindow* GetMainWindow() const { return m_mainWindow; }

        // 获取活动窗口
        ImWindow* GetActiveWindow() const { return m_activeWindow; }

        // 设置活动窗口
        void SetActiveWindow(ImWindow* window);

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
    private:
        void PushModalWindow(ImWindow* window);
        void PopModalWindow(ImWindow* window);
        ImWindow* GetTopModalWindow() const;
        bool HasActiveModal() const { return m_hasActiveModal; }



        void UpdateModalState();
    };
}