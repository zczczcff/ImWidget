#pragma once
#include "ImUserWidget.h"
#include "ImWidgetSerializer.h"
#include "ImWidgetCodeGenerator.h"
//该类用于编辑器中编辑一个用户控件类
namespace ImGuiWidget
{
	class ImUserWidgetClass
	{
	public:
		enum class variableType
		{
			widget,
			ImObject,
			property
		};
		enum class variableQualifiers
		{
			q_Public,
			q_Protected,
			q_Private
		};

		union VarValue
		{
			void* v_Voidp;
			ImWidget* v_widget;
			ImObject* v_object;
			int* v_int;
			float* v_flt;
			ImU32* v_color;
			bool* v_bool;
			std::string* v_str;
			ImVec2* v_vec2;
			std::vector<std::string>* v_strarray;
		};

		struct variable
		{
			std::string varName;
			variableType Vtype;
			variableQualifiers Qtype;
			PropertyType Ptype;
			VarValue var;
		};
	private:
		std::string ClassName;
		std::string BaseClassName = "ImUserWidget";
		std::vector<variable> m_vars;
	private:
		const variable* FindVariable(const std::string& varName) const;
		void DeletePropertyValue(PropertyType propType, void* valuePtr);
	public:
		std::string GetClassName() { return ClassName; }
		void SetClassName(const std::string& name) { ClassName = name; }
		size_t GetVariableCount() const
		{
			return m_vars.size();
		}
		variable* GetVariableAt(int index)
		{
			if (index < 0 || index >= static_cast<int>(m_vars.size()))
			{
				return nullptr;
			}
			return &m_vars[index];
		}

		variable* AddWidgetVar(const std::string& varName, const std::string& RegisterName, variableQualifiers qualifier = variableQualifiers::q_Protected);
		variable* AddStructVar(const std::string& varName, const std::string& RegisterName, variableQualifiers qualifier = variableQualifiers::q_Protected);
		variable* AddIntVar(const std::string& varName, variableQualifiers qualifier = variableQualifiers::q_Protected);
		variable* FindVarByName(const std::string& varName);
		bool RenameVar(const std::string& OldName, const std::string& NewName);

		bool RemovePropertyByName(const std::string& varName);


		// 序列化/反序列化
		nlohmann::ordered_json ToJson() const;
		bool FromJson(const nlohmann::ordered_json& j);



		// 生成C++类代码
		bool GenCppClassCode(const std::string& headerPath, const std::string& cppPath) const;

	};
}