#ifndef COMMON_H
#define COMMON_H

namespace common
{
	inline HMODULE hmodule;
	inline HANDLE mainThread;
	inline DWORD threadID;

	inline std::string_view version = "1.3.0";
}

#endif //COMMON_H