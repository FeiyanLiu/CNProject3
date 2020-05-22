#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "udp_server.h"

char tmp[100];
std::string ret;

void Bind()
{
	int err;
	wVersionRequested = MAKEWORD(1, 1);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		std::cout << "´´½¨WSADATAÊ§°Ü\n";
		return;
	}

	if (LOBYTE(wsaData.wVersion) != 1 ||
		HIBYTE(wsaData.wVersion) != 1)
	{
		std::cout << "WSADATA°æ±¾´íÎó\n";
		WSACleanup();
		return;
	}

	localsock = socket(AF_INET, SOCK_DGRAM, 0);
	localaddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	localaddr.sin_family = AF_INET;
	localaddr.sin_port = htons(srcPORT);

	dstsock = socket(AF_INET, SOCK_DGRAM, 0);

	dstaddr.sin_addr.S_un.S_addr = inet_addr(dstIP);
	dstaddr.sin_family = AF_INET;
	dstaddr.sin_port = htons(dstPORT);

	bind(localsock, (SOCKADDR*)&localaddr, sizeof(SOCKADDR));
}

void Sendto(std::string message)
{
	std::cout << message << std::endl;
	sendto(localsock, message.c_str(), message.size() + 1, 0,
		(SOCKADDR*)&dstaddr, sizeof(SOCKADDR));
}

std::string Recvfrom()
{
	memset(tmp, 0, sizeof(tmp));
	
	recvfrom(localsock, tmp, 100, 0, (SOCKADDR*)&dstaddr, &len);
	if (tmp != NULL)
		std::cout << tmp << std::endl;
	ret = tmp;
	return ret;
}

void Close()
{
	closesocket(dstsock);
	closesocket(localsock);
	WSACleanup();
}