#include "ImEvent/ImEventSystem.h"
#include "ImWidget/ImPanelWidget.h"
#include "ImEvent/ImEvent.h"
#include "ImEvent/ImMouseEvent.h"
#include "ImEvent/ImKeyEvent.h"
#include "ImEvent/ImFocusEvent.h"
#include "ImEvent/ImDragEvent.h"
#include "ImEvent/ImInputEvent.h"
namespace ImGuiWidget
{
	ImGuiWidget::ImEventSystem::ImEventSystem(ImWidget* root) : m_rootWidget(root) {}

	// 主处理入口

	void ImGuiWidget::ImEventSystem::ProcessEvents()
	{
		CollectEventsFromImGui();
		DispatchEvents();
	}

	// 从ImGui收集事件

	void ImGuiWidget::ImEventSystem::CollectEventsFromImGui()
	{
		ImGuiIO& io = ImGui::GetIO();

		CollectMouseEvents(io);
		CollectKeyboardEvents(io);
		CollectDragEvents(io);
		CollectHoverEvents(io);
	}

	void ImEventSystem::CollectMouseEvents(ImGuiIO& io)
	{
		// 获取当前修饰键状态
		ImModifierKeys mods = ImModifierKeys::GetCurrent();

		// 鼠标移动事件
		if (!ImVec2Equals(io.MousePos, m_lastMousePos))
		{
			auto event = std::make_unique<ImMouseMoveEvent>();
			event->SetPosition(io.MousePos);
			event->SetDelta(ImVec2(io.MousePos.x - m_lastMousePos.x,
				io.MousePos.y - m_lastMousePos.y));
			event->SetModifiers(mods);
			m_eventQueue.push_back(std::move(event));
			m_lastMousePos = io.MousePos;
		}

		// 鼠标按钮事件
		for (int button = 0; button < 5; ++button)
		{
			// 鼠标按下
			if (ImGui::IsMouseClicked(button))
			{
				HandleMouseButtonEvent(ImEventType::MouseDown, button, io.MousePos, mods);
			}

			// 鼠标释放
			if (ImGui::IsMouseReleased(button))
			{
				HandleMouseButtonEvent(ImEventType::MouseUp, button, io.MousePos, mods);
			}
		}

		// 鼠标滚轮
		if (io.MouseWheel != 0 || io.MouseWheelH != 0)
		{
			auto event = std::make_unique<ImMouseWheelEvent>(
				ImVec2(io.MouseWheelH, io.MouseWheel));
			event->SetPosition(io.MousePos);
			event->SetModifiers(mods);
			m_eventQueue.push_back(std::move(event));
		}

		// 新增：滚轮按下检测
		//if (ImGui::IsMouseClicked(2)) { // 中键按下
		//    auto event = std::make_unique<ImMouseWheelClickEvent>();
		//    event->SetPosition(io.MousePos);
		//    event->SetModifiers(mods);
		//    m_eventQueue.push_back(std::move(event));
		//}

		// 新增：滚轮倾斜检测（如果ImGui支持）
		// 注意：ImGui目前可能不支持滚轮倾斜检测，这里预留接口
		if (io.MouseWheel != 0 || io.MouseWheelH != 0)
		{
			// 可以在这里添加倾斜检测逻辑
		}
	}

