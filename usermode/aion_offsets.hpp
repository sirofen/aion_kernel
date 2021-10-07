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
        static constexpr WORD pointer_1 = 0x2B8;

        static constexpr BYTE radar_init = 0x5;    
        static constexpr BYTE radar_deinit = 0x0; 
    };
};
};// namespace AION_VARS