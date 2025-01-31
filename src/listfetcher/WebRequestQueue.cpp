#include "WebRequestQueue.hpp"

using namespace geode::prelude;

void WebRequestQueue::enqueue(Request req)
{
	m_requests.emplace(std::move(req));
}

void WebRequestQueue::flush()
{
	static bool hasSlept = false;

	if (m_requests.empty()) return;

	auto [task, callback] = m_requests.front();
	m_requests.pop();

	task.listen([this, callback](auto res) {
		callback(res);
		this->flush();

		hasSlept = false;
	},
	[this](auto progress) {
		if (!hasSlept)
		{
			hasSlept = true;
			std::this_thread::sleep_for(std::chrono::milliseconds(m_sleep_time));
		}
	},
	[this] {
		this->flush();
	});
}
