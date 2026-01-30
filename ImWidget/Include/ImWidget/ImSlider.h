#pragma once
#include "ImObject.h"  // 假设ImObject.h中包含了ROP系统的声明
#include "ImEvent/ImMouseEvent.h"
#include "ImEvent/ImDragEvent.h"

namespace ImGuiWidget
{
    // 滑块样式结构体 - 使用ROP系统
    struct ImSliderStyle : public ImObject
    {
        ImU32 background = IM_COL32(100, 100, 100, 255);
        ImU32 filled = IM_COL32(61, 133, 224, 255);
        ImU32 handle = IM_COL32(255, 255, 255, 255);
        ImU32 handle_border = IM_COL32(30, 30, 30, 255);
        ImU32 text = IM_COL32(255, 255, 255, 255);
        ImU32 handle_hovered = IM_COL32(220, 220, 220, 255);
        ImU32 handle_active = IM_COL32(200, 200, 200, 255);

        // 构造函数
        ImSliderStyle() = default;

        // 拷贝构造函数
        ImSliderStyle(const ImSliderStyle& other) = default;

        // 设置全部颜色的一键方法（可选）
        void SetAllColors(const ImU32& bg, const ImU32& fill, const ImU32& hand,
            const ImU32& border, const ImU32& txt,
            const ImU32& hover, const ImU32& active)
        {
            background = bg;
            filled = fill;
            handle = hand;
            handle_border = border;
            text = txt;
            handle_hovered = hover;
            handle_active = active;
        }

        // 浅色主题设置
        void SetLightTheme()
        {
            background = IM_COL32(220, 220, 220, 255);
            filled = IM_COL32(100, 150, 255, 255);
            handle = IM_COL32(255, 255, 255, 255);
            handle_border = IM_COL32(180, 180, 180, 255);
            text = IM_COL32(50, 50, 50, 255);
            handle_hovered = IM_COL32(240, 240, 240, 255);
            handle_active = IM_COL32(230, 230, 230, 255);
        }

        // 深色主题设置
        void SetDarkTheme()
        {
            background = IM_COL32(60, 60, 60, 255);
            filled = IM_COL32(80, 120, 200, 255);
            handle = IM_COL32(200, 200, 200, 255);
            handle_border = IM_COL32(40, 40, 40, 255);
            text = IM_COL32(220, 220, 220, 255);
            handle_hovered = IM_COL32(220, 220, 220, 255);
            handle_active = IM_COL32(180, 180, 180, 255);
        }

        virtual std::string GetRegisterTypeName() override { return "ImSliderStyle"; }

        DECLARE_IMOBJECT(ImSliderStyle, ImObject)
        registrar
            .RegisterProperty(PropertyType::Color, "BackgroundColor", &ImSliderStyle::background, u8"背景颜色")
            .RegisterProperty(PropertyType::Color, "FilledColor", &ImSliderStyle::filled, u8"填充颜色")
            .RegisterProperty(PropertyType::Color, "HandleColor", &ImSliderStyle::handle, u8"手柄颜色")
            .RegisterProperty(PropertyType::Color, "HandleBorderColor", &ImSliderStyle::handle_border, u8"手柄边框颜色")
            .RegisterProperty(PropertyType::Color, "TextColor", &ImSliderStyle::text, u8"文本颜色")
            .RegisterProperty(PropertyType::Color, "HandleHoveredColor", &ImSliderStyle::handle_hovered, u8"手柄悬停颜色")
            .RegisterProperty(PropertyType::Color, "HandleActiveColor", &ImSliderStyle::handle_active, u8"手柄激活颜色");
        END_DECLARE_IMOBJECT()
    };

    class ImSlider : public ImWidget
    {
    public:
        // 手柄类型枚举
        enum class SliderHandleType
        {
            SLIDER_HANDLE_CIRCLE,
            SLIDER_HANDLE_RECT
        };

        // 成员变量
        float v;             // 当前值
        float v_Min;        // 最小值
        float v_Max;        // 最大值
        SliderHandleType handle_type; // 手柄类型
        ImSliderStyle m_Style;  // 样式配置
        std::string format;   // 值显示格式
        float power;          // 非线性参数
        bool show_value;      // 是否显示数值
        bool show_handle = true;    // 是否显示滑块
        bool draggable = true;      // 滑块是否可拖动
        float handle_ratio = 0.8f;  // 滑块尺寸比例
        float handlewidth_ratio = 0.4f;  // 滑块宽度尺寸比例
        bool bReverse = false;    //是否反转滑动方向

