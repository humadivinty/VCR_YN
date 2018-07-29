// VCR.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "VCR.h"
#include "utilityTool/ToolFunction.h"
#include "Camera/Camera6467_VFR.h"

#define TOOL_CHECK_ARG(arg)\
if (arg == NULL) \
    {\
        Tool_WriteFormatLog("%s is NULL", #arg); \
        return 0; \
    }

std::shared_ptr<Camera6467_VFR> g_Camera_VFR;

VCR_API   BOOL VCR_DECL InitAutoVehType(LPCSTR lpszConnPara1, LPCSTR lpszConnPara2)
{
    Tool_WriteFormatLog("InitAutoVehType, lpszConnPara1= %p, lpszConnPara2 = %p", lpszConnPara1, lpszConnPara2);
    TOOL_CHECK_ARG(lpszConnPara1);
    //TOOL_CHECK_ARG(lpszConnPara2);

    std::string strVfrIP;
    if (NULL != strstr(lpszConnPara1, ":"))
    {
        std::string strPara1(lpszConnPara1);
        strVfrIP = strPara1.substr(0, strPara1.find_first_of(":"));
    }
    else if (1 == Tool_checkIP(lpszConnPara1))
    {
        strVfrIP = lpszConnPara1;
    }
    else
    {
        Tool_WriteFormatLog("InitAutoVehType, the ip address  %s  is invalid.", lpszConnPara1);
        return FALSE;
    }
    Tool_WriteFormatLog("InitAutoVehType, get the ip address  %s .", strVfrIP.c_str());

    if (g_Camera_VFR != NULL
        && 0 ==strcmp(strVfrIP.c_str(), g_Camera_VFR->GetCameraIP())
        )
    {
        Tool_WriteFormatLog("InitAutoVehType, the camera %s is already connecting, return true.", strVfrIP.c_str());
        return TRUE;
    }

    char chTemp[MAX_PATH] = {0};
    sprintf_s(chTemp, sizeof(chTemp), "1.1.1.1");
    Tool_ReadKeyValueFromConfigFile(INI_FILE_NAME, "PlateCamera", "ip", chTemp, sizeof(chTemp));
    Tool_WriteFormatLog("InitAutoVehType, get the plate Camera ip address  %s .", chTemp);

    g_Camera_VFR = std::make_shared<Camera6467_VFR>();
    TOOL_CHECK_ARG(g_Camera_VFR);

    g_Camera_VFR->SetCameraIP(strVfrIP.c_str());
    if ( 0 == g_Camera_VFR->ConnectToCamera() )
    {
        Tool_WriteFormatLog("InitAutoVehType, connect to VFR Camera %s success.", strVfrIP.c_str());
    }
    else
    {
        Tool_WriteFormatLog("InitAutoVehType, connect to VFR Camera %s failed.", strVfrIP.c_str());
    }

    if (0 != strcmp(chTemp, "1.1.1.1"))
    {
        if (g_Camera_VFR->OpenPlateCamera(chTemp))
        {
            Tool_WriteFormatLog("InitAutoVehType, connect to Plate Camera %s success.", chTemp);
        }
        else
        {
            Tool_WriteFormatLog("InitAutoVehType, connect to Plate Camera %s failed.", chTemp);
        }
    }
    else
    {
        Tool_WriteFormatLog("InitAutoVehType,  Plate Camera ip address %s invalid.", chTemp);
    }

    Tool_WriteFormatLog("InitAutoVehType, finish, return true.");
    return TRUE;
}

VCR_API   int VCR_DECL RegisterMessage(HWND hWnd, UINT nMsgID)
{
    Tool_WriteFormatLog("RegisterMessage, hWnd= %p, nMsgID = %d", hWnd, nMsgID);
    //if (hWnd == NULL || g_Camera_VFR == nullptr)
    //{
    //    Tool_WriteFormatLog("RegisterMessage, hWnd == NULL || g_Camera_VFR == nullptr, return 1");
    //    return 1;
    //}
    //g_Camera_VFR->SetMsgHandleAngMsg(hWnd, nMsgID);

    Tool_WriteFormatLog("RegisterMessage, the function is not realize, finish, return 0.");
    return 0;
}

VCR_API   int VCR_DECL GetVehType(int nIndex /*= -1*/)
{
    Tool_WriteFormatLog("GetVehType, nIndex = %d", nIndex);
    if (g_Camera_VFR == nullptr)
    {
        Tool_WriteFormatLog("GetVehType g_Camera_VFR == nullptr, return 0");
        return 0;
    }
    int iIndex = nIndex > 0 ? nIndex : 0;
    std::shared_ptr<CameraResult> pTempResult = g_Camera_VFR->GetFrontResultByPosition(iIndex);
    if (pTempResult == nullptr)
    {
        Tool_WriteFormatLog("GetVehType ,the result is not ready, return 0");
        return 0;
    }
    Tool_WriteFormatLog("GetVehType ,get the result , plate no = %s, vehType = %d", 
        pTempResult->chPlateNO,
        pTempResult->iVehTypeNo);
    int iVehType = pTempResult->iVehTypeNo;
    g_Camera_VFR->DeleteFrontResult(NULL);

    Tool_WriteFormatLog("GetVehType, finish, return %d.", iVehType);
    return iVehType;
}

VCR_API   void VCR_DECL ExitAutoVeh()
{
    Tool_WriteFormatLog("ExitAutoVeh, begin.");
    if (g_Camera_VFR)
    {
        g_Camera_VFR = nullptr;
    }
    Tool_WriteFormatLog("ExitAutoVeh, finish.");
}

VCR_API   int VCR_DECL DelVehType()
{
    Tool_WriteFormatLog("DelVehType, begin.");
    int iVehType = 0;
    if (g_Camera_VFR)
    {
        std::shared_ptr<CameraResult> pTempResult = g_Camera_VFR->GetFrontResult();
        if (pTempResult)
        {
            Tool_WriteFormatLog("DelVehType, get the result , plate number = %s, vehType = %d.",
                pTempResult->chPlateNO,
                pTempResult->iVehTypeNo);
            iVehType = pTempResult->iVehTypeNo;
            g_Camera_VFR->DeleteFrontResult(NULL);
        }
        else
        {
            Tool_WriteFormatLog("DelVehType, the result is empty.");
        }
    }
    Tool_WriteFormatLog("DelVehType, finish, return %d.", iVehType);
    return iVehType;
}

VCR_API   int VCR_DECL FlushQueue()
{    
    Tool_WriteFormatLog("FlushQueue, begin.");
    int iSize = 0;
    if (g_Camera_VFR)
    {
        iSize = g_Camera_VFR->GetResultListSize();
        g_Camera_VFR->ClearALLResult();
    }
    else
    {
        Tool_WriteFormatLog("the VFR camera is not connected, return 0");
    }
    Tool_WriteFormatLog("FlushQueue, finish, return %d.", iSize);
    return iSize;
}

VCR_API   int VCR_DECL GetQueueLength()
{
    Tool_WriteFormatLog("GetQueueLength, begin.");
    int iSize = 0;
    if (g_Camera_VFR)
    {
        iSize = g_Camera_VFR->GetResultListSize();
    }
    else
    {
        Tool_WriteFormatLog("the VFR camera is not connected, return 0");
    }
    Tool_WriteFormatLog("GetQueueLength, finish, return %d.", iSize);
    return iSize;
}

VCR_API   int VCR_DECL IsOccupied()
{
    Tool_WriteFormatLog("IsOccupied, begin.");
    int iRet = 0;
    if (g_Camera_VFR)
    {
        iRet = g_Camera_VFR->GetResultListSize();
    }
    Tool_WriteFormatLog("IsOccupied,   return %d.", iRet);
    return iRet;
}
