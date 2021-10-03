#include <stdafx.h>

namespace {
constexpr LPCTSTR mutex_name = TEXT("aiKMutex");
constexpr LPCTSTR file_mapping_name = TEXT("Global\\aiKFileMapping");
}// namespace

int main() {
    aik aik;

    aik.init_shared_mutex(mutex_name, AIK_INIT_APPROACH::OPEN);
    //HANDLE hmutex = CreateMutex(NULL, FALSE, mutex_name);
    //system("pause");
    aik.init_shared_memory(file_mapping_name, AIK_INIT_APPROACH::OPEN);
    //system("pause");
    for (int i = 0; i < 40; i++) {
        Sleep(1000);
        //wchar_t str[100];
        //std::wcin.get(str, 100);
        //std::getchar();
        //AIK_READ aik_read;
        //memcpy(aik_read.dbg_wprint, str, 100);
        //AIK_READ _n;
        //wchar_t str[100] = L"\n";
        //memcpy(_n.dbg_wprint, str, 100);
        //aik.write_shared_values(_n.contruct_dispatch());

        AIK_READ aik_read{.player_speed = (float)i,
                          .player_attack_speed = (uint32_t)i,
                          .target_speed = 10.3232233,
                          .target_attack_speed = 23232323,
                          .target_x = 23.332,
                          .target_y = 3223.123,
                          .target_z = 999.123};
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

    for (;0; Sleep(1000)) {
        DISPATCH_SHARED dis_sh;
        aik.read_shared_values(dis_sh);
        printf(".");
    }

   
    system("pause");
    //aik.init_driver();

    return 0;
}