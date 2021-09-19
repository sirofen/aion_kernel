#include <clean.hpp>

namespace piddbcache {
//https://github.com/BadPlayer555/TraceCleaner/blob/master/TraceCleaner/clean.hpp
UCHAR PiDDBLockPtr_sig[] = "\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x4C\x8B\x8C";
UCHAR PiDDBCacheTablePtr_sig[] = "\x66\x03\xD2\x48\x8D\x0D";
//you can also put the sig within the function, but some of the sig ends up on the stack and in the .text section, and causes issues when zeroing the sig memory.

EXTERN_C PVOID ResolveRelativeAddress(
        _In_ PVOID Instruction,
        _In_ ULONG OffsetOffset,
        _In_ ULONG InstructionSize) {
    ULONG_PTR Instr = (ULONG_PTR) Instruction;
    LONG RipOffset = *(PLONG) (Instr + OffsetOffset);
    PVOID ResolvedAddr = (PVOID) (Instr + InstructionSize + RipOffset);

    return ResolvedAddr;
}

NTSTATUS BBSearchPattern(IN PCUCHAR pattern, IN UCHAR wildcard, IN ULONG_PTR len, IN const VOID* base, IN ULONG_PTR size, OUT PVOID* ppFound, int index = 0) {
    ASSERT(ppFound != NULL && pattern != NULL && base != NULL);
    if (ppFound == NULL || pattern == NULL || base == NULL)
        return STATUS_ACCESS_DENIED;//STATUS_INVALID_PARAMETER;
    int cIndex = 0;
    for (ULONG_PTR i = 0; i < size - len; i++) {
        BOOLEAN found = TRUE;
        for (ULONG_PTR j = 0; j < len; j++) {
            if (pattern[j] != wildcard && pattern[j] != ((PCUCHAR) base)[i + j]) {
                found = FALSE;
                break;
            }
        }

        if (found != FALSE && cIndex++ == index) {
            *ppFound = (PUCHAR) base + i;
            return STATUS_SUCCESS;
        }
    }

    return STATUS_NOT_FOUND;
}

PVOID g_KernelBase = NULL;
ULONG g_KernelSize = 0;
PVOID GetKernelBase(OUT PULONG pSize) {
    NTSTATUS status = STATUS_SUCCESS;
    ULONG bytes = 0;
    PRTL_PROCESS_MODULES pMods = NULL;
    PVOID checkPtr = NULL;
    UNICODE_STRING routineName;

    // Already found
    if (g_KernelBase != NULL) {
        if (pSize)
            *pSize = g_KernelSize;
        return g_KernelBase;
    }

    RtlUnicodeStringInit(&routineName, L"NtOpenFile");

    checkPtr = MmGetSystemRoutineAddress(&routineName);
    if (checkPtr == NULL)
        return NULL;

    // Protect from UserMode AV
    status = ZwQuerySystemInformation(SystemModuleInformation, 0, bytes, &bytes);
    if (bytes == 0) {
        print("Invalid SystemModuleInformation size");
        return NULL;
    }

    pMods = (PRTL_PROCESS_MODULES) ExAllocatePoolWithTag(NonPagedPool, bytes, BB_POOL_TAG);
    RtlZeroMemory(pMods, bytes);

    status = ZwQuerySystemInformation(SystemModuleInformation, pMods, bytes, &bytes);

    if (NT_SUCCESS(status)) {
        PRTL_PROCESS_MODULE_INFORMATION pMod = pMods->Modules;

        for (ULONG i = 0; i < pMods->NumberOfModules; i++) {
            // System routine is inside module
            if (checkPtr >= pMod[i].ImageBase &&
                checkPtr < (PVOID) ((PUCHAR) pMod[i].ImageBase + pMod[i].ImageSize)) {
                g_KernelBase = pMod[i].ImageBase;
                g_KernelSize = pMod[i].ImageSize;
                if (pSize)
                    *pSize = g_KernelSize;
                break;
            }
        }
    }

    if (pMods)
        ExFreePoolWithTag(pMods, BB_POOL_TAG);
    print("g_KernelBase: %x", g_KernelBase);
    print("g_KernelSize: %x", g_KernelSize);
    return g_KernelBase;
}

NTSTATUS BBScanSection(IN PCCHAR section, IN PCUCHAR pattern, IN UCHAR wildcard, IN ULONG_PTR len, OUT PVOID* ppFound, PVOID base = nullptr) {
    //ASSERT(ppFound != NULL);
    if (ppFound == NULL)
        return STATUS_ACCESS_DENIED;//STATUS_INVALID_PARAMETER

    if (nullptr == base)
        base = GetKernelBase(&g_KernelSize);
    if (base == nullptr)
        return STATUS_ACCESS_DENIED;//STATUS_NOT_FOUND;

    PIMAGE_NT_HEADERS64 pHdr = RtlImageNtHeader(base);
    if (!pHdr)
        return STATUS_ACCESS_DENIED;// STATUS_INVALID_IMAGE_FORMAT;

    //PIMAGE_SECTION_HEADER pFirstSection = (PIMAGE_SECTION_HEADER)(pHdr + 1);
    PIMAGE_SECTION_HEADER pFirstSection = (PIMAGE_SECTION_HEADER) ((uintptr_t) &pHdr->FileHeader + pHdr->FileHeader.SizeOfOptionalHeader + sizeof(IMAGE_FILE_HEADER));

    for (PIMAGE_SECTION_HEADER pSection = pFirstSection; pSection < pFirstSection + pHdr->FileHeader.NumberOfSections; pSection++) {
        //DbgPrint("section: %s\r\n", pSection->Name);
        ANSI_STRING s1, s2;
        RtlInitAnsiString(&s1, section);
        RtlInitAnsiString(&s2, (PCCHAR) pSection->Name);
        if (RtlCompareString(&s1, &s2, TRUE) == 0) {
            PVOID ptr = NULL;
            NTSTATUS status = BBSearchPattern(pattern, wildcard, len, (PUCHAR) base + pSection->VirtualAddress, pSection->Misc.VirtualSize, &ptr);
            if (NT_SUCCESS(status)) {
                *(PULONG64) ppFound = (ULONG_PTR) (ptr);//- (PUCHAR)base
                //DbgPrint("found\r\n");
                return status;
            }
            //we continue scanning because there can be multiple sections with the same name.
        }
    }

    return STATUS_ACCESS_DENIED;//STATUS_NOT_FOUND;
}

extern "C" bool LocatePiDDB(PERESOURCE* lock, PRTL_AVL_TABLE* table) {
    PVOID PiDDBLockPtr = nullptr, PiDDBCacheTablePtr = nullptr;
    if (!NT_SUCCESS(BBScanSection("PAGE", PiDDBLockPtr_sig, 0, sizeof(PiDDBLockPtr_sig) - 1, reinterpret_cast<PVOID*>(&PiDDBLockPtr)))) {
        print("Unable to find PiDDBLockPtr sig.");
        return false;
    }

    if (!NT_SUCCESS(BBScanSection("PAGE", PiDDBCacheTablePtr_sig, 0, sizeof(PiDDBCacheTablePtr_sig) - 1, reinterpret_cast<PVOID*>(&PiDDBCacheTablePtr)))) {
        print("Unable to find PiDDBCacheTablePtr sig");
        return false;
    }

    PiDDBCacheTablePtr = PVOID((uintptr_t) PiDDBCacheTablePtr + 3);

    *lock = (PERESOURCE) (ResolveRelativeAddress(PiDDBLockPtr, 3, 7));
    *table = (PRTL_AVL_TABLE) (ResolveRelativeAddress(PiDDBCacheTablePtr, 3, 7));

    return true;
}

//https://github.com/dx9hk/PiDDBCacheTable
NTSTATUS ClearPiddbCacheTable(ULONG TimeStamp) {
    PERESOURCE lock;
    PRTL_AVL_TABLE PiDDBCacheTable;
    if (!NT_SUCCESS(LocatePiDDB(&lock, &PiDDBCacheTable))) {
        print("[PiDDBCT] Locating PiDDB Failed");
        return STATUS_NOT_FOUND;
    }

    //Found PiDDBCacheTable
    print("[PiDDBCT] PiDDBCacheTable -> 0x%llx", PiDDBCacheTable);

    //Check Entries
    print("[PiDDBCT] PiDDBCacheTable->NumberGenericTableElements -> %d", PiDDBCacheTable->NumberGenericTableElements);

    //Get First Entry
    PPiDDBCacheEntry FirstEntry = (PPiDDBCacheEntry) PiDDBCacheTable->BalancedRoot.RightChild;

    print("[PiDDBCT] First Entry -> 0x%p\t:\tFirst Entry TimeStamp -> 0x%x",
          FirstEntry,
          //FirstEntry->DriverName,
          FirstEntry->TimeDateStamp);

    ExAcquireResourceExclusiveLite(lock, TRUE);

    for (PPiDDBCacheEntry CurrEntry =
                 (PPiDDBCacheEntry) RtlEnumerateGenericTableAvl(PiDDBCacheTable, TRUE);     /* restart */
         CurrEntry != NULL;                                                                 /* as long as the current entry is valid */
         CurrEntry = (PPiDDBCacheEntry) RtlEnumerateGenericTableAvl(PiDDBCacheTable, FALSE) /* no restart, get latest element */
    ) {

        PRINT_DEBUG("[PiDDBCT] Entry Name -> %wZ\t:\tEntry TimeStamp -> 0x%x",
                    CurrEntry->DriverName,
                    CurrEntry->TimeDateStamp);

        if (CurrEntry->TimeDateStamp == TimeStamp) {
            print("[PiDDBCT] Found Entry -> %wZ\t:\tEntry TimeStamp -> 0x%x",
                  CurrEntry->DriverName,
                  CurrEntry->TimeDateStamp);

            //Unlink
            RemoveEntryList(&CurrEntry->List);
            //Delete
            RtlDeleteElementGenericTableAvl(PiDDBCacheTable, CurrEntry);
            //Clean up
            ExReleaseResourceLite(lock);

            return STATUS_SUCCESS;
        }
    }

    ExReleaseResourceLite(lock);

    return STATUS_NOT_FOUND;
}
//To get your timestamp dynamically read the following.
//https://blog.kowalczyk.info/articles/pefileformat.html
}// namespace piddbcache
namespace mmunloadeddrivers {
//https://github.com/BadPlayer555/TraceCleaner/blob/master/TraceCleaner/clean.hpp
PMM_UNLOADED_DRIVER MmUnloadedDrivers;
PULONG MmLastUnloadedDriver;
ERESOURCE PsLoadedModuleResource;
UINT64 ntoskrnlBase = 0, ntoskrnlSize = 0;

BOOLEAN bDataCompare(const BYTE* pData, const BYTE* bMask, const char* szMask) {
    for (; *szMask; ++szMask, ++pData, ++bMask)
        if (*szMask == 'x' && *pData != *bMask)
            return 0;

    return (*szMask) == 0;
}

UINT64 FindPattern(UINT64 dwAddress, UINT64 dwLen, BYTE* bMask, char* szMask) {
    for (UINT64 i = 0; i < dwLen; i++)
        if (bDataCompare((BYTE*) (dwAddress + i), bMask, szMask))
            return (UINT64) (dwAddress + i);

    return 0;
}

PVOID ResolveRelativeAddress(PVOID Instruction, ULONG OffsetOffset, ULONG InstructionSize) {
    ULONG_PTR Instr = (ULONG_PTR) Instruction;
    LONG RipOffset = *(PLONG) (Instr + OffsetOffset);
    PVOID ResolvedAddr = (PVOID) (Instr + InstructionSize + RipOffset);
    return ResolvedAddr;
}

//https://github.com/SeiV-K/kernelmode_ksocket/blob/main/kernel_socket_server/Utils.cpp
LONG RetrieveMmUnloadedDriversData(VOID) {
    ULONG bytes = 0;

    NTSTATUS status = ZwQuerySystemInformation(SystemModuleInformation, 0, bytes, &bytes);
    if (!bytes) return 1;
    PRTL_PROCESS_MODULES modules = (PRTL_PROCESS_MODULES) ExAllocatePoolWithTag(NonPagedPool, bytes, MMUD_POOL_TAG);
    status = ZwQuerySystemInformation(SystemModuleInformation, modules, bytes, &bytes);
    if (!NT_SUCCESS(status)) return 2;
    PRTL_PROCESS_MODULE_INFORMATION module = modules->Modules;

    for (ULONG i = 0; i < modules->NumberOfModules; i++) {
        if (!strcmp((char*) module[i].FullPathName, "\\SystemRoot\\system32\\ntoskrnl.exe")) {
            ntoskrnlBase = (UINT64) module[i].ImageBase;
            ntoskrnlSize = (UINT64) module[i].ImageSize;
            break;
        }
    }
    if (modules) ExFreePoolWithTag(modules, 0);

    UINT64 MmUnloadedDriversInstr = FindPattern((UINT64) ntoskrnlBase, (UINT64) ntoskrnlSize, (BYTE*) "\x4C\x8B\x00\x00\x00\x00\x00\x4C\x8B\xC9\x4D\x85\x00\x74", "xx?????xxxxx?x");
    if (MmUnloadedDriversInstr == NULL) return 3;

    UINT64 MmLastUnloadedDriverInstr = FindPattern((UINT64) ntoskrnlBase, (UINT64) ntoskrnlSize, (BYTE*) "\x8B\x05\x00\x00\x00\x00\x83\xF8\x32", "xx????xxx");
    if (MmLastUnloadedDriverInstr == NULL) return 4;

    MmUnloadedDrivers = *(PMM_UNLOADED_DRIVER*) ResolveRelativeAddress((PVOID) MmUnloadedDriversInstr, 3, 7);
    MmLastUnloadedDriver = (PULONG) ResolveRelativeAddress((PVOID) MmLastUnloadedDriverInstr, 2, 6);

    return 0;
}

BOOLEAN IsUnloadedDriverEntryEmpty(_In_ PMM_UNLOADED_DRIVER Entry) {
    if (Entry->Name.MaximumLength == 0 || Entry->Name.Length == 0 || Entry->Name.Buffer == NULL)
        return TRUE;
    else
        return FALSE;
}

BOOLEAN IsMmUnloadedDriversFilled(VOID) {
    for (ULONG Index = 0; Index < MM_UNLOADED_DRIVERS_SIZE; ++Index) {
        PMM_UNLOADED_DRIVER Entry = &MmUnloadedDrivers[Index];
        if (IsUnloadedDriverEntryEmpty(Entry)) return FALSE;
    }
    return TRUE;
}

NTSTATUS ClearMmUnloadedDrivers(_In_ PUNICODE_STRING DriverName, _In_ BOOLEAN AccquireResource) {
    if (NTSTATUS status = RetrieveMmUnloadedDriversData() != STATUS_SUCCESS) {
        return status;
    }
    print("[MmUD] MnUnloadedDrivers - retrieved");

    if (AccquireResource) {
        ExAcquireResourceExclusiveLite(&PsLoadedModuleResource, TRUE);
    }
    BOOLEAN Modified = FALSE;
    BOOLEAN Filled = IsMmUnloadedDriversFilled();
    for (ULONG Index = 0; Index < MM_UNLOADED_DRIVERS_SIZE; ++Index) {
        PMM_UNLOADED_DRIVER Entry = &MmUnloadedDrivers[Index];
        if (Entry->Name.Buffer == NULL && !Modified) {
            break;
        }

        PRINT_DEBUG("[MmUD] MmUnloadedDrivers Entry: %S, COMPARING TO %S", Entry->Name.Buffer, DriverName->Buffer);

        if (Modified) {
            PMM_UNLOADED_DRIVER PrevEntry = &MmUnloadedDrivers[Index - 1];
            RtlCopyMemory(PrevEntry, Entry, sizeof(MM_UNLOADED_DRIVER));
            if (Index == MM_UNLOADED_DRIVERS_SIZE - 1) {
                RtlFillMemory(Entry, sizeof(MM_UNLOADED_DRIVER), 0);
            }
//        } else if (RtlEqualUnicodeString(DriverName, &Entry->Name, TRUE)) {
        } else if (_wcsicmp(DriverName->Buffer, Entry->Name.Buffer) == 0) {

            PRINT_DEBUG("[MmUD] Match found! MmUnloadedDrivers Entry: %S", Entry->Name.Buffer);

            PVOID BufferPool = Entry->Name.Buffer;
            RtlFillMemory(Entry, sizeof(MM_UNLOADED_DRIVER), 0);
            ExFreePoolWithTag(BufferPool, MMUD_POOL_TAG);
            *MmLastUnloadedDriver = (Filled ? MM_UNLOADED_DRIVERS_SIZE : *MmLastUnloadedDriver) - 1;
            Modified = TRUE;
        }
    }
    if (Modified) {
        ULONG64 PreviousTime = 0;
        for (LONG Index = MM_UNLOADED_DRIVERS_SIZE - 2; Index >= 0; --Index) {
            PMM_UNLOADED_DRIVER Entry = &MmUnloadedDrivers[Index];
            if (IsUnloadedDriverEntryEmpty(Entry)) {
                continue;
            }
            if (PreviousTime != 0 && Entry->UnloadTime > PreviousTime) {
                Entry->UnloadTime = PreviousTime - 48;
            }
            PreviousTime = Entry->UnloadTime;
        }
        ClearMmUnloadedDrivers(DriverName, FALSE);
    }

    if (AccquireResource) {
        ExReleaseResourceLite(&PsLoadedModuleResource);
    }
    return Modified ? STATUS_SUCCESS : STATUS_NOT_FOUND;
}
}// namespace mmunloadeddrivers