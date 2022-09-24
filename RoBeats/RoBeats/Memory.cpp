// Roblox memory module by Viper (https://github.com/ViperTools)

#include "Memory.h"
#include <Psapi.h>
#include <iostream>
#include <sstream>
using std::string;
using std::wstring;

HMODULE ntdll = GetModuleHandle(L"ntdll.dll");
tNtReadVirtualMemory NtReadVirtualMemory = (tNtReadVirtualMemory)GetProcAddress(ntdll, "NtReadVirtualMemory");

DWORD procId;
HANDLE hProc = 0;
uintptr_t baseAddress;
int maxStringSize = 1000;

// Process

HWND hwnd;
BOOL CALLBACK EnumWindowsCallback(HWND _hwnd, LPARAM lParam) {
	DWORD procId;
	GetWindowThreadProcessId(_hwnd, &procId);
	if (procId == lParam) {
		hwnd = _hwnd;
		return FALSE;
	}
	return TRUE;
}

DWORD GetProcessId(wstring name) {
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
		return 0;
	cProcesses = cbNeeded / sizeof(DWORD);

	for (int i = 0; i < cProcesses; i++) {
		if (aProcesses[i] != 0) {
			HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, aProcesses[i]);
			if (hProc) {
				TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
				HMODULE hMod;
				DWORD cbNeeded;
				if (EnumProcessModules(hProc, &hMod, sizeof(hMod), &cbNeeded))
				{
					GetModuleBaseName(hProc, hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));
				}
				wstring str = wstring(szProcessName);
				CloseHandle(hProc);
				if (str == name) {
					return aProcesses[i];
				}
			}
		}
	}

	return 0;
}

void OpenRoblox() {
	procId = GetProcessId(L"RobloxPlayerBeta.exe");
	hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);
	EnumWindows(EnumWindowsCallback, procId);
	baseAddress = GetBaseAddress(hProc);
}

uintptr_t GetBaseAddress(const HANDLE hProc) {
	if (!hProc)
		return 0;

	HMODULE lphModule[1024];
	DWORD lpcbNeeded;

	if (!EnumProcessModules(hProc, lphModule, sizeof(lphModule), &lpcbNeeded))
		return 0;

	return (DWORD)lphModule[0];
}

// Read / Write

string ReadString(uintptr_t addr) {
	int len = Read<int>(addr + 0x10);
	if (len >= 16) {
		return ReadString(Read<uintptr_t>(addr), len);
	}
	return ReadString(addr, len);
}

string ReadString(uintptr_t addr, int len) {
	char* buf = (char*)malloc(len);
	for (int i = 0; i < len; i++)
		memset(buf + i, Read<char>(addr + (i * sizeof(char))), sizeof(char));
	string s(buf, buf + len);
	free(buf);
	return s;
}

NTSTATUS Read(const uintptr_t addr, const void* buf, const UINT size) {
	return NtReadVirtualMemory(hProc, (PVOID)addr, (PVOID)buf, size, 0);
}

// Scanning

vector<int> kmp_t(const vector<BYTE> vec) {
	int pos = 1, cnd = 0;
	vector<int> t = { -1 };
	t.resize(vec.size());
	while (pos < vec.size()) {
		if (vec[pos] == vec[cnd]) {
			t[pos] = t[cnd];
		}
		else {
			t[pos] = cnd;
			while (cnd >= 0 && vec[pos] != vec[cnd]) {
				cnd = t[cnd];
			}
		}
		pos++;
		cnd++;
	}
	return t;
}

int kmp(const vector<BYTE> region, const vector<BYTE> bytes, const char* mask) {
	int j = 0, k = 0;
	vector<int> t = kmp_t(bytes);
	while (j < region.size()) {
		if (region[j] == bytes[k] || (mask != nullptr && mask[k] == '?')) {
			j++;
			k++;
			if (k == bytes.size())
				return j - k;
		}
		else {
			k = t[k];
			if (k < 0) {
				j++;
				k++;
			}
		}
	}
	return -1;
}

uintptr_t Scan(const vector<BYTE> bytes, const char* mask, const DWORD pmask) {
	MEMORY_BASIC_INFORMATION mbi{};
	uintptr_t address = baseAddress;
	while (address <= 0x7FFFFFFF && VirtualQueryEx(hProc, (LPVOID)address, &mbi, sizeof(mbi))) {
		if ((mbi.State == MEM_COMMIT) && (mbi.Protect & pmask) && !(mbi.Protect & PAGE_GUARD)) {
			vector<BYTE> region(mbi.RegionSize);
			const uintptr_t begin = (uintptr_t)mbi.BaseAddress, end = begin + mbi.RegionSize;
			if (NtReadVirtualMemory(hProc, (PVOID)begin, region.data(), mbi.RegionSize, 0) == STATUS_SUCCESS) {
				int offset = kmp(region, bytes, mask);
				if (offset > -1) {
					return begin + offset;
				}
			}
		}
		address += mbi.RegionSize;
		mbi = {};
	}
	return 0;
}

uintptr_t ScanSignature(const string sig, int offset) {
	string mask, byte;
	std::istringstream iss(sig);
	vector<BYTE> bytes;
	while (std::getline(iss, byte, ' ')) {
		if (byte == "?") {
			bytes.push_back(0);
			mask += '?';
		}
		else {
			bytes.push_back(std::stoul(byte, 0, 16));
			mask += 'x';
		}
	}
	return Scan(bytes, mask.c_str()) + offset;
}

uintptr_t ScanVFTable(const uintptr_t vft) {
	BYTE* b = (BYTE*)&vft;
	return Scan({ b, b + sizeof(vft) / sizeof(BYTE) }, 0, PAGE_READWRITE | PAGE_READONLY);
}