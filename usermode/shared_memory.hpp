#pragma once
#include <shared_mutex.hpp>
#include <stdafx.h>

class shared_memory {
public:
    explicit shared_memory(LPCTSTR name, HANDLE mutex);

    shared_memory* create_file_mapping(HANDLE hFile,
                                       LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
                                       DWORD flProtect,
                                       DWORD dwMaximumSizeHigh,
                                       DWORD dwMaximumSizeLow);

    shared_memory* open_file_mapping(DWORD dwDesiredAccess,
                                     BOOL bInheritHandle = FALSE);

    
    void map_view(DWORD dwDesiredAccess,
                  DWORD dwFileOffsetHigh,
                  DWORD dwFileOffsetLow,
                  SIZE_T dwNumberOfBytesToMap);

    const LPVOID read_value(BYTE offset = 0) const;

    template<typename T>
    T read_value_typed(BYTE offset = 0) { 
        return *read_value(offset);
    }


    const LPCTSTR get_name() const noexcept;

    const HANDLE operator()() const;

private:
    HANDLE m_handle;
    const LPCTSTR& m_name;
    const HANDLE& m_hmutex;

    LPVOID m_mapped_addr;
};
