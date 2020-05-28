#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "udp_client.h"
char* TCHARToChar(const TCHAR* pTchar)
{
	char* pChar = NULL;
#ifdef _UNICODE
	int nLen = wcslen(pTchar) + 1;
	pChar = new char[nLen * 2];
	WideCharToMultiByte(CP_ACP, 0, pTchar, nLen, pChar, 2 * nLen, NULL, NULL);
#else
	int nLen = strlen(pTchar) + 1;
	pChar = new char[nLen];
	memcpy(pChar, pTchar, nLen * sizeof(char));
#endif
	return pChar;
}
char tmp[100];
void Bind()
{

        TCHAR s[10];
	InputBox(s, 10, _T("请输入端口号"));
	sscanf_s(TCHARToChar(s), "%d", &srcPORT);
	//std::cout << "请输入端口：" ;
	//std::cin >> srcPORT;
	int err;
	wVersionRequested = MAKEWORD(1, 1);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		std::cout << "创建WSADATA失败\n";
		return;
	}

	if (LOBYTE(wsaData.wVersion) != 1 ||
		HIBYTE(wsaData.wVersion) != 1)
	{
		std::cout << "WSADATA版本错误\n";
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
	
	sendto(localsock, message.c_str(), message.size() + 1, 0,
		(SOCKADDR*)&dstaddr, sizeof(SOCKADDR));
}

std::string Recvfrom()
{
	std::string ret;
	memset(tmp, 0, sizeof(tmp));
	for (int i = 0; i < 300000; i++)
	{
		recvfrom(localsock, tmp, 100, 0, (SOCKADDR*)&dstaddr, &len);
		if (tmp) break;
	}
	
	ret = tmp;
	if(ret.size() > 0) return ret;
	return "ERR";
}

void Close()
{
	closesocket(dstsock);
	closesocket(localsock);
	WSACleanup();
}
