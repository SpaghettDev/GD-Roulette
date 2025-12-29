#pragma once

#include <type_traits>
#include <fstream>

#include <Geode/loader/Mod.hpp>

#include "GameState.hpp"
#include "utils.hpp"

#define ADD_KEY_TRAIT(K, key_type) \
	template <> \
	struct DMKeyTrait<K> \
	{ using type = key_type; }


enum DMArrayKey
{
	DIFFICULTY_ARRAY,
	DEMON_DIFFICULTY_ARRAY,
	SELECTED_LIST_ARRAY,
};

enum DMMiscKey
{
	SAVE_DATA = 1000,
	GD_LIST_ID,
};


namespace DataManager
{
	// this is a bit over-engineered but whatever
	namespace impl
	{
		template <typename ...Bools>
		consteval std::bitset<sizeof...(Bools)> to_bitset(Bools... bools) requires(std::is_same_v<decltype(bools), bool> && ...)
		{
			std::size_t bits = 0;
			std::size_t i = 0;

			((bits |= (bools << i++)), ...);

			return bits;
		}

		template <std::size_t N>
		consteval auto to_bitset(const bool (&bools)[N])
		{
			return [&]<std::size_t... I>(std::index_sequence<I...>) {
				return to_bitset(bools[I]...);
			}(std::make_index_sequence<N>{});
		}

		template <template <std::size_t> typename T, std::size_t N>
		constexpr std::vector<bool> bitset_to_vec(const T<N>& bitset)
		{
			std::vector<bool> vec(N);

			for (std::size_t i = 0; i < N; ++i)
				vec[i] = bitset[i];

			return vec;
		}
	}

	namespace values
	{
		template <std::size_t N>
		using SavedBoolArrayInfo = std::bitset<N>;

		template <DMArrayKey key>
		consteval std::string_view getKeyString()
		{
			if constexpr (key == DMArrayKey::DIFFICULTY_ARRAY)
				return "difficulty-array";
			else if constexpr (key == DMArrayKey::DEMON_DIFFICULTY_ARRAY)
				return "demon-difficulty-array";
			else if constexpr (key == DMArrayKey::SELECTED_LIST_ARRAY)
				return "selected-list-array";

			throw "Invalid DMArray key!";
		}

		template <DMMiscKey key>
		consteval std::string_view getKeyString()
		{
			if constexpr (key == DMMiscKey::GD_LIST_ID)
				return "gd-list-id";

			throw "Invalid DMMisc key!";
		}

		template <DMArrayKey key>
		consteval auto getSAI()
		{
			if constexpr (key == DMArrayKey::DIFFICULTY_ARRAY)
				return SavedBoolArrayInfo<6>{ impl::to_bitset({ true, false, false, false, false, false }) };
			else if constexpr (key == DMArrayKey::DEMON_DIFFICULTY_ARRAY)
				return SavedBoolArrayInfo<7>{ impl::to_bitset({ false, true, false, false, false, false, false }) };
			else if constexpr (key == DMArrayKey::SELECTED_LIST_ARRAY)
				return SavedBoolArrayInfo<4>{ impl::to_bitset({ true, false, false, false }) };

			throw "Invalid DMArray key!";
		}
	}

	namespace
	{
		// inline static const auto SAVE_DATA_PATH = geode::Mod::get()->getSaveDir() / "save_data.bin";

		void writeGameState(const GameState& value)
		{
			static const auto SAVE_DATA_DIR = geode::Mod::get()->getSaveDir();

			std::ofstream saveDataFileIn(SAVE_DATA_DIR / "save_data.bin", std::ios::binary);
			std::size_t vecSize = value.playedLevels.size();

			saveDataFileIn.write(reinterpret_cast<const char*>(&value), sizeof(value) - sizeof(value.playedLevels));
			saveDataFileIn.write(reinterpret_cast<const char*>(&vecSize), sizeof(vecSize));
			saveDataFileIn.write(
				reinterpret_cast<const char*>(value.playedLevels.data()),
				vecSize * sizeof(decltype(value.playedLevels)::value_type)
			);

			geode::Mod::get()->setSavedValue("gamestate-checksum", rl::utils::calcChecksum(value));
		}

		GameState writeGameStateSafe(bool force_clean = false)
		{
			static const auto SAVE_DATA_DIR = geode::Mod::get()->getSaveDir();

			GameState fromDisk{};
			std::size_t vecSize;
			std::ifstream saveDataFileIn(SAVE_DATA_DIR / "save_data.bin", std::ios::binary);

			saveDataFileIn.seekg(0, std::ios::end);

			if (force_clean || saveDataFileIn.fail())
			{
				writeGameState(fromDisk = GameState{});
				return fromDisk;
			}

			saveDataFileIn.seekg(0);
			std::uint16_t fromDiskVersion;
			saveDataFileIn.read(reinterpret_cast<char*>(&fromDiskVersion), sizeof(fromDiskVersion));

			// TODO: maybe port old save data
			if (std::abs(fromDisk.version - fromDiskVersion) >= 10)
			{
				geode::log::warn("Save data version mismatch: {} (savedata) != {} (current)", fromDiskVersion, GameState::VERSION);
				std::filesystem::rename(SAVE_DATA_DIR / "save_data.bin", SAVE_DATA_DIR / "save_data_outdated.bin");
				writeGameState(fromDisk = GameState{});
				return fromDisk;
			}

			saveDataFileIn.seekg(0);
			saveDataFileIn.read(reinterpret_cast<char*>(&fromDisk), sizeof(GameState) - sizeof(GameState::playedLevels));
			saveDataFileIn.read(reinterpret_cast<char*>(&vecSize), sizeof(vecSize));
			fromDisk.playedLevels.resize(vecSize);
			saveDataFileIn.read(
				reinterpret_cast<char*>(fromDisk.playedLevels.data()),
				vecSize * sizeof(decltype(GameState::playedLevels)::value_type)
			);

			if (geode::Mod::get()->getSavedValue<unsigned int>("gamestate-checksum") != rl::utils::calcChecksum(fromDisk))
				writeGameState(fromDisk = GameState{});

			return fromDisk;
		}
	}

