#pragma once
#include "ImPanelWidget.h"
#include "imgui_internal.h"

namespace ImGuiWidget
{
	class ImVerticalSplitter:public ImPanelWidget
	{
	private:
		float splitRatio; // 分割比例 (0.0 = 全左, 1.0 = 全右)
		bool dragging;    // 当前是否正在拖动
		float barWidth = 4.f;
	public:
		ImVerticalSplitter(const std::string& WidgetName):
			ImPanelWidget(WidgetName),
			splitRatio(0.5f),
			dragging(false)
		{}

		ImSlot* SetTopPart(ImWidget* TopWidget)
		{
			if (m_Slots.size() >= 1)
			{
				delete m_Slots[0];
				m_Slots[0] = new ImSlot(TopWidget);
			}
			else
			{
				m_Slots.push_back(new ImSlot(TopWidget));
			}
			return m_Slots[0];
		}

		ImSlot* SetBottomPart(ImWidget* BottomWidget)
		{
			if (m_Slots.size() >= 2)
			{
				delete m_Slots[1];
				m_Slots[1] = new ImSlot(BottomWidget);
			}
			else if (m_Slots.size() == 1)
			{
				m_Slots.push_back(new ImSlot(BottomWidget));
			}
			else
			{
				m_Slots.push_back(new ImSlot(nullptr));
				m_Slots.push_back(new ImSlot(BottomWidget));
			}
			return m_Slots[1];
		}

	private:
		// 处理交互逻辑
		void HandleInteraction(const ImRect& barRect, ImGuiID id)
		{
			ImGuiWindow* window = ImGui::GetCurrentWindow();

			// 注册交互区域
			ImGui::ItemAdd(barRect, id);
			ImGui::SetItemAllowOverlap();

			// 鼠标悬停效果
			if (ImGui::IsItemHovered()) {
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
			}

			// 开始拖动
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered())
			{
				dragging = true;
				ImGui::SetActiveID(id, window);
			}

			// 处理拖动
			if (dragging && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
			{
				// 计算鼠标移动的增量（相对分割器宽度）
				const float delta = ImGui::GetIO().MouseDelta.y / Size.y;
				splitRatio = ImClamp(splitRatio + delta, 0.05f, 0.95f);
			}

			// 结束拖动
			if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
			{
				dragging = false;
				if (ImGui::GetActiveID() == id)
				{
					ImGui::ClearActiveID();
				}
			}
		}

		// 绘制分割条
		void RenderSplitterBar(const ImRect& barRect)
		{
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			ImU32 color;

			// 根据状态选择颜色
			if (dragging)
			{
				color = ImGui::GetColorU32(ImGuiCol_SeparatorActive);
			}
			else if (ImGui::IsItemHovered())
			{
				color = ImGui::GetColorU32(ImGuiCol_SeparatorHovered);
			}
			else
			{
				color = ImGui::GetColorU32(ImGuiCol_Separator);
			}

			// 绘制分割条
			window->DrawList->AddRectFilled(
				barRect.Min,
				barRect.Max,
				color,
				ImGui::GetStyle().FrameRounding
			);
		}

	public:
		virtual void Render() override
		{
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			if (window->SkipItems) return;

			ImGuiID id = window->GetID(m_WidgetName.c_str());

			// 计算分割条的位置和区域
			const float barCenterY = splitRatio * Size.y;

			const ImRect barRect(
				Position.x,
				Position.y + barCenterY - barWidth * 0.5f,
				Position.x + Size.x,
				Position.y + barCenterY + barWidth * 0.5f
			);

			const ImRect UpRect(
				Position.x,
				Position.y,
				Position.x + Size.x,
				Position.y + barCenterY - barWidth * 0.5f
			);

			const ImRect DownRect(
				Position.x,
				Position.y + barCenterY + barWidth * 0.5f,
				Position.x + Size.x,
				Position.y + Size.y
			);

			HandleInteraction(barRect, id);

			if (m_Slots.size() >= 1&&m_Slots[0])
			{
				m_Slots[0]->GetContent()->SetPosition(UpRect.Min);
				m_Slots[0]->GetContent()->SetSize(UpRect.Max - UpRect.Min);
				m_Slots[0]->GetContent()->Render();
			}
			if (m_Slots.size() >= 2 && m_Slots[1])
			{
				m_Slots[1]->GetContent()->SetPosition(DownRect.Min);
				m_Slots[1]->GetContent()->SetSize(DownRect.Max - DownRect.Min);
				m_Slots[1]->GetContent()->Render();
			}

			RenderSplitterBar(barRect);
		}
	};
}