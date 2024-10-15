
// FTPServer.cpp: 定义应用程序的类行为。
//

#include "pch.h"
#include "framework.h"
#include "FTPServer.h"
#include "CMainDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CApp

BEGIN_MESSAGE_MAP(CApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CApp 构造

CApp::CApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	/* 在堆内申请空间，防止静态成员最后释放，导致野指针释放问题。
	 * 默认初始化m_pszProfileName = _tcsdup(str); 也是堆内申请空间的
	*/
	this->m_pszProfileName = _tcsdup(_T(".\\ftps.ini"));   
	
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的 CApp 对象

CApp theApp;


// CApp 初始化

BOOL CApp::InitInstance()
{
	/* 
	*writeprofileint等方法的写入位置设置为 注册表 内
	*取消注释则是保存为ini文件，或者自己写文件加载文件
	*/
	//  SetRegistryKey(_T("App_name"));  

	WSADATA wd;
	WSAStartup(MAKEWORD(2, 2), &wd);  // 初始化
	CMainDlg dlg;
	dlg.DoModal();

	WSACleanup();
	return FALSE;
}

