#pragma once
#include <Windows.h>
#include <iostream>
#include <winternl.h>
#include <ntstatus.h>
#include <atomic>
#include <mutex>
#include <map>
#include <TlHelp32.h>

#include <utils.h>
#include <driver.h>

#include <aion_offsets.hpp>

#pragma comment(lib, "ntdll.lib")