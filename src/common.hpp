#ifndef COMMON_H
#define COMMON_H

// workaround linker errors lol
namespace
{
	namespace common
	{
		HMODULE hmodule;
		HANDLE mainThread;
		DWORD threadID;

		std::string_view version = "1.1.4";
	}
}

#endif //COMMON_H