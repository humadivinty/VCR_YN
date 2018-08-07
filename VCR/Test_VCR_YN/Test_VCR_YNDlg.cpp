
// Test_VCR_YNDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Test_VCR_YN.h"
#include "Test_VCR_YNDlg.h"
#include "afxdialogex.h"
#include "ToolFunction.h"
#include "../VCR/VCR.h"

#ifdef DEBUG
#pragma comment(lib, "../debug/VCR.lib")
#else
#pragma comment(lib, "../release/VCR.lib")
#endif // DEBUG

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MY_MGS (WM_USER+1)
#define TIME_ID_VEH 1

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTest_VCR_YNDlg 对话框



CTest_VCR_YNDlg::CTest_VCR_YNDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTest_VCR_YNDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTest_VCR_YNDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTest_VCR_YNDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_MESSAGE(MY_MGS, &CTest_VCR_YNDlg::OnMyMsgHandler)
    ON_BN_CLICKED(IDC_BUTTON_InitAutoVehType, &CTest_VCR_YNDlg::OnBnClickedButtonInitautovehtype)
    ON_BN_CLICKED(IDC_BUTTON_RegisterMessage, &CTest_VCR_YNDlg::OnBnClickedButtonRegistermessage)
    ON_BN_CLICKED(IDC_BUTTON_GetVehType, &CTest_VCR_YNDlg::OnBnClickedButtonGetvehtype)
    ON_BN_CLICKED(IDC_BUTTON_DelVehType, &CTest_VCR_YNDlg::OnBnClickedButtonDelvehtype)
    ON_BN_CLICKED(IDC_BUTTON_FlushQueue, &CTest_VCR_YNDlg::OnBnClickedButtonFlushqueue)
    ON_BN_CLICKED(IDC_BUTTON_GetQueueLength, &CTest_VCR_YNDlg::OnBnClickedButtonGetqueuelength)
    ON_BN_CLICKED(IDC_BUTTON_IsOccupied, &CTest_VCR_YNDlg::OnBnClickedButtonIsoccupied)
    ON_BN_CLICKED(IDC_BUTTON_ExitAutoVeh, &CTest_VCR_YNDlg::OnBnClickedButtonExitautoveh)
    ON_BN_CLICKED(IDC_CHECK_USEMSG, &CTest_VCR_YNDlg::OnBnClickedCheckUsemsg)
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CTest_VCR_YNDlg 消息处理程序

BOOL CTest_VCR_YNDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
    GetDlgItem(IDC_EDIT_INDEX)->SetWindowText("-1");
    m_bUseMsgModel = false;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CTest_VCR_YNDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTest_VCR_YNDlg::OnPaint()
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
HCURSOR CTest_VCR_YNDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTest_VCR_YNDlg::OnBnClickedButtonInitautovehtype()
{
    // TODO:  在此添加控件通知处理程序代码
    CString cstrIpAddress;
    GetDlgItem(IDC_IPADDRESS1)->GetWindowTextA(cstrIpAddress);
    if (-1 != cstrIpAddress.Find("0.0.0.0"))
    {
        MessageBox("Input ip address is invalid, please retry again.");
        return;
    }
    char szIpAddress[64] = {0};
    sprintf_s(szIpAddress, sizeof(szIpAddress), "%s:9910", cstrIpAddress.GetBuffer());
    cstrIpAddress.ReleaseBuffer();
    BOOL bConnect = InitAutoVehType(szIpAddress, "");

    TCHAR szLog[MAX_PATH] = { 0 };
    if (TRUE == bConnect)
    {
        sprintf_s(szLog, sizeof(szLog), "InitAutoVehType , %s, return true.", szIpAddress);

        SetTimer(TIME_ID_VEH, 2000, NULL);
    }
    else
    {
        sprintf_s(szLog, sizeof(szLog), "InitAutoVehType , %s, return false.", szIpAddress);
    }
    //MessageBox(szLog);
    ShowMessage(szLog);
}


void CTest_VCR_YNDlg::OnBnClickedButtonRegistermessage()
{
    // TODO:  在此添加控件通知处理程序代码
    int iRet = RegisterMessage(m_hWnd, MY_MGS);

    TCHAR szLog[MAX_PATH] = { 0 };
    sprintf_s(szLog, sizeof(szLog), "RegisterMessage ,m_hWnd = %p, msgID= %d, return code = %d.", m_hWnd, MY_MGS, iRet);
    //MessageBox(szLog);
    ShowMessage(szLog);
}


