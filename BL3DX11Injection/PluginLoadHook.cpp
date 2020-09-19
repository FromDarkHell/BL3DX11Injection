#include <set>
#include <fstream>
#include <tchar.h>
#include <list>
#include <vector>

#include "pch.h"
#include "INIReader.h" // https://github.com/jtilly/inih

#include "HookLib.h"
#include "PluginLoadHook.h"
#pragma comment(lib, "Zydis.lib")
#pragma comment(lib, "HookLib.lib")

std::wstring pluginsPath;
static std::wstring iniPath;
static std::vector<HMODULE> loadedModules;

#pragma region Hooks

#pragma region Plugin Freeing
using _ExitProcess = VOID(WINAPI*)(ULONG ExitCode);
_ExitProcess OriginalExitProcess = NULL;
VOID WINAPI ExitProcessHook(ULONG ExitCode)
{
    std::wcout << "Exiting Process...";

    Sleep(1000);

    // Now free all of our other libs that we loaded
    for (auto&& hMod : loadedModules) {
        FreeLibrary(hMod);
    }

    RemoveHook(OriginalExitProcess);
    ExitProcess(ExitCode);
}

#pragma endregion

#pragma region Plugin Loading
void LoadPlugins() {
    std::wcout << "Loading Plugins..." << std::endl;

    std::list<std::wstring> pluginsToLoad = {};

    std::fstream file;
    file.open(iniPath.c_str(), std::ios::out | std::ios::in | std::ios::app);
    if (!file) {
        file.open(iniPath.c_str(), std::ios::in || std::ios::out || std::ios::trunc);
        file << "[PluginLoader]\n";
        file.flush();
        file.close();
    }

    INIReader reader(iniPath.c_str());
    if (reader.ParseError() != 0) {
        std::wcout << "Unable to load 'pluginLoader.ini'" << std::endl;
    }

    WIN32_FIND_DATA fd; // This'll store our data about the plugin we're currently loading in.
    const HANDLE dllFile = FindFirstFile((pluginsPath + L"*.dll").c_str(), &fd); // Get the first DLL file in our plugins dir
    int dllCount = 0;

    if (dllFile == INVALID_HANDLE_VALUE) {
        std::wcout << "No Plugins Found..." << std::endl;
        return; // Just return now, no need to bother to execute the rest of the code
    }
    
    do {
        if ((!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))) {
            std::wstring pluginName = (std::wstring)fd.cFileName;
            std::string s(pluginName.begin(), pluginName.end());

            if (reader.Sections().count(s)) {
                float delayTime = reader.GetFloat(s, "delaySeconds", 0);
                std::wcout << "Waiting " << delayTime << " seconds to load " << WidenString(s) << std::endl;
                Sleep(delayTime * 1000);
            }


            std::wstring filePath = pluginsPath + (pluginName); // Generate our file path + the name of our plugin to load
            HMODULE hMod = LoadLibrary(filePath.c_str());
            if (hMod) {
                loadedModules.push_back(hMod);
                dllCount++;
            }
            else
                std::wcout << "Unable to load plugin: " << filePath << ": " << GetLastErrorAsString() << std::endl;
        }

    } while (FindNextFile(dllFile, &fd));
    
    FindClose(dllFile);
}

using _LoadLibrary = HMODULE(WINAPI*)(LPCWSTR lpLibFileName);
_LoadLibrary OriginalLoadLibrary = NULL;
HMODULE WINAPI LoadLibraryWHook(LPCWSTR lpLibFileName) {
    std::wcout << "Loading Library: " << std::wstring(lpLibFileName) << std::endl;

    if (std::wstring(lpLibFileName)._Equal(L"shcore.dll")) {
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)LoadPlugins, NULL, NULL, NULL);
        RemoveHook(OriginalLoadLibrary);
        return LoadLibrary(lpLibFileName);
    }

    return OriginalLoadLibrary(lpLibFileName);
}

#pragma endregion

#pragma endregion

void InitializePluginHooks(HMODULE gameModule) {
    WCHAR pluginsFilePath[513] = { 0 };
    GetModuleFileNameW(gameModule, pluginsFilePath, 512);
    std::wstring pPath = pluginsFilePath;
    pPath = pPath.substr(0, pPath.rfind('\\')) + L"\\Plugins\\";
    std::wcout << "Plugins Path: " << pPath << std::endl;

    // This'll hapen if we are magically unable to create the plugins dir.
    if (!CreateDirectory(pPath.c_str(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS) {
        std::wcout << "Unable to create plugins folder..." << std::endl;
        return;
    }
    pluginsPath = pPath;
    iniPath = pluginsPath + L"pluginLoader.ini";

    PVOID Target = GetProcAddress(GetModuleHandle(L"kernel32.dll"), "ExitProcess");
    
    if (SetHook(Target, ExitProcessHook, reinterpret_cast<PVOID*>(&OriginalExitProcess))) 
        std::wcout << "Initialized ExitProcess(...) hook" << std::endl;
    else 
        std::wcout << "Unable to initialize ExitProcess(...) hook" << std::endl;

    PVOID loadLibraryHook = GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryW");
    if (SetHook(loadLibraryHook, LoadLibraryWHook, reinterpret_cast<PVOID*>(&OriginalLoadLibrary)))
        std::wcout << "Initialized LoadLibraryW(...) hook..." << std::endl;
    else  
        std::wcout << "Unable to initialize LoadLibraryW(...) hook" << std::endl;

}
