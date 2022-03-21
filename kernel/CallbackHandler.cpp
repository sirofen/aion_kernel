#define PRINT_VERBOSE_DEBUG
#include "stdafx.h"
//Some stuff from: https://github.com/btbd/modmap/blob/master/driver/core.c

namespace {
constexpr DWORD page_array_sz = 0xFFFF;
}

NTSTATUS CallbackWRITE(PREQUEST_WRITE args)
{
	if (((PBYTE)args->Src + args->Size < (PBYTE)args->Src) ||
		((PBYTE)args->Dest + args->Size < (PBYTE)args->Dest) ||
		((PVOID)((PBYTE)args->Src + args->Size) > MM_HIGHEST_USER_ADDRESS) ||
		((PVOID)((PBYTE)args->Dest + args->Size) > MM_HIGHEST_USER_ADDRESS)) {
		return STATUS_ACCESS_VIOLATION;
	}

		PEPROCESS process = NULL;
		NTSTATUS status = (PsLookupProcessByProcessId)((HANDLE)args->ProcessId, &process);
		if (NT_SUCCESS(status)) {
			SIZE_T outSize = 0;
			status = MmCopyVirtualMemory(PsGetCurrentProcess(), args->Src, process, args->Dest, (SIZE_T)args->Size, KernelMode, &outSize);
			(ObfDereferenceObject)(process);
		}
		return status;
}

NTSTATUS CallbackREAD(PREQUEST_READ args)
{
	if (((PBYTE)args->Src + args->Size < (PBYTE)args->Src) ||
		((PBYTE)args->Dest + args->Size < (PBYTE)args->Dest) ||
		((PVOID)((PBYTE)args->Src + args->Size) > MM_HIGHEST_USER_ADDRESS) ||
		((PVOID)((PBYTE)args->Dest + args->Size) > MM_HIGHEST_USER_ADDRESS)) {

		return STATUS_ACCESS_VIOLATION;
	}

	PEPROCESS process = NULL;
    NTSTATUS status = (PsLookupProcessByProcessId) ((HANDLE) args->ProcessId, &process);
    if (NT_SUCCESS(status)) {
        SIZE_T outSize = 0;
        status = MmCopyVirtualMemory(process, args->Src, PsGetCurrentProcess(), args->Dest, (SIZE_T) args->Size, KernelMode, &outSize);
        (ObfDereferenceObject)(process);
    }
    return status;
}

NTSTATUS CallbackPROTECT(PREQUEST_PROTECT args)
{
	if (!args->ProcessId || !args->Address || !args->Size || !args->InOutProtect)
		return STATUS_INVALID_PARAMETER;

	NTSTATUS status = STATUS_SUCCESS;
	PEPROCESS target_process = NULL;

	if (!NT_SUCCESS((PsLookupProcessByProcessId)((HANDLE)args->ProcessId, &target_process)))
	{
		return STATUS_NOT_FOUND;
	}
	SIZE_T size = args->Size;
	DWORD protect = 0;
	RtlCopyMemory(&protect, args->InOutProtect, sizeof(protect));

	(KeAttachProcess)((PKPROCESS)target_process);
	status = (ZwProtectVirtualMemory)(NtCurrentProcess(), &args->Address, &size, protect, &protect);
	(KeDetachProcess)();
	if (NT_SUCCESS(status))
		RtlCopyMemory(args->InOutProtect, &protect, sizeof(protect));

	(ObfDereferenceObject)(target_process);
	return status;
}

