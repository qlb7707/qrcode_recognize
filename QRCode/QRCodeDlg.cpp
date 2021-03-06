// QRCodeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QRCode.h"
#include "QRCodeDlg.h"
/*
#include<opencv\cv.h>
#include<opencv\highgui.h>
#include<opencv2\calib3d\calib3d.hpp>*/
#include <mmsystem.h>


#include <iostream>
#include <zbar.h>
#include <locale.h>
#include <stdlib.h> 
#include "cv.h"
#include "highgui.h"

#define STR(s) #s
//#pragma comment(lib,"opencv_highgui246.lib")
//#pragma comment(lib,"opencv_imgproc246.lib")
//#pragma comment(lib,"opencv_core246.lib")
#pragma comment (lib, "winmm.lib")
using namespace std;
using namespace zbar;
using namespace std;
using namespace cv;

CString g_szVersion = "v1.00.001";

#define WM_ICON_NOTIFY  WM_APP+12
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
DWORD WINAPI QRDecodThread(LPVOID lpParameter);
HANDLE hThread =  NULL;
 NOTIFYICONDATA nid;  
CIniFile g_iniFile("QRCodeData.ini");
BOOL g_bIsCallByAP = FALSE;

CString GetAppPath()
{
	CString strAppDir;
	strAppDir.Empty();

	if (strAppDir.IsEmpty())
	{
		TCHAR szDir[MAX_PATH];
		DWORD dwLen = GetModuleFileName(AfxGetInstanceHandle(), szDir,
			MAX_PATH);
		if (dwLen > 0)
		{
			strAppDir = szDir;
			int iFind = strAppDir.ReverseFind(_T('\\'));
			if (iFind >= 0)
				strAppDir = strAppDir.Left(iFind + 1);
			else
				strAppDir = _T("");
		}
	}

	return strAppDir;
}

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CQRCodeDlg dialog




CQRCodeDlg::CQRCodeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQRCodeDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bIsOpen = FALSE;
}

CQRCodeDlg::~CQRCodeDlg()
{
	HANDLE hself = GetCurrentProcess();
	TerminateProcess(hself, 0);
	if(hThread != NULL)
		CloseHandle(hThread);
}

void CQRCodeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CQRCodeDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_QRDECODE, &CQRCodeDlg::OnBnClickedQRDecod)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CQRCodeDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDOK, &CQRCodeDlg::OnBnClickedOk)
	ON_MESSAGE(WM_ICON_NOTIFY, OnTrayNotification)
	ON_WM_SIZE()
	ON_COMMAND(ID_TRAYMENU_SHOW, &CQRCodeDlg::OnTraymenuShow)
	ON_COMMAND(ID_TRAYMENU_EXIT, &CQRCodeDlg::OnTraymenuExit)
END_MESSAGE_MAP()


// CQRCodeDlg message handlers

BOOL CQRCodeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon


	for (int i = 0; i < __argc; i++)
	{
		__argv[i];
		TRACE("command line : %s\n",__argv[i]);
	}
	CString szParam = __argv[1];
	if(szParam.Find("/a") == 0)
	{
		 g_bIsCallByAP = TRUE;
	}

	GetDlgItem(IDC_STATIC_VERSION)->SetWindowText(g_szVersion);
   	if (!m_TrayIcon.Create(this, WM_ICON_NOTIFY, "QRCode Running...", m_hIcon, IDR_MENU_POPUP))
	 	return -1;
	if(g_bIsCallByAP)
	{
		m_TrayIcon.SetTooltipText("QRCode Running...");
		m_TrayIcon.MinimiseToTray(m_TrayIcon.GetTargetWnd(), TRUE);
		m_TrayIcon.ShowBalloon("Double click to show QRCode", "QRCode Running...");
		SendMessage(WM_COMMAND, IDC_BUTTON_QRDECODE);
	}
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CQRCodeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CQRCodeDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CQRCodeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

 

void CQRCodeDlg::OnBnClickedQRDecod()
{
 
	GetDlgItem(IDC_BUTTON_QRDECODE)->EnableWindow(FALSE);
	if(!g_bIsCallByAP) 
		SendMessage(WM_COMMAND, IDC_BUTTON_OPEN);

	hThread = CreateThread(NULL, 0, QRDecodThread, this, 0, NULL);
	if(hThread != NULL)
		CloseHandle(hThread);
}

