
// Test_VCR_YNDlg.h : 头文件
//

#pragma once


// CTest_VCR_YNDlg 对话框
class CTest_VCR_YNDlg : public CDialogEx
{
// 构造
public:
	CTest_VCR_YNDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TEST_VCR_YN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonInitautovehtype();
    afx_msg void OnBnClickedButtonRegistermessage();
    afx_msg void OnBnClickedButtonGetvehtype();
    afx_msg void OnBnClickedButtonDelvehtype();
    afx_msg void OnBnClickedButtonFlushqueue();
    afx_msg void OnBnClickedButtonGetqueuelength();
    afx_msg void OnBnClickedButtonIsoccupied();
    afx_msg void OnBnClickedButtonExitautoveh();

private:
    afx_msg LRESULT OnMyMsgHandler(WPARAM wParam, LPARAM lParam);

    void ShowMessage(CString strMsg);
public:
    afx_msg void OnBnClickedCheckUsemsg();
private:
    bool m_bUseMsgModel;
public:
    afx_msg void OnTimer(UINT_PTR nIDEvent);
};
