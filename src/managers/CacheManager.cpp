#include "CacheManager.hpp"

#include <fstream>

CacheManager::CacheManager()
	: m_cache_filepath(geode::Mod::get()->getSaveDir() / "cache.json")
{
	auto on_read_file_fail = [&] {
		std::ofstream cacheFileWrite{ m_cache_filepath };
		cacheFileWrite << "{}\n";
		cacheFileWrite.close();
	};

	std::ifstream cacheFile(m_cache_filepath);

	if (!cacheFile.is_open())
	{
		on_read_file_fail();
		cacheFile.open(m_cache_filepath);
	}

	if (auto parsed = matjson::parse(cacheFile); parsed.isOk())
		m_cache = parsed.unwrap();
	else
	{
		cacheFile.close();
		on_read_file_fail();
		m_cache = {};
	}
}

CacheManager::~CacheManager()
{
	save();
}

void CacheManager::save()
{
	std::ofstream cacheFileWrite{ m_cache_filepath };
	cacheFileWrite << m_cache.dump() << '\n';
}
