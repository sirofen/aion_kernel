#include <stdafx.h>

#include <macro.h>
#include <aion_offsets.hpp>

#include <run_pe.hpp>
#include <aik_map_resource.hpp>

namespace {
constexpr char kLoadDriver[] = "-LOADDRIVER";

constexpr LPCTSTR mutex_name = TEXT("aiKMutex");
constexpr LPCTSTR file_mapping_name = TEXT("Global\\aiKFileMapping");
constexpr auto tick_delay = 100;
}// namespace

int main(int argc, char* argv[]) {

    if (argc > 1) {
        if (std::strcmp(argv[1], kLoadDriver) == 0) {
            run_pe(aik_map::pe);
            return 0;
        }
    }

    aik aik;

    if (!aik.init_shared_mutex(mutex_name, AIK_INIT_APPROACH::OPEN)) {
        aik.debug_wprintf(L"Init shared mutex error");
        return -1;
    }
    if (!aik.init_shared_memory(file_mapping_name, AIK_INIT_APPROACH::OPEN)) {
        aik.debug_wprintf(L"Init shared memory error");
        return -1;
    }

    aik.debug_wprintf(L"[-] aiK client shared memory initialized");


    while (auto status = aik.init_driver() != 0) {
        aik.debug_wprintf(L"Waiting for driver, status: 0x%lX", status);
        Sleep(4000);
    }

    reattach_process_label:

    while (auto status = aik.attach_proc(AION_VARS::aion_bin) != 0) {
        aik.debug_wprintf(L"Waiting for process, status: 0x%lX", status);
        Sleep(4000);
    }

    Driver::Module game_module;
    while (auto status = aik.get_proc_module(AION_VARS::GAMEDLL::module_name, game_module) != 0) {
        aik.debug_wprintf(L"Waiting for Game module to load, status: 0x%lX", status);
        Sleep(4000);
    }

    Driver::Module cryengine_module;
    while (auto status = aik.get_proc_module(AION_VARS::CRY3DENGINE::module_name, cryengine_module) != 0) {
        aik.debug_wprintf(L"Waiting for Cry3DEngine module to load, status: 0x%lX", status);
        Sleep(4000);
    }

    while (auto status = aik.find_client_patterns() != 0) {
        aik.debug_wprintf(L"Search pattern failed, status: 0x%lX, retrying", status);
    }

    aik.debug_wprintf(L"=======================================");

    DISPATCH_SHARED _d_shd;
    do {
        if (auto status = aik.read_client_values(game_module, cryengine_module, _d_shd) != 0) {
            if (!aik.is_process_running()) {
                aik.reset_service_values();
                goto reattach_process_label;
            }
            aik.debug_wprintf(L"Waiting for client values, status: 0x%lX", status);
            Sleep(4000);
        }
        aik.write_shared_values(_d_shd);
        Sleep(tick_delay);
        aik.read_shared_values(_d_shd);
        aik.write_client_values(_d_shd);
    } while (_d_shd.m_aik_read->m_run);

    return 0;
}
