#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winternl.h>
#include <ntstatus.h>
#include <TlHelp32.h>
#include <shlwapi.h>
#include <strsafe.h>

#include <iostream>
#include <atomic>
#include <mutex>
#include <memory>
#include <utility>
#include <cstring>
#include <stdio.h>
#include <map>

#include <utils.h>
#include <driver.h>

#include <aik_types.hpp>
#include <aik.hpp>
#include <shared_memory.hpp>
#include <shared_mutex.hpp>
#include <err.hpp>

#include <aion_offsets.hpp>

#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "Shlwapi.lib")