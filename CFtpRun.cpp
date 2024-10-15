#include "pch.h"
#include "CFtpRun.h"
#include <fcntl.h>
#include <io.h>
#include <tchar.h>
#define _u8(x) AnsiToUtf8(x) 


CFtpRun::CFtpRun() :m_path("")
{
	m_sock.Close();
	root = "/";  // 根目录 m_path的映射
	cPath = "";
	codeMode = 'A';
}

CFtpRun::~CFtpRun()
{
	stop();
	// delete this;
}

int aadd(int a, int b)
{
	return a + b;
}

void CFtpRun::stop()
{
	m_sock.Close();
}

int CFtpRun::strtoint(std::string str)
{
	for (int i=0;i<str.length();++i)
	{
		if (str[i] >= 'a' && str[i] <= 'z')
			str[i] = str[i] - 'a' + 'A';
	}
	for (int i = 0; i < DecodeDio.size(); ++i)
	{
		if (str == DecodeDio[i])
			return i;
	}
	return -1;
}

void CFtpRun::run()
{
	// 发送 欢迎 信息
	string sbuf = "220 FTPServerDio.\r\n";
	m_sock.Send(sbuf.c_str(), sbuf.length());   // for windows send once
	//sbuf = "200 Aready for Login.\r\n";
	//m_sock.Send(sbuf.c_str(), sbuf.length());  // 准备信息

	while (Menu(&m_sock) >= 0)
	{
		;
	}

}

int CFtpRun::Menu(CSocketDio* pSock)
{
	char rBuf[512] = { };
	int n = pSock->Receive(rBuf, sizeof(rBuf));  // 接受命令
	if (n <= 0)
		return -1;
	rBuf[n] = '\0';

	string recvStr(rBuf);			// 解析命令
	string token = GetCmd(rBuf);
	// auto token = strtok(rBuf, " ");
	
	switch (strtoint(token))
	{
	case USER:	return CheckUser(pSock,recvStr);
	case PASS:	return CheckPass(pSock, recvStr);
	case OPTS:	return CmdOpts(pSock, recvStr);
	case LIST:	return CmdList(pSock, recvStr);
	case CDUP:	recvStr = "CWD ..\r\n";
	case CWD:	return CmdCwd(pSock, recvStr);
	case CHDIR:	return CmdChdir(pSock, recvStr);
	case PASV:	return CmdPasv(pSock, recvStr);
	case PORT:	return CmdPort(pSock, recvStr);
	case NLST:	return CmdNlst(pSock, recvStr);
	case PWD:	
	case XPWD:	return CmdXpwd(pSock, recvStr);
	case TYPE:	return CmdType(pSock, recvStr);
	case SYST:	return CmdSyst(pSock, recvStr);
	case MKD:	return CmdMkd(pSock, recvStr);
	case AUTH:	return CmdAuth(pSock, recvStr);
	case FEAT:	return CmdFeat(pSock, recvStr);
	case RNFR:	return CmdRnfr(pSock, recvStr);
	case RNTO:	return CmdRnto(pSock, recvStr);
	case STOR:	return CmdStor(pSock, recvStr);
	case RMD:	CmdRmd(pSock, recvStr);	// cPath// 删除目录的他会进入目录
	case DELE:	return CmdDele(pSock, recvStr);
	case RETR:	return CmdRetr(pSock, recvStr);
	case SIZE:	return CmdSize(pSock, recvStr);
	case NOOP:	return CmdNoop(pSock, recvStr);
	default:
		break;
	}
	Sleep(16);
	return -1;
}

int CFtpRun::CheckUser(CSocketDio* pSock, string& str)
{   // str "USER xxx\r\n"
	
	string rbuf = GetData(str);  // 截取username
	if (rbuf == m_user)
	{ // "331 OK,need passwd!"
		rbuf = "331 OK, Need passwd!\r\n";
		pSock->Send(rbuf.c_str(), rbuf.length());

		return 1;
	}
	else
	{
		rbuf = "501 Unknow User\r\n";
		pSock->Send(rbuf.c_str(), rbuf.length());
		return -1;
	}
	return 0;
}

int CFtpRun::CheckPass(CSocketDio* pSock, string& str)
{// 230 
	
	string rbuf = GetData(str);  // 截取password
	if (rbuf == m_pass)
	{ // "331 OK,need passwd!"
		rbuf = "230 Uer login, can continue!\r\n";
		pSock->Send(rbuf.c_str(), rbuf.length());
		return 1;
	}
	else
	{
		rbuf = "501 error Passwd!\r\n";
		pSock->Send(rbuf.c_str(), rbuf.length());
		return 0;
	}
	return 0;
}

