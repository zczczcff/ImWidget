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
		virtual ~ImDragHandle(){}
	};


	class ImUserWidget :public ImWidget
	{
	private:
		ImWidget* m_RootWidget;

		// 拖拽状态变量
		bool m_IsDragging = false;
		bool m_IsDragSource = false;
		bool m_IsDragTarget = false;
		//允许拖拽动作
		bool bAllowDrag = false;//允许被拖拽
		bool bAllowDragOn = false;//允许拖拽放置此处
        ImDragHandle* m_CurrentDragHandle = nullptr;
	protected:
		//拖拽相关回调
		virtual ImDragHandle* OnDragBegin() { return nullptr; }
		virtual void OnDragOn(ImDragHandle* OriginalHandle) {}
		virtual void OnDragComplete() {}
		virtual void OnDragCancel() {}
		void HandleDragFinish()
		{
			delete m_CurrentDragHandle;
			m_CurrentDragHandle = nullptr;
		}
	public:
		ImUserWidget(const std::string& WidgetName) :ImWidget(WidgetName),m_RootWidget(nullptr) {}
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
		//处理交互
		void HandleInteraction()
		{
			if (bAllowDrag || bAllowDragOn)
			{
				ImGuiWindow* window = ImGui::GetCurrentWindow();
				const ImGuiID id = window->GetID(m_WidgetName.c_str());
				const ImRect bb(Position, Position + Size);
				if (!ImGui::ItemAdd(bb, id)) {}

				bool hovered, held;
				bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);

				if (bAllowDrag)
				{
					// ================= 拖拽源处理 =================
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


						//ImGui::Text("Dragging %s", m_WidgetName.c_str());

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
								//ImGuiWindowFlags_NoBorder |
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

					// ================= 拖拽取消处理 =================
					if (!m_IsDragSource && m_IsDragging)
					{
						OnDragCancel();
						HandleDragFinish();
						m_CurrentDragHandle = nullptr;
					}

					m_IsDragging = m_IsDragSource;
				}

				if (bAllowDragOn)
				{
					// ================= 拖拽目标处理 =================
					if (ImGui::BeginDragDropTarget())
					{
						m_IsDragTarget = true;


						bool test = ImGui::IsMouseDown(0);
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
			if (m_RootWidget)
			{
				m_RootWidget->Render();
			}

			HandleInteraction();

		}

		void SetRootWidget(ImWidget* RootWidget)
		{
			m_RootWidget = RootWidget;
			if (m_RootWidget)
			{
				m_RootWidget->SetPosition(Position);
				m_RootWidget->SetSize(Size);
			}
		}
	};
}