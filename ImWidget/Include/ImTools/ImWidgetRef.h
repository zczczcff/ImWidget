// ImWidgetRef.h
#pragma once
#include "ImWidgetControlBlock.h"

namespace ImGuiWidget
{
    class ImWidgetRef
    {
        friend class ImWidget;
    private:
        ImWidgetControlBlock* m_controlBlock;  // 控制块指针
        void Invalidate()
        {
            if (m_controlBlock)
            {
                m_controlBlock->Invalidate();
            }
        }
        // 从控件指针创建（仅由控件构造函数调用）
        explicit ImWidgetRef(ImWidget* widget)
        {
            if (widget)
            {
                m_controlBlock = new ImWidgetControlBlock(widget);
            }
            else
            {
                m_controlBlock = nullptr;
            }
        }
    public:
        // 默认构造函数（空引用）
        ImWidgetRef() : m_controlBlock(nullptr) {}

        // 拷贝构造函数
        ImWidgetRef(const ImWidgetRef& other)
            : m_controlBlock(other.m_controlBlock)
        {
            if (m_controlBlock)
            {
                m_controlBlock->AddRef();
            }
        }

        // 移动构造函数
        ImWidgetRef(ImWidgetRef&& other) noexcept
            : m_controlBlock(other.m_controlBlock)
        {
            other.m_controlBlock = nullptr;
        }

        // 析构函数
        ~ImWidgetRef()
        {
            if (m_controlBlock)
            {
                m_controlBlock->Release();
            }
        }

        // 拷贝赋值运算符
        ImWidgetRef& operator=(const ImWidgetRef& other)
        {
            if (this != &other)
            {
                // 释放当前引用
                if (m_controlBlock)
                {
                    m_controlBlock->Release();
                }

                // 复制新引用
                m_controlBlock = other.m_controlBlock;
                if (m_controlBlock)
                {
                    m_controlBlock->AddRef();
                }
            }
            return *this;
        }

        // 移动赋值运算符
        ImWidgetRef& operator=(ImWidgetRef&& other) noexcept
        {
            if (this != &other)
            {
                if (m_controlBlock)
                {
                    m_controlBlock->Release();
                }

                m_controlBlock = other.m_controlBlock;
                other.m_controlBlock = nullptr;
            }
            return *this;
        }

        // 检查引用是否有效
        bool IsValid() const
        {
            return m_controlBlock && m_controlBlock->IsValid();
        }

        // 获取控件指针（如果有效）
        ImWidget* GetWidget() const
        {
            return m_controlBlock ? m_controlBlock->GetWidget() : nullptr;
        }

        // 显式bool转换（用于条件判断）
        explicit operator bool() const { return IsValid(); }

        // 比较运算符
        bool operator==(const ImWidgetRef& other) const
        {
            return GetWidget() == other.GetWidget();
        }

        bool operator!=(const ImWidgetRef& other) const
        {
            return !(*this == other);
        }

        // 与原始指针比较
        bool operator==(const ImWidget* widget) const
        {
            return GetWidget() == widget;
        }

        bool operator!=(const ImWidget* widget) const
        {
            return GetWidget() != widget;
        }

        // 访问运算符（类似智能指针）
        ImWidget* operator->() const
        {
            ImWidget* widget = GetWidget();
            assert(widget && "Attempt to access invalid ImWidgetRef");
            return widget;
        }

        ImWidget& operator*() const
        {
            ImWidget* widget = GetWidget();
            assert(widget && "Attempt to dereference invalid ImWidgetRef");
            return *widget;
        }

        // 重置为nullptr
        void Reset()
        {
            if (m_controlBlock)
            {
                m_controlBlock->Release();
                m_controlBlock = nullptr;
            }
        }

        // 获取引用计数（调试用）
        int GetRefCount() const
        {
            return m_controlBlock ? m_controlBlock->GetRefCount() : 0;
        }
    };
}