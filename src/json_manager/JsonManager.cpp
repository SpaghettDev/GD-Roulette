#include "JsonManager.hpp"
#define DECLAREROULETTEMANAGER
#include "../roulette/manager/RouletteManager.hpp"

nlohmann::ordered_json JsonManager::m_default_savedata;
nlohmann::ordered_json JsonManager::m_savedata;

// maybe change to saving the std::array instead of its elements individually

JsonManager::JsonManager()
{
	m_default_savedata["normalList"] = true;
	m_default_savedata["demonList"] = false;
	m_default_savedata["challengeList"] = false;
	m_default_savedata["maxSkips"] = 3;

	// if (!std::filesystem::exists(m_savefile))
	// {
	// 	if (!std::filesystem::exists(m_savefile_directory))
	// 		std::filesystem::create_directory(m_savefile_directory);

	// 	std::ofstream file(m_savefile, std::ios::out | std::ios::trunc);
	// 	file << m_default_savedata.dump(4);
	// 	file.close();

	// 	m_savedata.update(m_default_savedata);
	// }

	// std::ifstream file(m_savefile);
	// try
	// {
	// 	file >> m_savedata;

	// 	// TODO: tidy & fix
	// 	if (
	// 		m_savedata["normalList"].get<bool>() == m_savedata["challengeList"].get<bool>() == m_savedata["challengeList"].get<bool>() == false ||
	// 		m_savedata["normalList"].get<bool>() == m_savedata["challengeList"].get<bool>() == m_savedata["challengeList"].get<bool>() == true
	// 		) {
	// 		m_savedata["normalList"] = true;
	// 		m_savedata["demonList"] = false;
	// 		m_savedata["challengeList"] = false;
	// 	}

	// 	RouletteManager.selectedListArr[0].assignNoSave(m_savedata["normalList"]);
	// 	RouletteManager.selectedListArr[1].assignNoSave(m_savedata["demonList"]);
	// 	RouletteManager.selectedListArr[2].assignNoSave(m_savedata["challengeList"]);
	// 	RouletteManager.maxSkips.assignNoSave(m_savedata["maxSkips"]);
	// }
	// catch (...)
	// {
	// 	file.close();
	// 	RouletteManager.isJsonCorrupted = true;
	// 	std::ofstream openedFile(m_savefile, std::ios::out | std::ios::trunc);
	// 	openedFile << m_default_savedata.dump(4);
	// 	openedFile.close();
	// }

	// file.close();
}

void JsonManager::save()
{
	m_savedata["normalList"] = static_cast<bool>(RouletteManager.selectedListArr[0]);
	m_savedata["demonList"] = static_cast<bool>(RouletteManager.selectedListArr[1]);
	m_savedata["challengeList"] = static_cast<bool>(RouletteManager.selectedListArr[2]);
	m_savedata["maxSkips"] = static_cast<int>(RouletteManager.maxSkips);

	std::ofstream file(m_savefile, std::ios::out | std::ios::trunc);
	file << m_savedata.dump(4);
	file.close();
}
