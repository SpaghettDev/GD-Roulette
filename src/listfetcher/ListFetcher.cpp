#include "ListFetcher.hpp"

#include <Geode/utils/web.hpp>
#include <matjson.hpp>

#include <rtrp/rtrp.hpp>

#include "CacheManager.hpp"
#include "WebRequestQueue.hpp"

#include "utils.hpp"

using namespace geode::prelude;

ListFetcher::ListFetcher()
	: m_cached_gd_list_id(-1), m_cached_gd_list_level_ids({})
{}

matjson::Value ListFetcher::normalListCacheFunction()
{
	for (const auto& difficulty : {
		GJDifficulty::Easy, GJDifficulty::Normal, GJDifficulty::Hard,
		GJDifficulty::Harder, GJDifficulty::Insane, static_cast<GJDifficulty>(-2),
		GJDifficulty::DemonEasy, GJDifficulty::DemonMedium, GJDifficulty::Demon,
		GJDifficulty::DemonInsane, GJDifficulty::DemonExtreme
	}) {
		WebRequestQueue::get().enqueue(WebRequestQueue::Request{
			web::WebRequest()
				.userAgent("")
				.bodyString(
					fmt::format("secret={}&type={}&star={}&len={}{}",
						GJ_SECRET, 0, 1, GJ_LEN_QUERY, getDifficultyQuery(difficulty)
					)
				)
				.post(GJ_LEVELS_URL),
			[difficulty](web::WebResponse* r) {
				const auto& resp = r->string();

				if (resp.isErr())
					return;

				const auto& unwrappedResp = resp.unwrap();
				const auto parsedResponse = rtrp::RtResponseParser::parseLevelResponse(unwrappedResp);

				if (parsedResponse.isError())
					return;

				auto response = parsedResponse.unwrap();
				int maxPageCount = std::ceil(response.page.pages / response.page.countPerPage);

				CacheManager::get().appendValue<CMKey::NORMAL_LIST_MAX_PAGES>(
					fmt::format("{}", static_cast<int>(difficulty)),
					maxPageCount
				);
			}
		});
	}

	WebRequestQueue::get().flush();

	return {};
}