	void ImEventSystem::HandleMouseButtonEvent(ImEventType type, int button, const ImVec2& pos, const ImModifierKeys& mods)
	{
		ImMouseButton mouseButton = static_cast<ImMouseButton>(button);
		int clickCount = 1;

		std::unique_ptr<ImMouseEvent> event;

		if (type == ImEventType::MouseDown)
		{
			clickCount = CalculateClickCount(button, pos);
			event = std::make_unique<ImMouseDownEvent>(mouseButton, clickCount);
			
			// 改进的焦点处理逻辑
			ImWidget* hitTarget = HitTest(m_rootWidget, pos);
			if (hitTarget)
			{
				// 查找第一个可聚焦的控件（沿着控件树向上）
				ImWidget* focusTarget = FindFocusableAncestor(hitTarget);
				if (focusTarget && focusTarget->IsFocusable())
				{
					// 发送焦点请求事件
					//auto focusRequest = std::make_unique<ImFocusRequestEvent>(focusTarget, ImFocusReason::User);
					//focusRequest->SetTarget(focusTarget);
					//m_eventQueue.push_back(std::move(focusRequest));
					//焦点请求暂不实现，暂时只使用强制设置焦点及强制清除焦点
					// 直接设置焦点
					SetFocus(focusTarget, ImFocusReason::User);
				}
				else if (m_focusedWidget)
				{
					// 点击非可聚焦区域，清除焦点
					ClearFocus(ImFocusReason::User);
				}
			}
		}
		else
		{
			event = std::make_unique<ImMouseUpEvent>(mouseButton, clickCount);

			// 如果是鼠标释放，额外发送点击事件
			if (clickCount > 0)
			{
				std::unique_ptr<ImMouseEvent> clickEvent;
				if (clickCount >= 2)
				{
					clickEvent = std::make_unique<ImMouseDoubleClickEvent>(mouseButton);
				}
				else
				{
					clickEvent = std::make_unique<ImMouseClickEvent>(mouseButton, clickCount);
				}
				clickEvent->SetPosition(pos);
				clickEvent->SetModifiers(mods);
				m_eventQueue.push_back(std::move(clickEvent));
			}
		}

		event->SetPosition(pos);
		event->SetModifiers(mods);
		m_eventQueue.push_back(std::move(event));
	}

	void ImEventSystem::CollectHoverEvents(ImGuiIO& io)
	{
		// 检测当前鼠标下的控件
		ImWidget* currentHovered = HitTest(m_rootWidget, io.MousePos);

		// 处理鼠标进入/离开事件
		if (currentHovered != m_lastHoveredWidget)
		{
			// 发送离开事件给之前悬停的控件
			if (m_lastHoveredWidget != nullptr)
			{
				auto leaveEvent = std::make_unique<ImMouseLeaveEvent>();
				leaveEvent->SetPosition(io.MousePos);
				leaveEvent->SetTarget(m_lastHoveredWidget);
				m_eventQueue.push_back(std::move(leaveEvent));
			}

			// 发送进入事件给新悬停的控件
			if (currentHovered != nullptr)
			{
				auto enterEvent = std::make_unique<ImMouseEnterEvent>();
				enterEvent->SetPosition(io.MousePos);
				enterEvent->SetTarget(currentHovered);
				m_eventQueue.push_back(std::move(enterEvent));

				// 开始悬停计时
				m_hoverStartTime = ImGui::GetTime();
				m_hoveredWidget = currentHovered;
			}
			else
			{
				m_hoverStartTime = 0.0;
				m_hoveredWidget = nullptr;
			}

			m_lastHoveredWidget = currentHovered;
		}

		// 更新当前悬停控件
		m_hoveredWidget = currentHovered;
	}

	int ImGuiWidget::ImEventSystem::CalculateClickCount(int button, const ImVec2& pos)
	{
		double currentTime = ImGui::GetTime();
		int buttonIndex = static_cast<int>(button);

		// 检查是否在双击时间和距离范围内
		bool isDoubleClick = (currentTime - m_lastClickTime[buttonIndex] <= DOUBLE_CLICK_TIME) &&
			(ImVec2Distance(pos, m_lastClickPos[buttonIndex]) <= DOUBLE_CLICK_DISTANCE);

		int clickCount = isDoubleClick ? 2 : 1;

		// 更新上次点击信息
		m_lastClickTime[buttonIndex] = currentTime;
		m_lastClickPos[buttonIndex] = pos;

		return clickCount;
	}

