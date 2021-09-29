#pragma once

class shared_mutex {
public:
    explicit shared_mutex(LPCTSTR name);
    ~shared_mutex();

    shared_mutex* create_mutex(LPSECURITY_ATTRIBUTES security_attr = NULL,
                               BOOL initial_owner = FALSE);

    shared_mutex* open_mutex(DWORD access_flags,
                             BOOL initial_owner = FALSE);

    const LPCTSTR get_name() const noexcept;

    HANDLE operator()() const;

private:
    HANDLE m_handle;
    LPCTSTR m_name;
};
