#pragma once
#include "ImWidget.h"
#include "imgui_internal.h"

namespace ImGuiWidget
{
	class ImDragHandle
	{
	public:
		std::string HandleType;
		ImVec2 FromPos;
		ImWidget* Preview;
		class ImUserWidget* FromWidget;
		virtual ~ImDragHandle() {}
	};

	class ImUserWidget :public ImWidget
	{
	private:
		ImWidget* m_RootWidget;

		ImGuiButtonFlags m_ButtonFlag = ImGuiButtonFlags_AllowOverlap;
		// 拖拽状态变量
		bool m_IsDragging = false;
		bool m_IsDragSource = false;
		bool m_IsDragTarget = false;
		bool bDragFinishedLastTick = false;
		//允许拖拽动作
		bool bAllowDrag = false;//允许被拖拽
		bool bAllowDragOn = false;//允许拖拽放置此处
		ImDragHandle* m_CurrentDragHandle = nullptr;

		// ================ 新增状态变量 ================ //
		bool m_WasHovered = false;     // 上一帧悬停状态
		bool m_LastFrameHeld = false;   // 上一帧按下状态
		bool m_HadFocus = false;        // 上一帧焦点状态

	protected:
		//拖拽相关回调
		virtual ImDragHandle* OnDragBegin() { return nullptr; }
		virtual void OnDragOn(ImDragHandle* OriginalHandle) {}
		virtual void OnDragComplete() {}
		virtual void OnDragCancel() {}
		virtual void Tick(){}
		void HandleDragFinish()
		{
			delete m_CurrentDragHandle;
			m_CurrentDragHandle = nullptr;
		}

		// ================ 新增事件回调 ================ //
		// 鼠标悬停事件
		std::function<void(void)> OnHovered;
		std::function<void(void)> OnUnhovered;

		// 鼠标点击事件
		std::function<void(void)> OnPressed;
		std::function<void(void)> OnReleased;

		// 焦点事件
		std::function<void(void)> OnFocusReceived;
		std::function<void(void)> OnFocusLost;

		// ================ 新增样式属性 ================ //
		bool m_EnableBackground = true;                  // 是否启用背景
		ImU32 m_BackgroundColor = IM_COL32(50, 50, 50, 255); // 默认背景色

		bool m_EnableBorder = true;                      // 是否启用边框
		ImU32 m_BorderColor = IM_COL32(100, 100, 100, 255); // 默认边框色
		float m_BorderThickness = 1.0f;                  // 边框厚度

	public:
		ImUserWidget(const std::string& WidgetName) :ImWidget(WidgetName), m_RootWidget(nullptr) {}
		ImUserWidget(const std::string& WidgetName, ImWidget* RootWidget) :ImWidget(WidgetName)
		{
			SetRootWidget(RootWidget);
		}

		virtual void SetPosition(ImVec2 Pos)
		{
			Position = Pos;
			if (m_RootWidget)
			{
				m_RootWidget->SetPosition(Pos);
			}
		}

		virtual void SetSize(ImVec2 size)
		{
			Size = size;
			if (m_RootWidget)
			{
				m_RootWidget->SetSize(size);
			}
		}

		virtual ImVec2 GetMinSize()
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

		void SetAllowDrag(bool NewSetting)
		{
			bAllowDrag = NewSetting;
		}

		void SetAllowDragOn(bool NewSetting)
		{
			bAllowDragOn = NewSetting;
		}


		// ================ 新增样式设置方法 ================ //
		// 背景设置
		void SetBackgroundEnabled(bool enable) { m_EnableBackground = enable; }
		void SetBackgroundColor(ImU32 color) { m_BackgroundColor = color; }

		// 边框设置
		void SetBorderEnabled(bool enable) { m_EnableBorder = enable; }
		void SetBorderColor(ImU32 color) { m_BorderColor = color; }
		void SetBorderThickness(float thickness) { m_BorderThickness = thickness; }

		// ================ 新增事件回调设置方法 ================ //
		void SetOnHovered(std::function<void(void)> callback) { OnHovered = callback; }
		void SetOnUnhovered(std::function<void(void)> callback) { OnUnhovered = callback; }
		void SetOnPressed(std::function<void(void)> callback) { OnPressed = callback; }
		void SetOnReleased(std::function<void(void)> callback) { OnReleased = callback; }
		void SetOnFocusReceived(std::function<void(void)> callback) { OnFocusReceived = callback; }
		void SetOnFocusLost(std::function<void(void)> callback) { OnFocusLost = callback; }

