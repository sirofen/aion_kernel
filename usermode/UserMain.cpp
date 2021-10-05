//#include "stdafx.h"
//
//#include <aion_offsets.hpp>
//
//#include <io.h>
//#include <fcntl.h>
//
//#include <macro.h>
//
//int main_old() {
//	//setlocale(LC_CTYPE, "rus");
//	_setmode(_fileno(stdout), _O_U16TEXT);
//
//	wprintf(L"[!] Open driver connection: ");
//	if (!driver->Init(FALSE)) {
//		wprintf(L"Failed!\n");
//		return -1;
//	}
//	wprintf(L"Success!\n");
//
//	wprintf(L"[!] Attach process: ");
//    if (!driver->Attach(AION_VARS::aion_bin)) {
//		wprintf(L"Failed!\n");
//		return -1;
//	}
//	wprintf(L"Success!\n");
//	wprintf(L"[!] aion.bin PID: %u\n", driver->ProcessId);
//
//	wprintf(L"[!] Get GAME.DLL module: ");
//    auto game_dll = driver->GetModuleBase(AION_VARS::GAMEDLL::module_name);
//	if (game_dll.empty()) {
//		wprintf(L"addr: 0x%llX, sz: %u\n", game_dll.addr, game_dll.size);
//		wprintf(L"Failed!\n");
//		return -1;
//	}
//	wprintf(L"Success!\n");
//    wprintf(L"addr: 0x%llX, sz: %u\n", game_dll.addr, game_dll.size);
//	wprintf(L"[!] GAME.DLL base: 0x%llX\n", game_dll.addr);
//
//	wprintf(L"[!] Pattern search ...");						/*0xAFF000*/
//    //auto base_addr_t = driver->FindPattern(game_dll.addr + 0x90000, game_dll.size, AION::GAMEDLL::target::pointer_pattern) /*+ AION::GAMEDLL::target::pattern_offset*/;
//	//auto base_addr_t = driver->FindPatternModule(game_dll, AION::GAMEDLL::target::pointer_pattern);
//    const auto& base_addr_t = game_dll.addr;
//
//	//game dll size dec 28667904
//    //auto base_addr_t = game_dll.addr + 0x217;
//	/*9E44A0*/
//	/*camel 0x4DD9824A*/
//	if (base_addr_t == 0) {
//		wprintf_s(L"Not Found");
//		return 0;
//	}
//	wprintf_s(L"Found: { 0x%llX }\n", base_addr_t);
//	//auto base_addr_t = game_dll.addr + 0xB45EAC - 0x8;
//	wprintf(L"[!] addr to read abs: 0x%llX\n", base_addr_t);
//    wprintf(L"[!] addr to read rel: 0x%llX\n", base_addr_t - game_dll.addr);
//	constexpr size_t read_sz = 0x10;
//
//	BYTE prev_bytes[read_sz];
//	BYTE new_bytes[read_sz];
//
//	std::uintptr_t ptr_cache[2] = {0, 0};
//    std::uintptr_t _addr_t = 0;
//
//    //DWORD attack_speed_100 = 0x64;
//    BYTE gravity = 0;
//
//	float _z_pos = 0, _z_pos_old = 0;
//    float _y_pos = 0, _y_pos_old = 0;
//    float _x_pos = 0, _x_pos_old = 0;
//
//    for (; !GetAsyncKeyState(VK_F12); Sleep(500)) {
//
//		//driver->dump_memory(base_addr_t, 1024 * 1024 * 1);
//		/* can't use just ! 'cos there's lambda declaration in front */
//        //0x9
//        if (POINTER(wprintf_s, base_addr_t, AION_VARS::GAMEDLL::self_pointer::pointer_0, _addr_t, 4, ptr_cache, 0, true) == false) {
//			continue;
//		}
//
//        if (POINTER(wprintf_s, _addr_t, AION_VARS::GAMEDLL::self_pointer::pointer_1, _addr_t, 4, ptr_cache, 1, false) == false) {
//			continue;
//		}
//        if (driver->ReadMemType(_addr_t + AION_VARS::GAMEDLL::target::pos::z_cord, _z_pos) != STATUS_SUCCESS ||
//            driver->ReadMemType(_addr_t + AION_VARS::GAMEDLL::target::pos::y_cord, _y_pos) != STATUS_SUCCESS ||
//            driver->ReadMemType(_addr_t + AION_VARS::GAMEDLL::target::pos::x_cord, _x_pos) != STATUS_SUCCESS ||
//            (_z_pos_old == _z_pos) && (_y_pos_old == _y_pos) && (_x_pos_old == _x_pos)) {
//            continue;
//        }
//
//		wprintf_s(L"\nx: %6.3f, y: %6.3f, z: %6.3f", _x_pos, _y_pos, _z_pos);
//
//		_z_pos_old = _z_pos;
//        _y_pos_old = _y_pos;
//        _x_pos_old = _x_pos;
//
//		driver->ReadMemType(_addr_t + AION::GAMEDLL::target::pos::z_cord, new_bytes, read_sz);
//
//        print_bytes_line(new_bytes, _addr_t, 16, " ");
//		//driver->dump_memory(_addr_t, 1024 * 1024 * 10);
//
//        //wprintf_s(L"\nWIDE: {%s}\n", new_bytes);
//
//        //driver->WriteMemType(_addr_t + AION::GAMEDLL::target::gravity, gravity);
//
//
//        //if (driver->ReadMemType(_addr_t + AION::GAMEDLL::target::gravity, new_bytes, read_sz) != STATUS_SUCCESS
//        //    //if (driver->ReadMemType(_addr_t, new_bytes, read_sz) != STATUS_SUCCESS
//        //    || (memcmp(prev_bytes, new_bytes, read_sz) == 0)) {
//        //    continue;
//        //}
//
//        //print_bytes_line(new_bytes, _addr_t, 16, " ");
//
//        //wprintf_s(L" -> {3}:(*){0x%llX}\n", _addr_t);
//
//		//memcpy_s(prev_bytes, read_sz, new_bytes, read_sz);
//        //break;
//	}
//	return 0;
//}