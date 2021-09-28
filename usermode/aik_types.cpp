#include <aik_types.hpp>

_DISPATCH_SHARED _AIK_READ::contruct_dispatch() {
    return DISPATCH_SHARED(this);
};

_DISPATCH_SHARED _AIK_WRITE::contruct_dispatch() {
    return DISPATCH_SHARED(0, this);
};

_DISPATCH_SHARED::_DISPATCH_SHARED(PAIK_READ _pa_r, PAIK_WRITE _wa_r) noexcept
    : m_paik_read(_pa_r)
    , m_paik_write(_wa_r) {}

const unsigned long _DISPATCH_SHARED::size() noexcept {
    return sizeof(_AIK_READ) + sizeof(_AIK_WRITE);
}
