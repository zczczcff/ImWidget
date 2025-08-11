#pragma once
#include <json.hpp>
#include <fstream>

class ImCreator_ProjectConfig
{
public:
	std::string UIFolder;
	std::string CPPFolder;
	std::string HeaderFolder;

public:
	ImCreator_ProjectConfig():
		UIFolder("./"),
		CPPFolder("./"),
		HeaderFolder("./")
	{
		std::ifstream file("./ProjectConfig.json");
		if (!file.is_open())
		{
			return;
		}

		nlohmann::ordered_json j = nlohmann::ordered_json::parse(file);

		auto UIFolder_J = j["UIFolder"];
		if (!UIFolder_J.is_null())
		{
			UIFolder = UIFolder_J.template get<std::string>();
		}


		auto CPPFolder_J = j["CPPFolder"];
		if (!CPPFolder_J.is_null())
		{
			CPPFolder = CPPFolder_J.template get<std::string>();
		}

		auto HeaderFolder_J = j["HeaderFolder"];
		if (!HeaderFolder_J.is_null())
		{
			HeaderFolder = HeaderFolder_J.template get<std::string>();
		}
	}

};