#pragma once

class shared_memory {
public:
    explicit shared_memory(const LPCTSTR& name, const HANDLE& mutex);
    ~shared_memory();

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

    const LPVOID value_pointer(const DWORD& offset = 0) const;

    template<typename T>
    T read_value_typed(const ULONG& offset = 0, const ULONG& sz = sizeof(T)) {
        WaitForSingleObject(m_hmutex, INFINITE);
        T _pval = *(T*)value_pointer(offset);
        ReleaseMutex(m_hmutex);
        return _pval;
    }

    template<typename T>
    void write_value_typed(const T& _val, const ULONG& sz = sizeof(T)) {
        WaitForSingleObject(m_hmutex, INFINITE);
        //printf("mapped addr: 0x%p", m_mapped_addr);
        CopyMemory(m_mapped_addr, &_val, sz);
        ReleaseMutex(m_hmutex);
    }

    const LPCTSTR get_name() const noexcept;

    const HANDLE operator()() const;

private:
    HANDLE m_handle;
    const LPCTSTR& m_name;
    const HANDLE& m_hmutex;

    LPVOID m_mapped_addr;
};
