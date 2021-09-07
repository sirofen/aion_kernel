#include "stdafx.h"

#include <io.h>
#include <fcntl.h>

int main() {
	//setlocale(LC_CTYPE, "rus");
	_setmode(_fileno(stdout), _O_U16TEXT);

	wprintf(L"[!] Open driver connection: ");
	if (!driver->Init(FALSE)) {
		wprintf(L"Failed!\n");
		return -1;
	}
	wprintf(L"Success!\n");

	wprintf(L"[!] Attach process: ");
	if (!driver->Attach(AION::aion_bin)) {
		wprintf(L"Failed!\n");
		return -1;
	}
	wprintf(L"Success!\n");
	wprintf(L"[!] aion.bin PID: %u\n", driver->ProcessId);

	wprintf(L"[!] Get GAME.DLL module: ");
	auto game_dll = driver->GetModuleBase(AION::GAMEDLL::module_name);
	if (game_dll.empty()) {
		wprintf(L"Failed!\n");
		return -1;
	}
	wprintf(L"Success!\n");
	wprintf(L"[!] GAME.DLL base: 0x%llX\n", game_dll.addr);
	auto chat_addr = game_dll.addr + AION::GAMEDLL::chat_addr;
	wprintf(L"[!] Chat address: 0x%llX\n", chat_addr);

	CHAR prev_chat_msg[AION::GAMEDLL::chat_size]{'\0'};
	while (!GetAsyncKeyState(VK_F12))
	{
		CHAR new_chat_msg[AION::GAMEDLL::chat_size];
		if (driver->ReadMemType(chat_addr, new_chat_msg) != STATUS_SUCCESS
			|| (strcmp(prev_chat_msg, new_chat_msg) == 0)) {
			continue;
		}

		wprintf(L"%s\n", new_chat_msg);

		strcpy_s(prev_chat_msg, new_chat_msg);
		delete[] new_chat_msg;

		Sleep(100);
	}
	return 0;
}