        // 事件系统相关成员
        bool m_IsMouseOver = false;
        bool m_IsHandleHovered = false;
        bool m_IsDragging = false;
        ImVec2 m_DragStartPos;
        float m_DragStartValue;
        ImRect m_HandleRect;

        ImSlider(const std::string& WidgetName) :
            ImWidget(WidgetName),
            v(0.f),
            v_Min(0.f),
            v_Max(100.f),
            handle_type(SliderHandleType::SLIDER_HANDLE_RECT),
            format("%.1f"),
            power(1.0f),
            show_value(true)
        {
            SetFocusable(true);
        }

        // 拷贝构造函数
        ImSlider(const ImSlider& other)
            : ImWidget(other),
            v(other.v),
            v_Min(other.v_Min),
            v_Max(other.v_Max),
            handle_type(other.handle_type),
            m_Style(other.m_Style),
            format(other.format),
            power(other.power),
            show_value(other.show_value),
            show_handle(other.show_handle),
            draggable(other.draggable),
            handle_ratio(other.handle_ratio),
            handlewidth_ratio(other.handlewidth_ratio),
            bReverse(other.bReverse),
            m_IsMouseOver(false),  // 重置状态
            m_IsHandleHovered(false),
            m_IsDragging(false)
        {
        }

        void SetValue(float NewValue)
        {
            if (NewValue <= v_Min)
            {
                v = v_Min;
            }
            else if (NewValue >= v_Max)
            {
                v = v_Max;
            }
            else
            {
                v = NewValue;
            }
        }

        virtual ImVec2 GetMinSize() { return ImVec2(50.f, 10.f); }

