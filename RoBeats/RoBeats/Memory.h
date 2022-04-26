// Roblox memory module by Viper (https://github.com/ViperTools)

#pragma once
#include <cstdint>
#include <Windows.h>
#include <iostream>
#include <string>
#include <ntstatus.h>
#include <vector>
using std::string;
using std::wstring;
using std::vector;

using tNtReadVirtualMemory = NTSTATUS(_stdcall*)(IN HANDLE hProcess, IN PVOID lpBaseAddress, OUT PVOID lpBuffer, IN ULONG nSize, OUT PULONG lpNumberOfBytesRead);
using tNtWriteVirtualMemory = NTSTATUS(_stdcall*)(IN HANDLE hProcess, IN PVOID lpBaseAddress, IN PVOID lpBuffer, IN ULONG nSize, OUT PULONG lpNumberOfBytesWritten);
extern tNtReadVirtualMemory NtReadVirtualMemory;
extern HANDLE hProc;
extern HWND hwnd;
extern DWORD procId;
extern uintptr_t baseAddress;

// Process
DWORD GetProcessId(wstring name);
void OpenRoblox();
uintptr_t GetBaseAddress(const HANDLE hProc);

// Reading
template<typename T> T Read(const uintptr_t addr);
NTSTATUS Read(const uintptr_t addr, const void* buf, const UINT size);
string ReadString(uintptr_t addr);
string ReadString(uintptr_t addr, int len);

// Scanning
vector<int> kmp_t(const vector<BYTE> vec);
int kmp(const vector<BYTE> region, const vector<BYTE> bytes, const char* mask = 0);
uintptr_t Scan(const vector<BYTE> bytes, const char* mask = 0, const DWORD pmask = PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY);
uintptr_t ScanSignature(const string sig, int offset = 0);
uintptr_t ScanVFTable(const uintptr_t vft);

template<typename T>
T Read(const uintptr_t addr) {
	T buf;
	if (NtReadVirtualMemory(hProc, (LPVOID)addr, &buf, sizeof(buf), 0) != STATUS_SUCCESS)
		return {};
	return buf;
}