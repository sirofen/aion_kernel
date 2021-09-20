#pragma once
#include <windef.h>

namespace AION {
	constexpr LPCWSTR aion_bin = L"AION.bin";
	namespace GAMEDLL {
		constexpr LPCWSTR module_name = L"Game.dll";
		namespace target {
			constexpr BYTE pointer_pattern[] = { 0x3E, 0x9A, 0x99, 0x99, 0x3E, 0x9A, 0x99, 0x99, 0x3E, 0x00, 0x00 };
			//constexpr BYTE pointer_pattern[] = {0x70, 0x75, 0x6e, 0x70, 0x63, 0x6b, 0x68, 0x77, 0x64, 0x76, 0x6d, 0x64, 0x65, 0x6f};
			constexpr BYTE pattern_offset = 0x22;
		}
		constexpr DWORD chat_addr = 0xC19B45;
		constexpr WORD chat_size = 0x1FF; /* exceeds max length by 1 for null terminator */
	};
};