        virtual void Render()
        {
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            ImGuiContext& g = *GImGui;
            const ImGuiStyle& style = g.Style;
            const ImGuiID id = window->GetID(m_WidgetID.c_str());

            // 使用成员变量设置控件位置和大小
            const ImRect frame_bb(Position, ImVec2(Position.x + Size.x, Position.y + Size.y));

            // 添加控件到绘制列表
            if (!ImGui::ItemAdd(frame_bb, id))
                return;

            // 确定滑块方向
            const bool is_horizontal = Size.x >= Size.y;
            const float grab_padding = 2.0f;
            const float slider_sz = is_horizontal ?
                (frame_bb.Max.x - frame_bb.Min.x - grab_padding * 2.0f) :
                (frame_bb.Max.y - frame_bb.Min.y - grab_padding * 2.0f);

            // 计算滑块位置
            float normalized_value = ImSaturate((v - v_Min) / (v_Max - v_Min));
            if (power != 1.0f && normalized_value > 0.0f)
                normalized_value = ImPow(normalized_value, 1.0f / power);

            float grab_pos;
            if (is_horizontal)
            {
                if (bReverse)
                {
                    grab_pos = frame_bb.Min.x + grab_padding + (1.f - normalized_value) * slider_sz;
                }
                else
                {
                    grab_pos = frame_bb.Min.x + grab_padding + normalized_value * slider_sz;
                }
            }
            else
            {
                if (bReverse)
                {
                    grab_pos = frame_bb.Min.y + grab_padding + normalized_value * slider_sz;
                }
                else
                {
                    grab_pos = frame_bb.Min.y + grab_padding + (1 - normalized_value) * slider_sz;
                }
            }

            // 渲染逻辑（移除交互逻辑）
            const ImU32 frame_col = m_Style.background;
            const ImU32 filled_col = m_Style.filled;

            // 根据状态选择手柄颜色
            ImU32 grab_col = m_Style.handle;
            if (m_IsDragging)
                grab_col = m_Style.handle_active;
            else if (m_IsHandleHovered)
                grab_col = m_Style.handle_hovered;

            const ImU32 grab_border_col = m_Style.handle_border;
            const ImU32 text_col = m_Style.text;

            // 绘制背景
            window->DrawList->AddRectFilled(
                frame_bb.Min,
                frame_bb.Max,
                frame_col,
                style.FrameRounding
            );

            // 绘制填充部分
            ImRect filled_bb;
            if (is_horizontal)
            {
                if (bReverse)
                {
                    filled_bb = ImRect(
                        ImVec2(grab_pos, frame_bb.Min.y + grab_padding),
                        ImVec2(frame_bb.Max.x - grab_padding, frame_bb.Max.y - grab_padding)
                    );
                }
                else
                {
                    filled_bb = ImRect(
                        ImVec2(frame_bb.Min.x + grab_padding, frame_bb.Min.y + grab_padding),
                        ImVec2(grab_pos, frame_bb.Max.y - grab_padding)
                    );
                }
            }
            else
            {
                if (bReverse)
                {
                    filled_bb = ImRect(
                        ImVec2(frame_bb.Min.x + grab_padding, frame_bb.Min.y + grab_padding),
                        ImVec2(frame_bb.Max.x - grab_padding, grab_pos)
                    );
                }
                else
                {
                    filled_bb = ImRect(
                        ImVec2(frame_bb.Min.x + grab_padding, grab_pos),
                        ImVec2(frame_bb.Max.x - grab_padding, frame_bb.Max.y - grab_padding)
                    );
                }
            }

            const float filled_rounding = style.FrameRounding > 0.0f ? style.FrameRounding - 1.0f : 0.0f;
            window->DrawList->AddRectFilled(
                filled_bb.Min,
                filled_bb.Max,
                filled_col,
                filled_rounding
            );

            if (show_handle)
            {
                // 计算手柄尺寸
                const float base_size = is_horizontal ?
                    (frame_bb.Max.y - frame_bb.Min.y) :
                    (frame_bb.Max.x - frame_bb.Min.x);

                const float handle_radius = base_size * 0.4f * handle_ratio;
                float handle_width;
                float handle_height;

                if (is_horizontal)
                {
                    handle_height = base_size * handle_ratio;
                    handle_width = handle_height * handlewidth_ratio;
                }
                else
                {
                    handle_width = base_size * handle_ratio;
                    handle_height = handle_width * handlewidth_ratio;
                }

                // 绘制滑块手柄
                if (is_horizontal)
                {
                    ImVec2 center = ImVec2(
                        grab_pos,
                        (frame_bb.Min.y + frame_bb.Max.y) * 0.5f
                    );

                    if (handle_type == SliderHandleType::SLIDER_HANDLE_CIRCLE)
                    {
                        window->DrawList->AddCircleFilled(center, handle_radius, grab_col);
                        window->DrawList->AddCircle(center, handle_radius, grab_border_col, 0, 2.0f);
                        m_HandleRect = ImRect(
                            ImVec2(center.x - handle_radius, center.y - handle_radius),
                            ImVec2(center.x + handle_radius, center.y + handle_radius)
                        );
                    }
                    else
                    {
                        ImRect handle_bb(
                            ImVec2(center.x - handle_width * 0.5f, center.y - handle_height * 0.5f),
                            ImVec2(center.x + handle_width * 0.5f, center.y + handle_height * 0.5f)
                        );
                        window->DrawList->AddRectFilled(handle_bb.Min, handle_bb.Max, grab_col, 3.0f);
                        window->DrawList->AddRect(handle_bb.Min, handle_bb.Max, grab_border_col, 3.0f, 0, 2.0f);
                        m_HandleRect = handle_bb;
                    }
                }
                else
                {
                    ImVec2 center = ImVec2(
                        (frame_bb.Min.x + frame_bb.Max.x) * 0.5f,
                        grab_pos
                    );

                    if (handle_type == SliderHandleType::SLIDER_HANDLE_CIRCLE)
                    {
                        window->DrawList->AddCircleFilled(center, handle_radius, grab_col);
                        window->DrawList->AddCircle(center, handle_radius, grab_border_col, 0, 2.0f);
                        m_HandleRect = ImRect(
                            ImVec2(center.x - handle_radius, center.y - handle_radius),
                            ImVec2(center.x + handle_radius, center.y + handle_radius)
                        );
                    }
                    else
                    {
                        ImRect handle_bb(
                            ImVec2(center.x - handle_width * 0.5f, center.y - handle_height * 0.5f),
                            ImVec2(center.x + handle_width * 0.5f, center.y + handle_height * 0.5f)
                        );
                        window->DrawList->AddRectFilled(handle_bb.Min, handle_bb.Max, grab_col, 3.0f);
                        window->DrawList->AddRect(handle_bb.Min, handle_bb.Max, grab_border_col, 3.0f, 0, 2.0f);
                        m_HandleRect = handle_bb;
                    }
                }
            }

            // 可选：显示当前值
            if (show_value)
            {
                char value_buf[64];
                ImFormatString(value_buf, IM_ARRAYSIZE(value_buf), format.c_str(), v);
                ImVec2 value_size = ImGui::CalcTextSize(value_buf);

                ImVec2 value_pos;
                if (is_horizontal)
                {
                    value_pos = ImVec2(
                        frame_bb.Min.x + (frame_bb.GetWidth() - value_size.x) * 0.5f,
                        frame_bb.Min.y + (frame_bb.GetHeight() - value_size.y) * 0.5f
                    );
                }
                else
                {
                    value_pos = ImVec2(
                        frame_bb.Min.x + (frame_bb.GetWidth() - value_size.x) * 0.5f,
                        frame_bb.Min.y + (frame_bb.GetHeight() - value_size.y) * 0.5f
                    );
                }
                window->DrawList->AddText(value_pos, text_col, value_buf);
            }
        }

