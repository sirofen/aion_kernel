#pragma once

#define PRINT_DEBUG_LEVEL 4

#define AION_KERNEL_PRINT_PREFIX "[AI_K] "

#define print(fmt, ...) DbgPrintEx(0, 0, AION_KERNEL_PRINT_PREFIX fmt "\n", ##__VA_ARGS__)

#ifdef PRINT_VERBOSE_TRACE
#define PRINT_TRACE(fmt, ...) DbgPrintEx(0, 0, AION_KERNEL_PRINT_PREFIX "[TRACE] " fmt "\n", ##__VA_ARGS__)
#define PRINT_VERBOSE_DEBUG
#define PRINT_VERBOSE_INFO
#define PRINT_VERBOSE_WARNING
#define PRINT_VERBOSE_ERROR
#else
#define PRINT_TRACE(...)
#endif
#ifdef PRINT_VERBOSE_DEBUG
#define PRINT_DEBUG(fmt, ...) DbgPrintEx(0, 0, AION_KERNEL_PRINT_PREFIX "[DEBUG] " fmt "\n", ##__VA_ARGS__)
#define PRINT_VERBOSE_INFO
#define PRINT_VERBOSE_WARNING
#define PRINT_VERBOSE_ERROR
#else
#define PRINT_DEBUG(...)
#endif
#ifdef PRINT_VERBOSE_INFO
#define PRINT_INFO(fmt, ...) DbgPrintEx(0, 0, AION_KERNEL_PRINT_PREFIX "[INFO] " fmt "\n", ##__VA_ARGS__)
#define PRINT_VERBOSE_WARNING
#define PRINT_VERBOSE_ERROR
#else
#define PRINT_INFO(...)
#endif
#ifdef PRINT_VERBOSE_WARNING
#define PRINT_WARNING(fmt, ...) DbgPrintEx(0, 0, AION_KERNEL_PRINT_PREFIX "[WARNING] " fmt "\n", ##__VA_ARGS__)
#define PRINT_VERBOSE_ERROR
#else
#define PRINT_WARNING(...)
#endif
#ifdef PRINT_VERBOSE_ERROR
#define PRINT_ERROR(fmt, ...) DbgPrintEx(0, 0, AION_KERNEL_PRINT_PREFIX "[ERROR] " fmt "\n", ##__VA_ARGS__)
#else
#define PRINT_ERROR(...)
#endif

//#define PRINT_ERROR(fmt, ...) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, AION_KERNEL_PRINT_PREFIX "[ERROR] " fmt, ##__VA_ARGS__)
//#define PRINT_WARNING(fmt, ...) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_WARNING_LEVEL, AION_KERNEL_PRINT_PREFIX "[WARNING] " fmt, ##__VA_ARGS__)
//#define PRINT_TRACE(fmt, ...) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_TRACE_LEVEL, AION_KERNEL_PRINT_PREFIX "[TRACE] " fmt, ##__VA_ARGS__)
//#define PRINT_INFO(fmt, ...) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, AION_KERNEL_PRINT_PREFIX "[INFO] " fmt, ##__VA_ARGS__)
//#define PRINT_DEBUG(fmt, ...) DbgPrintEx(DPFLTR_IHVDRIVER_ID, PRINT_DEBUG_LEVEL, AION_KERNEL_PRINT_PREFIX "[DEBUG] " fmt, ##__VA_ARGS__)
