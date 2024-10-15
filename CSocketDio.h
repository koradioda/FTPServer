#pragma once
#include <winsock2.h>
#include <string>
#pragma comment(lib,"ws2_32")
using namespace std;
class CSocketDio
{
public:
	SOCKET m_sock;
	CSocketDio(); 
	BOOL Create(int nPort=0, int sockMode=1, char* sIP=NULL); 
	// BOOL Create(int nPort, int sockMode, string& sIP);
	~CSocketDio();
	int Bind(const SOCKADDR* sAddr,int nLensAddr);
	int Listen(int backlogs=5);
	bool Accept(CSocketDio& socka, SOCKADDR* sAddr=NULL, int* nLensAddr=0);
	int Connect(const char* sIP, int nPort,const int netType=AF_INET);
	int GetPeerName(char* sIP, int& nPort);
	int GetSockName(char* sIP, int& nPort);
	int GetLastError();
	void Close();
	int Send(const void* lpBuf, int nBufLen, int nFlags = 0);
	int SendTo(const void* lpBuf, int nBufLen, const SOCKADDR* sAddr, int sAddrLen, int nFlags = 0);
	int Receive(void* lpBuf, int nBufLen, int nFlags = 0);
	int ReceiveFrom(void* lpBuf, int nBufLen, SOCKADDR* sAddr, int* sAddrLen,int nFlags = 0);

};

