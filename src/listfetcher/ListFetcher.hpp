#pragma once
#include <matjson.hpp>
#include <array>
#include <atomic>
#include <string>

class ListFetcher
{
private:
	// TODO: update
	inline static std::array<int, 10> m_normalListMaxPage{
		67, 160, 958, 1194, 490,
		171, 186, 178, 88, 72
	};
	inline static int m_demonListMaxPage = 490;

public:
	std::atomic_bool isFetching;

	void init();

	void getRandomNormalListLevel(int, matjson::Value&, std::string&);
	void getRandomDemonListLevel(matjson::Value&, std::string&);
	void getRandomChallengeListLevel(matjson::Value&, std::string&);

	void getLevelInfo(int, matjson::Value&, std::string&);
};
