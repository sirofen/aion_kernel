#pragma once
#define RVA(addr, size) (uint64_t)((PBYTE)((UINT_PTR)(addr) + *(PINT)((UINT_PTR)(addr) + ((size) - sizeof(INT))) + (size)))

//https://ntdiff.github.io/
#define WINDOWS_1803 17134
#define WINDOWS_1809 17763
#define WINDOWS_1903 18362
#define WINDOWS_1909 18363
#define WINDOWS_2004 19041
#define WINDOWS_20H2 19569
#define WINDOWS_21H1 20180

typedef struct _PEB_LDR_DATA {
	ULONG Length;
	UCHAR Initialized;
	PVOID SsHandle;
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
} PEB_LDR_DATA, * PPEB_LDR_DATA;

typedef struct _PEB {
	UCHAR InheritedAddressSpace;
	UCHAR ReadImageFileExecOptions;
	UCHAR BeingDebugged;
	UCHAR BitField;
	PVOID Mutant;
	PVOID ImageBaseAddress;
	PPEB_LDR_DATA Ldr;
	PVOID ProcessParameters;
	PVOID SubSystemData;
	PVOID ProcessHeap;
	PVOID FastPebLock;
	PVOID AtlThunkSListPtr;
	PVOID IFEOKey;
	PVOID CrossProcessFlags;
	PVOID KernelCallbackTable;
	ULONG SystemReserved;
	ULONG AtlThunkSListPtr32;
	PVOID ApiSetMap;
} PEB, * PPEB;

typedef struct _LDR_DATA_TABLE_ENTRY {
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	LIST_ENTRY HashLinks;
	ULONG TimeDateStamp;
} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

typedef struct _LDR_DATA_TABLE_ENTRY32 {
    LIST_ENTRY32 InLoadOrderLinks;
    LIST_ENTRY32 InMemoryOrderLinks;
    LIST_ENTRY32 InInitializationOrderLinks;
    ULONG DllBase;
    ULONG EntryPoint;
    ULONG SizeOfImage;
    UNICODE_STRING32 FullDllName;
    UNICODE_STRING32 BaseDllName;
    ULONG Flags;
    USHORT LoadCount;
    USHORT TlsIndex;
    LIST_ENTRY32 HashLinks;
    ULONG TimeDateStamp;
} LDR_DATA_TABLE_ENTRY32, *PLDR_DATA_TABLE_ENTRY32;

typedef struct _PEB_LDR_DATA32 {
    ULONG Length;
    UCHAR Initialized;
    ULONG SsHandle;
    LIST_ENTRY32 InLoadOrderModuleList;
    LIST_ENTRY32 InMemoryOrderModuleList;
    LIST_ENTRY32 InInitializationOrderModuleList;
} PEB_LDR_DATA32, *PPEB_LDR_DATA32;

typedef struct _PEB32 {
    UCHAR InheritedAddressSpace;
    UCHAR ReadImageFileExecOptions;
    UCHAR BeingDebugged;
    UCHAR BitField;
    ULONG Mutant;
    ULONG ImageBaseAddress;
    ULONG Ldr;
    ULONG ProcessParameters;
    ULONG SubSystemData;
    ULONG ProcessHeap;
    ULONG FastPebLock;
    ULONG AtlThunkSListPtr;
    ULONG IFEOKey;
    ULONG CrossProcessFlags;
    ULONG UserSharedInfoPtr;
    ULONG SystemReserved;
    ULONG AtlThunkSListPtr32;
    ULONG ApiSetMap;
} PEB32, *PPEB32;

typedef struct _MODULE_ENTRY {
    const PVOID& Base;
    const DWORD& Size;
    _MODULE_ENTRY(const PLDR_DATA_TABLE_ENTRY& entry)
        : Base(entry->DllBase)
        , Size(entry->SizeOfImage) {}
    _MODULE_ENTRY(const PLDR_DATA_TABLE_ENTRY32& entry)
        : Base((PVOID)entry->DllBase)
        , Size(entry->SizeOfImage) {}
    } MODULE_ENTRY, *PMODULE_ENTRY;

