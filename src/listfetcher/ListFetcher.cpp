#include "ListFetcher.hpp"

#include <Geode/utils/web.hpp>
#include <matjson.hpp>

#include <rtrp/rtrp.hpp>

#include "RouletteManager.hpp"

#include "CacheManager.hpp"
#include "WebRequestQueue.hpp"

#include "utils.hpp"

using namespace geode::prelude;

ListFetcher::ListFetcher()
	: m_cached_gd_list_id(-1), m_cached_gd_list_level_ids({}), m_finished_fetching_cb([] {})
{}

matjson::Value ListFetcher::normalListCacheFunction()
{
	auto& wrq = WebRequestQueue::get();

	// in case something fails, all difficulties have at least 100 pages
	matjson::Value defaultObj{};

	for (const auto& difficulty : {
		GJDifficulty::Easy, GJDifficulty::Normal, GJDifficulty::Hard,
		GJDifficulty::Harder, GJDifficulty::Insane, static_cast<GJDifficulty>(-2),
		GJDifficulty::DemonEasy, GJDifficulty::DemonMedium, GJDifficulty::Demon,
		GJDifficulty::DemonInsane, GJDifficulty::DemonExtreme
	}) {
		defaultObj[fmt::format("{}", static_cast<int>(difficulty))] = 100;

		wrq.enqueue(WebRequestQueue::Request{
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
				auto&& parsedResponse = rtrp::RtResponseParser::parseLevelResponse(unwrappedResp);

				if (parsedResponse.isErr())
					return;

				auto&& response = std::move(parsedResponse.unwrap());
				int maxPageCount = std::ceil(response.page.pages / response.page.countPerPage);

				CacheManager::get().appendValue<CMKey::NORMAL_LIST_MAX_PAGES>(
					fmt::format("{}", static_cast<int>(difficulty)),
					maxPageCount
				);
			}
		});
	}

	wrq.flush();

	return defaultObj;
}

