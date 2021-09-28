#pragma once
#include <stdafx.h>

namespace AIK_GLOBAL_SETTINGS {
/* file mapping */
// create
const HANDLE create_fm_hFile = INVALID_HANDLE_VALUE;
constexpr LPSECURITY_ATTRIBUTES create_fm_lpFileMappingAttributes = NULL;
constexpr DWORD create_fm_flProtect = PAGE_READWRITE;
// open
constexpr DWORD open_fm_dwDesiredAccess = FILE_MAP_ALL_ACCESS;
constexpr BOOL open_fm_bInheritHandle = FALSE;
/* shared mutex */
// create
constexpr LPSECURITY_ATTRIBUTES create_sm_lpMutexAttributes = NULL;
constexpr BOOL create_sm_bInitialOwner = FALSE;
// open
constexpr DWORD open_sm_dwDesiredAccess = MUTEX_ALL_ACCESS;
constexpr BOOL open_sm_bInheritHandle = FALSE;
}// namespace AIK_GLOBAL_SETTINGS