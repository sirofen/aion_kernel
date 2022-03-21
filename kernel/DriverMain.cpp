#define PRINT_VERBOSE_TRACE
#include <stdafx.h>
#include <clean.hpp>

namespace {
constexpr UNICODE_STRING kRegPath = RTL_CONSTANT_STRING(L"\\Registry\\Machine\\SOFTWARE\\ucflash");
constexpr UNICODE_STRING kXX = RTL_CONSTANT_STRING(L"xx");
constexpr UNICODE_STRING kXXX = RTL_CONSTANT_STRING(L"xxx");
constexpr UNICODE_STRING kXXXX = RTL_CONSTANT_STRING(L"xxxx");

constexpr UNICODE_STRING kRegSubKey = RTL_CONSTANT_STRING(L"\\Registry\\Machine\\SOFTWARE\\aik");
constexpr UNICODE_STRING kPid = RTL_CONSTANT_STRING(L"__p");
constexpr UNICODE_STRING kBufAddr = RTL_CONSTANT_STRING(L"__b");

constexpr SIZE_T kReqDataSz = sizeof(REQUEST_DATA);

constexpr auto kStubMsgOffset = 0x4E;
constexpr auto kPayloadOffset = 0x20;
constexpr auto kPayloadSize = 8;
constexpr char kDosStubMsg[] = "Surely it's just a DOS stub msg" /* \0 */;
constexpr wchar_t* kUserModuleName = L"user_mode.dll";
}

typedef NTSTATUS (*HookControl_t)(void* a1, void* a2);
HookControl_t poriginal_func;

static UINT64 process_image_base = 0;
static UINT64 dos_msg_addr = 0;
static UINT user_app_pid = 0;

static PVOID puser_app_control_buf_0 = 0; // first pointer
static PVOID puser_app_control_buf_1 = 0; // second pointer

static NTSTATUS status = 0;
static PEPROCESS process = 0;

static SIZE_T bytes_copied = 0;


