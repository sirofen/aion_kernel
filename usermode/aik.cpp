#include <stdafx.h>
#include <aik.hpp>
#include <global_vars.hpp>
#include <memory>

#include <macro.h>
#include <aion_offsets.hpp>

//#define AION_KERNEL_DEBUG_DUMP_PAGES_LIST

aik::aik()
    : m_ptrs_cache()
    , m_player_entity_pointer()
    , m_target_entity_pointer() {}

namespace {
constexpr auto& self_basic_properties = AION_VARS::GAMEDLL::self_pointer::player.basic_properties;
constexpr auto& self_pos = AION_VARS::GAMEDLL::self_pointer::player.pos;
constexpr AION_VARS::CRY3DENGINE::radar radar;
}

const int aik::read_client_values(const Driver::Module& _game_module, const Driver::Module& _cryengine_module, AIK_READ& _aik_read) {
    if (!driver) {
        return -0xA;
    }

    std::uintptr_t _traverse_pointer{};

    /* player pointer */
    /* can't use just ! 'cos there's lambda declaration in front */
    if (POINTER(debug_wprintf, _game_module.addr, AION_VARS::GAMEDLL::self_pointer::pointer_0, _traverse_pointer, 4, m_ptrs_cache, 0, true) == false) {
        return -0x3A;
    }
    if (POINTER(debug_wprintf, _traverse_pointer, AION_VARS::GAMEDLL::self_pointer::pointer_1, _traverse_pointer, 4, m_ptrs_cache, 1, false) == false) {
        return -0x3B;
    }
    if (POINTER(debug_wprintf, _traverse_pointer, AION_VARS::GAMEDLL::self_pointer::pointer_2, _traverse_pointer, 4, m_ptrs_cache, 2, false) == false) {
        return -0x3C;
    }
    if (POINTER(debug_wprintf, _traverse_pointer, AION_VARS::GAMEDLL::self_pointer::pointer_3, _traverse_pointer, 4, m_ptrs_cache, 3, false) == false) {
        return -0x3D;
    }

    /* read player basic properties */
    m_player_entity_pointer = _traverse_pointer;

    if (POINTER(debug_wprintf, m_player_entity_pointer, self_basic_properties.pointer, _traverse_pointer, 4, m_ptrs_cache, 4, false) == false) {
        return -0x40;
    }
    if (!NT_SUCCESS(driver->ReadMemType(_traverse_pointer + self_basic_properties.name, _aik_read.player_name))) {
        return -0x41;
    }
    if (!NT_SUCCESS(driver->ReadMemType(_traverse_pointer + self_basic_properties.name, _aik_read.player_level))) {
        return -0x42;
    }
    std::uint32_t _player_gravity{};
    if (!NT_SUCCESS(driver->ReadMemType(_traverse_pointer + self_basic_properties.gravity.offset, _player_gravity))) {
        return -0x43;
    }
    _aik_read.player_no_gravity = _player_gravity == self_basic_properties.gravity.no_gravity;
    if (!NT_SUCCESS(driver->ReadMemType(_traverse_pointer + self_basic_properties.attack_speed, _aik_read.player_attack_speed))) {
        return -0x44;
    }
    if (!NT_SUCCESS(driver->ReadMemType(_traverse_pointer + self_basic_properties.speed, _aik_read.player_speed))) {
        return -0x45;
    }

    /* read player position */
    if (POINTER(debug_wprintf, m_player_entity_pointer, self_pos.pointer, _traverse_pointer, 4, m_ptrs_cache, 5, false) == false) {
        return -0x50;
    }
    if (!NT_SUCCESS(driver->ReadMemType(_traverse_pointer + self_pos.x_coord, _aik_read.player_x))) {
        return -0x51;
    }
    if (!NT_SUCCESS(driver->ReadMemType(_traverse_pointer + self_pos.y_coord, _aik_read.player_y))) {
        return -0x52;
    }
    if (!NT_SUCCESS(driver->ReadMemType(_traverse_pointer + self_pos.z_coord, _aik_read.player_z))) {
        return -0x53;
    }

    /* Cry3DEngine */ 
    if (POINTER(debug_wprintf, _cryengine_module.addr, radar.pointer_0, _traverse_pointer, 4, m_ptrs_cache, 6, true) == false) {
        return -0x6A;
    }
    
    /* console */
    if (!NT_SUCCESS(driver->ReadMemType(m_ptrs_cache[7], _aik_read.disable_console))) {
        return -0x161;
    }
    return 0;
}

