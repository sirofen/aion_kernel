#include "stdafx.h"
#include "driver.h"

#include <fstream>

namespace {
    constexpr auto dump_dir = L"dump";
}

#define BUFSIZE MAX_PATH
void write_file(const BYTE* const& buffer, const std::size_t& length, const wchar_t*&& prefix = L"") {
    TCHAR dir_buffer[BUFSIZE];
    GetCurrentDirectory(BUFSIZE, dir_buffer);
    PathAppend(dir_buffer, dump_dir);
    CreateDirectory(dir_buffer, NULL);
    SYSTEMTIME time;
    GetSystemTime(&time);
    wchar_t _filebane_char_buf[BUFSIZE];
    swprintf_s(_filebane_char_buf, L"%s\\%s__%u-%u-%u_%u-%u-%u.bin", dir_buffer, prefix, time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
    wprintf_s(L"%s", _filebane_char_buf);
    std::ofstream buf_file(_filebane_char_buf, std::ios::binary | std::ios::out);
    buf_file.write((char*) buffer, length);
    buf_file.close();
}

void Driver::dump_memory(std::uintptr_t base, std::size_t length, const wchar_t*&& prefix) {
    BYTE* buffer = new BYTE[length]();
    if (this->ReadMem(base, buffer, length) != STATUS_SUCCESS) {
        delete[] buffer;
        return;
    }
    write_file(buffer, length, std::move(prefix));
}

const std::uintptr_t Driver::FindPattern(const std::uintptr_t& base, const std::size_t& length, const std::string& pattern, const BYTE& mask) {
	//wprintf_s(L"Searching - base: 0x%llx, length: %d ", base, length);
    BYTE* buffer = new BYTE[length]();
	if (this->ReadMem(base, buffer, length) != STATUS_SUCCESS) {
		delete[] buffer;
        wprintf_s(L"Find Pattern error\n");
		return 0;
	}
#ifdef AION_KERNEL_DEBUG_SAVE_PATTERN_BUFFER
    write_file(buffer, length, L"module");
#endif
	const auto& pattern_i_max = pattern.size() - 1;
	std::size_t pattern_i = 0;
	for (std::size_t i = 0; i < length; i++) {
		if (pattern_i == pattern_i_max) {
			delete[] buffer;
			return base + i;
		}
		if (buffer[i] == pattern[pattern_i]) {
			pattern_i++;
			continue;
		}
		pattern_i = 0;
	}
	delete[] buffer;
	return 0;
}
const std::uintptr_t Driver::FindPattern(const Module& module, const std::string& pattern, const std::size_t& sz_phys_mode, const BYTE& mask) {
	if (bPhysicalMode) {
        if (sz_phys_mode != 0) {
            return FindPattern(module.addr, sz_phys_mode, pattern, mask);
		}
        ULONGLONG phys_mem_sz;
		if (!GetPhysicallyInstalledSystemMemory(&phys_mem_sz)) {
            return 0;
		}
        wprintf(L" Phys mem sz: %llu ", phys_mem_sz *= 1024);
        return FindPattern(module.addr, phys_mem_sz -= module.addr, pattern, mask);
	}
	if (!module.pages.empty()) {
        for (const auto& [base, sz] : module.pages) {
            if (auto addr = FindPattern(base, sz, pattern, mask); addr != 0) {
                return addr;
            }
        }
	}

    return FindPattern(module.addr, module.size, pattern, mask);
}
const std::uintptr_t Driver::FindPattern(const PAGE& _p, const std::string& pattern, const BYTE& mask) {
    return FindPattern((uintptr_t)_p.Address, _p.Size, pattern, mask);
}
//namespace {
//constexpr auto page_arr_sz = 0x20;
//}

//const std::map<std::uintptr_t, unsigned long> Driver::GetModulePages(const Module& module) {
//    PAGE res_pages[page_arr_sz];
//    REQUEST_PAGES req;
//    req.ProcessId = this->ProcessId;
//    req.ModuleBase = (PVOID)module.addr;
//    req.ModuleSize = module.size;
//    req.Pages = &res_pages;
//    std::map<std::uintptr_t, unsigned long> map_pages;
//	if (this->SendRequest(REQUEST_TYPE::PAGES, &req) != STATUS_SUCCESS) {
//        return map_pages;
//	}
//	for (int i = 0; i < page_arr_sz; i++) {
//        const auto& cur_page = res_pages[i];
//        if (cur_page.empty()) {
//            continue;
//		}
//        map_pages.emplace((std::uintptr_t)cur_page.Address, cur_page.Size);
//	}
//    return map_pages;
//}
