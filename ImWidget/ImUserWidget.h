#pragma once
#include "ImWidget.h"


namespace ImGuiWidget
{
	class ImDragHandle
	{
	public:
		ImVec2 FromPos;
		ImWidget* Preview;
		ImUserWidget* FromWidget;
	};


	class ImUserWidget :public ImWidget
	{
	private:
		ImWidget* m_RootWidget;
		//拖拽相关回调
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

			//拖拽检测代码

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