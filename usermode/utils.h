#pragma once

#define print_bytes(ARRAY, DELIM) for (const auto& _c : ARRAY) { wprintf_s(L"%02X"##DELIM, _c); } wprintf_s(L"\n")
#define print_bytes_line(ARRAY, START_ADDRESS, N_ELEMENTS_IN_LINE, DELIM)									\
	const auto& arr_sz = sizeof(ARRAY);																		\
	for (std::size_t i = 0; i < arr_sz; i++) {																\
		auto last_iter = (i + 1) == arr_sz;																	\
		if (i % N_ELEMENTS_IN_LINE == 0 || last_iter) {														\
		std::size_t iter_sz_part;																			\
		if (last_iter) {																					\
			iter_sz_part = i - arr_sz % N_ELEMENTS_IN_LINE + 1;												\
			wprintf_s(L"%02X"##DELIM, ARRAY[i]);															\
			std::wcout << std::wstring((N_ELEMENTS_IN_LINE - (arr_sz % N_ELEMENTS_IN_LINE)) * 3, L' ');		\
		}																									\
		else { iter_sz_part = i - N_ELEMENTS_IN_LINE; }														\
			for (std::size_t j = 0; i != 0 && j < N_ELEMENTS_IN_LINE && j + iter_sz_part < arr_sz; j++) {	\
				const auto& _c = ARRAY[j + iter_sz_part];													\
				wprintf_s(j == 0 ? L"\t%C":L"%C", std::isgraph(_c) || _c == 0x20 ? _c : '.');				\
			}																								\
			if (!last_iter) {																				\
				wprintf_s(L"\n0x%llX:\t", START_ADDRESS + i);												\
			}																								\
		}																									\
		if (!last_iter) {																					\
		wprintf_s(L"%02X"##DELIM, ARRAY[i]);																\
		}																									\
	}																										\
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

typedef enum _KEY_VALUE_INFORMATION_CLASS {
	KeyValueBasicInformation,
	KeyValueFullInformation,
	KeyValuePartialInformation,
	KeyValueFullInformationAlign64,
	KeyValuePartialInformationAlign64,
	KeyValueLayerInformation,
	MaxKeyValueInfoClass  // MaxKeyValueInfoClass should always be the last enum
} KEY_VALUE_INFORMATION_CLASS;

typedef struct _KEY_VALUE_FULL_INFORMATION {
	ULONG   TitleIndex;
	ULONG   Type;
	ULONG   DataOffset;
	ULONG   DataLength;
	ULONG   NameLength;
	WCHAR   Name[1];            // Variable size
//          Data[1];            // Variable size data not declared
} KEY_VALUE_FULL_INFORMATION, * PKEY_VALUE_FULL_INFORMATION;


#ifdef __cplusplus
extern "C++"
{
	char _RTL_CONSTANT_STRING_type_check(const char* s);
	char _RTL_CONSTANT_STRING_type_check(const WCHAR* s);
	// __typeof would be desirable here instead of sizeof.
	template <size_t N> class _RTL_CONSTANT_STRING_remove_const_template_class;
template <> class _RTL_CONSTANT_STRING_remove_const_template_class<sizeof(char)> { public: typedef  char T; };
template <> class _RTL_CONSTANT_STRING_remove_const_template_class<sizeof(WCHAR)> { public: typedef WCHAR T; };
#define _RTL_CONSTANT_STRING_remove_const_macro(s) \
    (const_cast<_RTL_CONSTANT_STRING_remove_const_template_class<sizeof((s)[0])>::T*>(s))
}
#else
char _RTL_CONSTANT_STRING_type_check(const void* s);
#define _RTL_CONSTANT_STRING_remove_const_macro(s) (s)
#endif
#define RTL_CONSTANT_STRING(s) \
{ \
    sizeof( s ) - sizeof( (s)[0] ), \
    sizeof( s ) / sizeof(_RTL_CONSTANT_STRING_type_check(s)), \
    _RTL_CONSTANT_STRING_remove_const_macro(s) \
}

extern "C" {
	NTSYSAPI
		NTSTATUS
		NTAPI
		ZwQueryValueKey(
			_In_ HANDLE KeyHandle,
			_In_ PUNICODE_STRING ValueName,
			_In_ KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
			_Out_writes_bytes_to_opt_(Length, *ResultLength) PVOID KeyValueInformation,
			_In_ ULONG Length,
			_Out_ PULONG ResultLength
		);

	NTSYSAPI
		NTSTATUS
		NTAPI
		ZwClose(
			_In_ HANDLE Handle
		);

	NTSYSAPI
		NTSTATUS
		NTAPI
		ZwOpenKey(
			_Out_ PHANDLE KeyHandle,
			_In_ ACCESS_MASK DesiredAccess,
			_In_ POBJECT_ATTRIBUTES ObjectAttributes
		);

	NTSYSAPI
		NTSTATUS
		NTAPI
		ZwQueryValueKey(
			_In_ HANDLE KeyHandle,
			_In_ PUNICODE_STRING ValueName,
			_In_ KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
			_Out_writes_bytes_to_opt_(Length, *ResultLength) PVOID KeyValueInformation,
			_In_ ULONG Length,
			_Out_ PULONG ResultLength
		);

	NTSYSAPI
		NTSTATUS
		NTAPI
		ZwSetValueKey(
			_In_ HANDLE KeyHandle,
			_In_ PUNICODE_STRING ValueName,
			_In_opt_ ULONG TitleIndex,
			_In_ ULONG Type,
			_In_reads_bytes_opt_(DataSize) PVOID Data,
			_In_ ULONG DataSize
		);

	NTSYSAPI NTSTATUS ZwCreateKey(
		PHANDLE            KeyHandle,
		ACCESS_MASK        DesiredAccess,
		POBJECT_ATTRIBUTES ObjectAttributes,
		ULONG              TitleIndex,
		PUNICODE_STRING    Class,
		ULONG              CreateOptions,
		PULONG             Disposition
	);
}

namespace RegistryUtils
{
	__forceinline ULONG GetKeyInfoSize(HANDLE hKey, PUNICODE_STRING Key)
	{
		NTSTATUS Status;
		ULONG KeySize;

		Status = ZwQueryValueKey(hKey, Key, KeyValueFullInformation, 0, 0, &KeySize);

		if (Status == STATUS_BUFFER_TOO_SMALL || Status == STATUS_BUFFER_OVERFLOW)
			return KeySize;

		return 0;
	}

	template <typename type>
	__forceinline type ReadRegistry(UNICODE_STRING RegPath, UNICODE_STRING Key)
	{
		HANDLE hKey;
		OBJECT_ATTRIBUTES ObjAttr;
		NTSTATUS Status = STATUS_UNSUCCESSFUL;

		InitializeObjectAttributes(&ObjAttr, &RegPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

		Status = ZwOpenKey(&hKey, KEY_ALL_ACCESS, &ObjAttr);

		if (NT_SUCCESS(Status))
		{
			ULONG KeyInfoSize = GetKeyInfoSize(hKey, &Key);
			ULONG KeyInfoSizeNeeded;

			if (KeyInfoSize == NULL)
			{
				ZwClose(hKey);
				return 0;
			}

			PKEY_VALUE_FULL_INFORMATION pKeyInfo = (PKEY_VALUE_FULL_INFORMATION)malloc(KeyInfoSize);
			RtlZeroMemory(pKeyInfo, KeyInfoSize);

			Status = ZwQueryValueKey(hKey, &Key, KeyValueFullInformation, pKeyInfo, KeyInfoSize, &KeyInfoSizeNeeded);

			if (!NT_SUCCESS(Status) || (KeyInfoSize != KeyInfoSizeNeeded))
			{
				ZwClose(hKey);
				free(pKeyInfo);
				return 0;
			}

			ZwClose(hKey);
			free(pKeyInfo);

			return *(type*)((LONG64)pKeyInfo + pKeyInfo->DataOffset);
		}

		return 0;
	}

	__forceinline bool WriteRegistry(UNICODE_STRING RegPath, UNICODE_STRING Key, PVOID Address, ULONG Type, ULONG Size)
	{
		bool Success = false;
		HANDLE hKey;
		OBJECT_ATTRIBUTES ObjAttr;
		NTSTATUS Status = STATUS_UNSUCCESSFUL;

		InitializeObjectAttributes(&ObjAttr, &RegPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

		Status = ZwOpenKey(&hKey, KEY_ALL_ACCESS, &ObjAttr);

		if (NT_SUCCESS(Status))
		{
			Status = ZwSetValueKey(hKey, &Key, NULL, Type, Address, Size);

			if (NT_SUCCESS(Status))
				Success = true;

			ZwClose(hKey);
		}
		else {
			Status = ZwCreateKey(&hKey, KEY_ALL_ACCESS, &ObjAttr, 0, &RegPath, 0, 0);

			if (NT_SUCCESS(Status))
			{
				Status = ZwSetValueKey(hKey, &Key, NULL, Type, Address, Size);

				if (NT_SUCCESS(Status))
					Success = true;
			}
			ZwClose(hKey);
		}

		return Success;
	}
}
