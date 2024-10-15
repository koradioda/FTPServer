#pragma once
#include <thread>
#include <string>
#include <vector>
#include <afxsock.h>
#include <functional>
#include "CSocketDio.h"
using namespace std;

class CFtpRun
{
	enum {
		USER, PASS, OPTS, LIST, CWD, CHDIR, PASV, PORT, NLST,
		XPWD, PWD, TYPE, SYST, MKD, AUTH, FEAT, CDUP, RNFR, RNTO,
		STOR, DELE, RETR, SIZE,NOOP
	};
	vector<string> DecodeDio = { "USER", "PASS", "OPTS", "LIST", "CWD", "CHDI",
		"PASV","PORT","NLST","XPWD","PWD","TYPE","SYST","MKD","AUTH","FEAT",
		"CDUP","RNFR","RNTO" ,"STOR","DELE","RETR","SIZE","NOOP"
	};
	vector<string> Mons = {"Mon","Feb","Mar","Apr","May","Jun",
			"Jul","Aug","Sep","Oct","Nov","Dec"
	};

	string m_path;
	string cPath;
	string root;  // m_path µÄÓ³Éä ¸ô¶Ï·ûºÅ
	char codeMode;
	string oldName;
	string m_user;
	string m_pass;

public:
	CSocketDio m_sock;
	CSocketDio dSock;
	CSocketDio dlSock;

	CFtpRun();
	~CFtpRun();
	void run();
	void stop();
	int strtoint(std::string str);
	void SetPath(const char* path);

	int Menu(CSocketDio* pSock);

	int CheckUser(CSocketDio* pSock, string& str);
	int CheckPass(CSocketDio* pSock, string& str);
	int CmdList(CSocketDio* pSock, string& str);
	int CmdOpts(CSocketDio* pSock, string& str);
	bool CheckisDir(const string& str);
	int CmdCwd(CSocketDio* pSock, string& str);
	int CmdXpwd(CSocketDio* pSock, string& str);
	int CmdChdir(CSocketDio* pSock, string& str);
	int CmdPasv(CSocketDio* pSock, string& str);
	int CmdPort(CSocketDio* pSock, string& str);
	int CmdNlst(CSocketDio* pSock, string& str);
	int CmdType(CSocketDio* pSock, string& str);
	int CmdSyst(CSocketDio* pSock, string& str);
	int CmdMkd(CSocketDio* pSock, string& str);
	int CmdAuth(CSocketDio* pSock, string& str);
	int CmdFeat(CSocketDio* pSock, string& str);
	int CmdRnfr(CSocketDio* pSock, string& str);
	int CmdRnto(CSocketDio* pSock, string& str);
	int CmdStor(CSocketDio* pSock, string& str);
	int CmdDele(CSocketDio* pSock, string& str);
	int CmdRetr(CSocketDio* pSock, string& str);
	int CmdSize(CSocketDio* pSock, string& str);
	int CmdNoop(CSocketDio* pSock, string& str);

	void getIpPort(string str,string& sIP,int& nPort);
	string AnsiToUtf8(const std::string& ansiString);
	char* AnsiToUtf8(const char* ansiString);
	string GetCmd(const char* cmd);
	bool RecursiveDel(string &fileName);

	void SetUserName(const char* user);
	void SetUserPwd(const char*  pass);

};

