#pragma once
#include "ImWidget.h"
#include "imgui_internal.h"
#include "ImEvent/ImMouseEvent.h"
#include "ImEvent/ImDragEvent.h"
#include "ImEvent/ImFocusEvent.h"
#include "ImEvent/ImKeyEvent.h"

namespace ImGuiWidget
{
    class ImDragHandle :public ImDragObject
    {
    public:
        std::string HandleType;
        ImVec2 FromPos;
        
        virtual ~ImDragHandle() {}
    };

    class ImUserWidget : public ImWidget
    {
    protected:
        ImWidget* m_RootWidget;

        // 拖拽状态变量
        bool m_IsDragging = false;
        bool bDragFinishedLastTick = false;
        bool bAllowDrag = false;     // 允许被拖拽
        bool bAllowDragOn = false;   // 允许拖拽放置此处

        // ================ 事件系统状态变量 ================ //
        //bool m_IsHovered = false;
        //bool m_IsPressed = false;

        //// ================ 样式属性 ================ //
        //bool m_EnableBackground = true;
        //ImU32 m_BackgroundColor = IM_COL32(50, 50, 50, 255);
        //bool m_EnableBorder = true;
        //ImU32 m_BorderColor = IM_COL32(100, 100, 100, 255);
        //float m_BorderThickness = 1.0f;

    protected:
        // ================ 拖拽相关虚函数 ================ //
        // ================ 新增事件处理虚函数（参考UE4） ================ //
        virtual void OnMouseEnter(ImMouseEnterEvent& e) {}
        virtual void OnMouseLeave(ImMouseLeaveEvent& e) {}
        virtual void OnMouseMove(ImMouseMoveEvent& e) {}
        virtual void OnMouseDown(ImMouseDownEvent& e) {}
        virtual void OnMouseUp(ImMouseUpEvent& e) {}
        virtual void OnMouseClick(ImMouseClickEvent& e) {}
        virtual void OnMouseDoubleClick(ImMouseDoubleClickEvent& e) {}
        virtual void OnMouseWheel(ImMouseWheelEvent& e) {}

        virtual void OnKeyDown(ImKeyDownEvent& e) {}
        virtual void OnKeyUp(ImKeyUpEvent& e) {}
        virtual void OnKeyPress(ImKeyPressEvent& e) {}

        virtual void OnFocusReceived(ImFocusInEvent& e) {}
        virtual void OnFocusLost(ImFocusOutEvent& e) {}

        virtual void OnDragStart(ImDragStartEvent& e) {}
        virtual void OnDragUpdate(ImDragUpdateEvent& e) {}
        virtual void OnDragEnd(ImDragEndEvent& e) {}
        virtual void OnDragEnter(ImDragEvent& e) {}
        virtual void OnDragLeave(ImDragEvent& e) {}
        virtual void OnDrop(ImDragEvent& e) {}

        // ================ 渲染相关虚函数 ================ //
        virtual void PreRender() {}
        virtual void PostRender() {}

        // ================ 事件处理内部实现 ================ //
        virtual void HandleEventInternal(ImEvent* event) override
        {
            if (event->IsHandled()) return;

            switch (event->GetType())
            {
            case ImEventType::MouseEnter:
                HandleMouseEnter(event->As<ImMouseEnterEvent>());
                break;

            case ImEventType::MouseLeave:
                HandleMouseLeave(event->As<ImMouseLeaveEvent>());
                break;

            case ImEventType::MouseMove:
                HandleMouseMove(event->As<ImMouseMoveEvent>());
                break;

            case ImEventType::MouseDown:
                HandleMouseDown(event->As<ImMouseDownEvent>());
                break;

            case ImEventType::MouseUp:
                HandleMouseUp(event->As<ImMouseUpEvent>());
                break;

            case ImEventType::MouseClick:
                HandleMouseClick(event->As<ImMouseClickEvent>());
                break;

            case ImEventType::MouseDoubleClick:
                HandleMouseDoubleClick(event->As<ImMouseDoubleClickEvent>());
                break;

            case ImEventType::MouseWheel:
                HandleMouseWheel(event->As<ImMouseWheelEvent>());
                break;

            case ImEventType::KeyDown:
                HandleKeyDown(event->As<ImKeyDownEvent>());
                break;

            case ImEventType::KeyUp:
                HandleKeyUp(event->As<ImKeyUpEvent>());
                break;

            case ImEventType::KeyPress:
                HandleKeyPress(event->As<ImKeyPressEvent>());
                break;

            case ImEventType::FocusIn:
                HandleFocusIn(event->As<ImFocusInEvent>());
                break;

            case ImEventType::FocusOut:
                HandleFocusOut(event->As<ImFocusOutEvent>());
                break;

            case ImEventType::DragStart:
                HandleDragStart(event->As<ImDragStartEvent>());
                break;

            case ImEventType::DragUpdate:
                HandleDragUpdate(event->As<ImDragUpdateEvent>());
                break;

            case ImEventType::DragEnd:
                HandleDragEnd(event->As<ImDragEndEvent>());
                break;

            case ImEventType::DragEnter:
                HandleDragEnter(event->As<ImDragEvent>());
                break;

            case ImEventType::DragLeave:
                HandleDragLeave(event->As<ImDragEvent>());
                break;

            case ImEventType::Drop:
                HandleDrop(event->As<ImDragEvent>());
                break;

            default:
                break;
            }
        }

    private:
        // ================ 具体事件处理方法 ================ //
        void HandleMouseEnter(ImMouseEnterEvent* e)
        {
            OnMouseEnter(*e);
        }

        void HandleMouseLeave(ImMouseLeaveEvent* e)
        {
            OnMouseLeave(*e);
        }

