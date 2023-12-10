#ifndef COMMON_H
#define COMMON_H

// workaround linker errors lol
namespace common
{
	inline HMODULE hmodule;
	inline HANDLE mainThread;
	inline DWORD threadID;

	inline std::string_view version = "1.2.1";
}

#endif //COMMON_H