void CQRCodeDlg::OnBnClickedButtonOpen()
{
	CString szTemp;
	GetDlgItem(IDC_BUTTON_OPEN)->GetWindowText(szTemp);
	if(szTemp.Compare("Open") == 0)
	{
		m_bIsOpen = TRUE;
		GetDlgItem(IDC_BUTTON_OPEN)->SetWindowText("Close");
	}
	else
	{
		m_bIsOpen = FALSE;
		GetDlgItem(IDC_BUTTON_OPEN)->SetWindowText("Open");
	}
}

DWORD WINAPI QRDecodThread (LPVOID lpParameter)
{
	CQRCodeDlg *dlg = (CQRCodeDlg *) lpParameter;

	CvCapture* capture = NULL;   
	capture =cvCaptureFromCAM(0) ;
	if(capture == NULL)
	{
		dlg->GetDlgItem(IDC_BUTTON_QRDECODE)->EnableWindow(TRUE);
		AfxMessageBox("Didn't detect the camera, please connect camera!!!!");
		return -1;
	}
	CString szPrevQRCode, szCurQRCode;
	/* create a reader */
	zbar_image_scanner_t *scanner = zbar_image_scanner_create();

	/* configure the reader */
	zbar_image_scanner_set_config(scanner, ZBAR_NONE, ZBAR_CFG_ENABLE, 1);

	/* obtain image data */
	int width = 0, height = 0;
	const void *raw = NULL;
	IplImage* frame;
	IplImage* frame_gray;
	frame = cvQueryFrame( capture );
	frame_gray = cvCreateImage(cvGetSize(frame), 8, 1);
	if(dlg->m_bIsOpen)
		cvNamedWindow("Webcam", 0);
	while (true)
	{
		frame = cvQueryFrame(capture );
		if (!frame) break;
		cvCvtColor(frame, frame_gray, CV_RGB2GRAY);

		width = cvGetSize(frame_gray).width;
		height= cvGetSize(frame_gray).height;
		raw = (char*)frame_gray->imageData;
		/* wrap image data */
		zbar_image_t *image = zbar_image_create();
		zbar_image_set_format(image, *(int*)"Y800");
		zbar_image_set_size(image, width, height);
		zbar_image_set_data(image, raw, width * height, zbar_image_free_data);
		/* scan the image for barcodes */
		int n = zbar_scan_image(scanner, image); //n == 0 is failed
		if(dlg->m_bIsOpen)
			cvShowImage("Webcam", frame_gray);

		if(n > 0)
		{
			PlaySound("chord.wav", NULL, SND_ASYNC);
			const zbar_symbol_t *symbol = zbar_image_first_symbol(image);


			for(; symbol; symbol = zbar_symbol_next(symbol)) 
			{
				/* do something useful with results */
				zbar_symbol_type_t typ = zbar_symbol_get_type(symbol);
				const char *data = zbar_symbol_get_data(symbol);
				unsigned int len = zbar_symbol_get_data_length(symbol);
				szCurQRCode.Format("%s", data);
				//if SN not change, write one time only
				if(szPrevQRCode.Compare(szCurQRCode) == 0)
					continue;
				else 	
					g_iniFile.WriteString("QRDecod", "QRCode", szCurQRCode);

				szPrevQRCode = szCurQRCode;
			}
		}
		if(!dlg->m_bIsOpen)
			cvDestroyWindow("Webcam");	
		if(cvWaitKey(10) == 'q')
		{ 
			break;  
		}
	}
	cvReleaseCapture(&capture);
	cvDestroyWindow("Webcam");	
	return 0;
}
void CQRCodeDlg::OnBnClickedOk()
{
	OnOK();
}

LRESULT CQRCodeDlg::OnTrayNotification(WPARAM wParam, LPARAM lParam)
{
	// Delegate all the work back to the default 
	// implementation in CSystemTray.
	 return m_TrayIcon.OnTrayNotification(wParam, lParam);
	 
}

void CQRCodeDlg::OnTraymenuShow()
{
	m_TrayIcon.MaximiseFromTray(m_TrayIcon.GetTargetWnd());
}

void CQRCodeDlg::OnTraymenuExit()
{
	OnCancel();
}



BOOL CQRCodeDlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Add your specialized code here and/or call the base class
	switch (message)
	{
	case WM_SYSCOMMAND:
		switch (wParam)
		{
		case SC_MINIMIZE:
			{
				//Do whatever you want!
				m_TrayIcon.MinimiseToTray(m_TrayIcon.GetTargetWnd());
				m_TrayIcon.ShowBalloon("Double click to show QRCode", "QRCode Notice");
				return TRUE;
			}
		}
		break;
	}

	return CDialog::OnWndMsg(message, wParam, lParam, pResult);
}
