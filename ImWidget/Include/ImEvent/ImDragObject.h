#pragma once
#include <string>
namespace ImGuiWidget
{
	class ImDragObject
	{
	protected:
		std::string m_ID;
		class ImWidget* FromWidget = nullptr;
		ImWidget* Preview = nullptr;
	public:
		void SetID(const std::string& ID) { m_ID = ID; }
		std::string GetID() { return m_ID; }
		void SetFromWidget(ImWidget* widget) { FromWidget = widget; }
		ImWidget* GetFromWidget() { return FromWidget; }
		void SetPreviewWidget(ImWidget* widget) { Preview = widget; }
		ImWidget* GetPreviewWidget() { return Preview; }
		virtual ~ImDragObject(){}
	};
}