#pragma once

#define CallbackHandler(name)												\
 case REQUEST_TYPE::name: {													\
		REQUEST_##name args;												\
        RtlCopyMemory(&args, data.Arguments, sizeof(args));					\
        *data.Status = Callback##name(&args);								\
        break;																\
    }

typedef enum _REQUEST_TYPE : UINT {
	WRITE,
	READ,
	PROTECT,
	ALLOC,
	FREE,
    PAGES,
	MODULE,
	MAINBASE,
	THREADCALL,
} REQUEST_TYPE;

typedef struct _REQUEST_DATA {
	UINT Type;
	PVOID Arguments;
	NTSTATUS* Status;
} REQUEST_DATA, * PREQUEST_DATA;

typedef struct _REQUEST_WRITE {
	DWORD ProcessId;
	PVOID Dest;
	PVOID Src;
	DWORD Size;
	BOOL bPhysicalMem;
} REQUEST_WRITE, * PREQUEST_WRITE;

typedef struct _REQUEST_READ {
	DWORD ProcessId;
	PVOID Dest;
	PVOID Src;
    ULONGLONG Size;
	BOOL bPhysicalMem;
} REQUEST_READ, * PREQUEST_READ;

typedef struct _REQUEST_PROTECT {
	DWORD ProcessId;
	PVOID Address;
	DWORD Size;
	PDWORD InOutProtect;
} REQUEST_PROTECT, * PREQUEST_PROTECT;

typedef struct _REQUEST_ALLOC {
	DWORD ProcessId;
	PVOID OutAddress;
	DWORD Size;
	DWORD Protect;
} REQUEST_ALLOC, * PREQUEST_ALLOC;

typedef struct _REQUEST_FREE {
	DWORD ProcessId;
	PVOID Address;
} REQUEST_FREE, * PREQUEST_FREE;

typedef struct _PAGE {
    PVOID Address;
    ULONGLONG Size;
} PAGE, *PPAGE;

typedef struct _REQUEST_MODULE {
	DWORD ProcessId;
	WCHAR Module[0xFF];
	UINT_PTR* OutAddress;
	DWORD* OutSize;
    bool ListPages;
	PPAGE Pages;
} REQUEST_MODULE, * PREQUEST_MODULE;

typedef struct _REQUEST_MAINBASE {
	DWORD ProcessId;
	PBYTE* OutAddress;
} REQUEST_MAINBASE, * PREQUEST_MAINBASE;

typedef struct _REQUEST_PAGES {
    DWORD ProcessId;
    PVOID ModuleBase;
    ULONGLONG ModuleSize;
    PPAGE Pages;
} REQUEST_PAGES, *PREQUEST_PAGES;

NTSTATUS CallbackWRITE(PREQUEST_WRITE args);
NTSTATUS CallbackREAD(PREQUEST_READ args);
NTSTATUS CallbackPROTECT(PREQUEST_PROTECT args);
NTSTATUS CallbackALLOC(PREQUEST_ALLOC args);
NTSTATUS CallbackFREE(PREQUEST_FREE args);
NTSTATUS CallbackPAGES(PREQUEST_PAGES args);
NTSTATUS CallbackMODULE(PREQUEST_MODULE args);
NTSTATUS CallbackMAINBASE(PREQUEST_MAINBASE args);