	namespace traits
	{
		template <DMMiscKey K>
		struct DMKeyTrait;

		ADD_KEY_TRAIT(DMMiscKey::SAVE_DATA, GameState);
		ADD_KEY_TRAIT(DMMiscKey::GD_LIST_ID, int);
	}

	struct ArrayProxy
	{
	private:
		std::vector<matjson::Value>& m_value;

	public:
		ArrayProxy(std::vector<matjson::Value>& value)
			: m_value(value)
		{}

		template <typename T>
		T&& at(std::size_t idx)
		{
			// at this point the value is safe to unwrap since we've already verified it beforehand
			return m_value.at(idx).as<T>().unwrap();
		}

		template <typename T>
		void set(std::size_t idx, const T& value)
		{
			m_value.at(idx) = value;
		}

		std::vector<matjson::Value>& asVector()
		{
			return m_value;
		}
	};


	template <DMArrayKey key>
	[[nodiscard]] std::vector<matjson::Value>& setDefaultSafe();
	template <DMMiscKey key>
	[[nodiscard]] traits::DMKeyTrait<key>::type setDefaultSafe();


	/**
	 * @brief Gets the array @tparam key array from the save container. Verifies it beforehand.
	 *
	 * @tparam key The array to get.
	 * @return ArrayProxy
	 */
	template <DMArrayKey key>
	[[nodiscard]] ArrayProxy get()
	{
		return setDefaultSafe<key>();
	}

	/**
	 * @brief Gets the value of @tparam key from the save container. Verifies it beforehand.
	 *
	 * @tparam key The key to get.
	 * @return traits::KeyTrait<key>::type The value's type.
	 */
	template <DMMiscKey key>
	[[nodiscard]] traits::DMKeyTrait<key>::type get()
	{
		return setDefaultSafe<key>();
	}

	/**
	 * @brief Sets the value of @tparam key array in the save container to @param value.
	 *
	 * @tparam key The key to set the value of.
	 * @param value The new value.
	 */
	template <DMArrayKey key>
	void set(const std::vector<matjson::Value>& value)
	{
		geode::Mod::get()->setSavedValue(values::getKeyString<key>(), value);
	}

	/**
	 * @brief Sets the value of @tparam key array at index @param idx to @param value
	 *
	 * @tparam key
	 * @param idx
	 * @param value
	 */
	template <DMArrayKey key>
	void set(std::size_t idx, bool value)
	{
		geode::Mod::get()->getSaveContainer().get(
			values::getKeyString<key>()
		).unwrap().asArray().unwrap().at(idx) = value;
	}

	/**
	 * @brief Sets the value of @tparam key in the save container to @param value.
	 *
	 * @tparam key The key to set the value of.
	 * @param value The new value.
	 */
	template <DMMiscKey key>
	void set(const typename traits::DMKeyTrait<key>::type& value)
	{
		if constexpr (key == DMMiscKey::SAVE_DATA)
			writeGameState(value);
		else
			geode::Mod::get()->setSavedValue(values::getKeyString<key>(), value);
	}


	/**
	 * @brief Sets the value of @tparam key array to its default if it doesn't exist or there is an error getting it.
	 *
	 * @tparam key The key to set the value of.
	 * @return std::vector<matjson::Value>& The default value if there was an error, or the current value.
	 */
	template <DMArrayKey key>
	[[maybe_unused]] std::vector<matjson::Value>& setDefaultSafe()
	{
		auto& container = geode::Mod::get()->getSaveContainer();

		if (auto res = container.get(values::getKeyString<key>()); res.isOk())
			if (auto resv = res.unwrap().asArray(); resv.isOkAnd([](const auto& vec) {
					return vec.size() == values::getSAI<key>().size() &&
						std::all_of(vec.begin(), vec.end(), [](const auto& v) { return v.asBool().isOk(); });
			}))
				return resv.unwrap();

		container.set(values::getKeyString<key>(), impl::bitset_to_vec(values::getSAI<key>()));

		return container.get(values::getKeyString<key>()).unwrap().asArray().unwrap();
	}

	/**
	 * @brief Sets the value of @tparam key to its default if it doesn't exist or there is an error getting it.
	 *
	 * @tparam key The key to set the value of.
	 * @return traits::KeyTrait<key>::type The default value if there was an error, or the current value.
	 */
	template <DMMiscKey key>
	[[maybe_unused]] traits::DMKeyTrait<key>::type setDefaultSafe()
	{
		using value_t = typename traits::DMKeyTrait<key>::type;
		using save_container_t = std::conditional_t<std::is_same_v<value_t, int>, uint64_t, value_t>;

		if constexpr (key == DMMiscKey::SAVE_DATA)
			return writeGameStateSafe();
		else
		{
			if (auto res = geode::Mod::get()->getSavedValue(values::getKeyString<key>(), -1); res != -1)
				return res;

			geode::Mod::get()->setSavedValue(values::getKeyString<key>(), value_t{});

			return value_t{};
		}
	}
};

#undef ADD_KEY_TRAIT
