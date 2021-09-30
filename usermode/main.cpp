#include <stdafx.h>

namespace {
constexpr LPCTSTR mutex_name = TEXT("aiKMutex");
constexpr LPCTSTR file_mapping_name = TEXT("Global\\aiKFileMapping");
}// namespace

int main() {
    aik aik;

    aik.init_shared_mutex(mutex_name, AIK_INIT_APPROACH::CREATE);
    //HANDLE hmutex = CreateMutex(NULL, FALSE, mutex_name);
    //system("pause");
    aik.init_shared_memory(file_mapping_name, AIK_INIT_APPROACH::CREATE);
    //system("pause");
    AIK_READ aik_read{.dbg_wprint = {L"DBG"}};
    aik.write_shared_values(aik_read.contruct_dispatch());
    DISPATCH_SHARED dis_sh;
    aik.read_shared_values(dis_sh);
    printf("shared values %S\n", dis_sh.m_aik_read->dbg_wprint);
    //system("pause");
    //aik.init_driver();

    return 0;
}