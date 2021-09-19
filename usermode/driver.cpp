#include "stdafx.h"
#include "driver.h"

const std::uintptr_t Driver::FindPattern(const std::uintptr_t& base, const std::size_t& length, const BYTE*&& pattern, const BYTE& mask) {
	//wprintf_s(L"Searching - base: 0x%llx, length: %d ", base, length);
    BYTE* buffer = new BYTE[length]();
	if (this->ReadMem(base, buffer, length - 1) != STATUS_SUCCESS) {
		delete[] buffer;
		return 0;
	}
	const auto& pattern_i_max = sizeof(pattern) - 1;
	std::size_t pattern_i = 0;
	POINT caret_pos;
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
const std::uintptr_t Driver::FindPatternModule(const Module& module, const BYTE*&& pattern, const BYTE& mask) {
	for (const auto& [base, sz] : module.pages) {
        if (const auto& addr = FindPattern(base, sz, std::move(pattern), mask); addr != 0) {
            return addr;
		}
	}
    return 0;
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
