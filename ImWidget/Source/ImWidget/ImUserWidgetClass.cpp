#include "ImWidget/ImUserWidgetClass.h"
#include "ImWidget/ImWidgetFactory.h"
#include "ImWidget/ImObjectFactory.h"
namespace ImGuiWidget
{
    ImUserWidgetClass::variable* ImUserWidgetClass::AddWidgetVar(const std::string& varName, const std::string& RegisterName, variableQualifiers qualifier)
    {
		if (FindVariable(varName)) return nullptr;
		if (ImWidget* NewWidget = ImWidgetFactory::GetInstance().CreateWidget(RegisterName, varName))
		{
			variable newvar =
			{
				varName ,
				variableType::widget,
				qualifier,
				PropertyType::Struct,
				NewWidget
			};
			m_vars.push_back(newvar);
			return &m_vars.back();
		}
		return nullptr;
    }
    ImUserWidgetClass::variable* ImUserWidgetClass::AddStructVar(const std::string& varName, const std::string& RegisterName, variableQualifiers qualifier)
    {
		if (FindVariable(varName)) return nullptr;

        return nullptr;
    }
    ImUserWidgetClass::variable* ImUserWidgetClass::AddSinglePropertyVar(const std::string& varName, PropertyType Ptype, variableQualifiers qualifier)
    {
        return nullptr;
    }
    bool ImUserWidgetClass::RemovePropertyByName(const std::string& varName)
    {
        return false;
    }
    nlohmann::ordered_json ImUserWidgetClass::ToJson() const
	{
		nlohmann::ordered_json j;

		j["Name"] = ClassName;
		j["BaseClassName"] = BaseClassName;

		nlohmann::ordered_json varsJson = nlohmann::ordered_json::array();
		for (auto& var : m_vars)
		{
			nlohmann::ordered_json singleVarJson;
			singleVarJson["VarName"] = var.varName;
			singleVarJson["Qualifier"] = static_cast<int>(var.Qtype);

			if (var.Vtype == variableType::widget)
			{
				singleVarJson = SerializeWidgetTree(static_cast<ImWidget*>(var.var));
				singleVarJson["VarType"] = "Widget";
			}
			else if (var.Vtype == variableType::ImObject)
			{
				singleVarJson = SerializeProperty(PropertyType::Struct,static_cast<ImObject*>(var.var));
				singleVarJson["VarType"] = "Struct";
			}
			else if (var.Vtype == variableType::property)
			{
				singleVarJson = SerializeProperty(var.Ptype, var.var);
				//singleVarJson["VarType"] = PropertyTypeToString(var.Ptype);
			}
			
			varsJson.push_back(singleVarJson);
		}
		if (!varsJson.empty())
		{
			j["Vars"] = varsJson;
		}

		return j;
	}