int CFtpRun::CmdList(CSocketDio* pSock, string& str)
{
	string localPath = m_path + root + cPath + "/*" ;
	_finddata_t fileInfo;
	intptr_t fhandle = _findfirst(localPath.c_str(), &fileInfo);  // 获取文件信息并返回下一个句柄
	if (fhandle == -1)
	{
		char sbuf[128] = "550 Error no such file or dir.\r\n";
		return pSock->Send(sbuf,sizeof(sbuf));
	}

	string sbuf = "150 Open data connection for transfer data.\r\n";
	pSock->Send(sbuf.c_str(), sbuf.length());

	do{
		// 获取文件的所有属性   
		string str;   // such as "-rw-r--r-- 1 user group 12345 Oct 14 10:00 file.txt"
		str = fileInfo.attrib & _A_SUBDIR ? "d" : "-";
		str += "rwxrwxrwx 1 ";
		str += "dio ";
		str += "nogroup ";
		str += to_string(fileInfo.size);
		char ctm[64] = {};
		tm* stm = gmtime(&fileInfo.time_create);
		sprintf(ctm, " %s %d %d:%d ", 
			Mons[stm->tm_mon].c_str(), stm->tm_mday, stm->tm_hour, stm->tm_min);
		str += ctm;
		str += fileInfo.name ;
		str += "\r\n";
		// if(codeMode=='I')
			//str = stringToBinary(str);
		//str = _u8(str);
		dSock.Send(str.c_str(),str.length());
		
	}while (_findnext(fhandle,&fileInfo)!=-1);
	_findclose(fhandle);

	sbuf = "226 Close data connection, transfer complemented.\r\n";
	pSock->Send(sbuf.c_str(), sbuf.length());
	dSock.Close();
	dlSock.Close();   // 需要关闭数据连接才能让客户端结束接受信息，然后处理目录信息
	return 0;
}

int CFtpRun::CmdOpts(CSocketDio* pSock, string& str)
{
	string sbuf = "502 opts command can't imcomplement.\r\n";
	pSock->Send(sbuf.c_str(), sbuf.length());

	return 0;
}

bool CFtpRun::CheckisDir(const string& strDir)
{
	WIN32_FIND_DATA fileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	// 获取当前目录内容
	string path = cPath + "/*";
	hFind = FindFirstFile(bstr_t(path.c_str()), &fileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		return false;
	}

	do {
		// 格式化目录内容为FTP LIST格式
		string fileName = bstr_t(fileData.cFileName);
		if (strDir == fileName && FILE_ATTRIBUTE_DIRECTORY&fileData.dwFileAttributes)
			return true;

	} while (FindNextFile(hFind, &fileData) != 0);

	FindClose(hFind);
	return true;
}

int CFtpRun::CmdCwd(CSocketDio* pSock, string& str)
{   // str == "CWD filedir\r\n"
	
	string tdir = GetData(str);  // dir name
	// string u8tdir = AnsiToUtf8(tdir);
	string sbuf;
	// 0. cd /  
	if ("\\" == tdir || "/"==tdir)
	{
		sbuf = "250 CWD Successful. Dir is \"";
		string localPath =root;
		sbuf = sbuf + localPath + "\".\r\n";
		return pSock->Send(sbuf.c_str(), sbuf.length());

	}else if (".." == tdir) { // 1. cd ..
		if (cPath.empty()) {
			sbuf = "501 Failed to change directory.\r\n";
			return pSock->Send(sbuf.c_str(), sbuf.length());
		}
		size_t lastSep = cPath.rfind('/');  // 目录c:\aa\bbb的最有一个 分隔符"\"
		if (std::string::npos != lastSep)
			cPath.replace(cPath.begin() + lastSep, cPath.end(), "");   // parent dir
		else
			cPath = "";
		string localPath = root + cPath;   // full path
		sbuf = "250 CWD Successful. Dir is \"";
		sbuf = sbuf + localPath + "\".\r\n";
		pSock->Send(sbuf.c_str(),sbuf.length());
		return 0;

	}else if ((tdir[0]=='/' || tdir[0] == '\\') && (tdir[1]!='/' || tdir[1]!='\\')) {  // /root/user/..
		// 目录扩展;
		string localPath = tdir;
		cPath = localPath.substr(1);  // 截取目录后面部分
		sbuf = "250 CWD Successful. Dir is \"";
		sbuf +=  localPath + "\".\r\n";
		pSock->Send(sbuf.c_str(), sbuf.length());
	} else  {
		// 判断是否有 ./  有就去除。
		string prefix = tdir.substr(0, 2);
		if (prefix == "./")
			tdir = tdir.substr(2);

		string fullPath = m_path + root + cPath + "/" + tdir;   // 绝对路径  tdir == aaa/aaa/...

		DWORD attributes = GetFileAttributesA(fullPath.c_str());
		if (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY)) {
			if (cPath.empty())
				cPath = tdir;
			else
				cPath = cPath + "/" + tdir;
			string loaclPath = root + cPath;
			sbuf = "250 CWD Successful Set To \"";
			sbuf +=  loaclPath + "\".\r\n";
			pSock->Send(sbuf.c_str(), sbuf.length());
		}
		else
		{
			sbuf = "501 Failed to change directory.\r\n";
			pSock->Send(sbuf.c_str(), sbuf.length());
			return 0;
		}

	}
	
	return 0;
}

