// ImWidgetControlBlock.h
#pragma once

namespace ImGuiWidget
{
    class ImWidget; // 前向声明

    // 控制块类，管理控件弱引用的生命周期
    class ImWidgetControlBlock
    {
    private:
        ImWidget* m_widgetPtr;    // 指向控件的原始指针
        bool m_valid;             // 控件是否有效
        int m_refCount;           // 弱引用计数

    public:
        ImWidgetControlBlock(ImWidget* widget)
            : m_widgetPtr(widget), m_valid(true), m_refCount(1)
        {
        }

        ~ImWidgetControlBlock()
        {
            // 确保在销毁时控件指针已经被清空
            m_widgetPtr = nullptr;
            m_valid = false;
        }

        // 检查控件是否有效
        bool IsValid() const { return m_valid && m_widgetPtr != nullptr; }

        // 获取控件指针（仅在有效时）
        ImWidget* GetWidget() const { return IsValid() ? m_widgetPtr : nullptr; }

        // 标记为无效（控件析构时调用）
        void Invalidate()
        {
            m_valid = false;
            m_widgetPtr = nullptr;
        }

        // 引用计数管理
        void AddRef() { m_refCount++; }
        void Release()
        {
            m_refCount--;
            if (m_refCount <= 0)
            {
                delete this;
            }
        }

        int GetRefCount() const { return m_refCount; }
    };
}