#pragma once

template <typename T>
struct SingletonBase
{
public:
	static T& get()
	{
		struct InstanceCreator final : T {
			InstanceCreator() : T{} {}
		};
		static InstanceCreator instanceCreator;

		return instanceCreator;
	}

protected:
	SingletonBase() noexcept = default;
	SingletonBase(const SingletonBase&) = delete;
	SingletonBase(SingletonBase&&) = delete;
	SingletonBase& operator=(const SingletonBase&) = delete;
	SingletonBase& operator=(SingletonBase&&) = delete;
	~SingletonBase() noexcept = default;
};
