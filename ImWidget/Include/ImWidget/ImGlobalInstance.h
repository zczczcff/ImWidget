#pragma once

class ImApplication;

namespace ImGuiWidget
{
	class ImGlobalInstance
	{
	private:
		ImApplication* m_GlobalApp;
		class ImWidgetFactory* m_WidgetFactory;
		class ImObjectFactory* m_ObjectFactory;
		
	public:
		ImGlobalInstance();
		ImWidgetFactory* GetWidgetFactory() { return m_WidgetFactory; }
		ImObjectFactory* GetObjectFactory() { return m_ObjectFactory; }
		ImApplication* GetGlobalApp() { return m_GlobalApp; }
		void SetGlobalApp(ImApplication* app) { m_GlobalApp = app; }
	};
	extern ImGlobalInstance* GetGlobalInstance();
}