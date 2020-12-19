#include "pch.h"
#include "PluginLoadHook.h"

std::wstring pluginsPath;
static std::wstring iniPath;
static std::vector<HMODULE> loadedModules;

#pragma region Hooks

#pragma region Plugin Freeing
using _ExitProcess = VOID(WINAPI*)(ULONG ExitCode);
_ExitProcess OriginalExitProcess = NULL;
VOID WINAPI ExitProcessHook(ULONG ExitCode)
{
    LogString(L"Exiting Process...");

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

void PluginLoadFunction(std::wstring dll, long delay) {
   std::this_thread::sleep_for(std::chrono::milliseconds(delay));
   HMODULE hMod = LoadLibrary(dll.c_str());
   if (hMod) 
       loadedModules.push_back(hMod);
   else 
       LogString(L"Unable to load plugin: " + dll + L" : " + GetLastErrorAsString() + L"\n");
}

void LoadPlugins() {
    LogString(L"Loading Plugins...\n");

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
        LogString(L"Unable to load 'pluginLoader.ini'\n");
    }

    WIN32_FIND_DATA fd; // This'll store our data about the plugin we're currently loading in.
    const HANDLE dllFile = FindFirstFile((pluginsPath + L"*.dll").c_str(), &fd); // Get the first DLL file in our plugins dir
    int dllCount = 0;

    if (dllFile == INVALID_HANDLE_VALUE) {
        LogString(L"No Plugins Found...\n");
        return; // Just return now, no need to bother to execute the rest of the code
    }

    std::vector<std::thread> threads;

    do {
        if ((!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))) {
            std::wstring pluginName = (std::wstring)fd.cFileName;
            std::string s(pluginName.begin(), pluginName.end());

            std::wstring filePath = pluginsPath + (pluginName); // Generate our file path + the name of our plugin to load

            if (reader.Sections().count(s)) {
                float delayTime = reader.GetFloat(s, "delaySeconds", 0);
                LogString(L"Waiting " + std::to_wstring(delayTime) + L" seconds to load " + pluginName + L"\n");
                threads.push_back(std::thread(PluginLoadFunction, filePath, (long) (delayTime * 1000)));
            }
            else {
                HMODULE hMod = LoadLibrary(filePath.c_str());
                if (hMod)
                    loadedModules.push_back(hMod);
                else
                    LogString(L"Unable to load plugin: " + filePath + L" : " + GetLastErrorAsString() + L"\n");
            }
        }

    } while (FindNextFile(dllFile, &fd));
    FindClose(dllFile);

    for (auto& t : threads) {
        if (t.joinable()) 
            t.join();
    }

    // Add an extra new line just in case it all gets messed up with the whole multithreading
    LogString(L"\n");
}

using _LoadLibrary = HMODULE(WINAPI*)(LPCWSTR lpLibFileName);
_LoadLibrary OriginalLoadLibrary = NULL;
HMODULE WINAPI LoadLibraryWHook(LPCWSTR lpLibFileName) {
    LogString(L"Loading Library: " + std::wstring(lpLibFileName) + L"\n");

    if (std::wstring(lpLibFileName).find(L"APEX_Clothing_x64") != std::string::npos) {
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
    LogString(L"Plugins Path: " + pPath + L"\n");

    // This'll hapen if we are magically unable to create the plugins dir.
    if (!CreateDirectory(pPath.c_str(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS) {
        LogString(L"Unable to create plugins folder...\n");
        return;
    }
    pluginsPath = pPath;
    iniPath = pluginsPath + L"pluginLoader.ini";

    if (SetHook(&ExitProcess, ExitProcessHook, reinterpret_cast<PVOID*>(&OriginalExitProcess))) 
        LogString(L"Initialized ExitProcess(...) hook\n");
    else 
        LogString(L"Unable to initialize ExitProcess(...) hook\n");

    if (SetHook(&LoadLibraryW, LoadLibraryWHook, reinterpret_cast<PVOID*>(&OriginalLoadLibrary)))
        LogString(L"Initialized LoadLibraryW(...) hook...\n");
    else  
        LogString(L"Unable to initialize LoadLibraryW(...) hook\n");
}
