#include <stdafx.h>

#include <macro.h>
#include <aion_offsets.hpp>

namespace {
constexpr LPCTSTR mutex_name = TEXT("aiKMutex");
constexpr LPCTSTR file_mapping_name = TEXT("Global\\aiKFileMapping");
constexpr auto tick_delay = 100;
}// namespace

namespace {
constexpr BYTE pattern[] = {'p', '0x0', 'u', '0x0', 'n', '0x0', 'p', '0x0', 'c', '0x0', 'k', '0x0', 'h', '0x0', 'w', '0x0', 'd', '0x0', 'v', '0x0', 'm', '0x0', 'd', '0x0', 'e', '0x0', 'o'};

constexpr auto read_sz = 10;
BYTE new_bytes[read_sz];
}
#include <vector>
int main() {
    aik aik;

    //if (!aik.init_shared_mutex(mutex_name, AIK_INIT_APPROACH::OPEN)) {
    //    return -1;
    //}
    //if (!aik.init_shared_memory(file_mapping_name, AIK_INIT_APPROACH::OPEN)) {
    //    return -1;
    //}

    aik.debug_wprintf(L"[-] aiK client shared memory initialized");

    while (auto status = aik.init_driver() != 0) {
        aik.debug_wprintf(L"Waiting for driver, status: 0x%lX", status);
        Sleep(4000);
    }
    //AION_VARS::aion_bin
    while (auto status = aik.attach_proc(L"vlc.exe") != 0) {
        aik.debug_wprintf(L"Waiting for process, status: 0x%lX", status);
        Sleep(4000);
    }

    //Driver::Module game_module;
    //while (auto status = aik.get_proc_module(AION_VARS::GAMEDLL::module_name, game_module) != 0) {
    //    aik.debug_wprintf(L"Waiting for Game module to load, status: 0x%lX", status);
    //    Sleep(4000);
    //}

    //Driver::Module cryengine_module;
    //while (auto status = aik.get_proc_module(AION_VARS::CRY3DENGINE::module_name, cryengine_module) != 0) {
    //    aik.debug_wprintf(L"Waiting for Cry3DEngine module to load, status: 0x%lX", status);
    //    Sleep(4000);
    //}

    //Driver::Module aion_bin_module;
    //while (auto status = aik.get_proc_module(AION_VARS::GAMEDLL::module_name, aion_bin_module) != 0) {
    //    aik.debug_wprintf(L"Waiting for aion.bin module to load, status: 0x%lX", status);
    //    Sleep(4000);
    //    break;
    //}

    //auto _pattern_addr = aik.find_pattern(0xEB7DC000, 4000, AION_VARS::aion_con_disable_console_pattern);
    //wprintf_s(L"0x%llX", _pattern_addr);
    const auto pages = aik.list_pages(0, MAXULONGLONG);
    //74 74 74 FF 00 00 00 00 00 00 00 00 00 00 00 00 3A 3A 3A 80 4F 4F 4F AE 00 00 00 00 00 00 00 00 00 00 00 00 74 74 74 FF 74 74 74 FF 74 74 74 FF 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 74 74 74 FF 76 76 76 F1 11 11 11 26 00 00 00 00 3A 3A 3A 80 E4 E4 E4 F5 74 74 74 FE 00 00 00 00 00 00 00 00 74 74 74 FF EE EE EE FF 74 74 74 FF
    static constexpr BYTE pattern[] = {0x74, 0x74, 0x74, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3A, 0x3A, 0x3A, 0x80, 0x4F, 0x4F, 0x4F, 0xAE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x74, 0x74, 0x74, 0xFF, 0x74, 0x74, 0x74, 0xFF, 0x74, 0x74, 0x74, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x74, 0x74, 0x74, 0xFF, 0x76, 0x76, 0x76, 0xF1, 0x11, 0x11, 0x11, 0x26, 0x00, 0x00, 0x00, 0x00, 0x3A, 0x3A, 0x3A, 0x80, 0xE4, 0xE4, 0xE4, 0xF5, 0x74, 0x74, 0x74, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x74, 0x74, 0x74, 0xFF, 0xEE, 0xEE, 0xEE, 0xFF, 0x74, 0x74, 0x74, 0xFF};
    auto addr = aik.find_pattern(pages, ustring(std::begin(pattern), std::end(pattern)));
    //auto addr = aik.find_pattern(pages, std::string(std::begin(AION_VARS::aion_con_disable_console_pattern), std::end(AION_VARS::aion_con_disable_console_pattern)));
    printf("pattern addr: 0x%llX", addr);
    return 0;
    DISPATCH_SHARED _d_shd;
    do {
        //while (auto status = aik.read_client_values(game_module, cryengine_module, _d_shd) != 0) {
        //    aik.debug_wprintf(L"Waiting for client values, status: 0x%lX", status);
        //    Sleep(4000);
        //}
        aik.write_shared_values(_d_shd);
        Sleep(tick_delay);
        aik.read_shared_values(_d_shd);
        aik.write_client_values(_d_shd);
    } while (_d_shd.m_aik_read->m_run);
    return 0;
    constexpr size_t read_sz = 0x10;

    BYTE prev_bytes[read_sz];
    BYTE new_bytes[read_sz];

    std::uintptr_t ptr_cache[2] = {0, 0};
    std::uintptr_t _addr_t = 0;

    for (; !GetAsyncKeyState(VK_F12);)

        return 0;

    //system("pause");
    for (int i = 0; i < 40; i++) {
        Sleep(1000);

        AIK_READ aik_read{.player_speed = (float) i,
                          .player_attack_speed = (uint32_t) i,
                          .target_speed = 10.3232233,
                          .target_attack_speed = 23232323,
                          .target_x = 23.332,
                          .target_y = 3223.123,
                          .target_z = 999.123,
                          .dbg_wprint = L"printA"};
        //std::cout << "\nspeed :";
        //std::wcin >> aik_read.player_speed;
        //std::cout << "\ndbg str: ";
        //std::wcin.getline(aik_read.dbg_wprint, 100);
        //printf("shared speed %f\n", aik_read.player_attack_speed);
        aik.write_shared_values(aik_read.contruct_dispatch());
        DISPATCH_SHARED dis_sh;
        aik.read_shared_values(dis_sh);
        printf("values to write, speed: %f, attack speed: %u\n", dis_sh.m_aik_write->speed, dis_sh.m_aik_write->attack_speed);
    }

    for (; 0; Sleep(1000)) {
        DISPATCH_SHARED dis_sh;
        aik.read_shared_values(dis_sh);
        printf(".");
    }


    system("pause");
    //aik.init_driver();

    return 0;
}