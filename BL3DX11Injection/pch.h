#ifndef PCH_H
#define PCH_H

#define WIN32_LEAN_AND_MEAN

#define DLLImport   __declspec( dllimport ) 
#define DLLExport  extern "C" __declspec( dllexport )

#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <string>

#include <fstream>

std::wstring GetLastErrorAsString();
std::wstring WidenString(const std::string& s);
std::string FlattenString(const std::wstring& s);

std::wstring GetModulePath();

void InitializeConsole();

void LogString(const std::wstring& str);

#endif //PCH_H
