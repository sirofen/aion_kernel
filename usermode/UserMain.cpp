#include "stdafx.h"

int main() {
	printf("[!] Open driver connection: ");
	if (driver->Init(FALSE)) {
		printf("Success!\n");
		driver->Attach(L"vlc.exe");
		while (!GetAsyncKeyState(VK_F12))
		{
			clock_t begin = clock();
			auto module = driver->GetModuleBase(L"vlc.exe");
            auto libvlc_module = driver->GetModuleBase(L"libvlc.dll");
            auto msg_addr = module.addr + 0x4E;
            char msg[0x78 - 0x4E];
			if (driver->ReadMemType(msg_addr, msg) != STATUS_SUCCESS) {
                continue;
			}
			clock_t end = clock();
			double time_spent = (double)(end - begin);
			printf("Request took: %f | %fs\n", time_spent, time_spent / CLOCKS_PER_SEC);
			printf("base.addr: 0x%llX\n", module.addr);
			printf("base.size: 0x%llX\n", module.size);
            printf("libvlc module base: 0x%llX\n", libvlc_module.addr);
            printf("libvlc module size: 0x%llX\n", libvlc_module.size);
            printf("DOS stub message: %s\n", msg);
			Sleep(2000);
		}
		return 0;
	}
	printf("Failed!\n");
	system("pause");
	return 1;
}