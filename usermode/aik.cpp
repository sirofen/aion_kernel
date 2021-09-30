#include <stdafx.h>
#include <aik.hpp>
#include <global_vars.hpp>
#include <memory>

aik::aik() {}

void aik::debug_print(const wchar_t* _str, unsigned short sz) {
    AIK_READ _aik_read_struct;
    StringCchCopyW(_aik_read_struct.dbg_wprint, std::wcslen(_str) + 1, _str);
    this->write_shared_values(_aik_read_struct.contruct_dispatch());
}

int aik::read_shared_values(DISPATCH_SHARED& _pdispatch_shared_struct) {
    _pdispatch_shared_struct.m_aik_read = 
        std::make_unique<AIK_READ>(m_shared_memory->read_value_typed<AIK_READ>((ULONG) DISPATCH_SHARED::aik_read_offset()));
    _pdispatch_shared_struct.m_aik_write = 
        std::make_unique<AIK_WRITE>(m_shared_memory->read_value_typed<AIK_WRITE>((ULONG) DISPATCH_SHARED::aik_write_offset()));
    return 0;
}

int aik::write_shared_values(const DISPATCH_SHARED& _pdispatch_shared_struct) {
    if (_pdispatch_shared_struct.m_aik_read) {
        m_shared_memory->write_value_typed<AIK_READ>(*_pdispatch_shared_struct.m_aik_read);
    }
    if (_pdispatch_shared_struct.m_aik_write) {
        m_shared_memory->write_value_typed<AIK_WRITE>(*_pdispatch_shared_struct.m_aik_write);
    }
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
