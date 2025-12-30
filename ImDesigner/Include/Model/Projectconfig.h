#pragma once
#include <json.hpp>
#include <fstream>

class ProjectConfig
{
private:
	std::string UIFolder;
	std::string CPPFolder;
	std::string HeaderFolder;
	std::string CurrentConfigFilePath;
	void LoadConfigFromPath(const std::string& FilePath)
	{
		CurrentConfigFilePath = FilePath;
		std::ifstream file(FilePath);
		if (!file.is_open())
		{
			return;
		}

		nlohmann::ordered_json j = nlohmann::ordered_json::parse(file);

		auto& UIFolder_J = j["UIFolder"];
		if (!UIFolder_J.is_null())
		{
			UIFolder = UIFolder_J.template get<std::string>();
		}


		auto& CPPFolder_J = j["CPPFolder"];
		if (!CPPFolder_J.is_null())
		{
			CPPFolder = CPPFolder_J.template get<std::string>();
		}

		auto& HeaderFolder_J = j["HeaderFolder"];
		if (!HeaderFolder_J.is_null())
		{
			HeaderFolder = HeaderFolder_J.template get<std::string>();
		}
	}
public:
	ProjectConfig():
		UIFolder("./"),
		CPPFolder("./"),
		HeaderFolder("./")
	{
		LoadConfigFromPath("./ProjectConfig.json");
	}
	ProjectConfig(const std::string& FilePath)
	{
		LoadConfigFromPath(FilePath);
	}
	std::string GetUIFolder() { return UIFolder; }
	std::string GetCppFolder() { return CPPFolder; }
	std::string GetHeaderFolder() { return HeaderFolder; }
	std::string GetCurrentConfigFilePath() { return CurrentConfigFilePath; }
};