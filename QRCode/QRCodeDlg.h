// QRCodeDlg.h : header file
//

#pragma once
#pragma comment (lib, "libzbar-0.lib")
#include "zbar.h"
#include "IniFile.h"
#include "systemtray.h"

// CQRCodeDlg dialog
class CQRCodeDlg : public CDialog
{
// Construction
public:
	CQRCodeDlg(CWnd* pParent = NULL);	// standard constructor
	~CQRCodeDlg();
// Dialog Data
	enum { IDD = IDD_QRCODE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedQRDecod();
	afx_msg void OnBnClickedButtonOpen();
	
	CSystemTray m_TrayIcon;
public:
	BOOL m_bIsOpen;
	//CSystemTray m_TrayIcon;
	afx_msg void OnBnClickedOk();

	LRESULT OnTrayNotification(WPARAM wParam, LPARAM lParam);

	afx_msg void OnTraymenuShow();
	afx_msg void OnTraymenuExit();
	BOOL  OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};