int CFtpRun::CmdXpwd(CSocketDio* pSock, string& str)
{
	string sbuf = "257 ";
	sbuf += "\"" + root+ cPath + "\"";
	sbuf += "\r\n";  // 初始发送根目录 
	pSock->Send(sbuf.c_str(), sbuf.length());
	return 0;
}

int CFtpRun::CmdChdir(CSocketDio* pSock, string& str)
{
	return 0;
}

int CFtpRun::CmdPasv(CSocketDio* pSock, string& str)
{
	dlSock.Close();
	if (!dlSock.Create())
	{
		char sbuf[128] = "425 Can't open data connection.\r\n";   // 套接字创建失败
		return pSock->Send(sbuf,sizeof(sbuf));
	}
	char sIP[128] = {};
	int nPort;
	pSock->GetSockName(sIP, nPort);  // 获取对方连接时用的ip
	int n = strlen(sIP);
	for (int i = 0; i < n; ++i)
		if (sIP[i] == '.') sIP[i] = ',';
	string ips = sIP;
	string rbuf = "227 Entry Passive Mode ("; // 
	rbuf += ips + ',';	//(192,168,31,103,

	dlSock.GetSockName(sIP,nPort);  // 获取dListen的监听端口
	rbuf += to_string(HIBYTE(nPort));  
	rbuf+=","+ to_string(LOBYTE(nPort)) + ")\r\n";

	pSock->Send(rbuf.c_str(), rbuf.length());

	dlSock.Listen();
	dlSock.Accept(dSock);  // 等待连接

	return 1;
}

int CFtpRun::CmdPort(CSocketDio* pSock, string& str)
{  // str PORT ....(127,0,0,1,12,34);格式

	string sbuf = "200 PORT command successful.\r\n";
	pSock->Send(sbuf.c_str(), sbuf.length());    // PORT 命令响应

	string sIP;
	int nPort;
	getIpPort(str, sIP, nPort);  // 获取IP和端口
	dSock.Close();
	dSock.Create();
	int n = dSock.Connect(sIP.c_str(), nPort);
	if (n < 0)
		return -1;

	//char rbuf[256]={};
	//pSock->Receive(rbuf, sizeof(rbuf));   // NLST

	//sbuf = "150 Using transfer connection!\r\n";
	//pSock->Send(sbuf.c_str(), sbuf.length());
	//// 发数据
	//CmdNlst(pSock,str);

	//sbuf ="226 Closing transferconnection!\r\n";
	//pSock->Send(sbuf.c_str(), sbuf.length());
	//dSock.Close();

	return 0;
}

int CFtpRun::CmdNlst(CSocketDio* pSock, string& str)
{
	WIN32_FIND_DATA fileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	// 获取当前目录内容
	string localPath = m_path + root + cPath + "/*";
	hFind = FindFirstFile(bstr_t(localPath.c_str()), &fileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		return -1;
	}
	string sbuf = "150 Using transfer connection!\r\n";
	pSock->Send(sbuf.c_str(), sbuf.length());

	do {// 格式化目录内容为FTP LIST格式
		// way 1.
		string line;
		if(fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)  // 是目录的话
			line = "【"+bstr_t(fileData.cFileName)+"】";
		else
			line = bstr_t(fileData.cFileName);
		line += "\r\n";
		// line = _u8(line);
		dSock.Send(line.c_str(), line.length());
		
	} while (FindNextFile(hFind, &fileData) != 0);
	FindClose(hFind);

	sbuf = "\r\n";
	dSock.Send(sbuf.c_str(), sbuf.length()); // 发送换行。
	
	sbuf = "226 Closing transferconnection!\r\n";
	pSock->Send(sbuf.c_str(), sbuf.length());
	dSock.Close();

	return 0;
}