void ListFetcher::getRandomNormalListLevel(GJDifficulty difficulty, geode::Result<level_pair_t>& result)
{
	is_fetching = true;

	m_main_listener.bind([&](web::WebTask::Event* e) {
		if (web::WebResponse* res = e->getValue())
		{
			rl::utils::ScopedVar v(is_fetching, false);
			rl::utils::ScopedFunc f(m_finished_fetching_cb);

			const auto& respStr = res->string();

			if (respStr.isErr())
			{
				result = geode::Err(fmt::format("Servers returned an invalid response ({}). Try again later. (getGJLevels21.php, 2)", respStr.unwrapErr()));

				return;
			}

			const auto& unwrappedResp = respStr.unwrap();
			auto&& parsedResponse = rtrp::RtResponseParser::parseLevelResponse(unwrappedResp);

			if (parsedResponse.isErr())
			{
				if (unwrappedResp.starts_with("error code"))
					result = geode::Err(fmt::format("Server returned error code {}. Try again later. (getGJLevels21.php, 2)", unwrappedResp.substr(12)));
				else
					result = geode::Err(fmt::format(
						"Error parsing response from servers. Try again later. (getGJLevels21.php, 2)\n({})",
						parsedResponse.unwrapErr()
					));

				return;
			}

			auto&& response = parsedResponse.unwrap();

			// this game is stupid
			if (difficulty == GJDifficulty::Easy)
			{
				response.levels.erase(
					std::remove_if(
						response.levels.begin(), response.levels.end(),
						[](const auto& level) { return level.stars == 1; }
					),
					response.levels.end()
				);
			}

			auto randomIdx = rl::utils::randomNumber(0, response.levels.size() - 1);

			// :trolley:
			if (rl::utils::isAprilFools() && !RouletteManager::get().hasFinishedARound)
				response.levels[randomIdx].levelID = 68668045;

			result = geode::Ok(level_pair_t{
				response.levels[randomIdx],
				rl::utils::getCreatorFromLevelResponse(response.creators, response.levels[randomIdx])
			});
		}
		else if (e->isCancelled())
		{
			result = geode::Err("Request was cancelled. (getGJLevels21.php, 2)");
			is_fetching = false;

			m_finished_fetching_cb();
		}
	});

	auto req = web::WebRequest()
		.userAgent("")
		.bodyString(
			fmt::format("secret={}&type={}&star={}&page={}&len={}{}",
				GJ_SECRET, 0, 1,
				rl::utils::randomNumber(
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
			rl::utils::ScopedVar v(is_fetching, false);
			rl::utils::ScopedFunc f(m_finished_fetching_cb);

			const auto& respStr = res->json();

			if (respStr.isErr())
			{
				result = geode::Err(fmt::format("Pointercrate API returned an invalid response ({}). Try again later.", respStr.unwrapErr()));

				return;
			}

			const auto& jsonResp = respStr.unwrap();

			if (jsonResp.isNull() || !jsonResp.isArray())
			{
				result = geode::Err("Pointercrate API returned null or non-array. Try again later.");

				return;
			}

			const auto array = jsonResp.asArray().unwrap();

			std::uint16_t randomIndex;
			std::uint8_t iter = 0;
			do {
				randomIndex = rl::utils::randomNumber(0, array.size() - 1);
				iter++;
			} while (array[randomIndex]["level_id"].isNull() && iter < 100);

			int levelID = array[randomIndex].template get<int>("level_id").unwrapOr(-1);
			if (levelID == -1)
			{
				result = geode::Err("Pointercrate API returned non-number 'level_id'. Contact developer to fix this.");

				return;
			}

			// prevent is_fetching to be set to false
			v.engage(false);
			f.engage(false);

			getLevelInfo(levelID, result);
		}
		else if (e->isCancelled())
		{
			result = geode::Err("Request was cancelled. (Pointercrate)");
			is_fetching = false;

			m_finished_fetching_cb();
		}
	});

	auto req = web::WebRequest()
		.get(
			fmt::format(
				"{}/?limit=100&after={}",
				DEMONLIST_URL,
				rl::utils::randomNumber(
					0,
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
			rl::utils::ScopedVar v(is_fetching, false);
			rl::utils::ScopedFunc f(m_finished_fetching_cb);

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
			std::uint8_t iter;
			do {
				randomIndex = rl::utils::randomNumber(0, array.size() - 1);
				iter++;
			} while (array[randomIndex]["level_id"].isNull() && iter < 100);

			int levelID = array[randomIndex].template get<int>("level_id").unwrapOr(-1);
			if (levelID == -1)
			{
				result = geode::Err("Challenge List API returned non-number 'level_id'. Contact developer to fix this.");

				return;
			}

			// prevent is_fetching to be set to false
			v.engage(false);
			f.engage(false);

			getLevelInfo(levelID, result);
		}
		else if (e->isCancelled())
		{
			result = geode::Err("Request was cancelled. (Challenge List)");
			is_fetching = false;

			m_finished_fetching_cb();
		}
	});

	auto req = web::WebRequest()
		.get(
			fmt::format(
				"{}/?limit=100&after={}",
				CHALLENGELIST_URL,
				rl::utils::randomNumber(
					0,
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
			rl::utils::ScopedVar v(is_fetching, true, false);
			rl::utils::ScopedFunc f(m_finished_fetching_cb);

			const auto& respStr = res->string();

			if (respStr.isErr())
			{
				result = geode::Err(fmt::format("Servers returned an invalid response ({}). Try again later. (getGJLevelLists.php)", respStr.unwrapErr()));

				return;
			}

			const auto& unwrappedResp = respStr.unwrap();
			if (unwrappedResp.empty() || unwrappedResp == "-1")
			{
				result = geode::Err("Invalid List ID. (getGJLevelLists.php)");

				return;
			}

			const auto parsedResponse = rtrp::RtResponseParser::parseListResponse(unwrappedResp);

			if (parsedResponse.isErr())
			{
				result = geode::Err(fmt::format(
					"Error parsing response from servers. Try again later. (getGJLevelLists.php)\n({})",
					parsedResponse.unwrapErr()
				));

				return;
			}

			m_cached_gd_list_id = listID;
			m_cached_gd_list_level_ids = std::move(parsedResponse.unwrap().lists[0].levelIDs);

			// prevent is_fetching to be set to false
			v.engage(false);
			f.engage(false);

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

	web::WebRequest()
		.userAgent("")
		.bodyString(
			fmt::format("secret={}&type={}&str={}", GJ_SECRET, 0, levelID)
		)
		.post(GJ_LEVELS_URL)
		.listen([&](web::WebResponse* res) {
			rl::utils::ScopedVar v(is_fetching, true, false);
			rl::utils::ScopedFunc f(m_finished_fetching_cb);

			if (!res)
			{
				result = geode::Err("Response object malformed. (getGJLevels21.php, 1)");
				return;
			}

			const auto& respStr = res->string();

			if (respStr.isErr())
			{
				result = geode::Err(fmt::format("Servers returned an invalid response ({}). Try again later. (getGJLevels21.php)", respStr.unwrapErr()));

				return;
			}

			const auto& unwrappedResp = respStr.unwrap();

			if (unwrappedResp.empty() || unwrappedResp == "-1")
			{
				result = geode::Err("Invalid Level ID.");

				return;
			}

			auto&& parsedResponse = rtrp::RtResponseParser::parseLevelResponse(unwrappedResp);

			if (parsedResponse.isErr())
			{
				if (unwrappedResp.starts_with("error code"))
					result = geode::Err(fmt::format("Server returned error code {}. Try again later. (getGJLevels21.php, 1)", unwrappedResp.substr(12)));
				else
					result = geode::Err(fmt::format(
						"Error parsing response from servers. Try again later. (getGJLevels21.php, 1)\n({})",
						parsedResponse.unwrapErr()
					));

				return;
			}

			auto&& response = std::move(parsedResponse.unwrap());

			// :trolley:
			if (rl::utils::isAprilFools() && !RouletteManager::get().hasFinishedARound)
				response.levels[0].levelID = 68668045;

			result = geode::Ok(level_pair_t{
				response.levels[0], response.creators[0]
			});
		},
		[&](web::WebProgress*) {},
		[&] {
			result = geode::Err("Request was cancelled. (getGJLevels21.php, 1)");
				is_fetching = false;

				m_finished_fetching_cb();
		}
	);
}

void ListFetcher::setFinishedFetchingCallback(std::function<void()>&& cb)
{
	m_finished_fetching_cb = [cb = std::move(cb)] mutable {
		Loader::get()->queueInMainThread(std::move(cb));
	};
}
