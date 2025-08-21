#pragma once
#include "ImWidget.h"
#include <imgui_internal.h>

namespace ImGuiWidget
{
	class ImSlider :public ImWidget
	{
	public:    
        // 手柄类型枚举
        enum class SliderHandleType 
        {
            SLIDER_HANDLE_CIRCLE,
            SLIDER_HANDLE_RECT
        };

        // 颜色配置结构
        struct SliderColors 
        {
            ImU32 background;
            ImU32 filled;
            ImU32 handle;
            ImU32 handle_border;
            ImU32 text;

            SliderColors() :
                background(IM_COL32(100, 100, 100, 255)),
                filled(IM_COL32(61, 133, 224, 255)),
                handle(IM_COL32(255, 255, 255, 255)),
                handle_border(IM_COL32(30, 30, 30, 255)),
                text(IM_COL32(255, 255, 255, 255)) {}
        };

        // 成员变量
        float v;             // 指向当前值的指针
        float v_Min;            // 最小值
        float v_Max;            // 最大值
        SliderHandleType handle_type; // 手柄类型
        SliderColors colors;  // 颜色配置
        std::string format;   // 值显示格式
        float power;          // 非线性参数
        bool show_value;      // 是否显示数值
        bool show_handle = true;    // 是否显示滑块
        bool draggable = true;      // 滑块是否可拖动
        float handle_ratio = 0.8f;  // 滑块尺寸比例（水平时为高度比例，垂直时为宽度比例）
        float handlewidth_ratio = 0.4f;  // 滑块宽度尺寸比例
        bool bReverse = false;    //是否反转滑动方向
        ImSlider(const std::string& WidgetName) :
            ImWidget(WidgetName),
            v(0.f), 
            v_Min(0.f), 
            v_Max(100.f),
            handle_type(SliderHandleType::SLIDER_HANDLE_RECT),
            format("%.1f"),
            power(1.0f), 
            show_value(true)
        {}

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
                return ;

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
                    grab_pos = frame_bb.Min.x + grab_padding + (1.f-normalized_value) * slider_sz;
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
                

            // 交互逻辑
            bool hovered = false;
            bool held = false;
            bool pressed = false;
            if (draggable) {  // 新增可拖动检查
                pressed = ImGui::ButtonBehavior(frame_bb, id, &hovered, &held);
            }
            bool value_changed = false;

            if (hovered || held)
                ImGui::SetMouseCursor(is_horizontal ? ImGuiMouseCursor_ResizeEW : ImGuiMouseCursor_ResizeNS);

            if (held && (g.IO.MouseDelta.x != 0.0f || g.IO.MouseDelta.y != 0.0f))
            {
                if (is_horizontal)
                {
                    if (bReverse)
                    {
                        normalized_value = 1.f-ImSaturate((g.IO.MousePos.x - (frame_bb.Min.x + grab_padding)) / slider_sz);
                    }
                    else
                    {
                        normalized_value = ImSaturate((g.IO.MousePos.x - (frame_bb.Min.x + grab_padding)) / slider_sz);
                    }
                    
                    if (power != 1.0f)
                        normalized_value = ImPow(normalized_value, power);
                    v = v_Min + normalized_value * (v_Max - v_Min);
                    value_changed = true;
                }
                else
                {
                    if (bReverse)
                    {
                        normalized_value = ImSaturate((g.IO.MousePos.y - (frame_bb.Min.y + grab_padding)) / slider_sz);
                    }
                    else
                    {
                        normalized_value = 1.f - ImSaturate((g.IO.MousePos.y - (frame_bb.Min.y + grab_padding)) / slider_sz);
                    }
                    
                    if (power != 1.0f)
                        normalized_value = ImPow(normalized_value, power);
                    v = v_Min + normalized_value * (v_Max - v_Min);
                    value_changed = true;
                }
            }

