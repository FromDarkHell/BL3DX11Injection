#include "pch.h"


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID);

void LoadPlugins();
extern "C" void AssignProcAddresses();
int executionThread();

std::wstring GetLastErrorAsString();