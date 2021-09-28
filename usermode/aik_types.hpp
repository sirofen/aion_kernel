#pragma once
//#include <stdafx.h>

struct _DISPATCH_SHARED;
typedef _DISPATCH_SHARED DISPATCH_SHARED, *PDISPATCH_SHARED;

struct _AIK_READ {
    DISPATCH_SHARED contruct_dispatch();
    wchar_t name[32];
    unsigned char level;
    bool gravity;
    float speed;
    float attack_speed;

    /* Target */
    float x;
    float y;
    float z;
    wchar_t dbg_wprint[100];
};
typedef _AIK_READ AIK_READ, *PAIK_READ;

struct _AIK_WRITE {
    DISPATCH_SHARED contruct_dispatch();
    bool gravity;
    float speed;
    float attack_speed;

    /* Target */
    float x;
    float y;
    float z;
};
typedef _AIK_WRITE AIK_WRITE, *PAIK_WRITE;

struct _DISPATCH_SHARED {
    _DISPATCH_SHARED(PAIK_READ _pa_r = 0, PAIK_WRITE _wa_r = 0) noexcept;
    const static unsigned long size() noexcept;
    PAIK_READ m_paik_read;
    PAIK_WRITE m_paik_write;
};
typedef _DISPATCH_SHARED DISPATCH_SHARED, *PDISPATCH_SHARED;
