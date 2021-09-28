#pragma once
#include <aik_types.hpp>
#include <stdafx.h>

#include <utility>
#include <memory>
//#include <stdio.h>
/*
mutex::lock
Aion values go into shared memory.

mutex::unlock

Gui reads shared memory and prints values.
Gui modifies shared memory value.

*/

enum class AIK_INIT_APPROACH {
    CREATE,
    OPEN
};

class shared_mutex;
class shared_memory;

class aik {
public:
    explicit aik();
    int read_client_values();
    int write_client_values();

    int read_shared_values();
    int write_shared_values(const DISPATCH_SHARED& _pdispatch_shared_struct);

    bool init_shared_mutex(LPCTSTR name, AIK_INIT_APPROACH init_appr);
    bool init_shared_memory(LPCTSTR name, AIK_INIT_APPROACH init_appr);

    bool init_driver();

    template<typename... Args>
    void debug_wprintf(const wchar_t* format, Args&&... args) const {
        static constexpr auto dbg_buf_sz = 100;
        wchar_t _buf[dbg_buf_sz];
        swprintf_s(_buf, dbg_buf_sz, format, std::forward<Args>(args)...);
        debug_print(_buf, dbg_buf_sz);
    }
private:
    void debug_print(const wchar_t* _str, unsigned short sz);

    std::unique_ptr<shared_mutex> m_shared_mutex;
    std::unique_ptr<shared_memory> m_shared_memory;
};
