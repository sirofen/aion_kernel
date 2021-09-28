#include <stdafx.h>

namespace {
constexpr LPCTSTR mutex_name = TEXT("aiKMutex");
constexpr LPCTSTR file_mapping_name = TEXT("Global\\aiKFileMapping");
}

int main() {
    aik aik;
    aik.init_shared_mutex(mutex_name);

	return 0;
}