	bool ImUserWidgetClass::FromJson(const nlohmann::ordered_json& j)
	{
		if (j.empty()) return false;


		// 读取类名和基类名
		if (j.contains("Name") && j["Name"].is_string())
		{
			ClassName = j["Name"].get<std::string>();
		}

		if (j.contains("BaseClassName") && j["BaseClassName"].is_string())
		{
			BaseClassName = j["BaseClassName"].get<std::string>();
		}

		// 清空现有变量
		m_vars.clear();

		// 读取变量数组
		if (j.contains("Vars") && j["Vars"].is_array())
		{
			for (const auto& varJson : j["Vars"])
			{
				if (!varJson.contains("VarName") || !varJson.contains("VarType"))
				{
					continue; // 跳过无效的变量数据
				}

				std::string varName = varJson["VarName"].get<std::string>();
				std::string varTypeStr = varJson["VarType"].get<std::string>();

				variable newVar;
				newVar.varName = varName;

				// 设置变量类型
				if (varTypeStr == "Widget")
				{
					newVar.Vtype = variableType::widget;
					// 创建widget实例
					ImWidget* widget = CreateWidgetFromJson(varJson);
					if (widget)
					{
						newVar.var = widget;
					}
					else
					{
						continue; // 创建失败，跳过
					}
				}
				else if (varTypeStr == "Struct")
				{
					newVar.Vtype = variableType::ImObject;
					// 创建ImObject实例
					ImObject* structPtr = new ImObject();
					if (varJson.is_object())
					{
						DeserializeProperties(structPtr, varJson);
					}
					newVar.var = structPtr;
				}
				else
				{
					newVar.Vtype = variableType::property;
					// 转换属性类型
					//newVar.Ptype = StringToPropertyType(varTypeStr);

					// 根据类型创建相应的变量并反序列化
					switch (newVar.Ptype)
					{
					case PropertyType::Color:
					{
						ImU32* colorPtr = new ImU32(IM_COLOR_SET_RGBA(255, 255, 255, 255));
						if (DeserializeProperty(newVar.Ptype, colorPtr, varJson))
						{
							newVar.var = colorPtr;
						}
						else
						{
							delete colorPtr;
							continue;
						}
						break;
					}
					case PropertyType::Float:
					{
						float* floatPtr = new float(0.0f);
						if (DeserializeProperty(newVar.Ptype, floatPtr, varJson))
						{
							newVar.var = floatPtr;
						}
						else
						{
							delete floatPtr;
							continue;
						}
						break;
					}
					case PropertyType::Bool:
					{
						bool* boolPtr = new bool(false);
						if (DeserializeProperty(newVar.Ptype, boolPtr, varJson))
						{
							newVar.var = boolPtr;
						}
						else
						{
							delete boolPtr;
							continue;
						}
						break;
					}
					case PropertyType::Int:
					{
						int* intPtr = new int(0);
						if (DeserializeProperty(newVar.Ptype, intPtr, varJson))
						{
							newVar.var = intPtr;
						}
						else
						{
							delete intPtr;
							continue;
						}
						break;
					}
					case PropertyType::String:
					{
						std::string* stringPtr = new std::string();
						if (DeserializeProperty(newVar.Ptype, stringPtr, varJson))
						{
							newVar.var = stringPtr;
						}
						else
						{
							delete stringPtr;
							continue;
						}
						break;
					}
					case PropertyType::Vec2:
					{
						ImVec2* vec2Ptr = new ImVec2(0, 0);
						if (DeserializeProperty(newVar.Ptype, vec2Ptr, varJson))
						{
							newVar.var = vec2Ptr;
						}
						else
						{
							delete vec2Ptr;
							continue;
						}
						break;
					}
					case PropertyType::StringArray:
					{
						std::vector<std::string>* arrayPtr = new std::vector<std::string>();
						if (DeserializeProperty(newVar.Ptype, arrayPtr, varJson))
						{
							newVar.var = arrayPtr;
						}
						else
						{
							delete arrayPtr;
							continue;
						}
						break;
					}
					case PropertyType::Enum:
					{
						std::vector<std::string>* enumPtr = new std::vector<std::string>();
						if (DeserializeProperty(newVar.Ptype, enumPtr, varJson))
						{
							newVar.var = enumPtr;
						}
						else
						{
							delete enumPtr;
							continue;
						}
						break;
					}
					default:
						continue; // 未知类型，跳过
					}
				}

				// 设置默认的访问限定符（由于JSON中没有保存这个信息，使用默认值）
				newVar.Qtype = variableQualifiers::q_Private;

				// 添加到变量列表
				m_vars.push_back(newVar);
			}
		}

		return true;
	}


    const ImUserWidgetClass::variable* ImUserWidgetClass::FindVariable(const std::string& varName) const
    {
        for (const auto& var : m_vars)
        {
            if (var.varName == varName)
            {
                return &var;
            }
        }
        return nullptr;
    }

    void ImUserWidgetClass::DeletePropertyValue(PropertyType propType, void* valuePtr)
    {
        if (!valuePtr) return;

        switch (propType)
        {
        case PropertyType::Color:
            delete static_cast<ImU32*>(valuePtr);
            break;
        case PropertyType::Float:
            delete static_cast<float*>(valuePtr);
            break;
        case PropertyType::Bool:
            delete static_cast<bool*>(valuePtr);
            break;
        case PropertyType::Int:
            delete static_cast<int*>(valuePtr);
            break;
        case PropertyType::String:
            delete static_cast<std::string*>(valuePtr);
            break;
        case PropertyType::Vec2:
            delete static_cast<ImVec2*>(valuePtr);
            break;
        case PropertyType::StringArray:
            delete static_cast<std::vector<std::string>*>(valuePtr);
            break;
        case PropertyType::Enum:
            delete static_cast<std::vector<std::string>*>(valuePtr);
            break;
        default:
            break;
        }
    }

	bool ImUserWidgetClass::GenCppClassCode(const std::string& headerPath, const std::string& cppPath) const
	{
		return false;
	}
}