#include "pch.h"
#include <filesystem>
#include "dllmain.h"
#include "PluginLoadHook.h"
#include "ThreadManager.hpp"
#pragma pack(1)

static HINSTANCE hL;
static HMODULE gameModule;

FARPROC p[3] = { 0 };

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        std::string cmdArgs = GetCommandLineA(); // Get the command line args for our running process
                                                 
        // If we're running in debug mode, we wanna allocate the console
        if (cmdArgs.find("--debug") != std::string::npos) {
            AllocConsole(); // Allocate our console
            InitializeConsole(); // Redirect stdout, etc to the console
        }

        LogString(L"Console allocated...\n");
        LogString(L"==== Debug ====\n");

        LogString(L"Suspending all other threads...\n");

        // Suspend all other threads to prevent a giant race condition
        ThreadManager::Suspend();

        wchar_t DX11Path[MAX_PATH];
        GetSystemDirectory(DX11Path, MAX_PATH);
        wcscat_s(DX11Path, L"\\d3d11.dll");

        hL = LoadLibrary(DX11Path);
        p[0] = GetProcAddress(hL, "D3D11CoreCreateDevice");
        p[1] = GetProcAddress(hL, "D3D11CreateDevice");
        p[2] = GetProcAddress(hL, "D3D11CreateDeviceAndSwapChain");

        gameModule = hModule;
        DisableThreadLibraryCalls(hModule);

        // Resume all other threads.
        LogString(L"Resuming all other threads...\n");
        ThreadManager::Resume();

        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)executionThread, NULL, NULL, NULL);
    }
    else if (reason == DLL_PROCESS_DETACH || reason == DLL_THREAD_DETACH) {
        FreeLibrary(hL);
    }
    return TRUE;
}

int executionThread() {
    InitializePluginHooks(gameModule);
    return TRUE;
}

// This is all required in order to be actually fully loaded by the game.

// typedefs used from: https://github.com/doitsujin/dxvk/blob/master/src/d3d11/d3d11_main.cpp
typedef HRESULT(D3D11CoreCreateDev)(void* fact, void* adapt, unsigned int flag, void* fl, unsigned int featureLevels, void** ppDev);
DLLExport HRESULT D3D11CoreCreateDevice(void* fact, void* adapt, unsigned int flag, void* fl, unsigned int featureLevels, void** ppDev) {
    return ((D3D11CoreCreateDev*)p[0]) (fact, adapt, flag, fl, featureLevels, ppDev);
}

typedef HRESULT(D3D11CreateDev)(void* adapt, unsigned int dt, void* soft, unsigned int flags, int* ft, unsigned int fl, unsigned int ver, void** ppDevice, void* featureLevel, void** context);
DLLExport HRESULT D3D11CreateDevice(void* adapt, unsigned int dt, void* soft, unsigned int flags, int* ft, unsigned int fl, unsigned int ver, void** ppDevice, void* featureLevel, void** context) {
    return ( (D3D11CreateDev*)p[1] ) (adapt, dt, soft, flags, ft, fl, ver, ppDevice, featureLevel, context);
}

typedef HRESULT(D3D11CreateDevAndSwapChain)(void* adapt, unsigned int dt, void* soft, unsigned int flags, int* ft, unsigned int fl, unsigned int ver, void* swapChainDesc, void** swapChain, void** ppDevice, void* featureLevel, void** context);
DLLExport HRESULT D3D11CreateDeviceAndSwapChain(void* adapt, unsigned int dt, void* soft, unsigned int flags, int* ft, unsigned int fl, unsigned int ver, void* swapChainDesc, void** swapChain, void** ppDevice, void* featureLevel, void** context) {
    return ((D3D11CreateDevAndSwapChain*)p[2]) (adapt, dt, soft, flags, ft, fl, ver, swapChainDesc, swapChain, ppDevice, featureLevel, context);
}