const int aik::read_client_values(const Driver::Module& _game_module, const Driver::Module& _cryengine_module, DISPATCH_SHARED& _dispatch_shared) {
    AIK_READ _aik_read{};
    if (auto status = read_client_values(_game_module, _cryengine_module, _aik_read); status != 0) {
        this->m_aion_player_found = false;
        this->write_shared_values(AIK_READ{}.contruct_dispatch(), 0);
        return status;
    }
    this->m_aion_player_found = true;
    this->write_shared_values(AIK_READ{}.contruct_dispatch(), 0);
    _dispatch_shared.m_aik_read = std::make_unique<AIK_READ>(_aik_read);
    return 0;
}

const int aik::find_client_patterns() {
    const auto pages = list_pages(0, MAXULONGLONG);

    /* con_disable_console pattern */
    // prepare mask: high to low 
    std::string reversed_mask(AION_VARS::console::mask);
    std::reverse(reversed_mask.begin(), reversed_mask.end());

    debug_wprintf(L"[-] Find console pattern...");
    auto disable_console_pattern = find_pattern(pages,
                                             ustring(std::cbegin(AION_VARS::console::pattern), std::cend(AION_VARS::console::pattern)),
                                             std::bitset<256>(reversed_mask));

    if (disable_console_pattern == 0) {
        debug_wprintf(L"\t Failed!");
        return -0x141;
    }
    auto disable_console_addr = disable_console_pattern + AION_VARS::console::disable_console_offset;
    m_ptrs_cache[7] = disable_console_addr;

    this->m_aion_console_found = true;
    this->write_shared_values(AIK_READ{}.contruct_dispatch(), 0);

    debug_wprintf(L"\t Success!");
    debug_wprintf(L" Console addr: 0x%llX", disable_console_addr);
    return 0;
}

const Driver::PAGE* aik::list_pages(const std::uintptr_t base, const std::uint64_t sz) {
    static Driver::PAGE* m_pages = new Driver::PAGE[pages_ar_sz]{};
    //0xFFFFFFFFFFFFFFFF
    //MAXULONGLONG
    debug_wprintf(L"[-] List process pages...");
    if (!NT_SUCCESS(driver->GetPages(m_pages, base, sz))) {
        debug_wprintf(L"\t Failed!");
        return m_pages;
    }
    debug_wprintf(L"\t Success!");
    DWORD count = 0;
    for (DWORD i = 0; i < pages_ar_sz; i++) {
        if (m_pages[i].empty()) {
            count = i;
            break;
        }
        //debug_wprintf(L"N[%u]: page addr: 0x%p, sz: %llu\n", ++count, m_pages[i].Address, m_pages[i].Size);
    }
    debug_wprintf(L" Pages size: %lu", count);
#ifdef AION_KERNEL_DEBUG_DUMP_PAGES_LIST
    debug_wprintf(L" Dump pages called.", count);
    driver->dump_memory(m_pages, pages_ar_sz);
#endif
    return m_pages;
}

const int aik::write_client_values(const AIK_WRITE& _aik_write) {
    if (!driver) {
        return -0x1A;
    }
    if (m_ptrs_cache[4] != 0) {
        //return -0x100;

        if (_aik_write.no_gravity) {
            if (!NT_SUCCESS(driver->WriteMemType(m_ptrs_cache[4] + self_basic_properties.gravity.offset, self_basic_properties.gravity.no_gravity))) {
                return -0x101;
            }
        } else {
            if (!NT_SUCCESS(driver->WriteMemType(m_ptrs_cache[4] + self_basic_properties.gravity.offset, self_basic_properties.gravity.gravity))) {
                return -0x101;
            }
        }


        if (_aik_write.speed != 0 &&
            !NT_SUCCESS(driver->WriteMemType(m_ptrs_cache[4] + self_basic_properties.speed, _aik_write.speed))) {
            return -0x102;
        }
        if (_aik_write.attack_speed != 0 &&
            !NT_SUCCESS(driver->WriteMemType(m_ptrs_cache[4] + self_basic_properties.attack_speed, _aik_write.attack_speed))) {
            return -0x103;
        }
    }
    if (m_ptrs_cache[5] != 0) {
        if (_aik_write.player_x != 0 &&
            !NT_SUCCESS(driver->WriteMemType(m_ptrs_cache[5] + self_pos.x_coord, _aik_write.player_x))) {
            return -0x110;
        }
        if (_aik_write.player_y != 0 &&
            !NT_SUCCESS(driver->WriteMemType(m_ptrs_cache[5] + self_pos.y_coord, _aik_write.player_y))) {
            return -0x111;
        }
        if (_aik_write.player_z != 0 &&
            !NT_SUCCESS(driver->WriteMemType(m_ptrs_cache[5] + self_pos.z_coord, _aik_write.player_z))) {
            return -0x112;
        }
    }
    if (m_ptrs_cache[6] != 0) {
        if (_aik_write.radar) {
            if (!NT_SUCCESS(driver->WriteMemType(m_ptrs_cache[6] + radar.radar_offset, radar.radar_init))) {
                return -0x113;
            }
        } else {
            if (!NT_SUCCESS(driver->WriteMemType(m_ptrs_cache[6] + radar.radar_offset, radar.radar_deinit))) {
                return -0x114;
            }
        }
    }
    if (m_ptrs_cache[7] != 0) {
        if (!_aik_write.disable_console) {
            if (!NT_SUCCESS(driver->WriteMemType(m_ptrs_cache[7], AION_VARS::console::enable))) {
                return -0x115;
            }
        } else {
            if (!NT_SUCCESS(driver->WriteMemType(m_ptrs_cache[7], AION_VARS::console::disable))) {
                return -0x115;
            }
        }
    }

    return 0;
    }

