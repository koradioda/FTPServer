
// CMainDlg.h: 头文件
//

#pragma once
#include "CFtpRun.h"
// CMainDlg 对话框
class CMainDlg : public CDialogEx
{
// 构造
public:
	CMainDlg(CWnd* pParent = nullptr);	// 标准构造函数
	CMenu m_TrayMenu;
	thread m_thread; 
	bool m_running = false;
	CSocketDio m_sock;

	CString m_path;
	CString m_strUser;
	CString m_strPass;
	
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FTPSERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnTrayNotification(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
	
public:
	afx_msg void OnClose();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedChdir();
	afx_msg void OnBnClickedStartserver();
	static UINT FtpAccepter(LPVOID pParam);
	static UINT FtpRuner(LPVOID pParam);

	
	CButton m_btStart;
	CButton m_btChdir;
	CEdit m_UserEdit;
	CEdit m_PassEdit;
	CEdit m_pathEdit;
};