void CTest_VCR_YNDlg::OnBnClickedButtonGetvehtype()
{
    // TODO:  在此添加控件通知处理程序代码
    CString cstrIndex;
    GetDlgItem(IDC_EDIT_INDEX)->GetWindowText(cstrIndex);
    int iIndex = atoi(cstrIndex.GetBuffer());
    cstrIndex.ReleaseBuffer();

    int iVehType = GetVehType(iIndex);
    TCHAR szLog[MAX_PATH] = { 0 };
    sprintf_s(szLog, sizeof(szLog), "GetVehType ,VehType = %d.", iVehType);
    //MessageBox(szLog);
    ShowMessage(szLog);
}


void CTest_VCR_YNDlg::OnBnClickedButtonDelvehtype()
{
    // TODO:  在此添加控件通知处理程序代码
    int iVehType = DelVehType();
    TCHAR szLog[MAX_PATH] = { 0 };
    sprintf_s(szLog, sizeof(szLog), "DelVehType ,return code = %d.", iVehType);
    //MessageBox(szLog);
    ShowMessage(szLog);
}


void CTest_VCR_YNDlg::OnBnClickedButtonFlushqueue()
{
    // TODO:  在此添加控件通知处理程序代码
    int iVehType = FlushQueue(0);
    TCHAR szLog[MAX_PATH] = { 0 };
    sprintf_s(szLog, sizeof(szLog), "FlushQueue ,return code = %d.", iVehType);
    //MessageBox(szLog);
    ShowMessage(szLog);
}


void CTest_VCR_YNDlg::OnBnClickedButtonGetqueuelength()
{
    // TODO:  在此添加控件通知处理程序代码
    int iVehType = GetQueueLength();
    TCHAR szLog[MAX_PATH] = { 0 };
    sprintf_s(szLog, sizeof(szLog), "GetQueueLength ,return code = %d.", iVehType);
    //MessageBox(szLog);
    ShowMessage(szLog);
}


void CTest_VCR_YNDlg::OnBnClickedButtonIsoccupied()
{
    // TODO:  在此添加控件通知处理程序代码
    int iVehType = IsOccupied();
    TCHAR szLog[MAX_PATH] = { 0 };
    sprintf_s(szLog, sizeof(szLog), "IsOccupied ,return code = %d.", iVehType);
    //MessageBox(szLog);
    ShowMessage(szLog);
}


void CTest_VCR_YNDlg::OnBnClickedButtonExitautoveh()
{
    // TODO:  在此添加控件通知处理程序代码
    ExitAutoVeh();
    TCHAR szLog[MAX_PATH] = { 0 };
    sprintf_s(szLog, sizeof(szLog), "ExitAutoVeh ,finish.");
    //MessageBox(szLog);
    ShowMessage(szLog);
    KillTimer(TIME_ID_VEH);
}

LRESULT CTest_VCR_YNDlg::OnMyMsgHandler(WPARAM wParam, LPARAM lParam)
{
    if (m_bUseMsgModel)
    {
        OnBnClickedButtonGetqueuelength();
        OnBnClickedButtonGetvehtype();
    }
    return 0;
}

void CTest_VCR_YNDlg::ShowMessage(CString strMsg)
{
    ShowMsg((CEdit*)GetDlgItem(IDC_EDIT_MSG), strMsg);
}


void CTest_VCR_YNDlg::OnBnClickedCheckUsemsg()
{
    // TODO:  在此添加控件通知处理程序代码
    CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_USEMSG);
    int iState = pBtn->GetCheck();
    m_bUseMsgModel = (iState == 0) ? false : true;
}


void CTest_VCR_YNDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO:  在此添加消息处理程序代码和/或调用默认值
    int iVehType = GetQueueLength();
    TCHAR szLog[MAX_PATH] = { 0 };
    sprintf_s(szLog, sizeof(szLog), "GetQueueLength ,return code = %d.", iVehType);
    //MessageBox(szLog);
    ShowMessage(szLog);
    switch (nIDEvent)
    {
    case TIME_ID_VEH:
        iVehType = GetQueueLength();
        if (iVehType > 0)
        {
            sprintf_s(szLog, sizeof(szLog), "GetQueueLength ,return code = %d.", iVehType);
            //MessageBox(szLog);
            ShowMessage(szLog);
            OnBnClickedButtonGetvehtype();
        }
        else
        {
            if (0 == IsOccupied())
            {
                FlushQueue(0);
            }
        }
        break;
    default:
        break;
    }

    CDialogEx::OnTimer(nIDEvent);
}
