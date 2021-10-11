#pragma once
#include <windef.h>

namespace _aion_entity {
struct _entity {
    struct _basic_properties {
        static constexpr WORD pointer = 0x368;

        static constexpr BYTE name = 0x46;
        static constexpr BYTE lvl = 0x42;
        static struct _gravity {
            static constexpr WORD offset = 0x8D8;
            static constexpr BYTE gravity = 0x0;
            static constexpr BYTE no_gravity = 0x6;
        } gravity;
        static constexpr WORD attack_speed = 0x4FA;
        static constexpr WORD speed = 0x6CC; /*0x3C*/
    } basic_properties;

    struct _pos {
        static constexpr WORD pointer = 0x180;

        static constexpr BYTE z_coord = 0xA0;
        static constexpr BYTE y_coord = 0x9C;
        static constexpr BYTE x_coord = 0x98;
    } pos;
};
};// namespace _aion_entity

namespace AION_VARS {
constexpr LPCWSTR aion_bin = L"Aion.bin";
struct GAMEDLL {
    static constexpr LPCWSTR module_name = L"Game.dll";
    struct self_pointer {
        static constexpr DWORD pointer_0 = 0xDC7918;
        static constexpr BYTE pointer_1 = 0x70;
        static constexpr BYTE pointer_2 = 0x10;
        static constexpr BYTE pointer_3 = 0x20;

        static inline _aion_entity::_entity player;
    };
    struct target_pointer {
        static constexpr BYTE pointer_pattern[] = {0x9A, 0x99, 0x99, 0x3E, 0x9A, 0x99, 0x99, 0x3E, 0x9A, 0x99, 0x99, 0x3E, 0x00, 0x00};
        static constexpr BYTE pattern_offset = 0x9;

        static constexpr DWORD pointer_0 = 0xDC8250;

        static inline _aion_entity::_entity target;
    };
};
struct CRY3DENGINE {
    static constexpr LPCWSTR module_name = L"Cry3DEngine.dll";
    struct radar {
        static constexpr DWORD pointer_0 = 0x26BF68;

        static constexpr WORD radar_offset = 0x2B8;

        static constexpr BYTE radar_init = 0x5;    
        static constexpr BYTE radar_deinit = 0x0; 
    };
};
struct console {
    static constexpr BYTE pattern[] = {
        0x63, 0x6F, 0x6E, 0x5F, 0x64, 0x69, 0x73, 0x61, 0x62, 0x6C, 0x65, 0x5F, 0x63, 0x6F, 0x6E, 0x73, 
        0x6F, 0x6C, 0x65, 0x00, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 
        0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xC0, 0xEF, 0x45, 0x41, 0x00, 0x00, 0x00, 0x00, 0xCD, 
        0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 
        0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 
        0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 
        0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 
        0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 
        0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0xCD, 0x0A, 0xA5, 0x62, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x0A, 
        0x08, 0x00, 0x00, 0x03, 0x00, 0x00
    };
    static constexpr auto mask =
            "1111111111111111"
            "1111000000000000"
            "0000000000000000"
            "0000000000000000"
            "0000000000000000"
            "0000000000000000"
            "0000000000000000"
            "0000000000000000"
            "0000000000000001"
            "111111";
                               
    static constexpr WORD disable_console_offset = 0xAF;

    static constexpr BYTE enable = 0x0;
    static constexpr BYTE disable = 0x1;
};
//FB 1B EB A0 DD 15 EB 00 00 00 00 55 55 99 99 55 55 99 99 9C 6F 82 07 00 63 6F 6E 5F 64 69 73 61 62 6C 65 5F 63 6F 6E 73 6F 6C 65 00 55 99 99 55 55 99 99 41 1A A0 11 55 00 55 00 55 55 99 99 55 55 99 99 41 1A A0 11 55 00 55 00 55 55 99 99 55 55 99 99 41 1A 60 11 5F A0 5F A0 55 55 99 99 55 55 99 99 61 1A 40 09 55 AA F5 02 55 55 99 99 55 55 99 99 41 1A 40 09 55 AA 57 A8 55 55 99 99 55 55 99 99 41 12 40 09 55 AA F5 02 55 55 99 99 55 55 99 99 21 12 40 09 55 EA 5D C0 FD 8C 81 07 0A 08 00 00 03 00 00 00 E8 E8 33 EC E0 E8 33 EC E4 E8 33 EC 01 00 00 00 00 00 80 3F 31 00 00 00 00
};// namespace AION_VARS