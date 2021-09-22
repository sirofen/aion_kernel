#pragma once
#include <windef.h>



namespace AION {
	constexpr LPCWSTR aion_bin = L"Aion.bin";
	namespace GAMEDLL {
		constexpr LPCWSTR module_name = L"Game.dll";
		namespace target {
			constexpr BYTE pointer_pattern[] = {0x9A, 0x99, 0x99, 0x3E, 0x9A, 0x99, 0x99, 0x3E, 0x9A, 0x99, 0x99, 0x3E, 0x00, 0x00};
            constexpr BYTE pattern_offset = 0x9;

			constexpr DWORD pointer_1 = 0xDC8250;
            constexpr WORD pointer_2 = 0x368;

            constexpr BYTE name = 0x46;
			constexpr BYTE lvl = 0x42;
            constexpr WORD gravity = 0x8D8;
			constexpr WORD attack_speed = 0x4FA;
            constexpr WORD speed = 0x6CC; /*0x3C*/

			namespace pos {
				constexpr WORD pointer_2_pos = 0x180;
				constexpr BYTE z_cord = 0xA0;
				constexpr BYTE y_cord = 0x9C;
				constexpr BYTE x_cord = 0x98;
			}
		}
	};// namespace GAMEDLL
	namespace CRY3DENGINE {
		constexpr LPCWSTR module_name = L"Cry3DEngine.dll";
		constexpr DWORD radar = 0x26BF68;
	}
};