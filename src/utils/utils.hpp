#pragma once

#include <random>
#include <vector>
#include <chrono>

#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/binding/TextAlertPopup.hpp>

#include <matjson.hpp>

#include <rtrp/objects/LevelObject.hpp>
#include <rtrp/objects/CreatorObject.hpp>

#ifndef GEODE_IS_WINDOWS
#include <chrono>
#include <sstream>
#endif

namespace rl
{
	namespace impl
	{
		inline static std::random_device rand_device{};
		inline static std::mt19937 rand_generator(rand_device());
	}

	namespace utils
	{
		/**
		 * @brief Generates a random number between min and max (both inclusive)
		 * @param min Minimum value
		 * @param max Maximum value
		 *
		 * @tparam T type of number, floating type or integer type
		 */
		template <typename T1, typename T2>
		inline T2 randomNumber(T1 min, T2 max)
			requires(std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2> && std::is_convertible_v<T1, T2>)
		{
			static_assert(
				(std::is_integral_v<T1> && std::is_integral_v<T2>) ||
				(std::is_floating_point_v<T1> && std::is_floating_point_v<T2>),
				"Both min and max should be either floating type or integer type!"
			);

			return std::conditional_t<
				std::is_integral_v<T2>, std::uniform_int_distribution<T2>, std::uniform_real_distribution<T2>
			>(min, max)(impl::rand_generator);
		}

		/**
		 * @brief Get the Unix Epoch timestamp
		 *
		 * @return std::uint64_t
		 */
		inline std::uint64_t getUnixEpoch()
		{
			return std::chrono::duration_cast<std::chrono::seconds>(
				std::chrono::system_clock::now().time_since_epoch()
			).count();
		}

		inline bool isAprilFools()
		{
			return true;
			auto now = std::chrono::system_clock::now();
			auto timeNow = std::chrono::system_clock::to_time_t(now);
			auto tm_local = fmt::localtime(timeNow);

			return tm_local.tm_mon == 3 && tm_local.tm_mday == 1;
		}

		/**
		 * @brief Formats a Unix timestamp to a human-readable date-time string
		 *
		 * @param time
		 * @return std::string time as YYYY-MM-DD HH:MM:SS
		 */
		inline std::string formatUnixTimestamp(std::uint64_t time)
		{
#ifdef GEODE_IS_WINDOWS
			auto strTime = fmt::format(
				"{:%Y-%m-%d %H:%M:%S}",
				std::chrono::zoned_time{
					std::chrono::current_zone(),
					std::chrono::system_clock::from_time_t(time)
				}.get_local_time()
			);

			// remove miliseconds
			return strTime.substr(0, strTime.find_last_of("."));
#else
			std::time_t t = static_cast<std::time_t>(time);
			std::tm tm = *std::localtime(&t);

			std::ostringstream oss;
			oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");

			return oss.str();
#endif
		}

		/**
		 * @brief Formats a time in seconds to a human-readable time string
		 *
		 * @param time
		 * @return std::string time as HH:MM:SS
		 */
		inline std::string formatTime(std::uint64_t time)
		{
			return fmt::format(
				"{:02}:{:02}:{:02}",
				time / 3600,
				(time % 3600) / 60,
				time % 60
			);
		}

		/**
		 * @brief Gets the index of @param toFind in a std::vector<matjson::Value>
		 *
		 * @tparam T
		 * @param vec
		 * @param toFind
		 * @return std::ptrdiff_t Index of @param toFind in @param vec, -1 if not found
		 */
		template<typename T>
		inline std::ptrdiff_t getIndexOf(const std::vector<matjson::Value>& vec, T toFind)
		{
			auto it = std::find_if(vec.cbegin(), vec.cend(), [&](const matjson::Value& value) {
				return value.as<T>().unwrapOr(T{}) == toFind;
			});

			return it != vec.cend() ? (it - vec.cbegin()) : -1;
		}

		/**
		 * @brief Get the index of @param toFind in @param vec
		 *
		 * @tparam T
		 * @param vec
		 * @param toFind
		 * @return std::ptrdiff_t Index of @param toFind in @param vec, -1 if not found
		 */
		template<typename T>
		inline std::ptrdiff_t getIndexOf(const std::vector<T>& vec, T toFind)
		{
			auto it = std::find_if(vec.cbegin(), vec.cend(), [&](const T& value) {
				return value == toFind;
			});

			return it != vec.cend() ? (it - vec.cbegin()) : -1;
		}

		/**
		 * @brief Gets the count of @param toFind in a std::vector<matjson::Value>
		 *
		 * @tparam T
		 * @param vec
		 * @param toFind
		 * @return std::size_t
		 */
		template<typename T>
		inline std::size_t getCountOf(const std::vector<matjson::Value>& vec, T toFind)
		{
			return std::count_if(vec.cbegin(), vec.cend(), [&](const matjson::Value arr) {
				return arr.as<T>().unwrapOr(T{}) == toFind;
			});
		}

