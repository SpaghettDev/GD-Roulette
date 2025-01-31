#pragma once

#include <functional>
#include <queue>

#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>

#include "SingletonBase.hpp"

// thanks peter ida
class WebRequestQueue : public SingletonBase<WebRequestQueue>
{
public:
	struct Request
	{
		geode::utils::web::WebTask webTask;
		std::function<void(geode::utils::web::WebResponse*)> callback;
	};

	void setSleepTime(std::uint64_t time) { m_sleep_time = time; }

	void enqueue(Request);
	template <typename ...Args>
	void enqueue(Args&&... args) requires(std::is_same_v<std::decay_t<Args>, Request> && ...)
	{ (enqueue(std::forward<Args>(args)), ...); }

	void flush();

private:
	std::queue<Request> m_requests;
	std::uint64_t m_sleep_time = 300;
};
