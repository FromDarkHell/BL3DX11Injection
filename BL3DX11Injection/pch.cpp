#include "pch.h"


std::wstring GetLastErrorAsString()
{
    //Get the error message, if any.
    DWORD errorMessageID = ::GetLastError();
    if (errorMessageID == 0)
        return std::wstring(); // Not any errors

    LPWSTR messageBuffer = nullptr;
    size_t size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&messageBuffer, 0, NULL);

    std::wstring message(messageBuffer, size);

    //Free the buffer.
    LocalFree(messageBuffer);

    return message;
}

std::wstring WidenString(const std::string& s) {
    return std::wstring(s.begin(), s.end());
}

std::string FlattenString(const std::wstring& s) {
    return std::string(s.begin(), s.end());
}

std::wstring GetModulePath() {
    TCHAR buffer[MAX_PATH] = { 0 };
    int val = GetModuleFileName(NULL, buffer, MAX_PATH);
    if (val == 0) {
        // TODO: implement a stronger fallback method for this
        return L"C:";
    }


    std::wstring str(buffer);
    std::wstring::size_type pos = str.find_last_of(L"\\/");
    if (pos != std::string::npos) {
        std::wstring out = std::wstring(buffer).substr(0, pos);
        return out;
    }
    else {
        return L"C:";
    }
}

void LogString(const std::wstring& str) {
    std::wcout << str;

    std::fstream logFile;
    logFile.open(FlattenString(GetModulePath()) + "\\PluginLoader.log", std::ios::app);
    logFile << FlattenString(str);
    logFile.close();
}