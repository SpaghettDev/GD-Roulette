#pragma once
#include "pch.hpp"

namespace utils
{
	void setClipboardText(std::string text)
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
					CloseClipboard();
					return;
				}
			}

			CloseClipboard();
		}
	}
}