int CFtpRun::CmdType(CSocketDio* pSock, string& str)
{
	size_t index = str.find(' ');
	if (str[index + 1] == 'A') {
		codeMode = 'A';
		string sbuf = "200 Type set to ASCII.\r\n";
		pSock->Send(sbuf.c_str(), sbuf.length());
	}else if (str[index + 1] == 'I') {
		codeMode = 'I';
		string sbuf = "200 Type set to BINARY.\r\n";
		pSock->Send(sbuf.c_str(), sbuf.length());
	}else {
		string sbuf = "502 Type implement\r\n";
		pSock->Send(sbuf.c_str(), sbuf.length());
	}
	
	return 0;
}

int CFtpRun::CmdSyst(CSocketDio* pSock, string& str)
{
	string sbuf = "215 Windows_NT\r\n";
	pSock->Send(sbuf.c_str(), sbuf.length());
	return 0;
}

int CFtpRun::CmdMkd(CSocketDio* pSock, string& str)
{
	string dirName = GetData(str);  //   获取文件名字
	dirName = m_path + root + cPath + "/" + dirName;
	
	int result = _tmkdir(bstr_t(dirName.c_str()));   // 创建目录
	if (result == 0) {
		string sbuf = "257 Directory created.\r\n";
		return pSock->Send(sbuf.c_str(),sbuf.length());
	}
	else {
		string sbuf = "550 Error! Abnormal dir name or Permission denied.\r\n";
		return pSock->Send(sbuf.c_str(), sbuf.length());
	}

	return 0;
}

int CFtpRun::CmdAuth(CSocketDio* pSock, string& str)
{
	char sbuf[128] = "502 Command AUTH not implemented\r\n";
	pSock->Send(sbuf, sizeof(sbuf));
	return 0;
}

int CFtpRun::CmdFeat(CSocketDio* pSock, string& str)
{
	char sbuf[128] = "502 Command FEAT not implemented\r\n";
	pSock->Send(sbuf, sizeof(sbuf));
	return 0;
}

int CFtpRun::CmdRnfr(CSocketDio* pSock, string& str)
{
	// 是在当前目录下修改文件
	
	oldName = GetData(str);;  // RNFR xxxx\r\n;
	string sbuf = "350 Ready for RNTO\r\n";
	pSock->Send(sbuf.c_str(),sbuf.length());

	return 0;
}

int CFtpRun::CmdRnto(CSocketDio* pSock, string& str)
{
	string newName = GetData(str);  // RNTO xxxx\r\n;
	string FullOldName = m_path + root + cPath  + oldName;  // cPath-->aa/aa/..
	string FullNewName = m_path + root + cPath  + newName;
	int ret = rename(FullOldName.c_str(), FullNewName.c_str());

	if (!ret) {
		string sbuf = "250 Rename successful.\r\n";
		return pSock->Send(sbuf.c_str(),sbuf.length());
	}else {
		string sbuf = "550 Rename Faild.\r\n";
		return pSock->Send(sbuf.c_str(), sbuf.length());
	}

	return 0;
}

int CFtpRun::CmdStor(CSocketDio* pSock, string& str)
{
	// 获取文件名并设置完整路径。
	// size_t nStart = str.find(' ');
	string fileName = GetData(str);  // part name
	fileName = m_path + root + cPath + fileName;  // full name
	FILE* fp = fopen(fileName.c_str(),"wb");	// 二进制写入
	if (!fp)
	{
		string sbuf = "450 File can't create.\r\n";
		dSock.Close();
		return pSock->Send(sbuf.c_str(), sbuf.length());
	}
	//string sbuf = "125 Data connection opened\r\n";
	//pSock->Send(sbuf.c_str(),sbuf.length());
	string sbuf = "150 Ready for transfer data.\r\n";
	pSock->Send(sbuf.c_str(), sbuf.length());
	
	char rbuf[1024] = {};
	size_t n = 0; 
	while ((n=dSock.Receive(rbuf, sizeof(rbuf) - 1))>0) {// 接受上传
		rbuf[n] = '\0';
		// 写入文件
		n = fwrite(rbuf,1,n,fp);   // 收到多少写入多少。
		if (n < 0) {   // 传输错误
			string sbuf = "450 Some error in transfer.\r\n";
			fclose(fp);
			dSock.Close();
			return pSock->Send(sbuf.c_str(), sbuf.length());
		}
	}

	sbuf = "226 Close data connection ,STOR successfull.\r\n";
	pSock->Send(sbuf.c_str(), sbuf.length());
	dSock.Close();
	fclose(fp);
	// 文件接受
	return 0;
}

