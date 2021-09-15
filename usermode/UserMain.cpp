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
	auto _addr_to_read = game_dll.addr + 0x4E;
	wprintf(L"[!] addr to read: 0x%llX\n", _addr_to_read);
	constexpr size_t read_sz = 68;
	BYTE prev_chat_msg[read_sz];
	BYTE new_chat_msg[read_sz];
	while (!GetAsyncKeyState(VK_F12))
	{
		if (driver->ReadMemType(_addr_to_read, new_chat_msg, read_sz) != STATUS_SUCCESS
			|| (memcmp(prev_chat_msg, new_chat_msg, read_sz) == 0)) {
			continue;
		}
		print_bytes_line(new_chat_msg, _addr_to_read, 16, " ");
		//wprintf(L"%s\n", new_chat_msg);

		memcpy(prev_chat_msg, new_chat_msg, read_sz);

		Sleep(100);
	}
	return 0;
}