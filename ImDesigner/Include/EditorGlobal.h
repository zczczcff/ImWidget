#pragma once
#include <EditorKit/KEventBus.h>
#include <EditorKit/ActionSystem.h>
#include <EditorKit/StaticString.h>
#include "Tools/JLog.h"
using KeyStringType = StaticString;
using EditorActionID = ActionHandle<KeyStringType>;
using EditorEventID = ActionHandle<KeyStringType>;
class EditorGlobal
{
private:
	EventBus<KeyStringType>* m_eventbus;
	ActionSystem<KeyStringType>* m_ActionSystem;
	EditorGlobal():
		m_eventbus(new EventBus<KeyStringType>()),
		m_ActionSystem(new ActionSystem<KeyStringType>)
	{
		m_ActionSystem->AddGlobalCompletionListener([](const KeyStringType& actionName, const ActionResult& result)
			{
                std::stringstream ss;


                // 基本信息
                ss << "\n";
                ss << u8"┌─ Action执行报告 ──────────────────────────────────\n";
                ss << u8"│ Action:     " << actionName.toString() << "\n";
                ss << u8"│ 状态:       " <<  (result.success ? u8" 成功" : u8" 失败") << "\n";
                ss << u8"│ 验证:       " << (result.validationPassed ? u8" 通过" : u8" 未通过") << "\n";

                // 统计信息
                ss << u8"│\n";
                ss << u8"│ 统计信息:\n";
                ss << u8"│   验证器:   " << result.passedValidators << "/" << result.totalValidators
                    << u8" (" << (result.totalValidators > 0 ?
                        std::to_string(result.passedValidators * 100 / result.totalValidators) : "0")
                    << u8"%)\n";
                ss << u8"│   处理器:   " << result.executedProcessors << "/" << result.totalProcessors
                    << u8" (" << (result.totalProcessors > 0 ?
                        std::to_string(result.executedProcessors * 100 / result.totalProcessors) : "0")
                    << u8"%)\n";
                ss << u8"│   监听器:   " << result.executedListeners << "/" << result.totalListeners
                    << u8" (" << (result.totalListeners > 0 ?
                        std::to_string(result.executedListeners * 100 / result.totalListeners) : "0")
                    << u8"%)\n";

                // 错误信息
                if (!result.errorMessage.empty())
                {
                    ss << u8"│\n";
                    ss << u8"│ 错误信息:\n";
                    ss << u8"│   " << result.errorMessage << "\n";
                }

                ss << u8"└────────────────────────────────────────────────────";

                AddLogLine(ss.str());
			});
	}
	static EditorGlobal* GetInstance()
	{
		static EditorGlobal instance;
		return &instance;
	}

public:
	static EventBus<KeyStringType>* GetEventBusInstance()
	{
		return GetInstance()->m_eventbus;
	}
	static ActionSystem<KeyStringType>* GetActionSystemInstance()
	{
		return GetInstance()->m_ActionSystem;
	}
};