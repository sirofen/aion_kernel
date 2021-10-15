#pragma once
#include <windows.h>

// https://github.com/rob-DEV/RunPE
int run_pe(PVOID img) {

    IMAGE_DOS_HEADER* dos_header;
    IMAGE_NT_HEADERS64* nt_header;
    IMAGE_SECTION_HEADER* section_header;

    PROCESS_INFORMATION pi;
    STARTUPINFO si;

    UINT_PTR image_base;
    PVOID pimage_base;

    wchar_t cur_fpath[MAX_PATH];

    dos_header = PIMAGE_DOS_HEADER(img);
    nt_header = PIMAGE_NT_HEADERS64(DWORD64(img) + dos_header->e_lfanew);

    if (nt_header->Signature != IMAGE_NT_SIGNATURE) {
        return 1;
    }
    ZeroMemory(&pi, sizeof(pi));
    ZeroMemory(&si, sizeof(si));

    GetModuleFileName(0, cur_fpath, MAX_PATH);

    if (!CreateProcessW(cur_fpath, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi)) {
        return 1;
    }

    CONTEXT* ctx;
    ctx = LPCONTEXT(VirtualAlloc(NULL, sizeof(ctx), MEM_COMMIT, PAGE_READWRITE));
    if (ctx == NULL) {
        return 1;
    }
    ctx->ContextFlags = CONTEXT_FULL;

    if (!GetThreadContext(pi.hThread, LPCONTEXT(ctx))) {
        return 1;
    }
    //ReadProcessMemory(pi.hProcess, LPCVOID(ctx->Rbx + 0x10), LPVOID(&image_base), 0x8, 0);

    pimage_base = VirtualAllocEx(
            pi.hProcess,
            LPVOID(nt_header->OptionalHeader.ImageBase),
            nt_header->OptionalHeader.SizeOfImage,
            MEM_COMMIT | MEM_RESERVE,
            PAGE_EXECUTE_READWRITE);

    if (pimage_base == NULL) {
        return 1;
    }

    WriteProcessMemory(pi.hProcess, pimage_base, img, nt_header->OptionalHeader.SizeOfImage, NULL);

    for (DWORD i = 0; i < nt_header->FileHeader.NumberOfSections; i++) {
        section_header = PIMAGE_SECTION_HEADER(DWORD64(img) + dos_header->e_lfanew + 0x108 + (i * 40));

        WriteProcessMemory(
                pi.hProcess,
                LPVOID(DWORD64(pimage_base) + section_header->VirtualAddress),
                LPVOID(DWORD64(img) + section_header->PointerToRawData),
                section_header->SizeOfRawData,
                NULL);
    }
    WriteProcessMemory(
            pi.hProcess,
            LPVOID(ctx->Rdx + 0x10),
            LPVOID(&nt_header->OptionalHeader.ImageBase),
            0x8,
            NULL);

    ctx->Rcx = DWORD64(pimage_base) + nt_header->OptionalHeader.AddressOfEntryPoint;
    SetThreadContext(pi.hThread, LPCONTEXT(ctx));
    ResumeThread(pi.hThread);

    WaitForSingleObject(pi.hProcess, NULL);

    return 0;
}