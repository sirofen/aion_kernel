
#define print_bytes(ARRAY, DELIM) for (const auto& _c : ARRAY) { wprintf_s(L"%02X"##DELIM, _c); } wprintf_s(L"\n")
#define print_bytes_line(ARRAY, START_ADDRESS, N_ELEMENTS_IN_LINE, DELIM)													\
	for (std::size_t i = 0, arr_sz = sizeof(ARRAY); i < arr_sz; i++) {														\
		auto last_iter = (i + 1) == arr_sz;																					\
		if (i % N_ELEMENTS_IN_LINE == 0 || last_iter) {																		\
		std::size_t iter_sz_part;																							\
		if (last_iter) {																									\
			iter_sz_part = i - arr_sz % N_ELEMENTS_IN_LINE + 1;																\
			wprintf_s(L"%02X"##DELIM, ARRAY[i]);																			\
			std::wcout << std::wstring((N_ELEMENTS_IN_LINE - (arr_sz % N_ELEMENTS_IN_LINE)) * 3, L' ');	\
		}																													\
		else { iter_sz_part = i - N_ELEMENTS_IN_LINE; }																		\
			for (std::size_t j = 0; i != 0 && j < N_ELEMENTS_IN_LINE && j + iter_sz_part < arr_sz; j++) {					\
				const auto& _c = ARRAY[j + iter_sz_part];																	\
				wprintf_s(j == 0 ? L"\t%C":L"%C", std::isgraph(_c) || _c == 0x20 ? _c : '.');								\
			}																												\
			if (!last_iter) {																								\
				wprintf_s(L"\n0x%llX:\t", START_ADDRESS + i);																\
			}																												\
		}																													\
		if (!last_iter) {																									\
		wprintf_s(L"%02X"##DELIM, ARRAY[i]);																				\
		}																													\
	}																														\
	wprintf_s(L"\n")
//void print_bytes_line(const BYTE* cstr, const std::uintptr_t & start_address, const int n_lm_in_line, const wchar_t& delim) {
//		const auto& arr_sz = sizeof(cstr);
//	for (std::size_t i = 0; i < arr_sz; i++) {																
//		auto last_iter = (i + 1) == arr_sz;																	
//		if (i % n_lm_in_line == 0 || last_iter) {
//		std::size_t iter_sz_part;																			
//		if (last_iter) {																					
//			iter_sz_part = i - arr_sz % n_lm_in_line + 1;
//			wprintf_s(L"%02X%c", cstr[i], delim);
//			std::wcout << std::wstring((n_lm_in_line - (arr_sz % n_lm_in_line)) * 3, L' ');
//		}																									
//		else { iter_sz_part = i - n_lm_in_line; }														
//			for (std::size_t j = 0; i != 0 && j < n_lm_in_line && j + iter_sz_part < arr_sz; j++) {
//				const auto& _c = cstr[j + iter_sz_part];													
//				wprintf_s(j == 0 ? L"\t%C":L"%C", std::isgraph(_c) || _c == 0x20 ? _c : '.');				
//			}																								
//			if (!last_iter) {																				
//				wprintf_s(L"\n0x%llX:\t", start_address + i);
//			}																								
//		}																									
//		if (!last_iter) {																					
//		wprintf_s(L"%02X%c", delim , cstr[i]);																
//		}																									
//	}																										
//	wprintf_s(L"\n");
//}

#define POINTER_8(_HANDLER, _ADDRESS, _OFFSET, _RET_VAL, _SIZE, _CACHE, _ITERATION, _PRINT_BASE_ADDR)		\
	auto pointer = [&]() {																		\
		if (driver->ReadMemType(_ADDRESS + _OFFSET, _RET_VAL, _SIZE) != STATUS_SUCCESS			\
			|| _RET_VAL == 0x0) {																\
			return false;																		\
		}																						\
		if (_RET_VAL != _CACHE[_ITERATION]) {													\
			if (_PRINT_BASE_ADDR) { _HANDLER(L"\n\t{%u}:[0x%llX]", _ITERATION, _ADDRESS); }	\
			if (_OFFSET != 0x0) {																\
				_HANDLER(L" %S0x%llX", _OFFSET > 0 ? "+ " : "- ", _OFFSET);					\
			}																					\
			_HANDLER(L" -> {%u}:[0x%llX]", _ITERATION + 1, _RET_VAL);							\
			_CACHE[_ITERATION] = _RET_VAL;														\
		}																						\
		return true;																			\
	};																							\
	pointer()

#define POINTER_7(_HANDLER, _ADDRESS, _RET_VAL, _SIZE, _CACHE, _ITERATION, _PRINT_BASE_ADDR) POINTER_8(_HANDLER, _ADDRESS, 0x0, _RET_VAL, _SIZE, _CACHE, _ITERATION, _PRINT_BASE_ADDR)
#define POINTER_6(_HANDLER, _ADDRESS, _RET_VAL, _SIZE, _CACHE, _ITERATION) POINTER_7(_HANDLER, _ADDRESS, _RET_VAL, _SIZE, _CACHE, _ITERATION, false)
#define GET_MACRO(_1,_2,_3,_4,_5,_6,_7,_8, NAME, ...) NAME
#define MSVC_BUG(x) x /* it took me 3 hours */
#define POINTER(...) MSVC_BUG(GET_MACRO(##__VA_ARGS__, POINTER_8 ,POINTER_7, POINTER_6,,,,,,)(__VA_ARGS__))