const int aik::write_client_values(const DISPATCH_SHARED& _dispatch_shared) {
    if (auto status = write_client_values(*_dispatch_shared.m_aik_write); status != 0) {
        return status;
    }
    return 0;
}

void aik::debug_print(const wchar_t* _str, unsigned short sz) {
    AIK_READ _aik_read_struct;
    StringCchCopyW(_aik_read_struct.dbg_wprint, std::wcslen(_str) + 1, _str);
    this->write_shared_values(_aik_read_struct.contruct_dispatch());
}

const int aik::read_shared_values(DISPATCH_SHARED& _pdispatch_shared_struct) {
    _pdispatch_shared_struct.m_aik_read = 
        std::make_unique<AIK_READ>(m_shared_memory->read_value_typed<AIK_READ>((ULONG) DISPATCH_SHARED::aik_read_offset()));
    _pdispatch_shared_struct.m_aik_write = 
        std::make_unique<AIK_WRITE>(m_shared_memory->read_value_typed<AIK_WRITE>((ULONG) DISPATCH_SHARED::aik_write_offset()));
    //_pdispatch_shared_struct.m_run = m_shared_memory->read_value_typed<BYTE>((ULONG) DISPATCH_SHARED::run_offset);    
    return 0;
}

const int aik::write_shared_values(const DISPATCH_SHARED& _pdispatch_shared_struct, int sz_diff) {
    if (_pdispatch_shared_struct.m_aik_read) {
        _pdispatch_shared_struct.m_aik_read->m_aion_client_running = this->m_aion_client_running;
        _pdispatch_shared_struct.m_aik_read->m_aion_console_found = this->m_aion_console_found;
        _pdispatch_shared_struct.m_aik_read->m_aion_player_found = this->m_aion_player_found;
        _pdispatch_shared_struct.m_aik_read->m_run = this->m_run;
                                                                                                                            /* last 4 bytes reserved for servicing */
        m_shared_memory->write_value_typed<AIK_READ>(*_pdispatch_shared_struct.m_aik_read, DISPATCH_SHARED::aik_read_offset(), sizeof(AIK_READ) + sz_diff);
    }
    if (_pdispatch_shared_struct.m_aik_write) {
        m_shared_memory->write_value_typed<AIK_WRITE>(*_pdispatch_shared_struct.m_aik_write, DISPATCH_SHARED::aik_write_offset(), sizeof(AIK_READ) + sz_diff);
    }
    /* don't update DISPATCH_SHARED::m_run from client side */
    return 0;
}


bool aik::init_shared_mutex(LPCTSTR name, AIK_INIT_APPROACH init_appr) {
    if (m_shared_mutex) {
        return false;
    }
    m_shared_mutex = std::make_unique<shared_mutex>(name);
    if (!m_shared_mutex) {
        return false;
    }
    switch (init_appr) {
        case AIK_INIT_APPROACH::CREATE: {
            m_shared_mutex->create_mutex(AIK_GLOBAL_SETTINGS::create_sm_lpMutexAttributes,
                                         AIK_GLOBAL_SETTINGS::create_sm_bInitialOwner);
            return m_shared_mutex.operator bool();
        }
        case AIK_INIT_APPROACH::OPEN: {
            m_shared_mutex->open_mutex(AIK_GLOBAL_SETTINGS::open_sm_dwDesiredAccess,
                                       AIK_GLOBAL_SETTINGS::open_sm_bInheritHandle);
            return m_shared_mutex.operator bool();
        }
    }
    return false;
}