NTSTATUS CallbackALLOC(PREQUEST_ALLOC args)
{
	PEPROCESS process = NULL;
	NTSTATUS status = (PsLookupProcessByProcessId)((HANDLE)args->ProcessId, &process);
	if (NT_SUCCESS(status)) {
		PVOID address = NULL;
		SIZE_T size = args->Size;

		(KeAttachProcess)(process);
		(ZwAllocateVirtualMemory)(NtCurrentProcess(), &address, 0, &size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		(KeDetachProcess)();

		RtlCopyMemory(args->OutAddress, &address, sizeof(address));

		(ObfDereferenceObject)(process);
	}
	return status;
}

NTSTATUS CallbackFREE(PREQUEST_FREE args)
{
	PEPROCESS process = NULL;
	NTSTATUS status = (PsLookupProcessByProcessId)((HANDLE)args->ProcessId, &process);
	if (NT_SUCCESS(status)) {
		SIZE_T size = 0;

		(KeAttachProcess)(process);
		(ZwFreeVirtualMemory)(NtCurrentProcess(), &args->Address, &size, MEM_RELEASE);
		(KeDetachProcess)();

		(ObfDereferenceObject)(process);
	}
	return status;
}

NTSTATUS CallbackPAGES(PREQUEST_PAGES args) {
    PEPROCESS process = NULL;

    if (NTSTATUS status = PsLookupProcessByProcessId((HANDLE) args->ProcessId, &process);
        !NT_SUCCESS(status)) {
        return status;
    }

    KAPC_STATE apc;

    PRINT_DEBUG("[!] page_array_sz: 0x%lX", page_array_sz);

    auto _ppages = (PPAGE) ExAllocatePool(NonPagedPool, page_array_sz * sizeof(PAGE));
    RtlZeroMemory(_ppages, page_array_sz * sizeof(PAGE));

    PRINT_DEBUG("[!] Page list base: 0x%llX, sz: 0x%llX", args->ModuleBase, args->ModuleSize);

    MEMORY_BASIC_INFORMATION mem_basic_inf{};

    DWORD page_iter = 0;
    NTSTATUS status = STATUS_SUCCESS;
    __try {
        KeStackAttachProcess(process, &apc);
        for (auto cur_page_addr = (UINT64) args->ModuleBase;
             cur_page_addr < (UINT64) args->ModuleBase + args->ModuleSize;
             cur_page_addr = (UINT64) mem_basic_inf.BaseAddress + mem_basic_inf.RegionSize) {

            if (NTSTATUS status = ZwQueryVirtualMemory(ZwCurrentProcess(),
                                                       (PVOID) cur_page_addr,
                                                       MemoryBasicInformation,
                                                       &mem_basic_inf,
                                                       sizeof(mem_basic_inf),
                                                       NULL);
                !NT_SUCCESS(status)) {
                break;
            }

            if (mem_basic_inf.State == MEM_COMMIT && mem_basic_inf.Protect != PAGE_NOACCESS && !(mem_basic_inf.Protect & PAGE_GUARD)) {
                if (!(page_iter < page_array_sz)) {
                    PRINT_ERROR("[!] Page container is too small. Size: 0x%llX. Current iteration: 0x%lX", page_array_sz, page_iter);
                    break;
                    status = STATUS_ARRAY_BOUNDS_EXCEEDED;
                }
                //page_iter++;
                _ppages[page_iter++] = {mem_basic_inf.BaseAddress, (DWORD) mem_basic_inf.RegionSize};

                PRINT_DEBUG("[!] Accessible page - base: 0x%p, sz: 0x%lX", mem_basic_inf.BaseAddress, mem_basic_inf.RegionSize);
                continue;
            }
            PRINT_TRACE("[!] Page - base: 0x%p, sz: 0x%llX, state: 0x%lX, protect: 0x%lX",
                        mem_basic_inf.BaseAddress,
                        mem_basic_inf.RegionSize,
                        mem_basic_inf.State,
                        mem_basic_inf.Protect);
        }
    } __finally {
        KeUnstackDetachProcess(&apc);
        RtlCopyMemory(args->Pages, _ppages, page_array_sz * sizeof(PAGE));
        //SIZE_T sz;
        //MmCopyMemory(args->Pages, MM_COPY_ADDRESS{&_ppages}, page_array_sz * sizeof(PAGE), MM_COPY_MEMORY_VIRTUAL, &sz);
        ExFreePool(_ppages);
        ObfDereferenceObject(process);
    }
    PRINT_DEBUG("[!] Pages size: %lu", page_iter + 1);
    if (!NT_SUCCESS(status)) {
        return status;
    }
    return page_iter > 0 ? STATUS_SUCCESS : STATUS_NOT_FOUND;
}


NTSTATUS ListPages(PVOID base, DWORD size,  PAGE* pages) {
    MEMORY_BASIC_INFORMATION mem_basic_inf{};

    DWORD page_iter = 0;
    for (auto cur_page_addr = (UINT_PTR) base; cur_page_addr < (UINT_PTR) base + size; cur_page_addr = (ULONG_PTR) mem_basic_inf.BaseAddress + mem_basic_inf.RegionSize) {
        if (NTSTATUS status = ZwQueryVirtualMemory(ZwCurrentProcess(), (PVOID) ((UINT_PTR) cur_page_addr), MemoryBasicInformation, &mem_basic_inf, sizeof(mem_basic_inf), NULL); 
			!NT_SUCCESS(status)) {
            return status;
        }
        if (mem_basic_inf.State == MEM_COMMIT && mem_basic_inf.Protect != PAGE_NOACCESS && !(mem_basic_inf.Protect & PAGE_GUARD)) {
            PRINT_DEBUG("[!] Accessible page - base: 0x%p, sz: 0x%lX", mem_basic_inf.BaseAddress, mem_basic_inf.RegionSize);
            pages[page_iter++] = {mem_basic_inf.BaseAddress, (DWORD) mem_basic_inf.RegionSize};
        }
        PRINT_TRACE("[!] ALL PAGES - base: 0x%p, sz: 0x%lX, state: %lu, protect: %lu",
                    mem_basic_inf.BaseAddress,
                    mem_basic_inf.RegionSize,
                    mem_basic_inf.State,
                    mem_basic_inf.Protect);
    }
    return STATUS_SUCCESS;
}

NTSTATUS CallbackMODULE(PREQUEST_MODULE args)
{
	PEPROCESS process = NULL;
	NTSTATUS status = PsLookupProcessByProcessId((HANDLE)args->ProcessId, &process);
	if (NT_SUCCESS(status)) {
		PVOID base = NULL;
		DWORD size = 0;
        PAGE pages[0x200]{};
        KAPC_STATE apc;
		(KeStackAttachProcess)(process, &apc);

        PMODULE_ENTRY module_entry = Utils::GetModuleByName(process, args->Module);
        if (module_entry) {
            base = module_entry->Base;
            size = module_entry->Size;
			PRINT_DEBUG("[!] %S - base: 0x%p, sz: %u", args->Module, base, size);

			if (args->ListPages) {
                status = ListPages(base, size, pages);
                PRINT_TRACE("LIST PAGES");
			}

        } else {
            status = STATUS_NOT_FOUND;
        }

		(KeUnstackDetachProcess)(&apc);
		if (NT_SUCCESS(status)) {
            if (args->ListPages) {
                RtlCopyMemory(args->Pages, &pages, sizeof(pages));
            }
			RtlCopyMemory(args->OutAddress, &base, sizeof(base));
			RtlCopyMemory(args->OutSize, &size, sizeof(size));
		}
		(ObfDereferenceObject)(process);
	}
	return status;
}

NTSTATUS CallbackMAINBASE(PREQUEST_MAINBASE args)
{
    if (args->ProcessId == 0) {
        return STATUS_UNSUCCESSFUL;
    }

	PEPROCESS pProcess = NULL;

	NTSTATUS status = PsLookupProcessByProcessId((HANDLE)args->ProcessId, &pProcess);
	if (!NT_SUCCESS(status)) {
        return status;
	}

	auto base = PsGetProcessSectionBaseAddress(pProcess);
	RtlCopyMemory(args->OutAddress, &base, sizeof(base));
	ObDereferenceObject(pProcess);
	return STATUS_SUCCESS;
}
