#include "stdafx.h"
#include "driver.h"

const std::uintptr_t Driver::FindPattern(const std::uintptr_t& base, const std::size_t& length, const BYTE*& pattern, const BYTE& mask = 0) {
	//wprintf_s(L"Searching - base: 0x%llx, length: %d ", base, length);
	BYTE* buffer = new BYTE[length];
	if (this->ReadMem(base, buffer, length) != STATUS_SUCCESS) {
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
	return FindPattern(module.addr, module.size, pattern, mask);
	//std::uintptr_t match = 0;
	//PIMAGE_NT_HEADERS headers = (PIMAGE_NT_HEADERS)(module.addr + ((PIMAGE_DOS_HEADER)module.addr)->e_lfanew);
	//PIMAGE_SECTION_HEADER sections = IMAGE_FIRST_SECTION(headers);
	//for (int i = 0; i < headers->FileHeader.NumberOfSections; i++) {
	//	PIMAGE_SECTION_HEADER section = &sections[i];
	//	if (auto match = FindPattern(module.addr + section->VirtualAddress, section->Misc.VirtualSize, pattern, mask)) {
	//		return match;
	//	}
	//}
}