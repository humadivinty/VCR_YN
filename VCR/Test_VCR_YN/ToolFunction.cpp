#include "stdafx.h"
#include "ToolFunction.h"

void ShowMsg(CEdit *pEdit, CString strMsg)
{
    if (pEdit == NULL)
    {
        return;
    }

    CTime  time = CTime::GetCurrentTime();
    CString strTmp;
    pEdit->GetWindowText(strTmp);
    if (strTmp.IsEmpty() || strTmp.GetLength() > 4096)
    {
        strTmp = time.Format(_T("[%Y-%m-%d %H:%M:%S] "));
        strTmp += strMsg;
        pEdit->SetWindowText(strTmp);
        return;
    }

    strTmp += _T("\r\n");
    strTmp += time.Format(_T("[%Y-%m-%d %H:%M:%S] "));
    strTmp += strMsg;
    pEdit->SetWindowText(strTmp);

    if (pEdit != NULL)
    {
        pEdit->LineScroll(pEdit->GetLineCount() - 1);
    }
}
