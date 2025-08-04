// ProcessHandling.h: Process handling stuff
#pragma once

#include <windows.h>
#include <tlhelp32.h>

// Simple struct that holds all of the necessary values regarding the game's
// process.
struct Process {
	const char* name; // File name
	HANDLE handle;    // Process handle
	DWORD id;         // Process ID (PID)
	DWORD baddr;      // Base address
};

// Adapted from Fang's Extended Training Tool v1.6b
DWORD
get_pid(const char* name) {
	DWORD pid = 0;

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 process;
	process.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(snapshot, &process)) {
		do {
			if (strcmp(process.szExeFile, name) == 0) {
				pid = process.th32ProcessID;
				break;
			}
		} while (Process32Next(snapshot, &process));
	}

	CloseHandle(snapshot);

	return pid;
}

// Adapted from Fang's Extended Training Tool v1.6b
DWORD
get_base_address(struct Process proc) {
	HMODULE mod_list[1024];
	DWORD lpcbNeeded;

	if (EnumProcessModules(proc.handle, mod_list, sizeof(mod_list),
	    &lpcbNeeded)) {
		TCHAR szModName[MAX_PATH];

		if (GetModuleFileNameEx(proc.handle, mod_list[0], szModName,
		    sizeof(szModName) / sizeof(TCHAR))) {
			if (strstr(szModName, proc.name) != NULL)
				return (DWORD)mod_list[0];
		}
	}

	return 0;
}

HANDLE
get_process_handle(DWORD pid) {
	return OpenProcess(PROCESS_ALL_ACCESS, false, pid);
}

// Waits for the specified process to start, then get its id, handle, and base
// address.
struct Process
wait_process(const char* name) {
	struct Process proc;

	proc.name = name;

	printf("Waiting for %s to start...", name);

	while (true) {
		proc.id = get_pid(name);
		if (proc.id) {
			printf("\rFound %s (PID: %ld)           \n", name, proc.id);

			proc.handle = get_process_handle(proc.id);
			if (proc.handle == NULL)
				printf("ERROR: couldn't get process handle for %ld.\n", proc.id);
			proc.baddr = get_base_address(proc);
			if (proc.baddr == 0)
				printf("ERROR: couldn't get base address for %ld.\n", proc.id);

			return proc;
		}

		Sleep(1000);
	}
}

// Globally accessible struct containing everything about the MBAACC process.
// No need to pass this struct around everywhere.
//
// Initialized at the top of main.cpp
extern struct Process gProc;
