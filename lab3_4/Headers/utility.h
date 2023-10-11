#ifndef UTILS_H
#define UTILS_H

#include <windows.h>
#include <WinBase.h>
#include <psapi.h>
#include <vector>
#include <string>
#include <commctrl.h>
#include <shlwapi.h>
#include <algorithm>

std::string GetProcessName(DWORD processId);
std::string toLowerCase(const std::string& str);
void GetProcessMemoryUsage(DWORD processId, SIZE_T& workingSetSize, SIZE_T& privateUsage);

#endif // UTILS_H
