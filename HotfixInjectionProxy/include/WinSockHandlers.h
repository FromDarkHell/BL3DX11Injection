#pragma once

#include <winsock2.h>
#include <string>
#include <Windows.h>
#include "MinHook.h"
#include <iostream>
#include <WS2tcpip.h>
#include <windns.h>

#pragma comment(lib, "Dnsapi.lib")
#pragma comment(lib, "ws2_32.lib")

#define MINPACKETLEN 5

#pragma region Arbitrarily Complex Functions

namespace WinSockHelpers {
	const char* GetCanonName(SOCKET s, sockaddr in) {

		if (in.sa_family != AF_INET) { return "UNKNOWN"; }

		struct sockaddr_in* sin = (sockaddr_in*)&in;

		int portNumber = 0;
		portNumber = ntohs(sin->sin_port);

		char ipStr[INET_ADDRSTRLEN];
		inet_ntop(sin->sin_family, (void*)(&(sin->sin_addr)), (PSTR)ipStr, sizeof(ipStr));

		PCSTR port = std::to_string(portNumber).c_str();

		// https://docs.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-getaddrinfo
		// https://www.tenouk.com/Winsock/Winsock2example8.html

		// TODO: Maybe use some sort of DNS lookup type thingy

		/*
		// Now we actually get the canonical name from the IP Address & Port #
		struct addrinfo aiHints;
		struct addrinfo* aiList = NULL;
		memset(&aiHints, 0, sizeof(aiHints));
		aiHints.ai_family = AF_INET;
		aiHints.ai_socktype = SOCK_STREAM;
		aiHints.ai_protocol = IPPROTO_TCP;
		aiHints.ai_flags |= AI_CANONNAME;
		int retVal;

		if ((retVal = getaddrinfo(ipStr, port, &aiHints, &aiList)) != 0) {
			printf("getaddrinfo() failed.\n");
			return "UNKNOWN";
		}
		*/

		return "";
	}

	std::wstring sockAddrToIP(struct sockaddr outAddr) {
		char ipStr[INET6_ADDRSTRLEN];
		void* addr;
		const char* ipVer;
		if (outAddr.sa_family == AF_INET) {
			struct sockaddr_in* ipv4 = ((struct sockaddr_in*)(&outAddr));
			addr = &(ipv4->sin_addr);
			ipVer = "IPv4";
		}
		else {
			// IPv6
			struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)(&outAddr);
			addr = &(ipv6->sin6_addr);
			ipVer = "IPv6";
		}
		inet_ntop(outAddr.sa_family, addr, (PSTR)ipStr, sizeof(ipStr));
		std::string z = std::string(ipStr);
		return std::wstring(z.begin(), z.end());
	}

	std::wstring getIPAddress(SOCKET s, struct sockaddr outAddr) {
		int ilen = sizeof(outAddr);
		if (getpeername(s, &outAddr, &ilen) != 0) {
			std::wcout << "Unable to get sock name... Error Code: " << WSAGetLastError() << std::endl;
			return L"";
		}
		return sockAddrToIP(outAddr);
	}
}

#pragma endregion