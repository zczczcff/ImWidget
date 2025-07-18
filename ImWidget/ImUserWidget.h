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
		ImUserWidget* FromWidget;
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
		
        ImDragHandle* m_CurrentDragHandle = nullptr;
	protected:
		//拖拽相关回调
		virtual ImDragHandle* OnDragBegin() { return nullptr; };
		virtual void OnDragOn(ImDragHandle* OriginalHandle) {};
		virtual void OnDragComplete() {};
		virtual void OnDragCancel() {};
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

		virtual void Render() 
		{
			if (m_RootWidget)
			{
				m_RootWidget->Render();
			}

			ImGuiWindow* window = ImGui::GetCurrentWindow();
			const ImGuiID id = window->GetID(m_WidgetName.c_str());
			const ImRect bb(Position, Position + Size);
			if (!ImGui::ItemAdd(bb, id)) return;

            // ================= 拖拽源处理 =================
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
            {
                m_IsDragSource = true;

                // 拖拽开始，创建拖拽句柄
                m_CurrentDragHandle = OnDragBegin();
                m_CurrentDragHandle->FromPos = Position;
                m_CurrentDragHandle->FromWidget = this;

                // 设置拖拽payload
                ImGui::SetDragDropPayload("ImUserWidget_DragDrop",
                    &m_CurrentDragHandle,
                    sizeof(ImDragHandle*));

                // 显示拖拽预览
                if (m_CurrentDragHandle->Preview)
                {
                    ImVec2 mousePos = ImGui::GetMousePos();
                    m_CurrentDragHandle->Preview->SetPosition(mousePos);
                    m_CurrentDragHandle->Preview->Render();
                }

                ImGui::EndDragDropSource();
            }
            else
            {
                m_IsDragSource = false;
            }

            // ================= 拖拽目标处理 =================
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
                }

                ImGui::EndDragDropTarget();
            }
            else
            {
                m_IsDragTarget = false;
            }

            // ================= 拖拽取消处理 =================
            if (!m_IsDragSource && m_IsDragging)
            {
				OnDragCancel();
                m_CurrentDragHandle = nullptr;
            }

            m_IsDragging = m_IsDragSource;
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