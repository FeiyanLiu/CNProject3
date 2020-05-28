#pragma once

#include <bits/stdc++.h>
#include <Winsock2.h>
#include <stdio.h>
#pragma comment(lib,"ws2_32.lib")

unsigned short srcPORT = 34321;


#define dstPORT 54321
#define dstIP "127.0.0.1"

WORD wVersionRequested;
WSADATA wsaData;

SOCKET localsock;
SOCKADDR_IN localaddr;

SOCKET dstsock;
SOCKADDR_IN dstaddr;
TCHAR* CharToTCHAR(const char* pChar);
int len = sizeof(SOCKADDR);

void Bind();
void Sendto(std::string message);
std::string Recvfrom();
void Close();
