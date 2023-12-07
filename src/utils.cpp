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

bool utils::isProcessLoaded(std::string processName, DWORD processID)
{
	bool found = false;
	HMODULE modules[1024];
	HANDLE process;
	DWORD processesIdentifiers;

	process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
	if (process == NULL)
		return false;

	if (EnumProcessModules(process, modules, sizeof(modules), &processesIdentifiers))
	{
		for (unsigned int i = 0; i < (processesIdentifiers / sizeof(HMODULE)); i++)
		{
			char moduleName[MAX_PATH];

			if (GetModuleFileNameEx(process, modules[i], moduleName, sizeof(moduleName) / sizeof(char)))
			{
				if (std::string(moduleName).find(processName) != std::string::npos)
				{
					found = true;
					break;
				}
			}
		}
	}

	CloseHandle(process);
	return found;
}


CCLabelBMFont* utils::createTextLabel(const std::string text, const CCPoint& position, const float scale, CCNode* menu, const char* font)
{
	CCLabelBMFont* bmFont = CCLabelBMFont::create(text.c_str(), font);
	bmFont->setPosition(position);
	bmFont->setScale(scale);
	menu->addChild(bmFont);

	return bmFont;
}
