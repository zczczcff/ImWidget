#pragma once

class Controller_MainController
{
private:
	class MainUI* m_MainUI;
	class Model_MainModel* m_MainModel;
public:
	Controller_MainController(class MainUI* m_MainUI, class Model_MainModel* m_MainModel);
};