int CFtpRun::CmdRmd(CSocketDio* pSock, string& str)
{
	//size_t lastSep = cPath.rfind('/');  // 目录c:\aa\bbb的最有一个 分隔符"\"
	//if (std::string::npos != lastSep)
	//	cPath.replace(cPath.begin() + lastSep, cPath.end(), "");   // parent dir
	//else
	//	cPath = "";

	return 0;
}

int CFtpRun::CmdDele(CSocketDio* pSock, string& str)
{
	string fileName = GetData(str); //  获取要删除的文件名字。
	if(cPath.empty())
		fileName = m_path + root + cPath + fileName; // full name
	else
		fileName = m_path + root + cPath + "/" + fileName; // full name

	// _trmdir 只能删除空目录
	if (RecursiveDel(fileName))   // 递归删除
	{
		string sbuf = "250 File deleted successfully.\r\n";
		return pSock->Send(sbuf.c_str(), sbuf.length());
	} else {
		string sbuf = "501 Invalid file name.\r\n";
		return pSock->Send(sbuf.c_str(), sbuf.length());
	}
	
	return 0;
}

int CFtpRun::CmdRetr(CSocketDio* pSock, string& str)
{
	// dSock 发送数据
	string fileName = GetData(str);
	fileName = m_path + root + cPath + "/" + fileName;  // full name

	FILE* fp = fopen(fileName.c_str(),"rb");  // 二进制读取文件
	if (!fp)
	{
		string sbuf = "450 File can't opened\r\n";
		dSock.Close();
		return pSock->Send(sbuf.c_str(), sbuf.length());
	}
	// 打开后
	string sbuf = "150 Ready for transfer data.\r\n";
	pSock->Send(sbuf.c_str(), sbuf.length());
	// 发送
	char data[1024] = {};
	while (fread(data,1,sizeof(data),fp) > 0)
	{
		if (dSock.Send(data, sizeof(data)) < 0)
		{
			string sbuf = "450 Some error in transfer.\r\n";
			fclose(fp);
			dSock.Close();
			return pSock->Send(sbuf.c_str(), sbuf.length());
		}
	}

	sbuf = "226 Close data connection ,STOR successfull.\r\n";
	pSock->Send(sbuf.c_str(), sbuf.length());
	dSock.Close();
	fclose(fp);   // 结束发送

	return 0;
}

int CFtpRun::CmdSize(CSocketDio* pSock, string& str)
{
	// 获取文件大小并发送
	string fileName = GetData(str);  //  获取文件d大小。
	fileName = m_path + root + cPath + "/" + fileName;  // full name
	
	FILE* fp = fopen(fileName.c_str(),"rb");
	if (!fp)
	{
		string sbuf = "550 Cannot open file. Permission denied.\r\n";
		pSock->Send(sbuf.c_str(), sbuf.length());
	}

	fseek(fp, 0, SEEK_END);
	size_t fileSize = ftell(fp);

	string sbuf = "213 ";
	sbuf += to_string(fileSize) +"\r\n";
	return pSock->Send(sbuf.c_str(), sbuf.length());
}

int CFtpRun::CmdNoop(CSocketDio* pSock, string& str)
{
	string sbuf = "200 NOOP command successful.\r\n";
	return pSock->Send(sbuf.c_str(),sbuf.length());
}