NTSTATUS HookControl(PDEVICE_OBJECT device, PIRP irp) {
    PRINT_DEBUG("HookControl");

    bool retry = true;
retry:

    status = STATUS_SUCCESS;
    process = NULL;
    status = PsLookupProcessByProcessId((HANDLE) user_app_pid, &process);
    PRINT_DEBUG("PsLookupProcessByProcessId: user_app_pid: 0x%llX, status: 0x%llX", user_app_pid, status);
    if (!NT_SUCCESS(status) || Utils::BBCheckProcessTermination(process) || PsGetProcessExitStatus(process) != STATUS_PENDING || !dos_msg_addr) {
        PRINT_DEBUG("Updating pid and buf addr");

        // reset outdated addr
        puser_app_control_buf_0 = 0;
        puser_app_control_buf_1 = 0;
        dos_msg_addr = 0;
        user_app_pid = 0;

        //puser_app_control_buf = (PVOID) Utils::Registry::ReadRegistry<LONG64>(kRegSubKey, kBufAddr);
        user_app_pid = (UINT) Utils::Registry::ReadRegistry<LONG64>(kRegSubKey, kPid);
        PRINT_DEBUG("New pid: 0x%lX", user_app_pid);
        if (process_image_base = Utils::GetProcessImageBase((DWORD) user_app_pid); !process_image_base) {
            PRINT_ERROR("GetProcessImageBase user_app_pid err");
            return poriginal_func(device, irp);
        }
        PRINT_DEBUG("New process image base: 0x%llX", process_image_base);
        dos_msg_addr = process_image_base + kStubMsgOffset;

        if (retry) {
            retry = !retry;
            goto retry;
        } else {
            retry = true;
            PRINT_ERROR("PsLookupProcessByProcessId failed with updated values, 0x%lX", status);
            return poriginal_func(device, irp);
        }
    }

    if (!puser_app_control_buf_0) {
        BYTE buffer[40];
        bytes_copied = 0;
        if (status = MmCopyVirtualMemory(process, (PVOID) dos_msg_addr, PsGetCurrentProcess(), &buffer, 40, KernelMode, &bytes_copied);
            !NT_SUCCESS(status)) {
            PRINT_ERROR("MmCopyVirtualMemory puser_app_control_buf_0 err, 0x%lX", status);
            ObfDereferenceObject(process);
            poriginal_func(device, irp);
        }
        
        if (strcmp(kDosStubMsg, (char*)buffer) != 0) {
            PRINT_ERROR("Wrong process");
            return poriginal_func(device, irp);
        }

        // user_application_control_buffer = image_base + hardcoded offset
        KAPC_STATE apc;
        (KeStackAttachProcess)(process, &apc);
        PMODULE_ENTRY module_entry = Utils::GetModuleByName(process, kUserModuleName);
        (KeUnstackDetachProcess)(&apc);
        if (!module_entry) {
            puser_app_control_buf_0 = 0;
        } else {
            puser_app_control_buf_0 = (PVOID) ((UINT64) module_entry->Base + *(UINT64*) &buffer[32]);
        }

        PRINT_DEBUG("Updated buffer: 0x%p", puser_app_control_buf_0);
    }


    if (!puser_app_control_buf_1) {
        bytes_copied = 0;
        if (status = MmCopyVirtualMemory(process, (PVOID) puser_app_control_buf_0, PsGetCurrentProcess(), &puser_app_control_buf_1, sizeof(puser_app_control_buf_1), KernelMode, &bytes_copied);
            !NT_SUCCESS(status)) {
            PRINT_ERROR("MmCopyVirtualMemory puser_app_control_buf_1 err, 0x%lX", status);
            ObfDereferenceObject(process);
            poriginal_func(device, irp);
        }
    }

    REQUEST_DATA data;
    bytes_copied = 0;
    if (status = MmCopyVirtualMemory(process, puser_app_control_buf_1, PsGetCurrentProcess(), &data, kReqDataSz, KernelMode, &bytes_copied);
        !NT_SUCCESS(status)) {
        PRINT_ERROR("MmCopyVirtualMemory(src pid: 0x%lX, src: 0x%p) REQUEST_DATA err, 0x%lX", process, puser_app_control_buf_1, status);
        ObfDereferenceObject(process);
        return poriginal_func(device, irp);
    }

    switch (data.Type) {
        CallbackHandler(WRITE);
        CallbackHandler(READ);
        CallbackHandler(PROTECT);
        CallbackHandler(ALLOC);
        CallbackHandler(FREE);
        CallbackHandler(PAGES);
        CallbackHandler(MODULE);
        CallbackHandler(MAINBASE);
        case THREADCALL: {
            break;
        }
        case 99:
            print("99");
            *data.Status = STATUS_SUCCESS;
            break;
        default:
            break;
    }

    ObfDereferenceObject(process);
    IofCompleteRequest(irp, 0);
    return 0;
}

NTSTATUS driver_main() {
	print("[!] UC-Driver start loading!");
	PDRIVER_OBJECT DriverObject = NULL;
	UNICODE_STRING DriverObjectName = RTL_CONSTANT_STRING(L"\\Driver\\PEAUTH");
	
	if (auto status = ObReferenceObjectByName(
                &DriverObjectName,
                (ULONG) OBJ_CASE_INSENSITIVE,
                (PACCESS_STATE) 0,
                (ACCESS_MASK) 0,
                *IoDriverObjectType,
                KernelMode,
                (PVOID) 0,
                (PVOID*) &DriverObject); 
		status != STATUS_SUCCESS || !DriverObject) {
        PRINT_ERROR("ObReferenceObjectByName: 0x%lX", status);
        return status;
	}

    *(PVOID*) &poriginal_func = InterlockedExchangePointer((void**) &DriverObject->MajorFunction[IRP_MJ_FLUSH_BUFFERS], HookControl);

    print("[+] Driver Loaded!");

	return 0;
}

NTSTATUS DriversMaain(PVOID lpBaseAddress, DWORD32 dwSize) {
	if (lpBaseAddress) {
        print("driver args p 0x%p", lpBaseAddress);
	}

	driver_main();
	return 0;
}