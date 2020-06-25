
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <string>
#include <iostream>
#include <list>
#include <fstream>

HMODULE gM;

std::list<std::wstring> filesToDelete { L"2KLOGO.MP4",L"2KLOGO.webm",L"AMDLOGO.mp4", L"GBXLOGO.mp4", L"GBXLOGO.webm" };

int executionThread() {
    WCHAR exePath[513] = { 0 };
    GetModuleFileNameW(gM, exePath, 512);
    std::wstring gamePath = exePath;
    std::size_t pos = gamePath.find(L"OakGame");
    gamePath = gamePath.substr(0, pos) + L"OakGame\\Content\\Movies\\";
    for (const std::wstring& fileName : filesToDelete) {
        DeleteFile((gamePath + fileName).c_str()); // Just play it safe and don't really care about errors meh
    }

    return 1;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        gM = hModule;
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)executionThread, NULL, NULL, NULL);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