void ListFetcher::getRandomNormalListLevel(GJDifficulty difficulty, geode::Result<level_pair_t>& result)
{
	m_main_listener.bind([&](web::WebTask::Event* e) {
		if (web::WebResponse* res = e->getValue())
		{
			rl::utils::ScopedVar _(is_fetching, false);

			const auto& resp = res->string();

			if (resp.isErr())
			{
				result = geode::Err(fmt::format("Servers returned an invalid response ({}). Try again later. (getGJLevels21.php, 2)", resp.unwrapErr()));

				return;
			}

			const auto& unwrappedResp = resp.unwrap();
			const auto parsedResponse = rtrp::RtResponseParser::parseLevelResponse(unwrappedResp);

			if (parsedResponse.isError())
			{
				if (unwrappedResp.starts_with("error code"))
					result = geode::Err(fmt::format("Server returned error code {}. Try again later. (getGJLevels21.php, 2)", unwrappedResp.substr(12)));
				else
					result = geode::Err("Error parsing response from servers. Try again later. (getGJLevels21.php, 2)");

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

			auto randomIdx = rl::utils::randomNumber<std::size_t>(0, response.levels.size() - 1);
			result = geode::Ok(level_pair_t{
				response.levels[randomIdx],
				rl::utils::getCreatorFromLevelResponse(response.creators, response.levels[randomIdx])
			});

			if (m_finished_fetching_cb)
				m_finished_fetching_cb();
		}
		else if (e->isCancelled())
		{
			result = geode::Err("Request was cancelled. (getGJLevels21.php, 2)");
			is_fetching = false;

			if (m_finished_fetching_cb)
				m_finished_fetching_cb();
		}
	});

	auto req = web::WebRequest()
		.userAgent("")
		.bodyString(
			fmt::format("secret={}&type={}&star={}&page={}&len={}{}",
				GJ_SECRET, 0, 1,
				rl::utils::randomNumber<std::uint16_t>(
					1,
					CacheManager::get().getValue<CMKey::NORMAL_LIST_MAX_PAGES, int>(difficulty, 1)
				),
				GJ_LEN_QUERY, getDifficultyQuery(difficulty)
			)
		)
		.post(GJ_LEVELS_URL);

	m_main_listener.setFilter(req);
}

void ListFetcher::getRandomDemonListLevel(geode::Result<level_pair_t>& result)
{
	is_fetching = true;

	m_main_listener.bind([&](web::WebTask::Event* e) {
		if (web::WebResponse* res = e->getValue())
		{
			rl::utils::ScopedVar _(is_fetching, false);

			const auto& resp = res->json();

			if (resp.isErr())
			{
				result = geode::Err(fmt::format("Pointercrate API returned an invalid response ({}). Try again later.", resp.unwrapErr()));

				return;
			}

			const auto& jsonResp = resp.unwrap();

			if (jsonResp.isNull() || !jsonResp.isArray())
			{
				result = geode::Err("Pointercrate API returned null or non-array. Try again later.");

				return;
			}

			const auto array = jsonResp.asArray().unwrap();

			std::size_t randomIndex;
			do {
				randomIndex = rl::utils::randomNumber<std::size_t>(0, array.size() - 1);
			} while (array[randomIndex]["level_id"].isNull());

			int levelId = array[randomIndex].template get<int>("level_id").unwrapOr(-1);

			if (levelId == -1)
			{
				result = geode::Err("Pointercrate API returned non-number 'level_id'. Contact developer to fix this.");

				return;
			}

			// prevent is_fetching to be set to false
			_.engage(false);

			getLevelInfo(levelId, result);
		}
		else if (e->isCancelled())
		{
			result = geode::Err("Request was cancelled. (Pointercrate)");
			is_fetching = false;

			if (m_finished_fetching_cb)
				m_finished_fetching_cb();
		}
	});

	auto req = web::WebRequest()
		.get(
			fmt::format(
				"{}/?limit=100&after={}",
				DEMONLIST_URL,
				rl::utils::randomNumber(
					static_cast<unsigned short>(0),
					rl::constants::list_fetcher::DEMON_LIST_MAX_PAGE
				)
			)
		);

	m_main_listener.setFilter(req);
}

void ListFetcher::getRandomChallengeListLevel(geode::Result<level_pair_t>& result)
{
	is_fetching = true;

	m_main_listener.bind([&](web::WebTask::Event* e) {
		if (web::WebResponse* res = e->getValue())
		{
			rl::utils::ScopedVar _(is_fetching, false);

			const auto& resp = res->json();

			if (resp.isErr())
			{
				result = geode::Err(fmt::format("Challenge List API returned an invalid response ({}). Try again later.", resp.unwrapErr()));

				return;
			}

			const auto& jsonResp = resp.unwrap();

			if (jsonResp.isNull() || !jsonResp.isArray())
			{
				result = geode::Err("Challenge List API returned null or non-array. Try again later.");

				return;
			}

			const auto& array = jsonResp.asArray().unwrap();

			std::uint16_t randomIndex;
			do {
				randomIndex = rl::utils::randomNumber<std::uint16_t>(0, array.size() - 1);
			} while (array[randomIndex]["level_id"].isNull());

			int levelId = array[randomIndex].template get<int>("level_id").unwrapOr(-1);

			if (levelId == -1)
			{
				result = geode::Err("Challenge List API returned non-number 'level_id'. Contact developer to fix this.");

				return;
			}

			// prevent is_fetching to be set to false
			_.engage(false);

			getLevelInfo(levelId, result);
		}
		else if (e->isCancelled())
		{
			result = geode::Err("Request was cancelled. (Challenge List)");
			is_fetching = false;

			if (m_finished_fetching_cb)
				m_finished_fetching_cb();
		}
	});

	auto req = web::WebRequest()
		.get(
			fmt::format(
				"{}/?limit=100&after={}",
				CHALLENGELIST_URL,
				rl::utils::randomNumber(
					static_cast<unsigned short>(0),
					rl::constants::list_fetcher::CHALLENGE_LIST_MAX_PAGE
				)
			)
		);

	m_main_listener.setFilter(req);
}

void ListFetcher::getRandomGDListLevel(int listID, geode::Result<level_pair_t>& result)
{
	is_fetching = true;

	if (m_cached_gd_list_id == listID && m_cached_gd_list_level_ids.size() != 0)
		return getLevelInfo(
			std::stoi(
				m_cached_gd_list_level_ids[
					rl::utils::randomNumber(0ull, m_cached_gd_list_level_ids.size() - 1)
				]
			),
			result
		);

	m_main_listener.bind([&](web::WebTask::Event* e) {
		if (web::WebResponse* res = e->getValue())
		{
			rl::utils::ScopedVar _(is_fetching, true, false);

			const auto& resp = res->string();

			if (resp.isErr())
			{
				result = geode::Err(fmt::format("Servers returned an invalid response ({}). Try again later. (getGJLevelLists.php)", resp.unwrapErr()));

				return;
			}

			const auto& unwrappedResp = resp.unwrap();
			if (unwrappedResp.empty() || unwrappedResp == "-1")
			{
				result = geode::Err("Invalid List ID. (getGJLevelLists.php)");

				return;
			}

			const auto parsedResponse = rtrp::RtResponseParser::parseListResponse(unwrappedResp);

			if (parsedResponse.isError())
			{
				result = geode::Err("Error parsing response from servers. Try again later. (getGJLevelLists.php)");

				return;
			}

			const auto& levelIDs = parsedResponse.unwrap().lists[0].levelIDs;

			m_cached_gd_list_id = listID;
			m_cached_gd_list_level_ids = { levelIDs };

			// prevent is_fetching to be set to false
			_.engage(false);

			getLevelInfo(
				std::stoi(
					m_cached_gd_list_level_ids[
						rl::utils::randomNumber(0ull, m_cached_gd_list_level_ids.size() - 1)
					]
				),
				result
			);
		}
		else if (e->isCancelled())
		{
			result = geode::Err("Request was cancelled. (getGJLevelLists.php)");
			is_fetching = false;

			if (m_finished_fetching_cb)
				m_finished_fetching_cb();
		}
	});

	auto req = web::WebRequest()
		.userAgent("")
		.bodyString(
			fmt::format("secret={}&type={}&str={}", GJ_SECRET, 0, listID)
		)
		.post(GJ_LISTS_URL);

	m_main_listener.setFilter(req);
}

void ListFetcher::getLevelInfo(int levelID, geode::Result<level_pair_t>& result)
{
	is_fetching = true;

	m_secondary_listener.bind([&](web::WebTask::Event* e) {
		if (web::WebResponse* res = e->getValue())
		{
			rl::utils::ScopedVar _(is_fetching, true, false);

			const auto& resp = res->string();

			if (resp.isErr())
			{
				result = geode::Err(fmt::format("Servers returned an invalid response ({}). Try again later. (getGJLevels21.php)", resp.unwrapErr()));

				return;
			}

			const auto& unwrappedResp = resp.unwrap();

			if (unwrappedResp.empty() || unwrappedResp == "-1")
			{
				result = geode::Err("Invalid Level ID.");

				return;
			}

			const auto parsedResponse = rtrp::RtResponseParser::parseLevelResponse(unwrappedResp);

			if (parsedResponse.isError())
			{
				if (unwrappedResp.starts_with("error code"))
					result = geode::Err(fmt::format("Server returned error code {}. Try again later. (getGJLevels21.php, 1)", unwrappedResp.substr(12)));
				else
					result = geode::Err("Error parsing response from servers. Try again later. (getGJLevels21.php, 1)");

				return;
			}

			const auto response = parsedResponse.unwrap();

			result = geode::Ok(
				level_pair_t{ response.levels[0], response.creators[0] }
			);

			if (m_finished_fetching_cb)
				m_finished_fetching_cb();
		}
		else if (e->isCancelled())
		{
			result = geode::Err("Request was cancelled. (GDList)");
			is_fetching = false;

			if (m_finished_fetching_cb)
				m_finished_fetching_cb();
		}
	});

	auto req = web::WebRequest()
		.userAgent("")
		.bodyString(
			fmt::format("secret={}&type={}&str={}", GJ_SECRET, 0, levelID)
		)
		.post(GJ_LEVELS_URL);

	m_secondary_listener.setFilter(req);
}
