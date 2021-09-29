#include <stdafx.h>
#include <aik.hpp>
#include <global_vars.hpp>

aik::aik() {}

void aik::debug_print(const wchar_t* _str, unsigned short sz) {
    AIK_READ _aik_read_struct;
    StringCchCopyW(_aik_read_struct.dbg_wprint, std::wcslen(_str) + 1, _str);
    this->write_shared_values(_aik_read_struct.contruct_dispatch());
}

int aik::write_shared_values(const DISPATCH_SHARED& _pdispatch_shared_struct) {
    if (_pdispatch_shared_struct.m_paik_read) {
    }
    if (_pdispatch_shared_struct.m_paik_write) {
    }
    return 0;
}


bool aik::init_shared_mutex(LPCTSTR name, AIK_INIT_APPROACH init_appr) {
    if (m_shared_mutex) {
        return false;
    }
    m_shared_mutex = std::make_unique<shared_mutex>(shared_mutex(std::move(name)));
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
    m_shared_memory = std::make_unique<shared_memory>(shared_memory(name, m_shared_mutex.get()->operator()()));
    if (!m_shared_memory) {
        return false;
    }
    switch (init_appr) {
        case AIK_INIT_APPROACH::CREATE: {
            m_shared_memory->create_file_mapping(AIK_GLOBAL_SETTINGS::create_fm_hFile,
                                                 AIK_GLOBAL_SETTINGS::create_fm_lpFileMappingAttributes,
                                                 AIK_GLOBAL_SETTINGS::create_fm_flProtect,
                                                 0,
                                                 DISPATCH_SHARED::size());
            return m_shared_memory.operator bool();
        }
        case AIK_INIT_APPROACH::OPEN: {
            m_shared_memory->open_file_mapping(AIK_GLOBAL_SETTINGS::open_fm_dwDesiredAccess,
                                               AIK_GLOBAL_SETTINGS::open_fm_bInheritHandle);
            return m_shared_memory.operator bool();
        }
    }
    return false;
}

bool aik::init_driver() {
    if (!m_shared_memory) {
        return false;
    }
    debug_wprintf(L"[!] Open driver connection: ");
    if (!driver->Init(AIK_GLOBAL_SETTINGS::driver_mode)) {
        debug_wprintf(L"Failed!\n");
        return false;
    }
    debug_wprintf(L"Success!\n");
    return true;
}
