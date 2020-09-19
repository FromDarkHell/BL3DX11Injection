#pragma once

#include "include/WinSockHandlers.h"
#include "pch.h"

int (WINAPI* precv)(SOCKET socket, char* buffer, int length, int flags) = NULL;
int (WINAPI* psend)(SOCKET socket, const char* buffer, int length, int flags) = NULL;
int (WINAPI* pconn)(SOCKET socket, const sockaddr* name, int namelen) = NULL;

// Detour function which overrides MessageBoxW.
int WINAPI DetourRecv(SOCKET s, char* buffer, int length, int flags)
{	
	if (length > MINPACKETLEN) {
		struct sockaddr outAddr{};
		std::wstring ipAddress = WinSockHelpers::getIPAddress(s, outAddr);

		if (ipAddress._Equal(L"52.22.183.176") || ipAddress._Equal(L"54.209.121.132") || ipAddress._Equal(L"34.202.130.108") || ipAddress._Equal(L"52.0.2.43")) {
			std::wcout << "Receiving data from discovery.services.gearboxsoftware.com...\n";
			std::wcout << "[RECV-" << length << " : " << flags << "] DATA: ";
			for (int i = 0; i < length; i++) {
				printf("%02X ", (unsigned char)buffer[i]);
			}

		}
		else 
			std::wcout << "FROM: " << ipAddress << " :\n";

	}
	
	return precv(s, buffer, length, flags);
}

int WINAPI DetourSend(SOCKET s, char* buf, int len, int flags)
{
	std::wcout << "Sending data...\n";
	return psend(s, buf, len, flags);
}

int WINAPI DetourConnect(SOCKET s, sockaddr* name, int nameLen) {
	std::wstring ipAddress = WinSockHelpers::sockAddrToIP(*name);
	if (ipAddress._Equal(L"52.22.183.176")) 
		std::wcout << "Connecting to discovery.services.gearboxsoftware.com...\n";
	else 
		std::wcout << "Connecting to:  " << ipAddress << " :\n";

	return pconn(s, name, nameLen);
}

BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID) {
	switch (dwReason) {
	case DLL_PROCESS_ATTACH:

		std::wcout << "Initializing...\n";
		// Initialize MinHook.
		if (MH_Initialize() != MH_OK)
		{
			return 1;
		}
		
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0)
		{
			printf("WSAStartup() failed miserably! With error code %ld\n", WSAGetLastError());
			return 1;
		}
		else printf("WSAStartup() looks fine!\n");

		if (MH_CreateHookApi(L"Ws2_32", "recv", DetourRecv, (LPVOID*)&precv) != MH_OK)
		{
			std::wcout << "Unable to create recv hook...\n";
			return 1;
		}

		if (MH_CreateHookApi(L"Ws2_32", "connect", DetourConnect, (LPVOID*)&pconn) != MH_OK)
		{
			std::wcout << "Unable to create connect hook...\n";
			return 1;
		}

		/*
		if (MH_CreateHookApi(L"Ws2_32", "send", DetourSend, (LPVOID*)&psend) != MH_OK)
		{
			std::wcout << "Unable to create send hook...\n";
			return 1;
		}
		*/

		if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
		{
			std::wcout << "Unable to enable connect hook...\n";
			return 1;
		}
		
		break;
	}
	return 1;
}