bool aik::init_shared_memory(LPCTSTR name, AIK_INIT_APPROACH init_appr) {
    if (!m_shared_mutex || m_shared_memory) {
        return false;
    }
    m_shared_memory = std::make_unique<shared_memory>(name, m_shared_mutex->handle());
    if (!m_shared_memory) {
        return false;
    }
    switch (init_appr) {
        case AIK_INIT_APPROACH::CREATE: {
            if (!m_shared_memory->create_file_mapping(AIK_GLOBAL_SETTINGS::create_fm_hFile,
                                                      AIK_GLOBAL_SETTINGS::create_fm_lpFileMappingAttributes,
                                                      AIK_GLOBAL_SETTINGS::create_fm_flProtect,
                                                      0,
                                                      DISPATCH_SHARED::size())) {
                return false;
            }
        }
        case AIK_INIT_APPROACH::OPEN: {
            if (!m_shared_memory->open_file_mapping(AIK_GLOBAL_SETTINGS::open_fm_dwDesiredAccess,
                                                    AIK_GLOBAL_SETTINGS::open_fm_bInheritHandle)) {
                return false;
            }
        }
    }
    m_shared_memory->map_view(AIK_GLOBAL_SETTINGS::mapview_dwDesiredAccess,
                              AIK_GLOBAL_SETTINGS::mapview_dwFileOffsetHigh,
                              AIK_GLOBAL_SETTINGS::mapview_dwFileOffsetLow,
                              DISPATCH_SHARED::size());
    return true;
}

const int aik::init_driver() {
    if (!m_shared_memory) {
        return -0xA;
    }
    debug_wprintf(L"[-] Open driver connection...");
    if (!driver->Init(AIK_GLOBAL_SETTINGS::driver_mode)) {
        debug_wprintf(L"\t Failed!");
        return -0xB;
    }
    debug_wprintf(L"\t Success!");
    this->write_shared_values(AIK_READ{}.contruct_dispatch(), 0);

    return 0;
}

const int aik::attach_proc(const wchar_t* proc_name) {
    debug_wprintf(L"[-] Attach process %s...", proc_name);
    if (!driver->Attach(proc_name)) {
        debug_wprintf(L"\t Failed!");
        return -0x1A;
    }
    this->m_aion_client_running = true;
    this->write_shared_values(AIK_READ{}.contruct_dispatch(), 0);
    debug_wprintf(L"\t Success!");
    debug_wprintf(L" %s PID: %u", proc_name, driver->ProcessId);
    return 0;
}

const int aik::get_proc_module(const wchar_t* module_name, Driver::Module& _module) {
    debug_wprintf(L"[-] Get %s module...", module_name);
    _module = driver->GetModuleBase(module_name);
    if (_module.empty()) {
        debug_wprintf(L"\t Failed!");
        return -0x2A;
    }
    debug_wprintf(L"\t Success!");
    debug_wprintf(L" %s addr: 0x%llX, sz: %u", module_name, _module.addr, _module.size);
    return 0;
}
const std::uintptr_t aik::find_pattern(const std::uintptr_t addr, const std::size_t sz, const ustring& pattern, const std::bitset<256> mask) {
    return driver->FindPattern(addr, sz, pattern, mask);
}
const std::uintptr_t aik::find_pattern(const Driver::PAGE* mem_pages, const ustring& pattern, const std::bitset<256> mask) {
    for (DWORD i = 0; i < aik::pages_ar_sz; i++) {
        const auto& _page = mem_pages[i];
        if (_page.empty()) {
            break;
        }
        //debug_wprintf(L"[!] Seraching pattern in: 0x%llX, sz: %llu\n", _page.Address, _page.Size);
        if (auto pattern_addr = driver->FindPattern(_page, pattern, mask)) {
            return pattern_addr;
        }
    }
    return 0;
}

bool aik::is_process_running() {
    HANDLE phandle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, driver->ProcessId);
    if (phandle == NULL) {
        return phandle;
    }
    CloseHandle(phandle);
    return true;
}
void aik::reset_service_values() {
    m_aion_client_running = false;
    m_aion_console_found = false;
    m_aion_player_found = false;
    write_shared_values(AIK_READ{}.contruct_dispatch(), -1);
}