	void ImGuiWidget::ImEventSystem::CollectKeyboardEvents(ImGuiIO& io)
	{


		const double KEY_REPEAT_DELAY = 0.3;    // 重复按键初始延迟（秒）
		const double KEY_REPEAT_RATE = 0.05;    // 重复按键速率（秒）
		double currentTime = ImGui::GetTime();

		ImModifierKeys mods = ImModifierKeys::GetCurrent();

		// 处理所有命名按键事件
		for (int key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END; ++key)
		{
			bool isKeyDown = ImGui::IsKeyDown((ImGuiKey)key);
			bool wasKeyDown = keyStates[key];

			// 按键状态变化检测
			if (isKeyDown && !wasKeyDown)
			{
				// 按键按下（首次）
				auto event = std::make_unique<ImKeyDownEvent>(key, 0, false);
				event->SetModifiers(mods); // 正确设置修饰键
				if (m_focusedWidget)
				{
					event->SetTarget(m_focusedWidget);
				}
				m_eventQueue.push_back(std::move(event));

				keyStates[key] = true;
				keyPressTime[key] = currentTime;
				keyRepeatCount[key] = 0;
			}
			else if (!isKeyDown && wasKeyDown)
			{
				// 按键释放
				auto event = std::make_unique<ImKeyUpEvent>(key);
				event->SetModifiers(mods); // 正确设置修饰键
				if (m_focusedWidget)
				{
					event->SetTarget(m_focusedWidget);
				}
				m_eventQueue.push_back(std::move(event));

				keyStates[key] = false;
				keyPressTime[key] = 0.0;
				keyRepeatCount[key] = 0;
			}
			else if (isKeyDown && wasKeyDown)
			{
				// 重复按键检测
				double timeSincePress = currentTime - keyPressTime[key];

				if (timeSincePress > KEY_REPEAT_DELAY)
				{
					double repeatInterval = KEY_REPEAT_RATE;
					int expectedRepeatCount = static_cast<int>((timeSincePress - KEY_REPEAT_DELAY) / repeatInterval) + 1;

					if (expectedRepeatCount > keyRepeatCount[key])
					{
						auto event = std::make_unique<ImKeyDownEvent>(key, 0, true);
						event->SetModifiers(mods); // 正确设置修饰键
						if (m_focusedWidget)
						{
							event->SetTarget(m_focusedWidget);
						}
						m_eventQueue.push_back(std::move(event));

						keyRepeatCount[key] = expectedRepeatCount;
					}
				}
			}
		}

		// 处理字符输入（用于文本输入）
		if (io.InputQueueCharacters.Size > 0)
		{
			std::string inputText;

			// 收集所有输入字符
			for (int i = 0; i < io.InputQueueCharacters.Size; i++)
			{
				ImWchar c = io.InputQueueCharacters[i];

				// 过滤控制字符，只处理可打印字符
				if ((c >= 32 && c < 127) ||         // 基本ASCII可打印字符
					(c >= 0xA0 && c < 0x10000) ||   // 扩展字符
					c == '\t')                       // Tab键（作为文本输入）
				{
					// 插入字符 - 支持UTF-8编码
					if (c == '\t')
					{
						// Tab替换为4个空格
						inputText.append("    ");
					}
					else
					{
						// 处理UTF-8字符（最多4字节）
						char utf8_char[5] = { 0 };
						ImTextCharToUtf8(utf8_char, c);
						inputText.append(utf8_char);
					}
				}
			}

			if (!inputText.empty())
			{
				// 创建文本输入事件
				auto textEvent = std::make_unique<ImTextInputEvent>(inputText);
				textEvent->SetModifiers(mods);
				if (m_focusedWidget)
				{
					textEvent->SetTarget(m_focusedWidget);
				}
				m_eventQueue.push_back(std::move(textEvent));
			}
		}

		// 特殊按键处理 - Tab键导航
		if (ImGui::IsKeyPressed(ImGuiKey_Tab, false) && !ImGui::GetIO().KeyCtrl && !ImGui::GetIO().KeyAlt)
		{
			bool reverse = io.KeyShift; // Shift+Tab反向导航
			bool navigationHandled = FocusNextWidget(reverse);

			// 即使进行了导航，也发送Tab键事件
			auto tabEvent = std::make_unique<ImKeyDownEvent>(ImGuiKey_Tab);
			tabEvent->SetModifiers(mods); // 正确设置修饰键
			if (m_focusedWidget)
			{
				tabEvent->SetTarget(m_focusedWidget);
			}
			m_eventQueue.push_back(std::move(tabEvent));

			// 如果导航成功，可以设置WantCaptureKeyboard来阻止ImGui的默认处理
			if (navigationHandled)
			{
				io.WantCaptureKeyboard = true;
			}
		}

		// Escape键清除焦点
		if (ImGui::IsKeyPressed(ImGuiKey_Escape, false) && m_focusedWidget)
		{
			ClearFocus(ImFocusReason::User);

			// 发送Escape键事件
			auto escapeEvent = std::make_unique<ImKeyDownEvent>(ImGuiKey_Escape);
			escapeEvent->SetModifiers(mods); // 正确设置修饰键
			m_eventQueue.push_back(std::move(escapeEvent));
		}

		// 方向键导航增强
		//if (m_focusedWidget)
		//{
		//	// 方向键事件处理
		//	static const std::pair<ImGuiKey, ImGuiKey> arrowKeys[] = {
		//		{ImGuiKey_LeftArrow, ImGuiKey_RightArrow},
		//		{ImGuiKey_UpArrow, ImGuiKey_DownArrow}
		//	};

		//	for (const auto& arrowPair : arrowKeys)
		//	{
		//		if (ImGui::IsKeyPressed(arrowPair.first, false))
		//		{
		//			auto event = std::make_unique<ImKeyDownEvent>(arrowPair.first);
		//			event->SetModifiers(mods); // 正确设置修饰键
		//			event->SetTarget(m_focusedWidget);
		//			m_eventQueue.push_back(std::move(event));
		//		}

		//		if (ImGui::IsKeyPressed(arrowPair.second, false))
		//		{
		//			auto event = std::make_unique<ImKeyDownEvent>(arrowPair.second);
		//			event->SetModifiers(mods); // 正确设置修饰键
		//			event->SetTarget(m_focusedWidget);
		//			m_eventQueue.push_back(std::move(event));
		//		}
		//	}

		//	// 功能键处理（Home, End, PageUp, PageDown等）
		//	static const ImGuiKey functionKeys[] = {
		//		ImGuiKey_Home, ImGuiKey_End,
		//		ImGuiKey_PageUp, ImGuiKey_PageDown,
		//		ImGuiKey_Insert, ImGuiKey_Delete
		//	};

		//	for (ImGuiKey funcKey : functionKeys)
		//	{
		//		if (ImGui::IsKeyPressed(funcKey, false))
		//		{
		//			auto event = std::make_unique<ImKeyDownEvent>(funcKey);
		//			event->SetModifiers(mods); // 正确设置修饰键
		//			event->SetTarget(m_focusedWidget);
		//			m_eventQueue.push_back(std::move(event));
		//		}
		//	}
		//}

		// 修饰键单独事件处理（可选）
		static bool prevModifiers[4] = { false, false, false, false };
		bool currentModifiers[4] = { io.KeyCtrl, io.KeyShift, io.KeyAlt, io.KeySuper };
		static const ImGuiKey modifierKeys[4] = {
			ImGuiKey_LeftCtrl, ImGuiKey_LeftShift, ImGuiKey_LeftAlt, ImGuiKey_LeftSuper
		};

		for (int i = 0; i < 4; ++i)
		{
			if (currentModifiers[i] && !prevModifiers[i])
			{
				// 修饰键按下
				auto event = std::make_unique<ImKeyDownEvent>(modifierKeys[i]);
				event->SetModifiers(mods); // 正确设置修饰键
				if (m_focusedWidget)
				{
					event->SetTarget(m_focusedWidget);
				}
				m_eventQueue.push_back(std::move(event));
			}
			else if (!currentModifiers[i] && prevModifiers[i])
			{
				// 修饰键释放
				auto event = std::make_unique<ImKeyUpEvent>(modifierKeys[i]);
				event->SetModifiers(mods); // 正确设置修饰键
				if (m_focusedWidget)
				{
					event->SetTarget(m_focusedWidget);
				}
				m_eventQueue.push_back(std::move(event));
			}
			prevModifiers[i] = currentModifiers[i];
		}

		// 组合快捷键检测（示例：Ctrl+A, Ctrl+C, Ctrl+V等）
		if (io.KeyCtrl && m_focusedWidget)
		{
			static const std::pair<ImGuiKey, const char*> shortcuts[] = {
				{ImGuiKey_A, "SelectAll"},
				{ImGuiKey_C, "Copy"},
				{ImGuiKey_V, "Paste"},
				{ImGuiKey_X, "Cut"},
				{ImGuiKey_Z, "Undo"},
				{ImGuiKey_Y, "Redo"},
				{ImGuiKey_S, "Save"}
			};

			for (const auto& shortcut : shortcuts)
			{
				if (ImGui::IsKeyPressed(shortcut.first, false))
				{
					auto event = std::make_unique<ImKeyDownEvent>(shortcut.first);
					event->SetModifiers(mods); // 正确设置修饰键
					event->SetTarget(m_focusedWidget);
					m_eventQueue.push_back(std::move(event));

					// 可以在这里添加特殊处理逻辑
					// 例如：io.WantCaptureKeyboard = true;
					break;
				}
			}
		}
	}

