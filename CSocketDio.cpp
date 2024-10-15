#include "pch.h"
#include "CSocketDio.h"

CSocketDio::CSocketDio():m_sock(-1)
{

}

BOOL CSocketDio::Create(int nPort, int sockMode, char* sIP)
{
	m_sock = socket(AF_INET,sockMode,0);
	sockaddr_in saddr;
	ZeroMemory(&saddr, sizeof(saddr));
	saddr.sin_family = AF_INET;
	if(nPort)
		saddr.sin_port = htons(nPort);  // host port to net port
	// saddr.sin_addr.S_un.S_addr = inet_addr(sIP);   // ×ª»»ip  S_sun.S_addr == s_addr
	if (sIP)// sIP = "127.0.0.1";
		saddr.sin_addr.s_addr = inet_addr(sIP); //  Í¬ÉÏ
	int n = bind(m_sock, (sockaddr*)&saddr, sizeof(saddr));
	if (n == SOCKET_ERROR)
	{
		return 0;
	}
	
	return 1;
}

CSocketDio::~CSocketDio()
{
	closesocket(m_sock);
}

int CSocketDio::Bind(const SOCKADDR* sAddr, int nLensAddr)
{
	
	int n = bind(m_sock, sAddr, nLensAddr);
	if (n == SOCKET_ERROR)
		return SOCKET_ERROR;

	return 0;
}

int CSocketDio::Listen(int backlogs)
{
	int n = listen(m_sock, backlogs);
	if (n == SOCKET_ERROR) 
		return SOCKET_ERROR;

	return 0;
}

bool CSocketDio::Accept(CSocketDio& socka, SOCKADDR* sAddr, int* nLensAddr)
{
	socka.m_sock = accept(m_sock, sAddr, nLensAddr);
	if (socka.m_sock <= 0)
		return false;
	return true;
}

int CSocketDio::Connect(const char* sIP, int nPort, const int netType)
{
	sockaddr_in sAddr;
	sAddr.sin_port = htons(nPort);
	sAddr.sin_family = netType;
	sAddr.sin_addr.s_addr = inet_addr(sIP);

	int n = connect(m_sock, (SOCKADDR*)&sAddr, sizeof(sAddr));
	if (n == SOCKET_ERROR)
		return SOCKET_ERROR;
	return 0;
}

int CSocketDio::GetPeerName(char* sIP, int& nPort)
{
	sockaddr_in saddr;
	int nLen = sizeof(saddr);
	ZeroMemory(&saddr, sizeof(saddr));
	if (getpeername(m_sock, (sockaddr*)&saddr, &nLen) != 0)
	{
		return WSAGetLastError();
	}
	nPort = ntohs(saddr.sin_port);   // net to host 
	// saddr.sin_addr.S_un.S_un_b.s_b1
	char* ipAddressStr = inet_ntoa(*reinterpret_cast<in_addr*>(&saddr.sin_addr.s_addr));
	strcpy(sIP,ipAddressStr);

	return 0;
	
}

int CSocketDio::GetSockName(char* sIP, int& nPort)
{
	sockaddr_in saddr;
	int nLen = sizeof(saddr);
	ZeroMemory(&saddr, sizeof(saddr));
	if (getsockname(m_sock, (sockaddr*)&saddr, &nLen) != 0)
	{
		return WSAGetLastError();
	}
	nPort = ntohs(saddr.sin_port);   // net to host 
	char* ipAddressStr = inet_ntoa(*reinterpret_cast<in_addr*>(&saddr.sin_addr.s_addr));
	strcpy(sIP, ipAddressStr);

	return 0;
}

int CSocketDio::GetLastError()
{
	return WSAGetLastError();
}

void CSocketDio::Close()
{
	closesocket(m_sock);
}

int CSocketDio::Send(const void* lpBuf, int nBufLen, int nFlags)
{
	return send(m_sock, (const char*)lpBuf, nBufLen, nFlags);
}

int CSocketDio::SendTo(const void* lpBuf, int nBufLen, const SOCKADDR* sAddr, int sAddrLen, int nFlags)
{
	
	return sendto(m_sock, (const char*)lpBuf, nBufLen, nFlags, sAddr, sAddrLen);
}

int CSocketDio::Receive(void* lpBuf, int nBufLen, int nFlags)
{
	return recv(m_sock, (char*)lpBuf, nBufLen, nFlags);
}

int CSocketDio::ReceiveFrom(void* lpBuf, int nBufLen, SOCKADDR* sAddr, int* sAddrLen, int nFlags)
{
	return recvfrom(m_sock, (char*)lpBuf, nBufLen, nFlags, sAddr, sAddrLen);
}