		/**
		 * @brief Create a GJGameLevel from RTRP response objects
		 *
		 * @param lp
		 * @return GJGameLevel*
		 */
		inline GJGameLevel* createLevelFromResponse(const std::pair<rtrp::objects::LevelObject, rtrp::objects::CreatorObject>& lp)
		{
			const auto& [level, creator] = lp; 
			auto gjlevel = GJGameLevel::create();

			gjlevel->m_levelID = level.levelID;
			gjlevel->m_levelName = level.name;
			gjlevel->m_levelDesc = level.description;
			gjlevel->m_levelVersion = level.version;
			gjlevel->m_creatorName = creator.name;
			gjlevel->m_accountID = creator.accountID;
			gjlevel->m_userID = creator.userID;
			gjlevel->m_ratings = level.difficultyDenominator;
			gjlevel->m_ratingsSum = level.difficultyNumerator;
			gjlevel->m_stars = level.stars;
			gjlevel->m_coins = level.coins;
			gjlevel->m_downloads = level.downloads;
			gjlevel->m_likes = level.likes;
			gjlevel->m_levelLength = level.length;
			gjlevel->m_coinsVerified = level.verifiedCoins;
			gjlevel->m_starsRequested = level.starsRequested;
			gjlevel->m_featured = level.featureIdx;
			gjlevel->m_isEpic = level.epic;

			if (level.isDemon)
			{
				gjlevel->m_demon = 1;
				gjlevel->m_difficulty = static_cast<GJDifficulty>(level.demonDifficulty + 4);
				gjlevel->m_demonDifficulty = level.demonDifficulty;
			}
			else if (level.isAuto)
				gjlevel->m_difficulty = GJDifficulty::Auto;
			else
				gjlevel->m_difficulty = static_cast<GJDifficulty>(level.difficultyNumerator / level.difficultyDenominator);

			if (auto id = level.customSongID; id != 0)
				gjlevel->m_songID = id;
			else
				gjlevel->m_audioTrack = level.officialSong;

			gjlevel->m_levelNotDownloaded = true;
			gjlevel->m_levelType = GJLevelType::Saved;

			return gjlevel;
		}

		/**
		 * @brief Create a GJLevelList DS_Dictionary from a list of level IDs
		 *
		 * @param levels
		 * @param startTime
		 * @param editable
		 * @return std::unique_ptr<DS_Dictionary>
		 */
		inline std::unique_ptr<DS_Dictionary> createGJListDictFromLevels(const std::vector<int>& levels, unsigned long long startTime, bool editable)
		{
			std::string levelIDs = "";
			auto timeNow = getUnixEpoch();

			for (auto& id : levels)
				levelIDs += fmt::format("{},", id);

			auto dict = std::make_unique<DS_Dictionary>();

			dict->setIntegerForKey("kCEK", 12); // kCEK
			dict->setIntegerForKey("k1", 0); // list ID
			dict->setStringForKey("k2", "GD Roulette Round"); // list  name
			dict->setStringForKey("k3", // description
				cocos2d::ZipUtils::base64URLEncode(
					editable
						? fmt::format(
							"GD-Roulette round started at {} and finished at {}",
							formatUnixTimestamp(startTime),
							formatUnixTimestamp(timeNow)
						)
						: fmt::format(
							"GD-Roulette round started at {}",
							formatUnixTimestamp(startTime)
						)
				)
			);
			dict->setStringForKey("k5", "SpaghettDev"); // account name
			dict->setIntegerForKey("k60", 7670168); // account ID
			dict->setIntegerForKey("k7", 0); // difficulty
			dict->setIntegerForKey("k11", 0); // downloads
			dict->setIntegerForKey("k15", 0); // uploads
			dict->setIntegerForKey("k22", 0); // likes
			dict->setIntegerForKey("k21", static_cast<int>(editable ? GJLevelType::Editor : GJLevelType::Saved)); // list type
			dict->setIntegerForKey("k16", 0); // list verison
			dict->setBoolForKey("k79", false); // unlisted
			dict->setBoolForKey("k94", false); // friends only
			dict->setIntegerForKey("k46", 9); // revision
			dict->setBoolForKey("k47", false); // *(&m_k100 + 1) ??
			dict->setIntegerForKey("k83", 0); // list order
			dict->setStringForKey("k96", levelIDs); // comma-separated levelIDs
			dict->setBoolForKey("k82", false); // favorite
			dict->setBoolForKey("k27", false); // featured
			dict->setIntegerForKey("k42", editable); // original
			dict->setIntegerForKey("k98", startTime); // upload date
			dict->setIntegerForKey("k99", timeNow); // update date
			dict->setIntegerForKey("k113", 0); // diamonds
			dict->setIntegerForKey("k114", 0); // levels to claim
			dict->setBoolForKey("k100", editable); // isEditMode
			dict->setDictForKey("k97", cocos2d::CCDictionary::create()); // CCDictionary<std::string, GJGameLevel*>

			return dict;
		}

