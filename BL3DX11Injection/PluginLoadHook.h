#include <set>
#include <list>
#include <vector>
#include "INIReader.h" // https://github.com/jtilly/inih
#include "HookLib.h"
#include <thread>
#pragma comment(lib, "Zydis.lib")
#pragma comment(lib, "HookLib.lib")


VOID WINAPI ExitProcessHook(ULONG ExitCode);
void PluginLoadFunction(std::wstring dll, long delay);
void InitializePluginHooks(HMODULE gameModule);
void LoadPlugins();