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
    while (1) {
        AIK_READ aik_read;
        std::wcin >> aik_read.dbg_wprint;
        printf("shared values %S\n", aik_read.dbg_wprint);
        aik.write_shared_values(aik_read.contruct_dispatch());
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