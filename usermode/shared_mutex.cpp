#include <stdafx.h>
#include <shared_mutex.hpp>
#include <iostream>

shared_mutex::shared_mutex(const LPCTSTR& name)
    : m_handle()
    , m_name(name) {}

shared_mutex::~shared_mutex() {
    if (!CloseHandle(m_handle)) {
        display_error("Close mutex handle error");
    }
}

shared_mutex* shared_mutex::create_mutex(
        LPSECURITY_ATTRIBUTES security_attr,
        BOOL initial_owner) {
    m_handle = CreateMutex(security_attr, initial_owner, m_name);
    if (m_handle == NULL) {
        display_error("Create mutex error");
    }
    //std::printf("mutex: 0x%p\n", m_handle);
    return this;
}

shared_mutex* shared_mutex::open_mutex(
        DWORD access_flags,
        BOOL initial_owner) {
    m_handle = OpenMutex(access_flags, initial_owner, m_name);
    if (m_handle == NULL) {
        display_error("Open mutex error");
    }
    return this;
}

const HANDLE shared_mutex::operator()() const {
    if (m_handle == NULL) {
        RaiseException(EXCEPTION_INVALID_HANDLE, 0, NULL, NULL);
    }
    return m_handle;
}

const HANDLE shared_mutex::handle() const {
    if (m_handle == NULL) {
        RaiseException(EXCEPTION_INVALID_HANDLE, 0, NULL, NULL);
    }
    return m_handle;
}

const LPCTSTR shared_mutex::get_name() const noexcept {
    return m_name;
}
