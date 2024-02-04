#include <Geode/utils/web.hpp>
#include <matjson.hpp>

#include <random>
#include "ListFetcher.hpp"
#include "../utils.hpp"

void ListFetcher::init()
{
	isFetching = false;
}

void ListFetcher::getRandomNormalListLevel(int stars, matjson::Value& json)
{
	if (stars > 10)
	{
		json = {};
		isFetching = false;
		return;
	}

	isFetching = true;

	std::string link = fmt::format(
		"https://gdbrowser.com/api/search/*?diff={}&starred&page={}",
		stars, roulette::utils::randomInt(1, m_normalListMaxPage[stars - 1])
	);

	web::AsyncWebRequest()
		.get(link)
		.json()
		.then([&, stars](auto const& fjson) {
			json = fjson;

			// Prevent auto levels from appearing in the Easy difficulty
			if (!json.is_null() && stars == 1)
			{
				matjson::Array& arr = json.as_array();

				arr.erase(
					std::remove_if(
						arr.begin(), arr.end(),
						[](const auto& level) {
							return level.get<std::string>("difficulty") == "Auto";
						}
					),
					arr.end()
				);
			}

			if (!json.is_null())
				json = json[roulette::utils::randomInt(0, json.as_array().size() - 1)];

			isFetching = false;
		})
		.expect([&](auto const& err) {
			json = matjson::parse("null");

			isFetching = false;
		});
}

void ListFetcher::getRandomDemonListLevel(matjson::Value& json)
{
	isFetching = true;
	std::string link = "https://pointercrate.com/api/v2/demons/listed";

	web::AsyncWebRequest()
		.get(link)
		.json()
		.then([&](auto const& fjson) {
			json = fjson;

			if (json.is_null())
			{
				isFetching = false;
				return;
			}

			int randomIndex;
			do {
				randomIndex = roulette::utils::randomInt(0, json.as_array().size() - 1);
			} while (json[randomIndex]["level_id"].is_null());

			int levelId = json[randomIndex].get<int>("level_id");
			getLevelInfo(levelId, json);
		})
		.expect([&](auto const& err) {
			json = matjson::parse("null");
			isFetching = false;
		});
}

// TODO: figure out how to get extended list & the rest of the list (current limit is 50 levels)
void ListFetcher::getRandomChallengeListLevel(matjson::Value& json)
{
	isFetching = true;
	std::string link = "https://challengelist.gd/api/v1/demons/";

	web::AsyncWebRequest()
		.get(link)
		.json()
		.then([&](auto const& fjson) {
			json = fjson;

			if (json.is_null())
			{
				isFetching = false;
				return;
			}

			int randomIndex;
			do {
				randomIndex = roulette::utils::randomInt(0, json.as_array().size() - 1);
			} while (json[randomIndex]["level_id"].is_null());

			int levelId = json[randomIndex].get<int>("level_id");
			getLevelInfo(levelId, json);
		})
		.expect([&](auto const& err) {
			json = matjson::parse("null");
			isFetching = false;
		});
}

void ListFetcher::getLevelInfo(int levelID, matjson::Value& json)
{
	std::string link = fmt::format("https://gdbrowser.com/api/search/{}", levelID);

	web::AsyncWebRequest()
		.get(link)
		.json()
		.then([&](auto const& fjson) {
			json = fjson;

			if (json.is_null()) return;

			json = json.as_array()[0];
			isFetching = false;
		})
		.expect([&](auto const& err) {
			json = matjson::parse("null");
			isFetching = false;
		});
}
