#pragma once

#include "JsonManager.hpp"

// this class just writes the savedata after each assignment
// because 1: there is no main loop
// and 2: there aren't that many options to save, so i cba'd to hook the render loop or similar
template<typename T>
class WriteInvoking
{
public:
	WriteInvoking<T>& operator=(const T& t)
	{
		m_data = t;

		JsonManager::save();

		return *this;
	}

	constexpr bool operator==(const T t)
	{
		return t == m_data;
	}

	constexpr operator T()
	{
		return m_data;
	}

	WriteInvoking(T t)
		: m_data(t) {}

	WriteInvoking()
		: m_data{} {}

	void assignNoSave(T t)
	{
		m_data = t;
	}

private:
	T m_data;
};

template<typename T, std::size_t S>
class WriteInvoking<std::array<T, S>> : public std::array<WriteInvoking<T>, S>
{};
