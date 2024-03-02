#include <Geode/utils/web.hpp>
#include <matjson.hpp>

#include <rtrp/rtrp.hpp>
#include "ListFetcher.hpp"
#include "../utils.hpp"

void ListFetcher::init()
{
	is_fetching = false;
	m_cachedGDListID = -1;
	m_cachedGDListLevelIDs = {};
}

void ListFetcher::getRandomNormalListLevel(GJDifficulty difficulty, level_pair_t& level, std::string& error)
{
	is_fetching = true;
	error = "";

	web::AsyncWebRequest()
		.userAgent("")
		.bodyRaw(
			fmt::format("secret={}&type={}&star={}&page={}&len={}{}",
				GJ_SECRET, 0, 1,
				rl::utils::randomInt(1, m_cNormalListMaxPage[static_cast<int>(difficulty) - 1]),
				GJ_LEN_QUERY, getDifficultyQuery(difficulty)
			)
		)
		.post(GJ_LEVELS_URL)
		.text()
		.then([&, difficulty](const auto& resp) {
			if (resp == "-1")
			{
				error = "Servers returned -1. Try again later. (getGJLevels21.php, 2)";
				is_fetching = false;
				return;
			}

			const auto result = rtrp::RtResponseParser::parseLevelResponse(resp);

			if (result.isError())
			{
				error = "Error parsing response from servers. Try again later. (getGJLevels21.php, 2)";
				is_fetching = false;
				return;
			}

			auto response = result.unwrap();

			if (difficulty == GJDifficulty::Easy)
			{
				response.levels.erase(
					std::remove_if(
						response.levels.begin(), response.levels.end(),
						[=](const auto& level) { return level.stars == 1; }
					),
					response.levels.end()
				);
			}

			int randomIdx = rl::utils::randomInt(0, response.levels.size() - 1);
			level = {
				response.levels[randomIdx],
				rl::utils::getCreatorFromLevelResponse(response.creators, response.levels[randomIdx])
			};
			is_fetching = false;
		})
		.expect([&](const auto& err) {
			error = err;

			is_fetching = false;
		});
}

void ListFetcher::getRandomDemonListLevel(level_pair_t& level, std::string& error)
{
	is_fetching = true;
	error = "";

	web::AsyncWebRequest()
		.get(DEMONLIST_URL)
		.json()
		.then([&](const auto& fjson) {
			if (fjson.is_null())
			{
				error = "Pointercrate API returned null. Try again later.";
				is_fetching = false;
				return;
			}

			int randomIndex;
			do {
				randomIndex = rl::utils::randomInt(0, fjson.as_array().size() - 1);
			} while (fjson[randomIndex]["level_id"].is_null());

			int levelId = fjson[randomIndex].template get<int>("level_id");
			getLevelInfo(levelId, level, error);
		})
		.expect([&](const auto& err) {
			error = err;

			is_fetching = false;
		});
}

// TODO: figure out how to get extended list & the rest of the list (current limit is 50 levels)
void ListFetcher::getRandomChallengeListLevel(level_pair_t& level, std::string& error)
{
	is_fetching = true;
	error = "";

	web::AsyncWebRequest()
		.get(CHALLENGELIST_URL)
		.json()
		.then([&](const auto& fjson) {
			if (fjson.is_null())
			{
				error = "Challenge List API returned null. Try again later.";
				is_fetching = false;
				return;
			}

			int randomIndex;
			do {
				randomIndex = rl::utils::randomInt(0, fjson.as_array().size() - 1);
			} while (fjson[randomIndex]["level_id"].is_null());

			int levelId = fjson[randomIndex].template get<int>("level_id");
			getLevelInfo(levelId, level, error);
		})
		.expect([&](const auto& err) {
			error = err;

			is_fetching = false;
		});
}

void ListFetcher::getRandomGDListLevel(int listID, level_pair_t& level, std::string& error)
{
	error = "";
	is_fetching = true;

	if (m_cachedGDListID == listID && m_cachedGDListLevelIDs.size() != 0)
	{
		getLevelInfo(
			std::stoi(
				m_cachedGDListLevelIDs[rl::utils::randomInt(0, m_cachedGDListLevelIDs.size() - 1)]
			),
			level,
			error
		);
		return;
	}

	web::AsyncWebRequest()
		.userAgent("")
		.bodyRaw(fmt::format("secret={}&type={}&str={}", GJ_SECRET, 0, listID))
		.post(GJ_LISTS_URL)
		.text()
		.then([&, listID](const auto& resp) {
			if (resp.length() == 0 || resp == "-1")
			{
				error = "Invalid List ID.";
				is_fetching = false;
				return;
			}

			const auto result = rtrp::RtResponseParser::parseListResponse(resp);

			if (result.isError())
			{
				error = "Error parsing response from servers. Try again later. (getGJLevelLists.php)";
				is_fetching = false;
				return;
			}

			const auto& levelIDs = result.unwrap().lists[0].levelIDs;

			m_cachedGDListID = listID;
			m_cachedGDListLevelIDs = { levelIDs };

			getLevelInfo(
				std::stoi(
					m_cachedGDListLevelIDs[rl::utils::randomInt(0, m_cachedGDListLevelIDs.size() - 1)]
				),
				level,
				error
			);
		})
		.expect([&](const auto& err) {
			error = err;

			is_fetching = false;
		});
}

void ListFetcher::getLevelInfo(int levelID, level_pair_t& level, std::string& error)
{
	web::AsyncWebRequest()
		.userAgent("")
		.bodyRaw(fmt::format("secret={}&type={}&str={}", GJ_SECRET, 0, levelID))
		.post(GJ_LEVELS_URL)
		.text()
		.then([&](const auto& resp) {
			if (resp == "-1")
			{
				error = "Invalid Level ID.";
				is_fetching = false;
				return;
			}

			const auto result = rtrp::RtResponseParser::parseLevelResponse(resp);

			if (result.isError())
			{
				error = "Error parsing response from servers. Try again later. (getGJLevels21.php, 1)";
				is_fetching = false;
				return;
			}

			const auto response = result.unwrap();

			level = { response.levels[0], response.creators[0] };
			is_fetching = false;
		})
		.expect([&](const auto& err) {
			error = err;

			is_fetching = false;
		});
}
