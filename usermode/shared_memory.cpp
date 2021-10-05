#include <stdafx.h>
#include <shared_memory.hpp>

shared_memory::shared_memory(const LPCTSTR& name, const HANDLE& mutex)
    : m_handle()
    , m_name(name)
    , m_hmutex(mutex)
    , m_mapped_addr() {}

shared_memory::~shared_memory() {
    if (!UnmapViewOfFile(m_mapped_addr)) {
        display_error("Unmap view of file error");
    }
}

shared_memory* shared_memory::create_file_mapping(HANDLE hFile,
                                                  LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
                                                  DWORD flProtect,
                                                  DWORD dwMaximumSizeHigh,
                                                  DWORD dwMaximumSizeLow) {
    m_handle = CreateFileMapping(hFile,
                                 lpFileMappingAttributes,
                                 flProtect,
                                 dwMaximumSizeHigh,
                                 dwMaximumSizeLow,
                                 m_name);
    if (m_handle == NULL) {
        display_error("Create file mapping error");
    }
    //std::printf("mapping: 0x%p\n", m_handle);
    return this;
}

shared_memory* shared_memory::open_file_mapping(DWORD dwDesiredAccess,
                                                BOOL bInheritHandle) {
    m_handle = OpenFileMapping(dwDesiredAccess, bInheritHandle, m_name);
    if (m_handle == NULL) {
        display_error("Open file mapping error");
    }
    return this;
}


void shared_memory::map_view(DWORD dwDesiredAccess,
                             DWORD dwFileOffsetHigh,
                             DWORD dwFileOffsetLow,
                             SIZE_T dwNumberOfBytesToMap) {
    if (m_hmutex == NULL) {
        RaiseException(EXCEPTION_INVALID_HANDLE, 0, NULL, NULL);
        return;
    }
    //std::printf("mapping: 0x%p\n", m_handle);
    DWORD dw_wait_res;
    //for (int i = 0; i < 20; i++) {
    dw_wait_res = WaitForSingleObject(m_hmutex, INFINITE);

    switch (dw_wait_res) {
        case WAIT_OBJECT_0: {
            __try {
                m_mapped_addr = MapViewOfFile(m_handle,
                                       dwDesiredAccess,
                                       dwFileOffsetHigh,
                                       dwFileOffsetLow,
                                       dwNumberOfBytesToMap);
            } __finally {
                if (!ReleaseMutex(m_hmutex)) {
                    display_error("Release mutex error");
                    return;
                } 
                wprintf(L"Mapped address: 0x%llX, sz: %lu\n", m_mapped_addr, dwNumberOfBytesToMap);
            }
            break;
        }
        case WAIT_ABANDONED: {
            m_mapped_addr = 0x0;
        }
    }
}

const LPVOID shared_memory::value_pointer(const ULONG& offset) const {
    if (m_mapped_addr == 0x0) {
        throw std::runtime_error("Mapped address is null");
    }
    return (LPVOID)((unsigned long long)m_mapped_addr + offset);
}

const LPCTSTR shared_memory::get_name() const noexcept {
    return m_name;
}

const HANDLE shared_memory::operator()() const {
    if (m_handle == NULL) {
        RaiseException(EXCEPTION_INVALID_HANDLE, 0, NULL, NULL);
    }
    return m_handle;
}