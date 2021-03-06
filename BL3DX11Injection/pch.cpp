#include "pch.h"

#pragma warning(disable: 6031)

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
        std::wcout << "Error obtaining module path: " << GetLastErrorAsString() << std::endl;
        return L"C:";
    }


    std::wstring str(buffer);
    std::wstring::size_type pos = str.find_last_of(L"\\/");
    if (pos != std::string::npos) {
        std::wstring out = std::wstring(buffer).substr(0, pos);
        return out;
    }
    return L"C:";
}

void InitializeConsole() {
    SetConsoleTitle(L"Borderlands 3 Plugin Loader");

    // All of this is necessary so that way we can properly use the output of the console
    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stderr);
    freopen("CONOUT$", "w", stdout);
    HANDLE hStdout = CreateFile(L"CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    HANDLE hStdin = CreateFile(L"CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    // Make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog point to console as well
    std::ios::sync_with_stdio(true);

    SetStdHandle(STD_INPUT_HANDLE, hStdin);
    SetStdHandle(STD_OUTPUT_HANDLE, hStdout); // Set our STD handles
    SetStdHandle(STD_ERROR_HANDLE, hStdout); // stderr is going back to STDOUT

    // Clear the error states for all of the C++ stream objects.
    // Attempting to access the streams before they're valid causes them to enter an error state.
    std::wcout.clear();
    std::cout.clear();
    std::wcerr.clear();
    std::cerr.clear();
    std::wcin.clear();
    std::cin.clear();
}

void LogString(const std::wstring& str) {
    std::wcout << str;

    std::fstream logFile;
    logFile.open(FlattenString(GetModulePath()) + "\\PluginLoader.log", std::ios::app);
    logFile << FlattenString(str);
    logFile.close();
}