	void ImGuiWidget::ImEventSystem::CollectDragEvents(ImGuiIO& io)
	{
		ImModifierKeys mods = ImModifierKeys::GetCurrent();
		// 检查是否开始拖拽
		if (!m_isDragging && ImGui::IsMouseDragging(0))
		{ // 左键拖拽
// 检查拖拽阈值（避免误触）
			ImVec2 dragDelta = ImGui::GetMouseDragDelta(0);
			if (ImVec2Distance(ImVec2(0, 0), dragDelta) > 2.0f)
			{ // 2像素阈值
				StartDrag(io.MousePos - dragDelta, mods);
			}
		}

		// 处理拖拽更新
		if (m_isDragging)
		{
			if (ImGui::IsMouseDragging(0))
			{
				UpdateDrag(io.MousePos, mods);
			}
			else
			{
				// 鼠标释放，结束拖拽
				EndDrag(io.MousePos, mods);
			}
		}

		// 处理拖放目标
		if (m_isDragging)
		{
			ProcessDropTargets(io.MousePos, mods);
		}
	}

	// 事件分发

	void ImGuiWidget::ImEventSystem::DispatchEvents()
	{
		for (auto& eventPtr : m_eventQueue)
		{
			if (!eventPtr) continue;

			// 克隆事件用于分发
			std::unique_ptr<ImEvent> event(eventPtr->Clone());

			// 设置目标控件
			if (ImWidget* target = event->GetTarget())
			{
				// 计算本地坐标
				if (auto mouseEvent = event->As<ImMouseEvent>())
				{
					mouseEvent->SetLocalPosition(CalculateLocalPosition(mouseEvent->GetPosition(), target));
				}

				// 事件传播
				DispatchEventThroughHierarchy(event.get(), target);
			}
			else
			{
				target = FindEventTarget(event.get());
				if (!target) continue;

				event->SetTarget(target);

				// 计算本地坐标
				if (auto mouseEvent = event->As<ImMouseEvent>())
				{
					mouseEvent->SetLocalPosition(CalculateLocalPosition(mouseEvent->GetPosition(), target));
				}

				// 事件传播
				DispatchEventThroughHierarchy(event.get(), target);
			}

		}

		m_eventQueue.clear();
	}