// tools ..... 
void CFtpRun::getIpPort(string str, string& sIP, int& nPort)
{
	size_t nStart = str.find(' ', 0);
	size_t nEnd = str.find('\r', nStart);
	string sIPport = str.substr(nStart + 1, nEnd - nStart - 1);

	for (size_t pos = 0; (pos = sIPport.find(',', pos)) != std::string::npos; ++pos)
		sIPport.replace(pos, 1, 1, '.');	 // 127.0.0.1.12.34

	size_t i = sIPport.rfind('.');
	size_t i2 = sIPport.rfind('.', i - 1);

	sIP = sIPport.substr(0, i2);	// 127.0.0.1
	string sPort = sIPport.substr(i2 + 1);	// 12.34
	size_t pos = sPort.find('.',0);
	int n1 = atoi(sPort.substr(0, pos).c_str());
	int n2 = atoi(sPort.substr(pos+1).c_str());
	nPort = n1 * 256 + n2;
}

string CFtpRun::AnsiToUtf8(const std::string& ansiString) {
	if (ansiString.empty()) return std::string();

	// 首先，获取所需的UTF-16字符串的长度
	int wideLength = MultiByteToWideChar(CP_ACP, 0, ansiString.c_str(), -1, nullptr, 0);
	if (wideLength == 0) return std::string();

	wstring wideString(wideLength+5, 0);
	MultiByteToWideChar(CP_ACP, 0, ansiString.c_str(), -1, &wideString[0], wideLength);

	// 然后，从UTF-16转换到UTF-8
	int utf8Length = WideCharToMultiByte(CP_UTF8, 0, &wideString[0], -1, nullptr, 0, nullptr, nullptr);
	if (utf8Length == 0) return std::string();

	string utf8String(utf8Length+5, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wideString[0], -1, &utf8String[0], utf8Length, nullptr, nullptr);

	return utf8String;
}

char* CFtpRun::AnsiToUtf8(const char* ansiString) {
	// 首先，获取所需的UTF-16字符串的长度
	int wideLength = MultiByteToWideChar(CP_ACP, 0, ansiString, -1, nullptr, 0);
	if (wideLength == 0) return nullptr;

	wchar_t* wideString = new wchar_t[wideLength + 5] {};
	MultiByteToWideChar(CP_ACP, 0, ansiString, -1, wideString, wideLength);

	// 然后，从UTF-16转换到UTF-8
	int utf8Length = WideCharToMultiByte(CP_UTF8, 0, wideString, -1, nullptr, 0, nullptr, nullptr);
	if (utf8Length == 0) return nullptr;

	char* utf8String = new char[utf8Length + 5] {};
	WideCharToMultiByte(CP_UTF8, 0, wideString, -1, utf8String, utf8Length, nullptr, nullptr);
	delete[] wideString;
	utf8String[utf8Length + 1] = 0;
	return utf8String;
}

string CFtpRun::GetCmd(const char* str)
{
	int i = 0;
	size_t n = strlen(str);
	for (; i < n; ++i)
	{
		if (str[i] == ' ') {
			return string(str, i);
		}
		else if (str[i] == '\r')
		{
			return string(str, i);
		}
		else {
			continue;
		}
	}
	return string();
}

string CFtpRun::GetData(string& str)
{
	size_t nStart = str.find(' ');
	size_t nEnd = str.find('\r');
	return str.substr(nStart + 1, nEnd - nStart - 1);;
}

bool CFtpRun::RecursiveDel(string& fileName)
{
	DWORD attribs = GetFileAttributes(bstr_t(fileName.c_str()));
	if (attribs & FILE_ATTRIBUTE_DIRECTORY){
		// 目录，递归删除
		string dirpath = fileName;
		string path = fileName + "/*";

		WIN32_FIND_DATA fileData;
		HANDLE hfind = INVALID_HANDLE_VALUE;  // 无效句柄
		hfind = FindFirstFile(bstr_t(path.c_str()), &fileData);

		if (hfind == INVALID_HANDLE_VALUE)
			return false;

		do {
			string cname = bstr_t(fileData.cFileName);
			if (cname == "." || cname == "..")
				continue;
			string filePath = dirpath + "/" + cname;
			if (RecursiveDel(filePath) == false) {
				FindClose(hfind);
				return false;
			}
		} while (FindNextFile(hfind, &fileData) != 0); 
		FindClose(hfind);
		return _trmdir(bstr_t(fileName.c_str()))==0;


	}else{
		// 文件，直接删除
		return remove(bstr_t(fileName.c_str())) == 0;
	}


	return true;
}

void CFtpRun::SetUserName(const char* user)
{
	m_user = user;
}

void CFtpRun::SetUserPwd(const char* pass)
{
	m_pass = pass;
}

void CFtpRun::SetPath(const char* path)
{
	m_path = path;
}