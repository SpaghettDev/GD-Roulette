#pragma once
#include "../pch.hpp"
#include <nlohmann/json.hpp>
#define CURL_STATICLIB
#include <curl/curl.h>
#include "CurlResponse.hpp"

class ListFetcher
{
private:
	inline static std::array<int, 10> m_normalListMaxPage{
		67, 160, 958, 1194, 490,
		171, 186, 178, 88, 72
	};
	inline static int m_demonListMaxPage = 490;


	static curlResponse fetchLink(std::string link);

public:
	std::atomic_bool isFetching;

	void init();

	void getRandomNormalListLevel(int, nlohmann::json&, curlResponse&);
	void getRandomDemonListLevel(nlohmann::json&, curlResponse&);
	void getRandomChallengeListLevel(nlohmann::json&, curlResponse&);

	void getLevelInfo(int, nlohmann::json&, curlResponse&);
};