		/**
		 * @brief Get the CreatorObject from a LevelObject
		 *
		 * @param creators
		 * @param level
		 * @return const rtrp::objects::CreatorObject Creator or default CreatorObject if not found
		 */
		inline const rtrp::objects::CreatorObject getCreatorFromLevelResponse(
			const std::vector<rtrp::objects::CreatorObject>& creators,
			const rtrp::objects::LevelObject& level
		) {
			auto it = std::find_if(creators.cbegin(), creators.cend(), [&](const auto& creator) {
				return creator.userID == level.creatorUserID;
			});

			return it != creators.cend() ? creators[it - creators.cbegin()] : rtrp::objects::CreatorObject{ level.creatorUserID, "-", 0 };
		}

		/**
		 * @brief Get the difficulty from a LevelObject
		 *
		 * @param level
		 * @return GJDifficulty
		 */
		inline GJDifficulty getDifficultyFromResponse(const rtrp::objects::LevelObject& level)
		{
			if (level.isAuto)
				return GJDifficulty::Auto;

			if (level.isDemon)
			{
				switch (level.demonDifficulty)
				{
				case 3: return GJDifficulty::DemonEasy;
				case 4: return GJDifficulty::DemonMedium;
				case 5: return GJDifficulty::DemonInsane;
				case 6: return GJDifficulty::DemonExtreme;
				default:
				case 0: return GJDifficulty::Demon;
				}
			}

			if (level.difficultyDenominator == 0)
				return static_cast<GJDifficulty>(-1);

			return static_cast<GJDifficulty>(level.difficultyNumerator / level.difficultyDenominator);
		}

		/**
		 * @brief Get the feature state from a LevelObject
		 *
		 * @param level
		 * @return GJFeatureState
		 */
		inline GJFeatureState getFeatureStateFromResponse(const rtrp::objects::LevelObject& level)
		{
			return level.featureIdx == 0
				? GJFeatureState::None
				: level.epic
					? static_cast<GJFeatureState>(level.epic + 1)
					: GJFeatureState::Featured;
		}

		/**
		 * @brief Create a Notification Toast object and adds it to @param layer
		 *
		 * @param layer
		 * @param str
		 * @param time
		 * @param yPosition
		 */
		inline void createNotificationToast(cocos2d::CCLayer* layer, const std::string_view str, float time, float yPosition)
		{
			auto tap = TextAlertPopup::create(str.data(), time, .6f, 0x96, "bigFont.fnt");
			tap->setPositionY(yPosition);

			layer->addChild(tap, 420);
		}

		template <typename V, typename T = V>
		struct ScopedVar
		{
		public:
			ScopedVar(T& var, const V& ctorVal, const V& dtorVal)
				: m_var{ var }, m_dtor_val{ dtorVal }, m_engaged{ true }
			{ m_var = ctorVal; }

			ScopedVar(T& var, const V& dtorVal)
				: m_var{ var }, m_dtor_val{ dtorVal }, m_engaged{ true }
			{}

			~ScopedVar() { if (m_engaged) m_var = m_dtor_val; }

			void engage(bool engaged) { m_engaged = engaged; }

		private:
			T& m_var;
			V m_dtor_val;

			bool m_engaged;
		};

		template <typename V, typename T = V>
		ScopedVar(V, std::atomic<V>) -> ScopedVar<V, std::atomic<V>>;

		template <typename R, typename ...Args>
		struct ScopedFunc
		{
		public:
			ScopedFunc(std::function<R(Args...)>&& function)
				: m_function{ std::move(function) }, m_engaged{ true }
			{}

			ScopedFunc(const std::function<R(Args...)>& function)
				: m_function{ function }, m_engaged{ true }
			{}

			~ScopedFunc() { if (m_engaged) m_function(); }

			void engage(bool engaged) { m_engaged = engaged; }

		private:
			std::function<R(Args...)> m_function;
			bool m_engaged;
		};

		template <typename X>
		unsigned int calcChecksum(const X& v)
		{
			constexpr unsigned int B = 0x7FEDCBA9;
			unsigned int E = B;

			auto e = [](unsigned int x, unsigned char y) {
				return ((x >> 3) | (x << 29)) ^ (y * 13 + 0x21);
			};

			E = e(E, std::hash<X>{}(v));

			return E ^ (B >> 1);
		}
	}
}
