#pragma once

enum class AIK_INIT_APPROACH {
    CREATE,
    OPEN
};

class shared_mutex;
class shared_memory;

class aik {
public:
    explicit aik();
    int read_client_values(const Driver::Module& _game_module, const Driver::Module& _cryengine_module, AIK_READ& _aik_read);
    int read_client_values(const Driver::Module& _game_module, const Driver::Module& _cryengine_module, DISPATCH_SHARED& _dispatch_shared);
    int list_pages();

    int write_client_values(const AIK_WRITE& _aik_write);
    int write_client_values(const DISPATCH_SHARED& _dispatch_shared);

    int read_shared_values(DISPATCH_SHARED& _pdispatch_shared_struct);
    int write_shared_values(const DISPATCH_SHARED& _pdispatch_shared_struct);

    bool init_shared_mutex(LPCTSTR name, AIK_INIT_APPROACH init_appr);
    bool init_shared_memory(LPCTSTR name, AIK_INIT_APPROACH init_appr);

    int init_driver();
    int attach_proc(const wchar_t* proc_name);
    int get_proc_module(const wchar_t* module_name, Driver::Module& _module);
    const std::uintptr_t find_pattern(std::uintptr_t addr, const std::size_t sz, const BYTE*&& pattern);

    template<typename... Args>
    void debug_wprintf(const wchar_t* format, Args&&... args) {
        /* wait for previous value to flush */
        DISPATCH_SHARED _d_shd{};
        do {
            read_shared_values(_d_shd);
        } while (std::wcslen(_d_shd.m_aik_read->dbg_wprint) != 0);
        static constexpr auto dbg_buf_sz = 100;
        wchar_t _buf[dbg_buf_sz];
        swprintf_s(_buf, dbg_buf_sz, format, std::forward<Args>(args)...);
        wprintf_s(L"%s\n", _buf);
        debug_print(_buf, dbg_buf_sz);
    }

private:
    void debug_print(const wchar_t* _str, unsigned short sz);

    std::unique_ptr<shared_mutex> m_shared_mutex;
    std::unique_ptr<shared_memory> m_shared_memory;

    std::uintptr_t m_ptrs_cache[10]{};

    std::uintptr_t m_player_entity_pointer{};
    std::uintptr_t m_target_entity_pointer{};
};
