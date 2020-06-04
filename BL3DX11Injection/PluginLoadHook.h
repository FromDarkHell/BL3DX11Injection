
#include <set>
#include <fstream>
#include <tchar.h>
#include <list>
#include <vector>

#include "pch.h"
#include "INIReader.h" // https://github.com/jtilly/inih

#include "HookLib.h"

VOID WINAPI ExitProcessHook(ULONG ExitCode);

void InitializePluginHooks(HMODULE gameModule);

void LoadPlugins();