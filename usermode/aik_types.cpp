#include <stdafx.h>
#include <aik_types.hpp>

_DISPATCH_SHARED _AIK_READ::contruct_dispatch() {
    return _DISPATCH_SHARED(std::make_unique<_AIK_READ>(*this));
};
const unsigned long _AIK_READ::size() noexcept {
    return sizeof(_AIK_READ);
}

_DISPATCH_SHARED _AIK_WRITE::contruct_dispatch() {
    return _DISPATCH_SHARED(nullptr, std::make_unique<_AIK_WRITE>(*this));
};
const unsigned long _AIK_WRITE::size() noexcept {
    return sizeof(_AIK_WRITE);
}

_DISPATCH_SHARED::_DISPATCH_SHARED(std::unique_ptr<_AIK_READ> _aik_read,
                                   std::unique_ptr<_AIK_WRITE> _aik_write)
    : m_aik_read(std::move(_aik_read))
    , m_aik_write(std::move(_aik_write)) {}

const unsigned long _DISPATCH_SHARED::size() noexcept {
    return _AIK_READ::size() + _AIK_WRITE::size();
}
const unsigned char _DISPATCH_SHARED::aik_read_offset() noexcept {
    return 0;
}
const unsigned long _DISPATCH_SHARED::aik_write_offset() noexcept {
    return _AIK_READ::size();
}
