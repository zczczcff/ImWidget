#pragma once
#include <string>
#include <functional>
#include <vector>

namespace ImGuiWidget
{
	class ImWidget;
}
struct WidgetInfor
{
	std::string EN_DisplayName;
	std::string CN_DisplayName;
	std::string RegisterName;
};

class BasicWidgetList
{
public:
	static std::vector<WidgetInfor> GetBasicWidgetList();
};