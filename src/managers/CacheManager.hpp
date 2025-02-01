#pragma once

#include <memory>

#include <Geode/loader/Mod.hpp>
#include <matjson.hpp>

#include "SingletonBase.hpp"

#include "utils.hpp"

#define ADD_KEY_TRAIT(K, key_type) \
	template <> \
	struct CMKeyTrait<K> \
	{ using type = key_type; }


enum class CMKey
{
	NORMAL_LIST_MAX_PAGES
};

struct CacheManager : public SingletonBase<CacheManager>
{
private:
	template <CMKey K>
	struct CMKeyTrait;

	ADD_KEY_TRAIT(CMKey::NORMAL_LIST_MAX_PAGES, matjson::Value);


	struct CacheFunctionBase {};

	template <CMKey key>
	struct CacheFunction : CacheFunctionBase
	{
		using function_t = std::function<typename CMKeyTrait<key>::type()>;

		CacheFunction(function_t function)
			: m_function(std::move(function))
		{}
		CacheFunction(typename CMKeyTrait<key>::type(*function)())
			: m_function(std::move(function))
		{}

		CMKeyTrait<key>::type operator ()()
		{ return m_function(); }

		function_t m_function;
	};

public:
	void save();

	template <CMKey key>
	[[nodiscard]] CMKeyTrait<key>::type getValue()
	{
		if (auto cached = m_cache.get<matjson::Value>(getKeyString<key>()); cached.isOkAnd([](const matjson::Value& v) { return v.contains("time") && v.contains("value"); }))
		{
			auto time = cached.unwrap().template get<std::uint64_t>("time").unwrapOr(0);

			if (rl::utils::getUnixEpoch() - time >= CACHE_EXPIRY_TIME)
				setValue<key>(callCacheFunction<key>());
		}
		else
			setValue<key>(callCacheFunction<key>());

		return std::move(
			m_cache[getKeyString<key>()]["value"].template as<typename CMKeyTrait<key>::type>().unwrap()
		);
	}

	template <CMKey key, typename R, typename P>
	[[nodiscard]] R getValue(P keyName, R defaultValue = {}) requires(std::is_same_v<typename CMKeyTrait<key>::type, matjson::Value> && std::is_default_constructible_v<R>)
	{
		using result_t = std::conditional_t<std::is_same_v<R, int>, std::uint64_t, R>;

		if constexpr (std::is_enum_v<P> || std::is_integral_v<P>)
			return getValue<key>()[fmt::format("{}", static_cast<int>(keyName))].template as<result_t>().unwrapOr(defaultValue);
		else
			return getValue<key>()[keyName].template as<result_t>().unwrapOr(defaultValue);
	}

	template <CMKey key>
	void setValue(CMKeyTrait<key>::type& value)
	{
		m_cache.set(getKeyString<key>(), matjson::makeObject({
			{ "time", rl::utils::getUnixEpoch() },
			{ "value", value }
		}));
	}

	template <CMKey key>
	void setValue(const CMKeyTrait<key>::type& value)
	{
		m_cache.set(getKeyString<key>(), matjson::makeObject({
			{ "time", rl::utils::getUnixEpoch() },
			{ "value", value }
		}));
	}

	template <CMKey key>
	void appendValue(const std::string& keyName, const CMKeyTrait<key>::type& value)
		requires(std::is_same_v<typename CMKeyTrait<key>::type, matjson::Value>)
	{
		m_cache[getKeyString<key>()]["time"] = rl::utils::getUnixEpoch();
		m_cache[getKeyString<key>()]["value"][keyName] = value;
	}

	template <CMKey key>
	void addCacheFunction(CacheFunction<key> function)
	{
		m_cache_functions.emplace(key, std::make_unique<CacheFunction<key>>(function));
		static_cast<void>(getValue<CMKey::NORMAL_LIST_MAX_PAGES>());
	}

private:
	inline static constexpr auto CACHE_EXPIRY_TIME = 60 * 60 * 24 * 7; // every 7 days

	std::filesystem::path m_cache_filepath;
	matjson::Value m_cache;
	std::unordered_map<CMKey, std::unique_ptr<CacheFunctionBase>> m_cache_functions;

protected:
	CacheManager();
	~CacheManager();

private:
	template <CMKey key>
	inline static consteval std::string_view getKeyString()
	{
		if constexpr (key == CMKey::NORMAL_LIST_MAX_PAGES)
			return "normal-list-max-pages";

		throw "Invalid Cache key";
	}

	template <CMKey key>
	CMKeyTrait<key>::type callCacheFunction()
	{
		auto baseFunc = m_cache_functions.at(key).get();

		return static_cast<CacheFunction<key>*>(baseFunc)->operator()();
	}
};

#undef ADD_KEY_TRAIT
