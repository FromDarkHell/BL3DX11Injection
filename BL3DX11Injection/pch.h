#ifndef PCH_H
#define PCH_H

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <string>

std::wstring GetLastErrorAsString();
std::wstring WidenString(const std::string& s);
std::wstring GetModulePath();

#endif //PCH_H
