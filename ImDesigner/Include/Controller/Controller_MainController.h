#pragma once
#include <map>
#include <string>
#include "EditorGlobalInterface.h"

class Controller_MainController:public EditorGlobalInterface
{
private:
	class MainUI* m_MainUI;
	class Model_MainModel* m_MainModel;

	std::map<std::string, class Controller_WidgetEditor*> WidgetEidtorControllers;
	std::string CurrentFile;
public:
	Controller_MainController(class MainUI* m_MainUI, class Model_MainModel* m_MainModel);

	//void SwitchEditPage(const std::string& PageName);
};