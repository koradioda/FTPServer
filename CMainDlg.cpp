
// CMainDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "FTPServer.h"
#include "CMainDlg.h"
#include "afxdialogex.h"
#include "CSocketDio.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainDlg 对话框



CMainDlg::CMainDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FTPSERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_TrayMenu.LoadMenu(IDR_TRAYMENU);
	// m_sock.Create(21);

}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STARTSERVER, m_btStart);
	DDX_Control(pDX, IDC_CHDIR, m_btChdir);
	DDX_Control(pDX, IDC_USER, m_UserEdit);
	DDX_Control(pDX, IDC_PASS, m_PassEdit);
	DDX_Control(pDX, IDC_PATH, m_pathEdit);
}

BEGIN_MESSAGE_MAP(CMainDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_ICON_NOTIFY, &CMainDlg::OnTrayNotification)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CHDIR, &CMainDlg::OnBnClickedChdir)
	ON_BN_CLICKED(IDC_STARTSERVER, &CMainDlg::OnBnClickedStartserver)
END_MESSAGE_MAP()


// CMainDlg 消息处理程序

BOOL CMainDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.hWnd = m_hWnd;
	nid.uID = IDD_FTPSERVER_DIALOG;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = WM_ICON_NOTIFY;
	nid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	_tcscpy_s(nid.szTip, _T("FTPServer"));
	Shell_NotifyIcon(NIM_ADD, &nid);

	CWinApp* pApp = AfxGetApp();
	m_path = pApp->GetProfileString(_T("HOME"), _T("path"), _T(""));
	m_pathEdit.SetWindowText(m_path);
	m_strUser = pApp->GetProfileString(_T("USERS"), _T("user"), _T("user"));
	m_UserEdit.SetWindowText(m_strUser);
	m_strPass = pApp->GetProfileString( _T("USERS"), _T("pass"), _T("pass"));
	m_PassEdit.SetWindowText(m_strPass);



	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMainDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMainDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CMainDlg::OnTrayNotification(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case WM_RBUTTONUP: // 右键点击
	{
		CPoint pos;
		GetCursorPos(&pos);
		CMenu* pMenu = m_TrayMenu.GetSubMenu(0);
		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_VERTICAL, pos.x, pos.y, this);
		break;
	}
	case WM_LBUTTONDOWN:
	{
		ShowWindow(TRUE);
		break;
	}
	}
	return 1;
}


void CMainDlg::OnClose()
{
	ShowWindow(FALSE);
	// CDialogEx::OnClose();
}


BOOL CMainDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int menuID = LOWORD(wParam);
	switch (LOWORD(wParam))
	{
	case ID_SWIND:
	{
		ShowWindow(TRUE);
		return TRUE;
	}
	case ID_QUIT:
	{
		// DestroyWindow();    // assert error
		CDialogEx::OnCancel();
		return TRUE;
	}
	default:
		break;
	}
	
	return CDialogEx::OnCommand(wParam, lParam);
}


void CMainDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	m_running = false;
	m_sock.Close();

	// 清除托盘图标
	NOTIFYICONDATA nd = { 0 };
	nd.cbSize = sizeof(NOTIFYICONDATA);
	nd.hWnd = m_hWnd;
	nd.uID = IDD_FTPSERVER_DIALOG;
	nd.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nd.uCallbackMessage = WM_ICON_NOTIFY;
	nd.hIcon = m_hIcon;
	Shell_NotifyIcon(NIM_DELETE, &nd);
	CWinApp* pApp = AfxGetApp();
	pApp->WriteProfileString(_T("HOME"), _T("path"), m_path);
	pApp->WriteProfileString(_T("USERS"), _T("user"), m_strUser);
	pApp->WriteProfileString(_T("USERS"), _T("pass"), m_strPass);


}


void CMainDlg::OnBnClickedChdir()
{
	// TODO: 在此添加控件通知处理程序代码
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(bi));  // 初始化
	bi.ulFlags = BIF_USENEWUI;
	bi.lpszTitle = _T("请选择一个文件夹");

	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

	if (pidl != 0)
	{
		// 获取文件夹的路径
		TCHAR path[MAX_PATH];
		if (SHGetPathFromIDList(pidl, path))
		{
			m_path = path;
			m_path.Replace('\\', '/');  // 获取路径是替换路径分隔符
			SetDlgItemText(IDC_PATH,m_path);
			GetDlgItem(IDC_PATH)->SetFocus();
		}

		// 释放由SHBrowseForFolder分配的内存
		CoTaskMemFree(pidl);
	}

}

void CMainDlg::OnBnClickedStartserver()
{
	m_pathEdit.GetWindowText(m_path);
	// m_path.Replace('\\','/');
	if (m_path.IsEmpty()) {
		AfxMessageBox(_T("文件夹无效"));
		return;
	}
	
	m_UserEdit.GetWindowText(m_strUser);
	m_PassEdit.GetWindowText(m_strPass);  // 获取账号密码

	CString btText;
	m_btStart.GetWindowText(btText);   // 获取按钮文本
	string str = bstr_t(btText.GetString());

	if (str == "启动服务") {
		m_running = true;
		m_sock.Create(21);
		// m_sock.Listen();
		CWinThread* acceptThread = AfxBeginThread(&CMainDlg::FtpAccepter, (LPVOID)this, 0, 0, 0, 0);
		m_btStart.SetWindowText(bstr_t("关闭服务"));
		m_btChdir.EnableWindow(FALSE);
		m_UserEdit.EnableWindow(FALSE);
		m_PassEdit.EnableWindow(FALSE);
		m_pathEdit.EnableWindow(FALSE);
	}else if (str == "关闭服务")
	{
		m_running = false;
		m_sock.Close();
		m_btStart.SetWindowText(bstr_t("启动服务"));
		m_btChdir.EnableWindow(TRUE);
		m_UserEdit.EnableWindow(TRUE);
		m_PassEdit.EnableWindow(TRUE);
		m_pathEdit.EnableWindow(TRUE);
	}
	
}

UINT CMainDlg::FtpAccepter(LPVOID pParam)
{	
	/*WSADATA wd;
	WSAStartup(MAKEWORD(2,2),&wd);*/
	CMainDlg* p = (CMainDlg*) pParam;
	p->m_sock.Listen();
	CFtpRun* pfr = new CFtpRun;
	while (p->m_running)
	{
		if (p->m_sock.Accept(pfr->m_sock)) // 处理命令
		{
			pfr->SetPath(bstr_t(p->m_path.GetString()));
			pfr->SetUserName(bstr_t(p->m_strUser.GetString()));
			pfr->SetUserPwd(bstr_t(p->m_strPass.GetString()));
			CWinThread* ftpRunner = AfxBeginThread(&CMainDlg::FtpRuner, (LPVOID)pfr, 0, 0, 0, 0);
			pfr = new CFtpRun;
		}
		else
		{
			if (pfr)
				delete pfr;
			return -1;
		}
	}
	if (pfr)
		delete pfr;
	return 0;
}

UINT CMainDlg::FtpRuner(LPVOID pParam)
{
	CFtpRun* pfr = (CFtpRun*)pParam;
	pfr->run();
	delete pfr;  // 瑞出程序清理对象
	return 0;
}
