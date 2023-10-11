#include "utility.h"

typedef NTSTATUS(NTAPI* NtSuspendProcess)(IN HANDLE ProcessHandle);
typedef NTSTATUS(NTAPI* NtResumeProcess)(IN HANDLE ProcessHandle);

NtSuspendProcess pfnNtSuspendProcess = NULL;
NtResumeProcess pfnNtResumeProcess = NULL;
HWND hwndEdit;
std::string searchQuery;


int selectedPID = -1;

std::string GetProcessName(DWORD processId) {
    std::string name;
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (hProcess != nullptr) {
        char buffer[MAX_PATH];
        if (GetModuleFileNameExA(hProcess, NULL, buffer, MAX_PATH)) {
            name = PathFindFileNameA(buffer);
        }
        CloseHandle(hProcess);
    }
    return name;
}

std::string toLowerCase(const std::string& str) {
    std::string lowerStr(str);
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
    [](unsigned char c){ return std::tolower(c); });
    return lowerStr;
}

void GetProcessMemoryUsage(DWORD processId, SIZE_T& workingSetSize, SIZE_T& privateUsage) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (hProcess) {
        PROCESS_MEMORY_COUNTERS pmc;
        if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
            workingSetSize = pmc.WorkingSetSize;
            privateUsage = pmc.PagefileUsage;
        }
        CloseHandle(hProcess);
    }
}
