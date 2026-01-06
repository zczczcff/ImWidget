#pragma once
#include <string>
#include <functional>
#include <vector>
#include "UI/IconManager.h"

namespace ImGuiWidget
{
	class ImWidget;
}
struct WidgetInfor
{
	std::string EN_DisplayName;
	std::string CN_DisplayName;
	std::string RegisterName;
	ImDesignerIcon IconID = (ImDesignerIcon)-1;
};

class BasicWidgetList
{
public:
	static std::vector<WidgetInfor> GetBasicWidgetList();
};