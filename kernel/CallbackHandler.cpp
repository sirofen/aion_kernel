#include "stdafx.h"
//Some stuff from: https://github.com/btbd/modmap/blob/master/driver/core.c
_IRQL_requires_max_(APC_LEVEL)
NTSTATUS CallbackWRITE(PREQUEST_WRITE args)
{
	if (((PBYTE)args->Src + args->Size < (PBYTE)args->Src) ||
		((PBYTE)args->Dest + args->Size < (PBYTE)args->Dest) ||
		((PVOID)((PBYTE)args->Src + args->Size) > MM_HIGHEST_USER_ADDRESS) ||
		((PVOID)((PBYTE)args->Dest + args->Size) > MM_HIGHEST_USER_ADDRESS)) {
		return STATUS_ACCESS_VIOLATION;
	}

	if (args->bPhysicalMem) {
		PEPROCESS pProcess = NULL;
		if (args->ProcessId == 0) 
			return STATUS_UNSUCCESSFUL;

		NTSTATUS NtRet = PsLookupProcessByProcessId((HANDLE)args->ProcessId, &pProcess);
		if (NtRet != STATUS_SUCCESS) return NtRet;

		ULONG_PTR process_dirbase = Utils::PhysicalMemory::GetProcessCr3(pProcess);
		ObDereferenceObject(pProcess);

		SIZE_T CurOffset = 0;
		SIZE_T TotalSize = args->Size;
		while (TotalSize)
		{
			INT64 CurPhysAddr = Utils::PhysicalMemory::TranslateLinearAddress(process_dirbase, (ULONG64)args->Src + CurOffset);
			if (!CurPhysAddr) return STATUS_UNSUCCESSFUL;

			ULONG64 WriteSize = min(PAGE_SIZE - (CurPhysAddr & 0xFFF), TotalSize);
			SIZE_T BytesWritten = 0;
			NtRet = Utils::PhysicalMemory::WritePhysicalAddress(PVOID(CurPhysAddr), (PVOID)((ULONG64)args->Dest+ CurOffset), WriteSize, &BytesWritten);
			TotalSize -= BytesWritten;
			CurOffset += BytesWritten;
			if (NtRet != STATUS_SUCCESS) break;
			if (BytesWritten == 0) break;
		}
		//*args-> = CurOffset;
		return NtRet;
	}
	else {
		PEPROCESS process = NULL;
		NTSTATUS status = (PsLookupProcessByProcessId)((HANDLE)args->ProcessId, &process);
		if (NT_SUCCESS(status)) {
			SIZE_T outSize = 0;
			status = (MmCopyVirtualMemory)((PsGetCurrentProcess)(), args->Src, process, args->Dest, (SIZE_T)args->Size, KernelMode, &outSize);
			(ObfDereferenceObject)(process);
		}
		return status;
	}
	return STATUS_UNSUCCESSFUL;
}

NTSTATUS CallbackREAD(PREQUEST_READ args)
{
	if (((PBYTE)args->Src + args->Size < (PBYTE)args->Src) ||
		((PBYTE)args->Dest + args->Size < (PBYTE)args->Dest) ||
		((PVOID)((PBYTE)args->Src + args->Size) > MM_HIGHEST_USER_ADDRESS) ||
		((PVOID)((PBYTE)args->Dest + args->Size) > MM_HIGHEST_USER_ADDRESS)) {

		return STATUS_ACCESS_VIOLATION;
	}
	if (args->bPhysicalMem) {
		PEPROCESS pProcess = NULL;
		if (args->ProcessId == 0) return STATUS_UNSUCCESSFUL;

		NTSTATUS NtRet = PsLookupProcessByProcessId((HANDLE)args->ProcessId, &pProcess);
		if (NtRet != STATUS_SUCCESS) return NtRet;

		ULONG_PTR process_dirbase = Utils::PhysicalMemory::GetProcessCr3(pProcess);
		ObDereferenceObject(pProcess);

		SIZE_T CurOffset = 0;
		SIZE_T TotalSize = args->Size;
        PRINT_TRACE("Physical memory to read sz: %llu", TotalSize);
        while (TotalSize > 0)
		{
			INT64 CurPhysAddr = Utils::PhysicalMemory::TranslateLinearAddress(process_dirbase, (ULONG64)args->Src + CurOffset);
            PRINT_TRACE("CurPhysAddr: 0x%llX", CurPhysAddr);
			if (!CurPhysAddr) return STATUS_UNSUCCESSFUL;

			ULONG64 ReadSize = min(PAGE_SIZE - (CurPhysAddr & 0xFFF), TotalSize);
			SIZE_T BytesRead = 0;
			NtRet = Utils::PhysicalMemory::ReadPhysicalAddress(PVOID(CurPhysAddr), (PVOID)((ULONG64)args->Dest + CurOffset), ReadSize, &BytesRead);
			TotalSize -= BytesRead;
			CurOffset += BytesRead;
            PRINT_TRACE("Phys mem read sz: %llu", BytesRead);
			if (NtRet != STATUS_SUCCESS) break;
			if (BytesRead == 0) break;
		}
		//*read = CurOffset;
		return NtRet;
	}
	else {
		PEPROCESS process = NULL;
		NTSTATUS status = (PsLookupProcessByProcessId)((HANDLE)args->ProcessId, &process);
		if (NT_SUCCESS(status)) {
			SIZE_T outSize = 0;
			status = (MmCopyVirtualMemory)(process, args->Src, (PsGetCurrentProcess)(), args->Dest, (SIZE_T)args->Size, KernelMode, &outSize);
			(ObfDereferenceObject)(process);
		}
		return status;
	}
	return STATUS_UNSUCCESSFUL;
}

