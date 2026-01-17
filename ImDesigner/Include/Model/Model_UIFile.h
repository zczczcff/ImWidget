#pragma once
#include <vector>
#include <string>
class Model_UIFile
{
private:
	enum class variableType
	{
		widget,
		style
	};
	enum class variableQualifiers
	{
		q_Public,
		q_Protected,
		q_Private
	};
	struct variable
	{
		variableType Vtype;
		variableQualifiers Qtype;
		void* var;
	};
private:
	std::string Path;
	std::vector<variable> m_vars;

	bool DeserializeFromUIFile(const std::string& FilePath);
	bool SerializeToUIFile(const std::string& FilePath);

public:
	Model_UIFile(const std::string& Path) :Path(Path)
	{

	}
};