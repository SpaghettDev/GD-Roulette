#include <random>
#include "utils.hpp"

int utils::randomInt(int min, int max)
{
	std::random_device device;
	std::mt19937 generator(device());
	std::uniform_int_distribution<int> distribution(min, max);

	return distribution(generator);
}

void utils::setClipboardText(std::string text)
{
	if (OpenClipboard(0))
	{
		HGLOBAL clipbuffer;
		char* buffer;
		EmptyClipboard();

		clipbuffer = GlobalAlloc(GMEM_DDESHARE, text.size() + 1);
		if (clipbuffer)
		{
			buffer = static_cast<char*>(GlobalLock(clipbuffer));
			if (buffer)
			{
				strcpy(buffer, text.c_str());
				GlobalUnlock(clipbuffer);
				SetClipboardData(CF_TEXT, clipbuffer);
			}
		}

		CloseClipboard();
	}
}