        void HandleMouseMove(ImMouseMoveEvent* e)
        {
            OnMouseMove(*e);
        }

        void HandleMouseDown(ImMouseDownEvent* e)
        {
            OnMouseDown(*e);
        }

        void HandleMouseUp(ImMouseUpEvent* e)
        {
            OnMouseUp(*e);
        }

        void HandleMouseClick(ImMouseClickEvent* e)
        {
            OnMouseClick(*e);
        }

        void HandleMouseDoubleClick(ImMouseDoubleClickEvent* e)
        {
            OnMouseDoubleClick(*e);
        }

        void HandleMouseWheel(ImMouseWheelEvent* e)
        {
            OnMouseWheel(*e);
        }

        void HandleKeyDown(ImKeyDownEvent* e)
        {
            OnKeyDown(*e);
        }

        void HandleKeyUp(ImKeyUpEvent* e)
        {
            OnKeyUp(*e);
        }

        void HandleKeyPress(ImKeyPressEvent* e)
        {
            OnKeyPress(*e);
        }

        void HandleFocusIn(ImFocusInEvent* e)
        {
            OnFocusReceived(*e);
        }

        void HandleFocusOut(ImFocusOutEvent* e)
        {
            OnFocusLost(*e);
        }

        void HandleDragStart(ImDragStartEvent* e)
        {
            if (bAllowDrag)
            {
                OnDragStart(*e);
            }
        }

        void HandleDragUpdate(ImDragUpdateEvent* e)
        {
            if (m_IsDragging)
            {
                OnDragUpdate(*e);
                e->StopPropagation();
            }
        }

        void HandleDragEnd(ImDragEndEvent* e)
        {
            if (m_IsDragging)
            {
                m_IsDragging = false;
                OnDragEnd(*e);
                e->StopPropagation();
            }
        }

        void HandleDragEnter(ImDragEvent* e)
        {
            if (bAllowDragOn)
            {
                OnDragEnter(*e);
            }
        }

        void HandleDragLeave(ImDragEvent* e)
        {
            if (bAllowDragOn)
            {
                OnDragLeave(*e);
            }
        }

        void HandleDrop(ImDragEvent* e)
        {
            if (bAllowDragOn)
            {
                OnDrop(*e);
            }
        }

    public:
        ImUserWidget(const std::string& WidgetName) :
            ImWidget(WidgetName),
            m_RootWidget(nullptr)
        {
            //SetFocusable(true);
        }

        ImUserWidget(const std::string& WidgetName, ImWidget* RootWidget) :
            ImWidget(WidgetName)
        {
            SetRootWidget(RootWidget);
            //SetFocusable(true);
        }

        virtual ~ImUserWidget()
        {
        }

        // ================ 基础方法重写 ================ //
        virtual void SetPosition(ImVec2 Pos) override
        {
            Position = Pos;
            if (m_RootWidget)
            {
                m_RootWidget->SetPosition(Pos);
            }
        }

        virtual void SetSize(ImVec2 size) override
        {
            Size = size;
            if (m_RootWidget)
            {
                m_RootWidget->SetSize(size);
            }
        }

        virtual ImVec2 GetMinSize() override
        {
            if (m_RootWidget)
            {
                return m_RootWidget->GetMinSize();
            }
            else
            {
                return ImVec2(0.f, 0.f);
            }
        }

        virtual ImWidget* ChildHitTest(ImVec2 Pos) override
        {
            if (m_RootWidget)
            {
                return m_RootWidget->ChildHitTest(Pos);
            }
            else
            {
                ImRect HitRect(Position, Position + Size);
                return HitRect.Contains(Pos) ? this : nullptr;
            }
        }

        // ================ 拖拽设置 ================ //
        void SetAllowDrag(bool NewSetting) { bAllowDrag = NewSetting; }
        void SetAllowDragOn(bool NewSetting) { bAllowDragOn = NewSetting; }

        // ================ 状态查询方法 ================ //
        bool IsDragging() const { return m_IsDragging; }

        // ================ 渲染方法 ================ //
        virtual void Render() override
        {
            PreRender();


            // 渲染根控件
            if (m_RootWidget)
            {
                m_RootWidget->Render();
            }


            PostRender();
        }

        // ================ 根控件管理 ================ //
        void SetRootWidget(ImWidget* RootWidget, bool DeleteOldRoot = true)
        {
            if (m_RootWidget && DeleteOldRoot)
            {
                delete m_RootWidget;
            }
            else if (m_RootWidget)
            {
                m_RootWidget->SetParents(nullptr);
            }

            m_RootWidget = RootWidget;
            if (m_RootWidget)
            {
                m_RootWidget->SetPosition(Position);
                m_RootWidget->SetSize(Size);
                m_RootWidget->SetParents(this);
            }
        }

        ImWidget* GetRootWidget() { return m_RootWidget; }

        // ================ 属性系统 ================ //
        virtual std::unordered_set<PropertyInfo, PropertyInfo::Hasher> GetProperties() override
        {
            auto props = ImWidget::GetProperties();

            // 拖拽行为属性
            props.insert({
                "AllowDrag", PropertyType::Bool, "DragDrop",
                [this](void* v) { bAllowDrag = *static_cast<bool*>(v); },
                [this]() -> void* { return &bAllowDrag; }
                });

            props.insert({
                "AllowDragOn", PropertyType::Bool, "DragDrop",
                [this](void* v) { bAllowDragOn = *static_cast<bool*>(v); },
                [this]() -> void* { return &bAllowDragOn; }
                });


            return props;
        }

        virtual std::string GetRegisterTypeName() override { return "ImUserWidget"; }

        virtual ImWidget* CopyWidget() override
        {
            return new ImUserWidget(*this);//错误的
        }
    };
}