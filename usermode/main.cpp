#include <stdafx.h>

namespace {
constexpr LPCTSTR mutex_name = TEXT("aiKMutex");
constexpr LPCTSTR file_mapping_name = TEXT("Global\\aiKFileMapping");
}// namespace

int main() {
    aik aik;

    aik.init_shared_mutex(mutex_name, AIK_INIT_APPROACH::CREATE);
    aik.init_shared_memory(file_mapping_name, AIK_INIT_APPROACH::CREATE);
    aik.init_driver();

    return 0;
}