	ImWidget* ImGuiWidget::ImEventSystem::FindEventTarget(ImEvent* event)
	{
		if (auto mouseEvent = event->As<ImMouseEvent>())
		{
			return HitTest(m_rootWidget, mouseEvent->GetPosition());
		}
		return m_focusedWidget;  // 键盘事件发送给焦点控件
	}

	ImVec2 ImGuiWidget::ImEventSystem::CalculateLocalPosition(const ImVec2& globalPos, ImWidget* widget)
	{
		if (!widget) return globalPos;
		return ImVec2(globalPos.x - widget->GetPosition().x,
			globalPos.y - widget->GetPosition().y);
	}

	void ImGuiWidget::ImEventSystem::DispatchEventThroughHierarchy(ImEvent* event, ImWidget* target)
	{
		// 构建传播路径
		std::vector<ImWidget*> path;
		ImWidget* current = target->GetParents();
		while (current)
		{
			path.push_back(current);
			current = current->GetParents();
		}

		// 捕获阶段（从根到目标）
		event->SetPhase(ImEventPhase::Capture);
		for (auto it = path.rbegin(); it != path.rend() && !event->IsHandled(); ++it)
		{
			event->SetCurrentTarget(*it);
			(*it)->HandleEvent(event);
		}

		// 目标阶段
		if (!event->IsHandled())
		{
			event->SetPhase(ImEventPhase::Target);
			event->SetCurrentTarget(target);
			target->HandleEvent(event);
		}

		// 冒泡阶段（从目标到根）
		if (event->Bubbles() && !event->IsHandled())
		{
			event->SetPhase(ImEventPhase::Bubble);
			for (auto it = path.begin(); it != path.end() && !event->IsHandled(); ++it)
			{
				event->SetCurrentTarget(*it);
				(*it)->HandleEvent(event);
			}
		}
	}