_IRQL_requires_max_(APC_LEVEL)
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

_IRQL_requires_max_(APC_LEVEL)
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

_IRQL_requires_max_(APC_LEVEL)
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

    if (NTSTATUS status = (PsLookupProcessByProcessId) ((HANDLE) args->ProcessId, &process);
        !NT_SUCCESS(status)) {
        return status;
    }

	KAPC_STATE apc;

    static constexpr DWORD page_array_sz = 0x3FF;
    PRINT_DEBUG("[!] page_array_sz: 0x%lX", page_array_sz);

    const auto module_base = (UINT_PTR) args->ModuleBase;
    const auto module_size = args->ModuleSize;
    PRINT_DEBUG("[!] Page list base: 0x%llX, sz: 0x%llX", module_base, module_size);

    MEMORY_BASIC_INFORMATION mem_basic_inf{};

    DWORD page_iter = 0;
    KeStackAttachProcess(process, &apc);
    for (auto cur_page_addr = module_base;
         cur_page_addr < module_base + module_size;
         cur_page_addr = (ULONG_PTR) mem_basic_inf.BaseAddress + mem_basic_inf.RegionSize) {

        if (NTSTATUS status = ZwQueryVirtualMemory(ZwCurrentProcess(),
                                                   (PVOID) (cur_page_addr),
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
                return STATUS_ARRAY_BOUNDS_EXCEEDED;
            }
            (args->Pages)[page_iter++] = {mem_basic_inf.BaseAddress, (DWORD) mem_basic_inf.RegionSize};

            PRINT_DEBUG("[!] Accessible page - base: 0x%p, sz: 0x%lX", mem_basic_inf.BaseAddress, mem_basic_inf.RegionSize);
            continue;
        }
        PRINT_TRACE("[!] Page - base: 0x%p, sz: 0x%llX, state: 0x%lX, protect: 0x%lX",
                    mem_basic_inf.BaseAddress,
                    mem_basic_inf.RegionSize,
                    mem_basic_inf.State,
                    mem_basic_inf.Protect);
    }
    KeUnstackDetachProcess(&apc);
    (ObfDereferenceObject)(process);
    PRINT_DEBUG("[!] Pages size: %lu", page_iter + 1);
    return page_iter > 0 ? STATUS_SUCCESS : STATUS_NOT_FOUND;
}

NTSTATUS ListPages(PVOID base, DWORD size,  PAGE* const& pages) {
    MEMORY_BASIC_INFORMATION mem_basic_inf = {0};

    DWORD page_iter = 0;
    for (auto cur_page_addr = (UINT_PTR) base; cur_page_addr < (UINT_PTR) base + size; cur_page_addr = (ULONG_PTR) mem_basic_inf.BaseAddress + mem_basic_inf.RegionSize) {
        if (ZwQueryVirtualMemory(ZwCurrentProcess(), (PVOID) ((UINT_PTR) cur_page_addr), MemoryBasicInformation, &mem_basic_inf, sizeof(mem_basic_inf), NULL) != STATUS_SUCCESS) {
            return STATUS_UNSUCCESSFUL;
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

_IRQL_requires_max_(APC_LEVEL)
NTSTATUS CallbackMODULE(PREQUEST_MODULE args)
{
	PEPROCESS process = NULL;
	NTSTATUS status = (PsLookupProcessByProcessId)((HANDLE)args->ProcessId, &process);
	if (NT_SUCCESS(status)) {
		PVOID base = NULL;
		DWORD size = 0;
        PAGE pages[0x20]{};
		(KeAttachProcess)(process);

        PMODULE_ENTRY module_entry = Utils::GetModuleByName(process, args->Module);
        if (module_entry) {
            base = module_entry->Base;
            size = module_entry->Size;
			PRINT_DEBUG("[!] %S - base: 0x%p, sz: %u", args->Module, base, size);

			if (args->ListPages) {
                status = ListPages(base, size, pages);
			}

        } else {
            status = STATUS_NOT_FOUND;
        }

		(KeDetachProcess)();
		if (NT_SUCCESS(status)) {
            RtlCopyMemory(args->Pages, &pages, sizeof(pages));
			RtlCopyMemory(args->OutAddress, &base, sizeof(base));
			RtlCopyMemory(args->OutSize, &size, sizeof(size));
		}
		(ObfDereferenceObject)(process);
	}
	return status;
}

_IRQL_requires_max_(APC_LEVEL)
NTSTATUS CallbackMAINBASE(PREQUEST_MAINBASE args)
{
	PEPROCESS pProcess = NULL;
	if (args->ProcessId == 0)
		return STATUS_UNSUCCESSFUL;

	NTSTATUS NtRet = PsLookupProcessByProcessId((HANDLE)args->ProcessId, &pProcess);
	if (NtRet != STATUS_SUCCESS)
		return STATUS_UNSUCCESSFUL;

	auto base = PsGetProcessSectionBaseAddress(pProcess);
	RtlCopyMemory(args->OutAddress, &base, sizeof(base));
	ObDereferenceObject(pProcess);
	return STATUS_SUCCESS;
}
