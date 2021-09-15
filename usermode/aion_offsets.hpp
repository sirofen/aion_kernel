#pragma once
#include <windef.h>

namespace AION {
	constexpr LPCWSTR aion_bin = L"Aion.bin";
	namespace GAMEDLL {
		constexpr LPCWSTR module_name = L"Game.dll";
		constexpr DWORD chat_addr = 0xC19B45;
		constexpr WORD chat_size = 0x1FF; /* exceeds max length by 1 for null terminator */
	};
};