	// HitTest实现（与之前相同）

	ImWidget* ImGuiWidget::ImEventSystem::HitTest(ImWidget* widget, const ImVec2& point)
	{
		if (!widget || !widget->IsVisible()) return nullptr;

		if (auto panel = dynamic_cast<ImPanelWidget*>(widget))
		{
			for (int i = panel->GetSlotNum() - 1; i >= 0; --i)
			{
				if (auto child = panel->GetChildAt(i))
				{
					if (auto result = HitTest(child, point))
					{
						return result;
					}
				}
			}
		}

		ImRect rect(widget->GetPosition(), widget->GetPosition() + widget->GetSize());
		return rect.Contains(point) ? widget : nullptr;
	}

	// 工具函数

	bool ImGuiWidget::ImEventSystem::ImVec2Equals(const ImVec2& a, const ImVec2& b, float epsilon)
	{
		return std::abs(a.x - b.x) < epsilon && std::abs(a.y - b.y) < epsilon;
	}

	float ImGuiWidget::ImEventSystem::ImVec2Distance(const ImVec2& a, const ImVec2& b)
	{
		float dx = a.x - b.x;
		float dy = a.y - b.y;
		return std::sqrt(dx * dx + dy * dy);
	}

	void ImEventSystem::StartDrag(const ImVec2& pos, const ImModifierKeys& mods)
	{
		m_isDragging = true;
		m_dragStartPos = pos;
		m_dragSourceWidget = m_lastHoveredWidget;

		// 创建拖拽开始事件
		auto event = std::make_unique<ImDragStartEvent>();
		event->SetTarget(m_dragSourceWidget);
		event->SetPosition(pos);
		event->SetModifiers(mods);

		m_eventQueue.push_back(std::move(event));
	}

