#pragma once

#include <chrono>

class GameTimer
{
public:
	constexpr GameTimer()
		: m_isRunning(false), m_elapsedTime(0)
	{}

	constexpr GameTimer(std::uint64_t& elapsedTime)
		: m_isRunning(false), m_elapsedTime(elapsedTime)
	{}

	constexpr void start()
	{
		if (m_isRunning)
			return;

		m_startTime = std::chrono::steady_clock::now();
		m_isRunning = true;
	}

	constexpr void pause()
	{
		if (!m_isRunning)
			return;

		auto now = std::chrono::steady_clock::now();
		m_elapsedTime += std::chrono::duration_cast<std::chrono::seconds>(now - m_startTime).count();
		m_isRunning = false;
	}

	constexpr void saveTime()
	{
		if (!m_isRunning)
			return;

		auto now = std::chrono::steady_clock::now();
		m_elapsedTime += std::chrono::duration_cast<std::chrono::seconds>(now - m_startTime).count();
		m_startTime = now;
	}

	constexpr std::uint64_t getElapsedTime() const
	{
		if (!m_isRunning)
			return m_elapsedTime;

		auto now = std::chrono::steady_clock::now();
		return m_elapsedTime + std::chrono::duration_cast<std::chrono::seconds>(now - m_startTime).count();
	}

	constexpr void setElapsedTime(std::uint64_t elapsedTime)
	{
		m_elapsedTime = elapsedTime;
	}

	constexpr void reset()
	{
		m_isRunning = false;
		m_elapsedTime = 0;
	}

private:
	bool m_isRunning;
	std::chrono::steady_clock::time_point m_startTime;
	std::uint64_t m_elapsedTime; // in seconds
};