extern "C" POBJECT_TYPE * IoDriverObjectType;
extern "C" {
	NTSTATUS NTAPI MmCopyVirtualMemory(PEPROCESS SourceProcess, PVOID SourceAddress, PEPROCESS TargetProcess, PVOID TargetAddress, SIZE_T BufferSize, KPROCESSOR_MODE PreviousMode, PSIZE_T ReturnSize);
	NTSYSAPI NTSTATUS NTAPI ObReferenceObjectByName(PUNICODE_STRING ObjectPath, ULONG Attributes, PACCESS_STATE PassedAccessState, ACCESS_MASK DesiredAccess, POBJECT_TYPE ObjectType, KPROCESSOR_MODE AccessMode, PVOID ParseContext, PVOID* ObjectPtr);
	NTKERNELAPI PVOID PsGetProcessSectionBaseAddress(PEPROCESS Process);
	NTKERNELAPI PPEB  NTAPI PsGetProcessPeb(IN PEPROCESS Process);
    NTKERNELAPI PVOID NTAPI PsGetProcessWow64Process(_In_ PEPROCESS Process);
	NTSTATUS NTAPI ZwProtectVirtualMemory(HANDLE ProcessHandle, PVOID* BaseAddress, SIZE_T* NumberOfBytesToProtect, ULONG NewAccessProtection, PULONG OldAccessProtection);
}

namespace Utils {
	namespace Registry //https://www.unknowncheats.me/forum/3108590-post2.html
	{
		ULONG GetKeyInfoSize(HANDLE hKey, PUNICODE_STRING Key);
		//template <typename type>
		//type ReadRegistry(UNICODE_STRING RegPath, UNICODE_STRING Key);
		bool WriteRegistry(UNICODE_STRING RegPath, UNICODE_STRING Key, PVOID Address, ULONG Type, ULONG Size);
		template<typename type>
		static type ReadRegistry(UNICODE_STRING RegPath, UNICODE_STRING Key)
		{
            //DbgPrintEx(0, 0, __FUNCTION__);
            //DbgPrintEx(0, 0, __FUNCTION__ "key: %S", Key.Buffer);
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

				//DbgPrintEx(0, 0, "KeyInfoSize: %lu\n", KeyInfoSize);

				PKEY_VALUE_FULL_INFORMATION pKeyInfo = (PKEY_VALUE_FULL_INFORMATION)ExAllocatePool(NonPagedPool, KeyInfoSize);
				RtlZeroMemory(pKeyInfo, KeyInfoSize);

				Status = ZwQueryValueKey(hKey, &Key, KeyValueFullInformation, pKeyInfo, KeyInfoSize, &KeyInfoSizeNeeded);

				if (!NT_SUCCESS(Status) || (KeyInfoSize != KeyInfoSizeNeeded))
				{
					ZwClose(hKey);
					ExFreePoolWithTag(pKeyInfo, 0);
					return 0;
				}
                //DbgPrintEx(0, 0, "ZwQueryValueKey name: %s\n", pKeyInfo->Name);
				ZwClose(hKey);
                type _v = *(type*) ((LONG64) pKeyInfo + pKeyInfo->DataOffset);
                //DbgPrintEx(0, 0, "read reg: %llu\n", _v);
				ExFreePoolWithTag(pKeyInfo, 0);
                return _v;
			}

			return 0;
		}
	}
	//Defaults:
	//const bool bCheckMask(PCHAR base, PCHAR pattern, PCHAR mask);
	//const UINT64 FindPattern(const UINT64 dwAddress, const UINT dwLen, const PCHAR bMask, const PCHAR szMask);
	PMODULE_ENTRY GetModuleByName(PEPROCESS process, PWCHAR moduleName);
	//Physical Memory:
	namespace PhysicalMemory { //https://www.unknowncheats.me/forum/anti-cheat-bypass/444289-read-process-physical-memory-attach.html
		const DWORD GetUserDirectoryTableBaseOffset();
		const UINT64 GetProcessCr3(const PEPROCESS pProcess);
		const UINT64 GetKernelDirBase();
		const UINT64 TranslateLinearAddress(UINT64 directoryTableBase, UINT64 virtualAddress);
		NTSTATUS ReadVirtual(UINT64 dirbase, UINT64 address, UINT8* buffer, SIZE_T size, SIZE_T* read);
		NTSTATUS WriteVirtual(UINT64 dirbase, UINT64 address, UINT8* buffer, SIZE_T size, SIZE_T* written);
		NTSTATUS ReadPhysicalAddress(PVOID TargetAddress, PVOID lpBuffer, SIZE_T Size, SIZE_T* BytesRead);
		NTSTATUS WritePhysicalAddress(PVOID TargetAddress, PVOID lpBuffer, SIZE_T Size, SIZE_T* BytesWritten);
	}
	//Protection:

}
