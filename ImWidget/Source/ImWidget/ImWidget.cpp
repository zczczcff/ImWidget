#include "ImWidget/ImWidget.h"
#include "ImWidget/ImPanelWidget.h"
#include "ImEvent/ImEvent.h"
#include "ImEvent/ImHoverEvent.h"
#include "ImGlobal.h"
// 事件系统新增：事件处理入口


//控件最小尺寸发生变化时调用

void ImGuiWidget::ImWidget::MarkSizeDirty()
{
	ImWidget* p = GetParents();
	while (p)
	{
		p->HandleChildSizeDirty();
		p = p->GetParents();
	}
}

void ImGuiWidget::ImWidget::HandleEvent(ImEvent* event)
{
	// 调用虚函数处理（子类可以重写）
	if (!event->IsHandled())
	{
		if (event->GetType() == ImEventType::HoverIn&&bHoverable)
		{
			bHovered = true;
			OnHoverStart();
			event->As<ImHoverInEvent>()->SetHoverTime_Second(m_HoverTime);
		}
		else if (event->GetType() == ImEventType::Hover && bHoverable)
		{
			OnHover();
		}
		else if (event->GetType() == ImEventType::HoverOut && bHoverable)
		{
			bHovered = false;
			OnHoverEnd();
		}
		HandleEventInternal(event);
	}
}

// 请求焦点

bool ImGuiWidget::ImWidget::RequestFocus()
{
	if (!IsFocusable()) return false;

	ImGuiWidget::RequestFocus(this);
}

// 构建从当前控件到目标控件的路径
// 如果目标控件不在当前控件的子树中，返回空字符串
// 如果目标控件==当前控件，返回 "."


// 根据路径查找子控件（路径格式：child1/child2/grandchild  ./child1/child2/grandchild）

ImGuiWidget::ImWidget* ImGuiWidget::ImWidget::FindChildByPath(const std::string& path)
{
	if (path.empty()) return this;

	// 分割路径
	std::vector<std::string> pathParts;
	std::stringstream ss(path);
	std::string part;
	while (std::getline(ss, part, '/'))
	{
		if (!part.empty())
		{
			pathParts.push_back(part);
		}
	}

	if (pathParts.empty()) return this;

	// 处理绝对路径
	ImWidget* current = this;

	// 遍历路径部分
	for (const auto& widgetName : pathParts)
	{
		// 特殊路径处理
		if (widgetName == ".")
		{
			// 当前控件，继续
			continue;
		}
		else if (widgetName == "..")
		{
			// 父控件
			current = current->m_Parents;
			if (!current) return nullptr;
			continue;
		}

		// 查找子控件
		ImWidget* foundChild = nullptr;

		// 遍历子控件
		for (int j = 0; j < current->GetChildNum(); ++j)
		{
			ImWidget* child = current->GetChildAt(j);
			if (child && child->GetWidgetName() == widgetName)
			{
				foundChild = child;
				break;
			}
		}

		if (!foundChild) return nullptr;
		current = foundChild;
	}

	return current;
}

std::string ImGuiWidget::ImWidget::BuildPathTo(ImWidget* descendant)
{
	if (!descendant || !descendant->IsInTree(this))
	{
		return "";
	}

	std::vector<std::string> pathParts;
	ImWidget* current = descendant;

	// 向上遍历直到当前控件
	while (current != this && current != nullptr)
	{
		pathParts.push_back(current->GetWidgetName());
		current = current->m_Parents;
	}
	pathParts.push_back(".");
	// 反转路径（从当前控件到目标控件）
	std::reverse(pathParts.begin(), pathParts.end());

	// 构建路径字符串
	std::stringstream pathStream;
	for (size_t i = 0; i < pathParts.size(); ++i)
	{
		if (i > 0) pathStream << "/";
		pathStream << pathParts[i];
	}

	return pathStream.str();
}
