#pragma once
#include "../pch.hpp"

class JsonManager
{
public:
	JsonManager();
	~JsonManager() = default;

	static void save();

private:
	inline static std::filesystem::path m_savefile_directory = std::filesystem::path(std::getenv("appdata")) / "GD-Roulette";
	inline static std::filesystem::path m_savefile = m_savefile_directory / "options.json";

	static nlohmann::ordered_json m_default_savedata;
	static nlohmann::ordered_json m_savedata;
};
