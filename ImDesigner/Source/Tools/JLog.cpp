#include "Tools/JLog.h"
#include <iostream>

static std::function<void(const std::string&)> LogFun;

void SetLogFun(std::function<void(const std::string&)> Fun)
{
	LogFun = Fun;
}

void AddLogLine(const std::string& Msg)
{
#ifdef J_DEBUG_MODE
	std::cout << Msg.c_str() << std::endl;
#endif // DEBUG

	if (LogFun)
	{
		LogFun(Msg);
	}
}

void AddLogLine(const char* Msg)
{
	AddLogLine(std::string(Msg));
}