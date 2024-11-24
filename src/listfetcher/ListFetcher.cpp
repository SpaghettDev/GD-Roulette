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

	m_listener.bind([&](web::WebTask::Event* e) {
		if (web::WebResponse* res = e->getValue())
		{
			const auto& resp = res->string();

			if (resp.isErr())
			{
				error = fmt::format("Servers returned an invalid response ({}). Try again later. (getGJLevels21.php, 2)", resp.unwrapErr());
				is_fetching = false;
				return;
			}

			const auto& unwrappedResp = resp.unwrap();
			const auto parsedResponse = rtrp::RtResponseParser::parseLevelResponse(unwrappedResp);

			if (parsedResponse.isError())
			{
				if (unwrappedResp.starts_with("error code"))
					error = fmt::format("Server returned error code {}. Try again later. (getGJLevels21.php, 2)", unwrappedResp.substr(12));
				else
					error = "Error parsing response from servers. Try again later. (getGJLevels21.php, 2)";
				is_fetching = false;
				return;
			}

			auto response = parsedResponse.unwrap();

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
		}
		else if (e->isCancelled())
		{
			error = "Request was cancelled. (getGJLevels21.php, 2)";
			is_fetching = false;
		}
	});

	auto req = web::WebRequest()
		.userAgent("")
		.bodyString(
			fmt::format("secret={}&type={}&star={}&page={}&len={}{}",
				GJ_SECRET, 0, 1,
				rl::utils::randomInt(1, m_cNormalListMaxPage[static_cast<int>(difficulty) - 1]),
				GJ_LEN_QUERY, getDifficultyQuery(difficulty)
			)
		)
		.post(GJ_LEVELS_URL);

	m_listener.setFilter(req);
}

void ListFetcher::getRandomDemonListLevel(level_pair_t& level, std::string& error)
{
	is_fetching = true;
	error = "";

	m_listener.bind([&](web::WebTask::Event* e) {
		if (web::WebResponse* res = e->getValue())
		{
			const auto& resp = res->json();

			if (resp.isErr())
			{
				error = fmt::format("Pointercrate API returned an invalid response ({}). Try again later.", resp.unwrapErr());
				is_fetching = false;
				return;
			}

			const auto& jsonResp = resp.unwrap();

			if (jsonResp.isNull() || !jsonResp.isArray())
			{
				error = "Pointercrate API returned null or non-array. Try again later.";
				is_fetching = false;
				return;
			}

			const auto array = jsonResp.asArray().unwrap();

			int randomIndex;
			do {
				randomIndex = rl::utils::randomInt(0, array.size() - 1);
			} while (array[randomIndex]["level_id"].isNull());

			int levelId = array[randomIndex].template get<int>("level_id").unwrapOr(-1);

			if (levelId == -1)
			{
				error = "Pointercrate API returned non-number 'level_id'. Contact developer to fix this.";
				is_fetching = false;
				return;
			}

			getLevelInfo(levelId, level, error);
		}
		else if (e->isCancelled())
		{
			error = "Request was cancelled. (Pointercrate)";
			is_fetching = false;
		}
	});

	auto req = web::WebRequest()
		.get(DEMONLIST_URL);

	m_listener.setFilter(req);
}

// TODO: figure out how to get extended list & the rest of the list (current limit is 50 levels)
void ListFetcher::getRandomChallengeListLevel(level_pair_t& level, std::string& error)
{
	is_fetching = true;
	error = "";

	m_listener.bind([&](web::WebTask::Event* e) {
		if (web::WebResponse* res = e->getValue())
		{
			const auto& resp = res->json();

			if (resp.isErr())
			{
				error = fmt::format("Challenge List API returned an invalid response ({}). Try again later.", resp.unwrapErr());
				is_fetching = false;
				return;
			}

			const auto& jsonResp = resp.unwrap();

			if (jsonResp.isNull() || !jsonResp.isArray())
			{
				error = "Challenge List API returned null or non-array. Try again later.";
				is_fetching = false;
				return;
			}

			const auto& array = jsonResp.asArray().unwrap();

			int randomIndex;
			do {
				randomIndex = rl::utils::randomInt(0, array.size() - 1);
			} while (array[randomIndex]["level_id"].isNull());

			int levelId = array[randomIndex].template get<int>("level_id").unwrapOr(-1);

			if (levelId == -1)
			{
				error = "Challenge List API returned non-number 'level_id'. Contact developer to fix this.";
				is_fetching = false;
				return;
			}

			getLevelInfo(levelId, level, error);
		}
		else if (e->isCancelled())
		{
			error = "Request was cancelled. (Challenge List)";
			is_fetching = false;
		}
	});

	auto req = web::WebRequest()
		.get(CHALLENGELIST_URL);

	m_listener.setFilter(req);
}

