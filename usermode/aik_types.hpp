#pragma once

struct _DISPATCH_SHARED;
typedef _DISPATCH_SHARED DISPATCH_SHARED, *PDISPATCH_SHARED;

struct _AIK_READ {
    DISPATCH_SHARED contruct_dispatch() const;
    const static unsigned long size() noexcept;

    wchar_t player_name[32];
    unsigned char player_level;

    bool player_gravity;
    float player_speed;
    std::uint32_t player_attack_speed;

    /* Target */
    float target_speed;
    std::uint32_t target_attack_speed;

    float target_x;
    float target_y;
    float target_z;

    wchar_t dbg_wprint[100];
};
typedef _AIK_READ AIK_READ, *PAIK_READ;

struct _AIK_WRITE {
    DISPATCH_SHARED contruct_dispatch() const;
    const static unsigned long size() noexcept;
    bool gravity;
    float speed;
    std::uint32_t attack_speed;

    /* Target */
    float x;
    float y;
    float z;
};
typedef _AIK_WRITE AIK_WRITE, *PAIK_WRITE;

struct _DISPATCH_SHARED {
    _DISPATCH_SHARED(std::unique_ptr<_AIK_READ> _aik_read = nullptr,
                     std::unique_ptr<_AIK_WRITE> _aik_write = nullptr);
    const static unsigned long size() noexcept;
    const static unsigned char aik_read_offset() noexcept;
    const static unsigned long aik_write_offset() noexcept;
    std::unique_ptr<_AIK_READ> m_aik_read;
    std::unique_ptr<_AIK_WRITE> m_aik_write;
};
typedef _DISPATCH_SHARED DISPATCH_SHARED, *PDISPATCH_SHARED;