	void ImEventSystem::UpdateDrag(const ImVec2& pos, const ImModifierKeys& mods)
	{
		auto event = std::make_unique<ImDragUpdateEvent>();
		event->SetPosition(pos);
		event->SetTarget(m_dragSourceWidget);
		event->SetDelta(ImVec2(pos.x - m_lastDragPos.x, pos.y - m_lastDragPos.y));
		event->SetModifiers(mods);

		m_eventQueue.push_back(std::move(event));
		m_lastDragPos = pos;
	}

	void ImEventSystem::EndDrag(const ImVec2& pos, const ImModifierKeys& mods)
	{
		auto event = std::make_unique<ImDragEndEvent>();
		event->SetPosition(pos);
		event->SetTarget(m_dragSourceWidget);
		event->SetModifiers(mods);

		// 处理拖放
		ImWidget* dropTarget = HitTest(m_rootWidget, pos);
		if (dropTarget && dropTarget != m_dragSourceWidget)
		{
			ProcessDrop(dropTarget, pos, mods);
		}

		m_eventQueue.push_back(std::move(event));

		// 重置拖拽状态
		m_isDragging = false;
		m_dragSourceWidget = nullptr;
		m_lastDragPos = ImVec2(0, 0);
		ImDragEvent::SetDragData(nullptr);
	}

	void ImEventSystem::ProcessDropTargets(const ImVec2& pos, const ImModifierKeys& mods)
	{
		ImWidget* currentTarget = HitTest(m_rootWidget, pos);

		// 处理拖拽进入/离开事件
		if (currentTarget != m_lastDragHoveredWidget)
		{
			// 发送拖拽离开事件给之前的控件
			if (m_lastDragHoveredWidget)
			{
				auto leaveEvent = std::make_unique<ImDragEvent>(ImEventType::DragLeave);
				leaveEvent->SetPosition(pos);
				leaveEvent->SetModifiers(mods);
				leaveEvent->SetTarget(m_lastDragHoveredWidget);
				m_eventQueue.push_back(std::move(leaveEvent));
			}

			// 发送拖拽进入事件给新控件
			if (currentTarget)
			{
				auto enterEvent = std::make_unique<ImDragEvent>(ImEventType::DragEnter);
				enterEvent->SetPosition(pos);
				enterEvent->SetModifiers(mods);
				enterEvent->SetTarget(currentTarget);
				m_eventQueue.push_back(std::move(enterEvent));
			}

			m_lastDragHoveredWidget = currentTarget;
		}

		// 发送拖拽经过事件给当前控件
		if (currentTarget)
		{
			auto overEvent = std::make_unique<ImDragEvent>(ImEventType::DragOver);
			overEvent->SetPosition(pos);
			overEvent->SetModifiers(mods);
			overEvent->SetTarget(currentTarget);
			m_eventQueue.push_back(std::move(overEvent));
		}
	}

	void ImEventSystem::ProcessDrop(ImWidget* target, const ImVec2& pos, const ImModifierKeys& mods)
	{
		if (!m_dragSourceWidget) return;

		auto dropEvent = std::make_unique<ImDragEvent>(ImEventType::Drop);
		dropEvent->SetPosition(pos);
		dropEvent->SetModifiers(mods);
		dropEvent->SetTarget(target);

		m_eventQueue.push_back(std::move(dropEvent));
	}

	// 设置焦点到指定控件

