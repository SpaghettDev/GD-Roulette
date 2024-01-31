#include <random>
#include <fstream>
#include "ListFetcher.hpp"
#include "../utils.hpp"

std::size_t curlWriteCallback(void* contents, size_t size, size_t nmemb, std::string* s)
{
	std::size_t new_length = size * nmemb;

	// Append the contents to the string
	s->append(reinterpret_cast<char*>(contents), new_length);

	// Check for memory allocation failure
	if (s->length() < new_length)
		return 0;

	return new_length;
}


void ListFetcher::init()
{
	static bool init = true;
	isFetching = false;

	if (init)
	{
		curl_global_init(CURL_GLOBAL_ALL);

		init = false;
	}
}

curlResponse ListFetcher::fetchLink(std::string link)
{
	curlResponse error;

	try
	{
		CURLcode res = CURLcode::CURLE_OK;
		std::string readBuffer;
		std::string info;
		nlohmann::json data;

		CURL* curl = curl_easy_init();
		if (curl)
		{
			curl_easy_setopt(curl, CURLOPT_URL, link.c_str());
			curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);

			info = readBuffer;

			if (info == "-1")
				return error;

			try
			{
				data = nlohmann::json::parse(info);
			}
			catch (...)
			{
				return error;
			}
		}

		return { res, info, data };
	}
	catch (...)
	{
		return error;
	}
}

void ListFetcher::getRandomNormalListLevel(int stars, nlohmann::json& json, curlResponse& cfr) {
	if (stars > 10)
	{
		json = {};
		cfr = { CURLcode::CURLE_HTTP_RETURNED_ERROR, "", {} };
		isFetching = false;
		return;
	}

	isFetching = true;

	std::stringstream link;
	link
		<< "https://gdbrowser.com/api/search/*"
		<< "?diff=" << stars
		<< "&starred"
		<< "&page=" << roulette::utils::randomInt(1, m_normalListMaxPage[stars - 1]);

	curlResponse response = fetchLink(link.str());

	nlohmann::json responseJson = response.jsonResponse;

	// Prevent auto levels from appearing in the Easy difficulty
	if (responseJson.size() != 0 && stars == 1)
	{
		responseJson.erase(
			std::remove_if(
				responseJson.begin(), responseJson.end(),
				[](const auto& level) {
					return level["difficulty"].get<std::string>() == "Auto";
				}
			),
			responseJson.end()
		);
	}

	if (response.responseCode == CURLcode::CURLE_OK)
		json = responseJson[roulette::utils::randomInt(0, responseJson.size() - 1)];
	else
		json = {};

	cfr = response;
	isFetching = false;
}

void ListFetcher::getRandomDemonListLevel(nlohmann::json& json, curlResponse& cfr)
{
	isFetching = true;
	std::string link = "https://pointercrate.com/api/v2/demons/listed";

	curlResponse response = fetchLink(link);

	if (response.responseCode != CURLcode::CURLE_OK)
	{
		json = {};
		cfr = response;
		isFetching = false;
		return;
	}

	int randomIndex;
	do {
		randomIndex = roulette::utils::randomInt(0, response.jsonResponse.size() - 1);
	} while (response.jsonResponse[randomIndex]["level_id"].is_null());

	int levelId = response.jsonResponse[randomIndex]["level_id"].get<int>();
	getLevelInfo(levelId, json, cfr);

	isFetching = false;
}

// TODO: figure out how to get extended list & the rest of the list (current limit is 50 levels)
void ListFetcher::getRandomChallengeListLevel(nlohmann::json& json, curlResponse& cfr)
{
	isFetching = true;
	std::string link = "https://challengelist.gd/api/v1/demons/";

	curlResponse response = fetchLink(link);

	if (response.responseCode != CURLcode::CURLE_OK)
	{
		json = {};
		isFetching = false;
		return;
	}

	int randomIndex;
	do {
		randomIndex = roulette::utils::randomInt(0, response.jsonResponse.size() - 1);
	} while (response.jsonResponse[randomIndex]["level_id"].is_null());

	int levelId = response.jsonResponse[randomIndex]["level_id"].get<int>();
	getLevelInfo(levelId, json, cfr);

	isFetching = false;
}

void ListFetcher::getLevelInfo(int levelID, nlohmann::json& json, curlResponse& cfr)
{
	std::string link = "https://gdbrowser.com/api/search/" + std::to_string(levelID);
	curlResponse response = fetchLink(link);

	if (response.responseCode != CURLcode::CURLE_OK)
	{
		json = {};
		return;
	}

	json = response.jsonResponse.at(0);
	cfr = response;
}
