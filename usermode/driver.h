#pragma once
//#define AION_KERNEL_DEBUG_SAVE_PATTERN_BUFFER

class Driver
{
public:
	UINT ProcessId;
	/*
	This is not thread safe! 
	So dont call the driver from a thread!
	*/
	const bool Init(const BOOL PhysicalMode) {
		this->bPhysicalMode = PhysicalMode;
		this->hDriver = CreateFileA(("\\\\.\\\PEAuth"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
		if (this->hDriver != INVALID_HANDLE_VALUE) {
			if (this->SharedBuffer = VirtualAlloc(0, sizeof(REQUEST_DATA), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE)) {
				UNICODE_STRING RegPath = RTL_CONSTANT_STRING(L"\\Registry\\Machine\\SOFTWARE\\ucflash");
				RegistryUtils::WriteRegistry(RegPath, RTL_CONSTANT_STRING(L"xxx"), &this->SharedBuffer, REG_QWORD, 8);
				PVOID pid = (PVOID)GetCurrentProcessId();
				RegistryUtils::WriteRegistry(RegPath, RTL_CONSTANT_STRING(L"xx"), &pid, REG_QWORD, 8);
				auto OLD_MAGGICCODE = this->MAGGICCODE;
				SendRequest(99, 0);
				if(this->MAGGICCODE == OLD_MAGGICCODE)
					this->MAGGICCODE = (ULONG64)RegistryUtils::ReadRegistry<LONG64>(RegPath, RTL_CONSTANT_STRING(L"xxxx"));
				return true;
			}
		}
		return false;
	}
    void change_mode(const BOOL physical_mode) {
        this->bPhysicalMode = physical_mode;
    }

	const NTSTATUS SendRequest(const UINT type, const PVOID args) {
		REQUEST_DATA req;
		NTSTATUS status;
		req.MaggicCode = &this->MAGGICCODE;
		req.Type = type;
		req.Arguments = args;
		req.Status = &status;
		memcpy(this->SharedBuffer, &req, sizeof(REQUEST_DATA));
		FlushFileBuffers(this->hDriver);
		return status;
	}

	const NTSTATUS WriteMem(const ULONG_PTR& addr, void* _val, const ULONGLONG sz) {
        REQUEST_WRITE req;
        req.ProcessId = this->ProcessId;
        req.Src = _val;
        req.Dest = (PVOID)addr;
        req.Size = sz;
        req.bPhysicalMem = this->bPhysicalMode;
		return this->SendRequest(REQUEST_TYPE::WRITE, &req);
	}

    template<typename T>
	const NTSTATUS WriteMemType(const ULONG_PTR& addr, T& _val, const ULONGLONG sz = sizeof(T)) {
        REQUEST_WRITE req;
        req.ProcessId = this->ProcessId;
        req.Src = (PVOID) &_val;
        req.Dest = (PVOID) addr;
        req.Size = sz;
        req.bPhysicalMem = this->bPhysicalMode;
        return this->SendRequest(REQUEST_TYPE::WRITE, &req);
    }

	const NTSTATUS ReadMem(const ULONG_PTR& addr, void* _val, const ULONGLONG sz) {
		REQUEST_READ req;
		req.ProcessId = this->ProcessId;
		req.Dest = _val;
		req.Src = (PBYTE*)addr;
		req.Size = sz;
        req.bPhysicalMem = this->bPhysicalMode;
		return this->SendRequest(REQUEST_TYPE::READ, &req);
	}

	template<typename T>
    const NTSTATUS ReadMemType(const ULONG_PTR& addr, T& _val, const ULONGLONG sz = sizeof(T)) {
        REQUEST_READ req;
        req.ProcessId = this->ProcessId;
        req.Dest = &_val;
        req.Src = (PBYTE*) addr;
        req.Size = sz;
        req.bPhysicalMem = this->bPhysicalMode;
        return this->SendRequest(REQUEST_TYPE::READ, &req);
    }

	const NTSTATUS GetPages(const std::uintptr_t base, const DWORD sz) {
        REQUEST_PAGES req;
        req.ProcessId = this->ProcessId;
        req.ModuleBase = reinterpret_cast<PVOID>(base);
        req.ModuleSize = sz;
        return this->SendRequest(REQUEST_TYPE::PAGES, &req);
	}


	const UINT GetProcessId(const wchar_t* process_name) {
		UINT pid = 0;
		// Create toolhelp snapshot.
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		PROCESSENTRY32 process;
		ZeroMemory(&process, sizeof(process));
		process.dwSize = sizeof(process);
		// Walkthrough all processes.
		if (Process32First(snapshot, &process))
		{
			do {
                //if (wcsstr(process.szExeFile, process_name))
                if (_wcsicmp(process.szExeFile, process_name) == 0)
				{
					pid = process.th32ProcessID;
					break;
				}
			} while (Process32Next(snapshot, &process));
		}
		CloseHandle(snapshot);
		return pid;
	}

	const bool Attach(const wchar_t* Processname, const wchar_t* Classname = 0) {
		if (Classname) {
			while (!FindWindowW(Classname, 0)){ Sleep(50); }
		}
		if (this->ProcessId = this->GetProcessId(Processname))
			return true;
		return false;
	}

	typedef struct Module {
		uint64_t addr; 
		DWORD size;
        std::map<std::uintptr_t, DWORD> pages;
        const bool empty() const noexcept {
			return !addr && !size;
		}
	};

	const std::uintptr_t FindPattern(const std::uintptr_t& base, const std::size_t& length, const BYTE*&& pattern, const BYTE& mask = 0);
    const std::uintptr_t FindPatternModule(const Module& module, const BYTE*&& pattern, const std::size_t& sz_phys_mode = 0, const BYTE& mask = 0);
    //const std::map<std::uintptr_t, unsigned long> GetModulePages(const Module& module);

	const Module GetModuleBase(const wchar_t* ModuleName = 0, bool ListPages = false) {
		if (bPhysicalMode) {
			REQUEST_MAINBASE req;
			uint64_t base = NULL;
			req.ProcessId = this->ProcessId;
			req.OutAddress = (PBYTE*)&base;
			this->SendRequest(REQUEST_TYPE::MAINBASE, &req);
			return { base, 0 };
		}
		else {
			if (!ModuleName)
				return { 0, 0 };
			REQUEST_MODULE req;
			uint64_t base = NULL;
			DWORD size = NULL;
            PAGE pages[0x20];
			req.ProcessId = this->ProcessId;
			req.OutAddress = (PBYTE*)&base;
			req.OutSize = &size;
            req.ListPages = ListPages;
            req.Pages = pages;
			wcscpy_s(req.Module, sizeof(req.Module) / sizeof(req.Module[0]), ModuleName);
			this->SendRequest(REQUEST_TYPE::MODULE, &req);
            std::map<std::uintptr_t, DWORD> map_pages;
			if (ListPages) {
                for (int i = 0; i < 0x20; i++) {
                    if (!pages[i].empty()) {
                        map_pages.emplace((std::uintptr_t) pages[i].Address, pages[i].Size);
                    }
                }
			}
            return {base, size, map_pages};
		}
	}

	void dump_memory(std::uintptr_t base_addr, std::size_t size, const wchar_t*&& prefix = L"");

private:
	PVOID SharedBuffer;
	HANDLE hDriver;
	ULONG64 MAGGICCODE = 0x59002360218c1e2dul;
	BOOL bPhysicalMode = FALSE;
	typedef enum _REQUEST_TYPE : UINT {
		WRITE,
		READ,
		PROTECT,
		ALLOC,
		FREE,
		PAGES,
		MODULE,
		MAINBASE,
		THREADCALL,
	} REQUEST_TYPE;

	typedef struct _REQUEST_DATA {
		ULONG64* MaggicCode;
		UINT Type;
		PVOID Arguments;
		NTSTATUS* Status;
	} REQUEST_DATA, * PREQUEST_DATA;

	typedef struct _REQUEST_WRITE {
		DWORD ProcessId;
		PVOID Dest;
		PVOID Src;
		DWORD Size;
		BOOL bPhysicalMem;
	} REQUEST_WRITE, * PREQUEST_WRITE;

	typedef struct _REQUEST_READ {
		DWORD ProcessId;
		PVOID Dest;
		PVOID Src;
		ULONGLONG Size;
		BOOL bPhysicalMem;
	} REQUEST_READ, * PREQUEST_READ;

	typedef struct _REQUEST_PROTECT {
		DWORD ProcessId;
		PVOID Address;
		DWORD Size;
		PDWORD InOutProtect;
	} REQUEST_PROTECT, * PREQUEST_PROTECT;

	typedef struct _REQUEST_ALLOC {
		DWORD ProcessId;
		PVOID OutAddress;
		DWORD Size;
		DWORD Protect;
	} REQUEST_ALLOC, * PREQUEST_ALLOC;

	typedef struct _REQUEST_FREE {
		DWORD ProcessId;
		PVOID Address;
	} REQUEST_FREE, * PREQUEST_FREE;

    typedef struct _PAGE {
        void* Address;
        DWORD Size;
        const bool empty() const noexcept {
            return !Address || !Size;
        }
    } PAGE, *PPAGE;

	typedef struct _REQUEST_MODULE {
		DWORD ProcessId;
		WCHAR Module[0xFF];
		PBYTE* OutAddress;
		DWORD* OutSize;
        bool ListPages;
        PPAGE Pages;
	} REQUEST_MODULE, * PREQUEST_MODULE;

	typedef struct _REQUEST_MAINBASE {
		DWORD ProcessId;
		PBYTE* OutAddress;
	} REQUEST_MAINBASE, * PREQUEST_MAINBASE;

	typedef struct _REQUEST_PAGES {
        DWORD ProcessId;
        PVOID ModuleBase;
        DWORD ModuleSize;
        PAGE Pages[0xFF];
    } REQUEST_PAGES, *PREQUEST_PAGES;
};

static Driver* driver = new Driver;