	bool ImEventSystem::SetFocus(ImWidget* widget, ImFocusReason reason)
	{
		if (widget && !widget->IsFocusable())
		{
			return false;
		}

		// 如果焦点没有变化，直接返回
		if (widget == m_focusedWidget)
		{
			return true;
		}

		ImWidget* oldFocus = m_focusedWidget;

		// 发送失去焦点事件
		if (oldFocus)
		{
			auto focusOutEvent = std::make_unique<ImFocusOutEvent>(widget, reason);
			focusOutEvent->SetTarget(oldFocus);
			DispatchEventImmediately(focusOutEvent.get());
			oldFocus->LoseFocus();
		}

		// 设置新焦点
		m_focusedWidget = widget;

		// 发送获得焦点事件
		if (m_focusedWidget)
		{
			auto focusInEvent = std::make_unique<ImFocusInEvent>(oldFocus, reason);
			focusInEvent->SetTarget(m_focusedWidget);
			DispatchEventImmediately(focusInEvent.get());
			m_focusedWidget->GetFocus();
		}

		return true;
	}

	// 获取当前焦点控件

	ImWidget* ImEventSystem::GetFocusedWidget() const
	{
		return m_focusedWidget;
	}

	// 清除焦点

	void ImEventSystem::ClearFocus(ImFocusReason reason)
	{
		SetFocus(nullptr, reason);
	}

	// Tab键导航到下一个可聚焦控件

	bool ImEventSystem::FocusNextWidget(bool reverse)
	{
		if (!m_rootWidget) return false;

		std::vector<ImWidget*> focusableWidgets;
		CollectFocusableWidgets(m_rootWidget, focusableWidgets);

		if (focusableWidgets.empty()) return false;

		// 如果没有当前焦点，聚焦第一个或最后一个控件
		if (!m_focusedWidget)
		{
			ImWidget* target = reverse ? focusableWidgets.back() : focusableWidgets.front();
			return SetFocus(target, ImFocusReason::Navigation);
		}

		// 查找当前焦点在列表中的位置
		auto it = std::find(focusableWidgets.begin(), focusableWidgets.end(), m_focusedWidget);
		if (it == focusableWidgets.end())
		{
			ImWidget* target = reverse ? focusableWidgets.back() : focusableWidgets.front();
			return SetFocus(target, ImFocusReason::Navigation);
		}

		// 计算下一个焦点控件
		ImWidget* nextFocus = nullptr;
		if (reverse)
		{
			if (it == focusableWidgets.begin())
			{
				nextFocus = focusableWidgets.back(); // 循环到最后一个
			}
			else
			{
				nextFocus = *(it - 1);
			}
		}
		else
		{
			if (it + 1 == focusableWidgets.end())
			{
				nextFocus = focusableWidgets.front(); // 循环到第一个
			}
			else
			{
				nextFocus = *(it + 1);
			}
		}

		return SetFocus(nextFocus, ImFocusReason::Navigation);
	}

	// 收集所有可聚焦的控件

	void ImEventSystem::CollectFocusableWidgets(ImWidget* root, std::vector<ImWidget*>& result)
	{
		if (!root || !root->IsVisible()) return;

		if (root->IsFocusable())
		{
			result.push_back(root);
		}

		// 递归收集子控件
		if (auto panel = dynamic_cast<ImPanelWidget*>(root))
		{
			for (int i = 0; i < panel->GetSlotNum(); ++i)
			{
				if (auto child = panel->GetChildAt(i))
				{
					CollectFocusableWidgets(child, result);
				}
			}
		}
	}

	// 立即分发事件（不加入队列）

	void ImEventSystem::DispatchEventImmediately(ImEvent* event)
	{
		if (!event || !event->GetTarget()) return;

		// 计算本地坐标（如果是鼠标事件）
		if (auto mouseEvent = event->As<ImMouseEvent>())
		{
			mouseEvent->SetLocalPosition(CalculateLocalPosition(mouseEvent->GetPosition(), event->GetTarget()));
		}

		DispatchEventThroughHierarchy(event, event->GetTarget());
	}

	// 沿着控件树向上查找第一个可聚焦的控件

	ImWidget* ImEventSystem::FindFocusableAncestor(ImWidget* widget)
	{
		ImWidget* current = widget;
		while (current)
		{
			if (current->IsFocusable())
			{
				return current;
			}
			current = current->GetParents();
		}
		return nullptr;
	}
}