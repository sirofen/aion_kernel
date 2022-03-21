#pragma once

namespace utils::registry {
inline static NTSTATUS write_registry(PCWSTR reg_path, PCWSTR val_name, ULONG val_type, PVOID val_data, ULONG val_sz) {
    return RtlWriteRegistryValue(RTL_REGISTRY_ABSOLUTE, reg_path, val_name, val_type, val_data, val_sz);
}

inline static RTL_QUERY_REGISTRY_TABLE query_table(PWSTR key, PVOID res) {
    RTL_QUERY_REGISTRY_TABLE query_table;
    query_table.QueryRoutine = NULL;
    query_table.Name = key;
    query_table.EntryContext = res;
    query_table.DefaultType = REG_NONE;
    query_table.DefaultData = NULL;
    query_table.DefaultLength = NULL;
    query_table.Flags = RTL_QUERY_REGISTRY_DIRECT;
    return query_table;
}

inline static NTSTATUS read(LPWSTR reg_path, LPWSTR name, PVOID res) {
    //ULONGLONG default_val = 0;
    ULONGLONG result = 0;
    PVOID context = NULL;
    RTL_QUERY_REGISTRY_TABLE query[2];
    RtlZeroMemory(query, sizeof(query));

    NTSTATUS status;

    query[0].Flags = RTL_QUERY_REGISTRY_DIRECT;
    query[0].Name = L"__b";
    query[0].EntryContext = &result;
    query[0].DefaultType = REG_QWORD;
    query[0].DefaultData = &result;
    query[0].DefaultLength = sizeof(result);

    status = RtlQueryRegistryValues(RTL_REGISTRY_ABSOLUTE, L"\\Registry\\Machine\\SOFTWARE\\aik", &query[0], &context, NULL);

    if (!NT_SUCCESS(status)) {
        print("RtlQueryRegistryValues err: 0x%lX", status);
    }
    print("val: %llu", result);
    return status;
}

inline static NTSTATUS read_registries(PWSTR reg_path, PRTL_QUERY_REGISTRY_TABLE query_tables) {
    return RtlQueryRegistryValues(RTL_REGISTRY_ABSOLUTE, reg_path, query_tables, NULL, NULL);
}
}// namespace utils::registry