void ListFetcher::getRandomGDListLevel(int listID, level_pair_t& level, std::string& error)
{
	error = "";
	is_fetching = true;

	if (m_cachedGDListID == listID && m_cachedGDListLevelIDs.size() != 0)
		return getLevelInfo(
			std::stoi(
				m_cachedGDListLevelIDs[rl::utils::randomInt(0, m_cachedGDListLevelIDs.size() - 1)]
			),
			level,
			error
		);

	m_listener.bind([&](web::WebTask::Event* e) {
		if (web::WebResponse* res = e->getValue())
		{
			const auto& resp = res->string();

			if (resp.isErr())
			{
				error = fmt::format("Servers returned an invalid response ({}). Try again later. (getGJLevelLists.php)", resp.unwrapErr());
				is_fetching = false;
				return;
			}

			const auto& unwrappedResp = resp.unwrap();
			if (unwrappedResp.empty() || unwrappedResp == "-1")
			{
				error = "Invalid List ID. (getGJLevelLists.php)";
				is_fetching = false;
				return;
			}

			const auto parsedResponse = rtrp::RtResponseParser::parseListResponse(unwrappedResp);

			if (parsedResponse.isError())
			{
				error = "Error parsing response from servers. Try again later. (getGJLevelLists.php)";
				is_fetching = false;
				return;
			}

			const auto& levelIDs = parsedResponse.unwrap().lists[0].levelIDs;

			m_cachedGDListID = listID;
			m_cachedGDListLevelIDs = { levelIDs };

			getLevelInfo(
				std::stoi(
					m_cachedGDListLevelIDs[rl::utils::randomInt(0, m_cachedGDListLevelIDs.size() - 1)]
				),
				level,
				error
			);
		}
		else if (e->isCancelled())
		{
			error = "Request was cancelled. (getGJLevelLists.php)";
			is_fetching = false;
		}
	});

	auto req = web::WebRequest()
		.userAgent("")
		.bodyString(
			fmt::format("secret={}&type={}&str={}", GJ_SECRET, 0, listID)
		)
		.post(GJ_LISTS_URL);

	m_listener.setFilter(req);
}

void ListFetcher::getLevelInfo(int levelID, level_pair_t& level, std::string& error)
{
	is_fetching = true;

	m_listener2.bind([&](web::WebTask::Event* e) {
		if (web::WebResponse* res = e->getValue())
		{
			const auto& resp = res->string();

			if (resp.isErr())
			{
				error = fmt::format("Servers returned an invalid response ({}). Try again later. (getGJLevels21.php)", resp.unwrapErr());
				is_fetching = false;
				return;
			}

			const auto& unwrappedResp = resp.unwrap();

			if (unwrappedResp.empty() || unwrappedResp == "-1")
			{
				error = "Invalid Level ID.";
				is_fetching = false;
				return;
			}

			const auto parsedResponse = rtrp::RtResponseParser::parseLevelResponse(unwrappedResp);

			if (parsedResponse.isError())
			{
				if (unwrappedResp.starts_with("error code"))
					error = fmt::format("Server returned error code {}. Try again later. (getGJLevels21.php, 1)", unwrappedResp.substr(12));
				else
					error = "Error parsing response from servers. Try again later. (getGJLevels21.php, 1)";
				is_fetching = false;
				return;
			}

			const auto response = parsedResponse.unwrap();

			level = { response.levels[0], response.creators[0] };
			is_fetching = false;
		}
		else if (e->isCancelled())
		{
			error = "Request was cancelled. (GDList)";
			is_fetching = false;
		}
	});

	auto req = web::WebRequest()
		.userAgent("")
		.bodyString(
			fmt::format("secret={}&type={}&str={}", GJ_SECRET, 0, levelID)
		)
		.post(GJ_LEVELS_URL);

	m_listener2.setFilter(req);
}