		//处理交互
		virtual void HandleInteraction()
		{

			if (bAllowDrag || bAllowDragOn)
			{
				ImGuiWindow* window = ImGui::GetCurrentWindow();
				const ImGuiID id = window->GetID(m_WidgetID.c_str());
				const ImRect bb(Position, Position + Size);

				// 添加控件到交互系统
				if (!ImGui::ItemAdd(bb, id,0, ImGuiItemFlags_AllowOverlap)) return;

				bool hovered = false, held = false;
				bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, m_ButtonFlag);
				// ================ 新增事件检测 ================ //
				// 悬停状态变化检测
				if (hovered && !m_WasHovered) {
					if (OnHovered) OnHovered();
				}
				if (!hovered && m_WasHovered) {
					if (OnUnhovered) OnUnhovered();
				}
				m_WasHovered = hovered;

				// 点击状态变化检测
				if (held && !m_LastFrameHeld) {
					if (OnPressed) OnPressed();
				}
				if (!held && m_LastFrameHeld) {
					if (OnReleased) OnReleased();
				}
				m_LastFrameHeld = held;

				// 焦点状态变化检测
				bool isFocused = ImGui::IsItemFocused();
				if (isFocused && !m_HadFocus) {
					if (OnFocusReceived) OnFocusReceived();
				}
				if (!isFocused && m_HadFocus) {
					if (OnFocusLost) OnFocusLost();
				}
				m_HadFocus = isFocused;
				if (bAllowDrag)
				{
					// ================= 拖拽源处理 ================= //
					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoPreviewTooltip))
					{
						if (!m_IsDragSource && !m_CurrentDragHandle)
						{
							// 拖拽开始，创建拖拽句柄
							m_CurrentDragHandle = OnDragBegin();
							m_CurrentDragHandle->FromPos = Position;
							m_CurrentDragHandle->FromWidget = this;
						}
						m_IsDragSource = true;

						// 设置拖拽payload
						ImGui::SetDragDropPayload("ImUserWidget_DragDrop",
							&m_CurrentDragHandle,
							sizeof(ImDragHandle*));

						ImGui::EndDragDropSource();
					}
					else
					{
						m_IsDragSource = false;
					}

					if (m_IsDragSource)
					{
						// 显示拖拽预览
						if (m_CurrentDragHandle->Preview)
						{
							ImVec2 WindowPos = ImGui::GetMousePos();
							ImVec2 PreviewWidgetSize = m_CurrentDragHandle->Preview->GetMinSize();
							WindowPos.y -= PreviewWidgetSize.y;
							// 设置子窗口位置和大小（单位：像素）
							ImGui::SetNextWindowPos(WindowPos, 0);
							ImGui::SetNextWindowSize(PreviewWidgetSize, ImGuiCond_FirstUseEver);

							// 组合窗口标志
							ImGuiWindowFlags flags =
								ImGuiWindowFlags_NoTitleBar |
								ImGuiWindowFlags_NoBackground |
								ImGuiWindowFlags_NoInputs |
								ImGuiWindowFlags_NoResize;   // 可选：固定大小
							ImGuiWindowFlags_NoMove;      // 可选：固定位置

														  // 开始创建窗口
							if (ImGui::Begin("Invisible Window", nullptr, flags))
							{
								// 添加内容（无背景，确保内容可见）
								m_CurrentDragHandle->Preview->SetPosition(WindowPos);
								m_CurrentDragHandle->Preview->SetSize(PreviewWidgetSize);
								m_CurrentDragHandle->Preview->Render();
								ImGui::End();
							}


						}
					}

					// ================= 拖拽取消处理 ================= //
					if (bDragFinishedLastTick)
					{
						if (m_CurrentDragHandle)
						{
							OnDragCancel();
							HandleDragFinish();
						}
						bDragFinishedLastTick = false;
					}

					if (!m_IsDragSource && m_IsDragging)
					{
						bDragFinishedLastTick = true;
					}

					m_IsDragging = m_IsDragSource;
				}

				if (bAllowDragOn)
				{
					// ================= 拖拽目标处理 ================= //
					if (ImGui::BeginDragDropTarget())
					{
						m_IsDragTarget = true;

						// 接受拖拽payload
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ImUserWidget_DragDrop"))
						{
							IM_ASSERT(payload->DataSize == sizeof(ImDragHandle*));
							ImDragHandle* dragHandle = *(ImDragHandle**)payload->Data;

							// 触发拖拽完成回调
							OnDragOn(dragHandle);

							//通知源控件
							dragHandle->FromWidget->OnDragComplete();
							dragHandle->FromWidget->HandleDragFinish();
						}

						ImGui::EndDragDropTarget();
					}
					else
					{
						m_IsDragTarget = false;
					}
				}
			}
		}

		virtual void Render()
		{
			ImGuiWindow* window = ImGui::GetCurrentWindow();

			// ================ 新增背景绘制 ================ //
			if (m_EnableBackground)
			{
				window->DrawList->AddRectFilled(
					Position,
					Position + Size,
					m_BackgroundColor
				);
			}

			// 处理交互事件
			HandleInteraction();

			// 渲染根控件
			if (m_RootWidget)
			{
				m_RootWidget->Render();
			}
			// ================ 新增边框绘制 ================ //
			if (m_EnableBorder && m_BorderThickness > 0.0f)
			{
				window->DrawList->AddRect(
					Position,
					Position + Size,
					m_BorderColor,
					0.0f, // 圆角半径（可选，默认为0）
					ImDrawFlags_None,
					m_BorderThickness
				);
			}
			Tick();
		}

		void SetRootWidget(ImWidget* RootWidget,bool DeleteOldRoot=true)
		{
			if (m_RootWidget && DeleteOldRoot)
			{
				delete m_RootWidget;
			}
			m_RootWidget = RootWidget;
			if (m_RootWidget)
			{
				m_RootWidget->SetPosition(Position);
				m_RootWidget->SetSize(Size);
			}
		}

		bool GetIsFocused() { return m_HadFocus; }
	};
}