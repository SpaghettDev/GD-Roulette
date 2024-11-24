#pragma once

#include <fstream>

#include <Geode/loader/Mod.hpp>

#include "GameState.hpp"

#define ADD_KEY_TRAIT(K, key_type) \
	template <> \
	struct KeyTrait<K> \
	{ using type = key_type; }

enum DMArrayKey
{
	DIFFICULTY_ARRAY,
	DEMON_DIFFICULTY_ARRAY,
	SELECTED_LIST_ARRAY,
};

enum DMMiscKey
{
	SAVE_DATA,
	GD_LIST_ID,
};


namespace DataManager
{
	namespace values
	{
		const std::unordered_map<DMArrayKey, std::string_view> ARRAY_KEY_TO_NAME{
			{ DMArrayKey::DIFFICULTY_ARRAY, "difficulty-array" },
			{ DMArrayKey::DEMON_DIFFICULTY_ARRAY, "demon-difficulty-array" },
			{ DMArrayKey::SELECTED_LIST_ARRAY, "selected-list-array" },
		};

		const std::unordered_map<DMMiscKey, std::string_view> MISC_KEY_TO_NAME{
			{ DMMiscKey::GD_LIST_ID, "gdListID" },
		};

		struct SavedArrayInfo
		{
			const std::size_t size;
			const std::vector<bool> default_value;

			SavedArrayInfo(std::size_t size, std::vector<bool>&& default_value)
				: size(size), default_value(std::move(default_value))
			{}
		};

		const std::unordered_map<DMArrayKey, const SavedArrayInfo> ARRAY_TO_SAI{
			{ DMArrayKey::DIFFICULTY_ARRAY, { 6, { true, false, false, false, false, false } } },
			{ DMArrayKey::DEMON_DIFFICULTY_ARRAY, { 5, { true, false, false, false, false, false }} },
			{ DMArrayKey::SELECTED_LIST_ARRAY, { 4, { true, false, false, false }} }
		};
	}

	namespace traits
	{
		template <DMMiscKey K>
		struct KeyTrait;

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
	[[nodiscard]] traits::KeyTrait<key>::type setDefaultSafe();


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
	[[nodiscard]] traits::KeyTrait<key>::type get()
	{
		using value_t = typename traits::KeyTrait<key>::type;

		if constexpr (key == DMMiscKey::SAVE_DATA)
		{
			value_t fromDisk;
			std::ifstream saveDataFile(geode::Mod::get()->getSaveDir() / "save_data", std::ios::binary);
			saveDataFile.read(reinterpret_cast<char*>(&fromDisk), sizeof(value_t));

			return fromDisk;
		}

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
		geode::Mod::get()->setSavedValue(values::ARRAY_KEY_TO_NAME.at(key), value);
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
			values::ARRAY_KEY_TO_NAME.at(key)
		).unwrap().asArray().unwrap().at(idx) = value;
	}

	/**
	 * @brief Sets the value of @tparam key in the save container to @param value.
	 *
	 * @tparam key The key to set the value of.
	 * @param value The new value.
	 */
	template <DMMiscKey key>
	void set(const typename traits::KeyTrait<key>::type& value)
	{
		if constexpr (key == DMMiscKey::SAVE_DATA)
		{
			std::ofstream saveDataFile(geode::Mod::get()->getSaveDir() / "save_data");
			saveDataFile.write(
				reinterpret_cast<const char*>(&value),
				sizeof(typename traits::KeyTrait<key>::type)
			);
		}
		else
		{
			geode::Mod::get()->setSavedValue(values::MISC_KEY_TO_NAME.at(key), value);
		}
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

		if (auto res = container.get(values::ARRAY_KEY_TO_NAME.at(key)); res.isOk())
			if (auto resv = res.unwrap().asArray(); resv.isOkAnd([](auto&& vec) {
					return vec.size() == values::ARRAY_TO_SAI.at(key).size &&
						std::all_of(vec.begin(), vec.end(), [](auto& v) { return v.asBool().isOk(); });
			}))
				return resv.unwrap();

		container.set(values::ARRAY_KEY_TO_NAME.at(key), values::ARRAY_TO_SAI.at(key).default_value);

		return container.get(values::ARRAY_KEY_TO_NAME.at(key)).unwrap().asArray().unwrap();
	}

	/**
	 * @brief Sets the value of @tparam key to its default if it doesn't exist or there is an error getting it.
	 *
	 * @tparam key The key to set the value of.
	 * @return traits::KeyTrait<key>::type The default value if there was an error, or the current value.
	 */
	template <DMMiscKey key>
	[[maybe_unused]] traits::KeyTrait<key>::type setDefaultSafe()
	{
		using value_t = typename traits::KeyTrait<key>::type;
		using save_container_t = std::conditional_t<std::is_same_v<value_t, int>, uint64_t, value_t>;

		if constexpr (key == DMMiscKey::SAVE_DATA)
		{
			std::ifstream saveDataFileIn(geode::Mod::get()->getSaveDir() / "save_data", std::ios::binary);
			saveDataFileIn.seekg(0, std::ios::end);

			if (saveDataFileIn.fail() || saveDataFileIn.tellg() != sizeof(value_t))
			{
				value_t defaultValue;

				std::ofstream saveDataFileOut(geode::Mod::get()->getSaveDir() / "save_data", std::ios::binary);
				saveDataFileOut.write(reinterpret_cast<char*>(&defaultValue), sizeof(value_t));

				return defaultValue;
			}

			value_t fromDisk;
			saveDataFileIn.seekg(0);
			saveDataFileIn.read(reinterpret_cast<char*>(&fromDisk), sizeof(value_t));

			return fromDisk;
		}
		else
		{
			auto& container = geode::Mod::get()->getSaveContainer();

			if (auto res = container.get(values::MISC_KEY_TO_NAME.at(key)); res.isOk())
				if (auto resv = res.unwrap().as<save_container_t>(); resv.isOk())
					return resv.unwrap();

			container.set(values::MISC_KEY_TO_NAME.at(key), value_t{});

			return value_t{};
		}
	}
};
