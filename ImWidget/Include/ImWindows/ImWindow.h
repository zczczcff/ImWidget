// ImWindow.h
#pragma once
#include "ImEvent/ImEventSystem.h"
#include <string>
#include "ImTools/ImDelegate.h"

namespace ImGuiWidget
{
    class ImWindow
    {
        friend class ImWindowManager;
    private:
        // 窗口的根控件
        ImWidget* m_rootWidget = nullptr;
        class ImWindowManager* m_manager;
        // 每个窗口有自己的事件系统
        //std::unique_ptr<ImEventSystem> m_eventSystem;
        std::string m_windowId;
        std::string m_title;
        ImVec2 m_size;
        ImVec2 m_position;

        // 父子窗口关系
        ImWindow* m_parentWindow = nullptr;
        std::vector<ImWindow*> m_childWindows;
        // 弹出窗口特定属性
        bool bIsPopup = false;
        bool bIsSubMenu = false; // 是否为子菜单
        //ImVec2 m_popupPosition; // 弹出位置
    public:
        bool bIsOpen = true;
        bool bIsActive = false;
        bool bIsMovable = true;
        bool bIsResizable = true;
        bool bHasTitleBar = true;
        bool bHasBackground = true;
        bool bFillSysWindow = false;
        bool bCollapsible = true;
        bool bHaveScrollbar = false;
        bool bAllowBringToFrontOnFocus = false;
        ImU32 m_bgColor = IM_COL32(255, 255, 255, 255);
        ImU32 m_borderColor = IM_COL32(0, 0, 0, 255);
        float m_borderThickness = 1.0f;
        bool bAutoCloseWhenLostFocus = false;
        bool bAutoDeleteWhenLostFocus = false;

        bool m_JustOpened = false;
        bool ControlRootWidget = false;

        ImMulticastDelegate<> OnGetFocus;
        ImMulticastDelegate<> OnLoseFocus;
    private:
        virtual ~ImWindow();
        //void SetIsOpen(bool open);
        //void SetIsActive(bool active);
    public:
        ImWindow(const std::string& title, const ImVec2& size, const ImVec2& pos,const std::string& ID, ImWindowManager* m_manager);

        // 设置根控件
        void SetRootWidget(ImWidget* rootWidget, bool ControlNewRootWidget = false);

        // 获取根控件
        ImWidget* GetRootWidget() const { return m_rootWidget; }

        // 窗口属性设置
        void SetTitle(const std::string& title) { m_title = title; }
        const std::string& GetTitle() const { return m_title; }

        void SetSize(const ImVec2& size) { m_size = size; }
        const ImVec2& GetSize() const { return m_size; }

        void SetPosition(const ImVec2& pos) { m_position = pos; }
        const ImVec2& GetPosition() const { return m_position; }


        bool IsOpen() const { return bIsOpen; }
        void Close();
        void Open();

        void SetActive();
        void SetInactive();
        bool IsActive() const { return bIsActive; }

        void SetIsMovable(bool movable) { bIsMovable = movable; }
        bool IsMovable() const { return bIsMovable; }

        void SetIsResizable(bool resizable) { bIsResizable = resizable; }
        bool IsResizable() const { return bIsResizable; }

        void SetHasTitleBar(bool hasTitleBar) { bHasTitleBar = hasTitleBar; }
        bool HasTitleBar() const { return bHasTitleBar; }

        void SetHasBackground(bool hasBackground) { bHasBackground = hasBackground; }
        bool HasBackground() const { return bHasBackground; }

        void SetBackgroundColor(ImU32 color) { m_bgColor = color; }
        ImU32 GetBackgroundColor() const { return m_bgColor; }

        void SetBorderColor(ImU32 color) { m_borderColor = color; }
        ImU32 GetBorderColor() const { return m_borderColor; }

        void SetBorderThickness(float thickness) { m_borderThickness = thickness; }
        float GetBorderThickness() const { return m_borderThickness; }

        // 获取窗口ID
        const std::string& GetWindowId() const { return m_windowId; }

        // 渲染窗口
        void Render();

        // 处理事件
        //void ProcessEvents();

        // 检查点是否在窗口内（用于点击测试）
        virtual bool ContainsPoint(const ImVec2& point) const;

        // 获取事件系统

        // 弹出窗口相关方法
        void SetPopup(bool popup) { bIsPopup = popup; }
        bool IsPopup() const { return bIsPopup; }

        void SetSubMenu(bool subMenu) { bIsSubMenu = subMenu; }
        bool IsSubMenu() const { return bIsSubMenu; }

        //void SetPopupPosition(const ImVec2& pos) { m_popupPosition = pos; }
        //const ImVec2& GetPopupPosition() const { return m_popupPosition; }

        // 父子窗口管理
        void SetParentWindow(ImWindow* parent) { m_parentWindow = parent; }
        ImWindow* GetParentWindow() const { return m_parentWindow; }

        void AddChildWindow(ImWindow* child);
        void RemoveChildWindow(ImWindow* child);
        const std::vector<ImWindow*>& GetChildWindows() const { return m_childWindows; }

        bool HasChildWindows() const { return !m_childWindows.empty(); }
        void CloseAllChildren(); // 递归关闭所有子窗口

        // 检查是否为某个窗口的祖先
        bool IsAncestorOf(const ImWindow* window) const;

        // 获取窗口层级深度
        int GetHierarchyDepth() const;
    };
}