            // 渲染
            const ImU32 frame_col = colors.background;
            const ImU32 filled_col = colors.filled;
            const ImU32 grab_col = colors.handle;
            const ImU32 grab_border_col = colors.handle_border;
            const ImU32 text_col = colors.text;

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
                    filled_bb = ImRect
                    (
                        ImVec2(grab_pos, frame_bb.Min.y + grab_padding),
                        ImVec2(frame_bb.Max.x - grab_padding, frame_bb.Max.y - grab_padding)
                    );
                }
                else
                {
                    filled_bb = ImRect
                    (
                        ImVec2(frame_bb.Min.x + grab_padding, frame_bb.Min.y + grab_padding),
                        ImVec2(grab_pos, frame_bb.Max.y - grab_padding)
                    );
                }

            }
            else
            {
                if (bReverse)
                {
                    filled_bb = ImRect
                    (
                        ImVec2(frame_bb.Min.x + grab_padding, frame_bb.Min.y + grab_padding),
                        ImVec2(frame_bb.Max.x - grab_padding, grab_pos)
                    );
                }
                else
                {
                    filled_bb = ImRect
                    (
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
				// 计算手柄尺寸（使用handle_ratio）
				const float base_size = is_horizontal ?
					(frame_bb.Max.y - frame_bb.Min.y) :
					(frame_bb.Max.x - frame_bb.Min.x);

				const float handle_radius = base_size * 0.4f * handle_ratio;   // 应用比例
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

				//const float handle_width = is_horizontal ?
				//	base_size * 0.6f * handle_ratio :
				//	base_size * 0.6f * handle_ratio;  // 统一应用比例

				//const float handle_height = is_horizontal ?
				//	base_size * 0.8f * handle_ratio :
				//	base_size * 0.8f * handle_ratio;  // 统一应用比例

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
					}
					else
					{
						ImRect handle_bb(
							ImVec2(center.x - handle_width * 0.5f, center.y - handle_height * 0.5f),
							ImVec2(center.x + handle_width * 0.5f, center.y + handle_height * 0.5f)
						);
						window->DrawList->AddRectFilled(handle_bb.Min, handle_bb.Max, grab_col, 3.0f);
						window->DrawList->AddRect(handle_bb.Min, handle_bb.Max, grab_border_col, 3.0f, 0, 2.0f);
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
					}
					else
					{
						ImRect handle_bb(
							ImVec2(center.x - handle_width * 0.5f, center.y - handle_height * 0.5f),
							ImVec2(center.x + handle_width * 0.5f, center.y + handle_height * 0.5f)
						);
						window->DrawList->AddRectFilled(handle_bb.Min, handle_bb.Max, grab_col, 3.0f);
						window->DrawList->AddRect(handle_bb.Min, handle_bb.Max, grab_border_col, 3.0f, 0, 2.0f);
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

        virtual std::unordered_set<PropertyInfo, PropertyInfo::Hasher> GetProperties() override 
        {
            auto props = ImWidget::GetProperties();

            // 值范围
            props.insert({
                "MinValue", PropertyType::Float, "Value",
                [this](void* v) { this->v_Min = *static_cast<float*>(v); },
                [this]() -> void* { return &this->v_Min; }
                });

            props.insert({
                "MaxValue", PropertyType::Float, "Value",
                [this](void* v) { this->v_Max = *static_cast<float*>(v); },
                [this]() -> void* { return &this->v_Max; }
                });

            props.insert({
                "CurrentValue", PropertyType::Float, "Value",
                [this](void* v) { this->v = *static_cast<float*>(v); },
                [this]() -> void* { return &this->v; }
                });

            // 手柄类型
            props.insert({
                "HandleType", PropertyType::Int, "Appearance",
                [this](void* v) { this->handle_type = static_cast<SliderHandleType>(*static_cast<int*>(v)); },
                [this]() -> void* {
                    static int handle_type_int = static_cast<int>(this->handle_type);
                    return &handle_type_int;
                }
                });

            // 显示选项
            props.insert({
                "ShowValue", PropertyType::Bool, "Appearance",
                [this](void* v) { this->show_value = *static_cast<bool*>(v); },
                [this]() -> void* { return &this->show_value; }
                });

            props.insert({
                "ValueFormat", PropertyType::String, "Appearance",
                [this](void* v) { this->format = *static_cast<std::string*>(v); },
                [this]() -> void* { return &format; }
                });

            // 颜色设置
            props.insert({
                "BackgroundColor", PropertyType::Color, "Colors",
                [this](void* v) { this->colors.background = *static_cast<ImU32*>(v); },
                [this]() -> void* { return &this->colors.background; }
                });

            props.insert({
                "FilledColor", PropertyType::Color, "Colors",
                [this](void* v) { this->colors.filled = *static_cast<ImU32*>(v); },
                [this]() -> void* { return &this->colors.filled; }
                });

            props.insert({
                "HandleColor", PropertyType::Color, "Colors",
                [this](void* v) { this->colors.handle = *static_cast<ImU32*>(v); },
                [this]() -> void* { return &this->colors.handle; }
                });

            props.insert({
                "HandleBorderColor", PropertyType::Color, "Colors",
                [this](void* v) { this->colors.handle_border = *static_cast<ImU32*>(v); },
                [this]() -> void* { return &this->colors.handle_border; }
                });

            props.insert({
                "TextColor", PropertyType::Color, "Colors",
                [this](void* v) { this->colors.text = *static_cast<ImU32*>(v); },
                [this]() -> void* { return &this->colors.text; }
                });

			props.insert({
	            "ShowHandle", PropertyType::Bool, "Appearance",
	            [this](void* v) { this->show_handle = *static_cast<bool*>(v); },
	            [this]() -> void* { return &this->show_handle; }
				});

			props.insert({
				"Draggable", PropertyType::Bool, "Behavior",
				[this](void* v) { this->draggable = *static_cast<bool*>(v); },
				[this]() -> void* { return &this->draggable; }
				});

			props.insert({
				"HandleRatio", PropertyType::Float, "Appearance",
				[this](void* v) { this->handle_ratio = *static_cast<float*>(v); },
				[this]() -> void* { return &this->handle_ratio; }
				});

            props.insert({
                "HandleWidthRatio", PropertyType::Float, "Appearance",
                [this](void* v) { this->handlewidth_ratio = *static_cast<float*>(v); },
                [this]() -> void* { return &this->handlewidth_ratio; }
                });

            props.insert({
                "Reserve", PropertyType::Bool, "Appearance",
                [this](void* v) { this->bReverse = *static_cast<bool*>(v); },
                [this]() -> void* { return &this->bReverse; }
                });
            // 非线性参数
            props.insert({
                "Power", PropertyType::Float, "Behavior",
                [this](void* v) { this->power = *static_cast<float*>(v); },
                [this]() -> void* { return &this->power; }
                });

            return props;
        }

        virtual std::string GetRegisterTypeName()override { return "ImSlider"; }

        virtual ImWidget* CopyWidget()
        {
            return new ImSlider(*this);
        }

	};
}
