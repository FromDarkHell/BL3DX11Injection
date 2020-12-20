#include "pch.h"
#include <filesystem>
#include "dllmain.h"
#include "PluginLoadHook.h"
#include "ThreadManager.hpp"
#pragma pack(1)

static HINSTANCE hL;
static HMODULE gameModule;

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

        LogString(L"Suspending all other threads...");

        // Suspend all other threads to prevent a giant race condition
        ThreadManager::Suspend();

        // Check if the file exists to just avoid constantly copying the file on launch
        std::string DX11OrgPath = FlattenString(GetModulePath()) + "\\d3d11_org.dll";
        std::filesystem::remove(FlattenString(GetModulePath()) + "\\PluginLoader.log");
        LogString(L"Checking for existence of " + WidenString(DX11OrgPath) + L"\n");

        // We copy the original DX11 dll from Sys32 in order to avoid weird version differences and crashes
        // The linker exports at the bottom let windows handle all of the DX11 proxy stuff after a `d3d11_org` exists in the root dir
        if (!std::filesystem::exists(DX11OrgPath)) {
            // Get the path to `C:\Windows\System32\d3d11.dll` (or whatever it is for whoever's using this)
            wchar_t DX11Path[MAX_PATH];
            GetSystemDirectory(DX11Path, MAX_PATH);
            wcscat_s(DX11Path, L"\\d3d11.dll");
            LogString(L"Copying original DX11 at " + std::wstring(DX11Path) + L" to " + WidenString(DX11OrgPath) + L"\n");

            // Copy the file from Sys32 over to the root directory
            std::filesystem::copy(DX11Path, DX11OrgPath);
        }

        LogString(L"Proxy DX11 Exists: " + std::wstring(std::filesystem::exists(DX11OrgPath) ? L"Yes" : L"No") + L"\n");

        gameModule = hModule;
        DisableThreadLibraryCalls(hModule);

        // Resume all other threads.
        LogString(L"Resuming all other threads...\n");
        ThreadManager::Resume();

        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)executionThread, NULL, NULL, NULL);
    }
    return TRUE;
}

int executionThread() {

    InitializePluginHooks(gameModule);
    return TRUE;

}


#pragma region Linker Exports
#pragma comment(linker, "/export:D3D11CoreCreateDevice=d3d11_org.D3D11CoreCreateDevice")
#pragma comment(linker, "/export:D3D11CreateDevice=d3d11_org.D3D11CreateDevice")
#pragma comment(linker, "/export:D3D11CreateDeviceAndSwapChain=d3d11_org.D3D11CreateDeviceAndSwapChain")
#pragma endregion
