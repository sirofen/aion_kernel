// https://github.com/BadPlayer555/TraceCleaner/blob/master/TraceCleaner/clean.hpp
#pragma once
#include <stdafx.h>

#define BB_POOL_TAG 'enoB'
#define MMUD_POOL_TAG 'DUmM'
#define MM_UNLOADED_DRIVERS_SIZE 50

#define DEBUG_PIDDBCACHE TRUE
#define DEBUG_MMUNLOADEDDRIVERS TRUE

namespace piddbcache {
NTSTATUS ClearPiddbCacheTable(ULONG TimeStamp);
}
namespace mmunloadeddrivers {
LONG RetrieveMmUnloadedDriversData(VOID);
NTSTATUS ClearMmUnloadedDrivers(_In_ PUNICODE_STRING DriverName, _In_ BOOLEAN AccquireResource);
}
