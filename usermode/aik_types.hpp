#pragma once

struct _DISPATCH_SHARED;
typedef _DISPATCH_SHARED DISPATCH_SHARED, *PDISPATCH_SHARED;

struct _AIK_READ {
    //_AIK_READ();
    DISPATCH_SHARED contruct_dispatch();
    const static unsigned long size() noexcept;
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
    //_AIK_WRITE();
    DISPATCH_SHARED contruct_dispatch();
    const static unsigned long size() noexcept;
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
    //_DISPATCH_SHARED(PAIK_READ _pa_r = 0, PAIK_WRITE _wa_r = 0)
    _DISPATCH_SHARED(std::unique_ptr<_AIK_READ> _aik_read,
                     std::unique_ptr<_AIK_WRITE> _aik_write = nullptr);
    const static unsigned long size() noexcept;
    const static unsigned char aik_read_offset() noexcept;
    const static unsigned char aik_write_offset() noexcept;
    std::unique_ptr<_AIK_READ> m_aik_read;
    std::unique_ptr<_AIK_WRITE> m_aik_write;
};
typedef _DISPATCH_SHARED DISPATCH_SHARED, *PDISPATCH_SHARED;
