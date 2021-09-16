﻿#include "stdafx.h"

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
	//auto base_addr_t = game_dll.addr + 0xDC8243 + 0xD;
	wprintf(L"[!] Pattern search ...");
	auto base_addr_t = driver->FindPatternModule(game_dll, AION::GAMEDLL::target::pointer_pattern) + AION::GAMEDLL::target::pattern_offset;
	if (base_addr_t == 0) {
		wprintf_s(L"Not Found");
		return 0;
	}
	wprintf_s(L"Found: { 0x%llX }\n", base_addr_t);
	//auto base_addr_t = game_dll.addr + 0xB45EAC - 0x8;
	wprintf(L"[!] addr to read: 0x%llX\n", base_addr_t);
	constexpr size_t read_sz = 100;
	BYTE prev_bytes[read_sz];
	BYTE new_bytes[read_sz];

	std::uintptr_t ptr_cache[3] = {0, 0, 0};
	std::uintptr_t _addr_t = 0;
	while (!GetAsyncKeyState(VK_F12))
	{
		if (driver->ReadMemType(base_addr_t, _addr_t, 4) != STATUS_SUCCESS) {
			continue;
		}
		//if (driver->ReadMemType(base_addr_t, new_bytes, read_sz) != STATUS_SUCCESS
		//	|| (memcmp(prev_bytes, new_bytes, read_sz) == 0)) {
		//	continue;
		//}
		//print_bytes_line(new_bytes, _addr_t, 16, " ");
		//memcpy(prev_bytes, new_bytes, read_sz);

		if (_addr_t != ptr_cache[0]) {
			ptr_cache[0] = _addr_t;
			wprintf_s(L"\n\t{0}:[0x%llX]", base_addr_t);
			wprintf_s(L" -> {1}:[0x%llX]", _addr_t);
		}
		if (driver->ReadMemType(_addr_t + 0x254, _addr_t, 4) != STATUS_SUCCESS) {
			continue;
		}
		if (_addr_t != ptr_cache[1]) {
			ptr_cache[1] = _addr_t;
			wprintf_s(L" -> {2}:[0x%llX]", _addr_t);
		}
		if ((_addr_t += 0x3A) != ptr_cache[2]) {
			ptr_cache[2] = _addr_t;
			wprintf_s(L" -> {3}:(*){0x%llX}\n", _addr_t);
		}

		if (driver->ReadMemType(_addr_t, new_bytes, read_sz) != STATUS_SUCCESS
			|| (memcmp(prev_bytes, new_bytes, read_sz) == 0)) {
			continue;
		}

		print_bytes_line(new_bytes, _addr_t, 16, " ");
		wprintf(L"\nWIDE: {%s}\n", new_bytes);

		memcpy(prev_bytes, new_bytes, read_sz);

		Sleep(100);
	}
	return 0;
}