        // 事件处理（新增）
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

            case ImEventType::MouseWheel:
                HandleMouseWheel(event->As<ImMouseWheelEvent>());
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

            default:
                // 其他事件继续冒泡
                break;
            }
        }

    private:
        // 鼠标进入事件处理
        void HandleMouseEnter(ImMouseEnterEvent* event)
        {
            m_IsMouseOver = true;
            UpdateHandleHoverState(event->GetPosition());
            event->StopPropagation();
        }

        // 鼠标离开事件处理
        void HandleMouseLeave(ImMouseLeaveEvent* event)
        {
            m_IsMouseOver = false;
            m_IsHandleHovered = false;
            event->StopPropagation();
        }

        // 鼠标移动事件处理
        void HandleMouseMove(ImMouseMoveEvent* event)
        {
            if (m_IsMouseOver)
            {
                UpdateHandleHoverState(event->GetPosition());
            }
            event->StopPropagation();
        }

        // 鼠标按下事件处理
        void HandleMouseDown(ImMouseDownEvent* event)
        {
            if (event->GetButton() == ImMouseButton::Left && draggable)
            {
                const ImVec2 mousePos = event->GetPosition();

                if (m_IsHandleHovered)
                {
                    m_IsDragging = true;
                    m_DragStartPos = mousePos;
                    m_DragStartValue = v;
                    event->StopPropagation();
                }
                else if (GetRect().Contains(mousePos))
                {
                    // 点击滑块轨道，直接设置值
                    SetValueFromPosition(mousePos);
                    m_IsDragging = true;
                    m_DragStartPos = mousePos;
                    m_DragStartValue = v;
                    event->StopPropagation();
                }
            }
        }

        // 鼠标释放事件处理
        void HandleMouseUp(ImMouseUpEvent* event)
        {
            if (event->GetButton() == ImMouseButton::Left && m_IsDragging)
            {
                m_IsDragging = false;
                event->StopPropagation();
            }
        }

        // 鼠标滚轮事件处理
        void HandleMouseWheel(ImMouseWheelEvent* event)
        {
            if (m_IsMouseOver)
            {
                ImVec2 wheelDelta = event->GetWheelDelta();
                float step = (v_Max - v_Min) * 0.1f; // 10% of range per wheel step

                if (wheelDelta.y > 0)
                {
                    SetValue(v + step);
                }
                else if (wheelDelta.y < 0)
                {
                    SetValue(v - step);
                }
                event->StopPropagation();
            }
        }

        // 拖拽开始事件处理
        void HandleDragStart(ImDragStartEvent* event)
        {
            if (m_IsDragging)
            {
                event->StopPropagation();
            }
        }

        // 拖拽更新事件处理
        void HandleDragUpdate(ImDragUpdateEvent* event)
        {
            if (m_IsDragging)
            {
                const ImVec2 mousePos = event->GetPosition();
                SetValueFromPosition(mousePos);
                event->StopPropagation();
            }
        }

        // 拖拽结束事件处理
        void HandleDragEnd(ImDragEndEvent* event)
        {
            if (m_IsDragging)
            {
                m_IsDragging = false;
                event->StopPropagation();
            }
        }

        // 根据鼠标位置设置值
        void SetValueFromPosition(const ImVec2& mousePos)
        {
            const bool is_horizontal = Size.x >= Size.y;
            const ImRect frame_bb(Position, Position + Size);
            const float grab_padding = 2.0f;
            const float slider_sz = is_horizontal ?
                (frame_bb.Max.x - frame_bb.Min.x - grab_padding * 2.0f) :
                (frame_bb.Max.y - frame_bb.Min.y - grab_padding * 2.0f);

            float normalized_value;
            if (is_horizontal)
            {
                if (bReverse)
                {
                    normalized_value = 1.f - ImSaturate((mousePos.x - (frame_bb.Min.x + grab_padding)) / slider_sz);
                }
                else
                {
                    normalized_value = ImSaturate((mousePos.x - (frame_bb.Min.x + grab_padding)) / slider_sz);
                }
            }
            else
            {
                if (bReverse)
                {
                    normalized_value = ImSaturate((mousePos.y - (frame_bb.Min.y + grab_padding)) / slider_sz);
                }
                else
                {
                    normalized_value = 1.f - ImSaturate((mousePos.y - (frame_bb.Min.y + grab_padding)) / slider_sz);
                }
            }

            if (power != 1.0f)
                normalized_value = ImPow(normalized_value, power);

            v = v_Min + normalized_value * (v_Max - v_Min);
            v = ImClamp(v, v_Min, v_Max);
        }

        // 更新手柄悬停状态
        void UpdateHandleHoverState(const ImVec2& mousePos)
        {
            m_IsHandleHovered = m_HandleRect.Contains(mousePos);

            // 更新鼠标光标
            if (m_IsHandleHovered || m_IsDragging)
            {
                const bool is_horizontal = Size.x >= Size.y;
                ImGui::SetMouseCursor(is_horizontal ? ImGuiMouseCursor_ResizeEW : ImGuiMouseCursor_ResizeNS);
            }
        }

    public:
        virtual std::string GetRegisterTypeName()override { return "ImSlider"; }

        virtual ImWidget* CopyWidget()
        {
            return new ImSlider(*this);
        }

        DECLARE_IMOBJECT(ImSlider, ImWidget)
        registrar
            // 值范围
            .RegisterProperty(PropertyType::Float, "MinValue", &ImSlider::v_Min, u8"最小值")
            .RegisterProperty(PropertyType::Float, "MaxValue", &ImSlider::v_Max, u8"最大值")
            .RegisterProperty(PropertyType::Float, "CurrentValue", &ImSlider::v, u8"当前值")

            // 手柄类型
            .RegisterOptionalProperty(
                PropertyType::Enum, "HandleType", &ImSlider::handle_type,
                { "SLIDER_HANDLE_CIRCLE", "SLIDER_HANDLE_RECT" },
                u8"手柄类型")

            // 样式配置（作为结构体属性）
            .RegisterProperty(PropertyType::Struct, "Style", &ImSlider::m_Style, u8"滑块样式")

            // 显示选项
            .RegisterProperty(PropertyType::Bool, "ShowValue", &ImSlider::show_value, u8"是否显示数值")
            .RegisterProperty(PropertyType::String, "ValueFormat", &ImSlider::format, u8"值显示格式")

            // 行为设置
            .RegisterProperty(PropertyType::Bool, "ShowHandle", &ImSlider::show_handle, u8"是否显示手柄")
            .RegisterProperty(PropertyType::Bool, "Draggable", &ImSlider::draggable, u8"是否可拖动")
            .RegisterProperty(PropertyType::Float, "HandleRatio", &ImSlider::handle_ratio, u8"手柄尺寸比例")
            .RegisterProperty(PropertyType::Float, "HandleWidthRatio", &ImSlider::handlewidth_ratio, u8"手柄宽度比例")
            .RegisterProperty(PropertyType::Bool, "Reverse", &ImSlider::bReverse, u8"是否反转滑动方向")
            .RegisterProperty(PropertyType::Float, "Power", &ImSlider::power, u8"非线性参数");
        END_DECLARE_IMOBJECT()
    };
}