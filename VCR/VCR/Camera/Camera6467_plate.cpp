#include "stdafx.h"
#include "Camera6467_plate.h"
#include "HvDevice/HvDeviceBaseType.h"
#include "HvDevice/HvDeviceCommDef.h"
#include "HvDevice/HvDeviceNew.h"
#include "HvDevice/HvCamera.h"
#include "utilityTool/ToolFunction.h"
#include <process.h>

#ifdef  USE_VIDEO
#include "H264_Api/H264.h"
#pragma comment(lib, "H264_Api/H264.lib")
#endif


Camera6467_plate::Camera6467_plate() :
BaseCamera(),
m_iTimeInvl(3),
m_iCompressBigImgSize(COMPRESS_BIG_IMG_SIZE),
m_iCompressSamllImgSize(COMPRESS_PLATE_IMG_SIZE),
m_pTempBin(NULL),
m_pTempBig1(NULL),
m_pCaptureImg(NULL),
m_pTempBig(NULL),
g_pUser(NULL),
g_func_ReconnectCallback(NULL),
g_ConnectStatusCallback(NULL),
g_func_DisconnectCallback(NULL),
m_CameraResult(NULL),
m_BufferResult(NULL),
m_bResultComplete(false),
m_bJpegComplete(false),
m_bSaveToBuffer(false),
m_bOverlay(false),
m_bCompress(false),
m_bStatusCheckThreadExit(false),
m_hFirstWinHandle(NULL),
m_hSecondWinHandle(NULL),
m_hPlayFirstH264(NULL),
m_hPlaySecondh264(NULL),
m_hStatusCheckThread(NULL)
{
    ReadConfig();
    InitializeCriticalSection(&m_csResult);

    m_hStatusCheckThread = (HANDLE)_beginthreadex(NULL, 0, Camera_StatusCheckThread, this, 0, NULL);
}


Camera6467_plate::Camera6467_plate(const char* chIP, HWND hWnd, int Msg) :
BaseCamera(chIP, hWnd, Msg),
m_iTimeInvl(3),
m_iCompressBigImgSize(COMPRESS_BIG_IMG_SIZE),
m_iCompressSamllImgSize(COMPRESS_PLATE_IMG_SIZE),
m_pTempBin(NULL),
m_pTempBig1(NULL),
m_pCaptureImg(NULL),
m_pTempBig(NULL),
g_pUser(NULL),
g_func_ReconnectCallback(NULL),
g_ConnectStatusCallback(NULL),
g_func_DisconnectCallback(NULL),
m_CameraResult(NULL),
m_BufferResult(NULL),
m_bResultComplete(false),
m_bJpegComplete(false),
m_bSaveToBuffer(false),
m_bOverlay(false),
m_bCompress(false),
m_bStatusCheckThreadExit(false),
m_hFirstWinHandle(NULL),
m_hSecondWinHandle(NULL),
m_hPlayFirstH264(NULL),
m_hPlaySecondh264(NULL),
m_hStatusCheckThread(NULL)
{
    //SetConnectStatus_Callback(NULL, NULL, 10);
    ReadConfig();

    InitializeCriticalSection(&m_csResult);

    m_hStatusCheckThread = (HANDLE)_beginthreadex(NULL, 0, Camera_StatusCheckThread, this, 0, NULL);
}

Camera6467_plate::~Camera6467_plate()
{
    SetCheckThreadExit(true);
    SetConnectStatus_Callback(NULL, NULL, 10);
    Tool_SafeCloseThread(m_hStatusCheckThread);

    InterruptionConnection();
#ifdef USE_VIDEO
    StopPlayVideoByChannel(0);
    StopPlayVideoByChannel(1);
#endif

    SAFE_DELETE_OBJ(m_CameraResult);
    SAFE_DELETE_OBJ(m_BufferResult);

    SAFE_DELETE_ARRAY(m_pTempBin);
    SAFE_DELETE_ARRAY(m_pTempBig1);
    SAFE_DELETE_ARRAY(m_pCaptureImg);
    SAFE_DELETE_ARRAY(m_pTempBig);

    DeleteCriticalSection(&m_csResult);
}

void Camera6467_plate::AnalysisAppendXML(CameraResult* CamResult)
{
    if (NULL == CamResult)
        return;
    if (0 != CamResult->dw64TimeMS)
    {
        CTime tm(CamResult->dw64TimeMS / 1000);
    	//sprintf_s(record->chPlateTime, "%04d%02d%02d%02d%02d%02d%03d", tm.GetYear(), tm.GetMonth(), tm.GetDay(), tm.GetHour(), tm.GetMinute(), tm.GetSecond(), record->dw64TimeMS%1000);
        sprintf_s(CamResult->chPlateTime, sizeof(CamResult->chPlateTime), "%04d-%02d-%02d %02d:%02d:%02d",
            tm.GetYear(), 
            tm.GetMonth(), 
            tm.GetDay(),
            tm.GetHour(), 
            tm.GetMinute(),
            tm.GetSecond());
    }
    else
    {
    	SYSTEMTIME st;
    	GetLocalTime(&st);
    	//sprintf_s(record->chPlateTime, "%04d%02d%02d%02d%02d%02d%03d", st.wYear, st.wMonth, st.wDay	,st.wHour, st.wMinute,st.wSecond, st.wMilliseconds);
        sprintf_s(CamResult->chPlateTime, sizeof(CamResult->chPlateTime), "%04d-%02d-%02d %02d:%02d:%02d:%03d",
            st.wYear, 
            st.wMonth,
            st.wDay,
            st.wHour, 
            st.wMinute, 
            st.wSecond);
    }

    char chTemp[BUFFERLENTH] = { 0 };
    int iLenth = BUFFERLENTH;
    
    if (Tool_GetDataFromAppenedInfo(CamResult->pcAppendInfo, "VehicleType", chTemp, &iLenth))
    {
        CamResult->iVehTypeNo = AnalysisVelchType(chTemp);
    }
    memset(chTemp, 0, sizeof(chTemp));
    iLenth = BUFFERLENTH;
    if (Tool_GetDataFromAppenedInfo(CamResult->pcAppendInfo, "AxleCnt", chTemp, &iLenth))
    {
        int iAxleCount = 0;
        sscanf_s(chTemp, "%d", &iAxleCount);
        CamResult->iAxletreeCount = iAxleCount;
        //printf("the Axletree count is %d.\n", iAxleCount);
    }
    memset(chTemp, 0, sizeof(chTemp));
    iLenth = BUFFERLENTH;
    if (Tool_GetDataFromAppenedInfo(CamResult->pcAppendInfo, "Wheelbase", chTemp, &iLenth))
    {
        float fWheelbase = 0;
        sscanf_s(chTemp, "%f", &fWheelbase);
        CamResult->fDistanceBetweenAxles = fWheelbase;
        //printf("the Wheelbase  is %f.\n", fWheelbase);
    }
    memset(chTemp, 0, sizeof(chTemp));
    iLenth = BUFFERLENTH;
    if (Tool_GetDataFromAppenedInfo(CamResult->pcAppendInfo, "CarLength", chTemp, &iLenth))
    {
        float fCarLength = 0;
        sscanf_s(chTemp, "%f", &fCarLength);
        CamResult->fVehLenth = fCarLength;
        //printf("the CarLength  is %f.\n", fCarLength);
    }
    memset(chTemp, 0, sizeof(chTemp));
    iLenth = BUFFERLENTH;
    if (Tool_GetDataFromAppenedInfo(CamResult->pcAppendInfo, "CarHeight", chTemp, &iLenth))
    {
        float fCarHeight = 0;
        sscanf_s(chTemp, "%f", &fCarHeight);
        CamResult->fVehHeight = fCarHeight;
        //printf("the CarHeight  is %f.\n", fCarHeight);
    }
    memset(chTemp, 0, sizeof(chTemp));
    iLenth = BUFFERLENTH;
    if (Tool_GetDataFromAppenedInfo(CamResult->pcAppendInfo, "BackUp", chTemp, &iLenth))
    {
        CamResult->bBackUpVeh = true;
    }
    iLenth = BUFFERLENTH;
    if (Tool_GetDataFromAppenedInfo(CamResult->pcAppendInfo, "Confidence", chTemp, &iLenth))
    {
        float fConfidence = 0;
        sscanf_s(chTemp, "%f", &fConfidence);
        CamResult->fConfidenceLevel = fConfidence;
        //printf("the CarHeight  is %f.\n", fCarHeight);
    }
    
    TiXmlElement element = Tool_SelectElementByName(CamResult->pcAppendInfo, "PlateName", 2);
    if (strlen(element.GetText()) > 0)
    {
        memset(CamResult->chPlateNO, 0, sizeof(CamResult->chPlateNO));
        strcpy_s(CamResult->chPlateNO, sizeof(CamResult->chPlateNO), element.GetText());

        memset(chTemp, 0, sizeof(chTemp));
        strcpy_s(chTemp, sizeof(chTemp), element.GetText());

        iLenth = strlen(chTemp);
        printf("find the plate number = %s, plate length = %d\n", chTemp, iLenth);
        if (strlen(chTemp) > 0)
        {
            if (NULL != strstr(chTemp, "蓝") && NULL == strstr(chTemp, "白"))
            {
                CamResult->iPlateColor = COLOR_BLUE;
                printf("find plate color COLOR_BLUE.\n");
            }
            else if (NULL != strstr(chTemp, "黄"))
            {
                CamResult->iPlateColor = COLOR_YELLOW;
                printf("find plate color COLOR_YELLOW.\n");
            }
            else if (NULL != strstr(chTemp, "黑"))
            {
                CamResult->iPlateColor = COLOR_BLACK;
                printf("find plate color COLOR_BLACK.\n");
            }
            else if (NULL != strstr(chTemp, "白") && NULL == strstr(chTemp, "蓝"))
            {
                CamResult->iPlateColor = COLOR_WHITE;
                printf("find plate color COLOR_WHITE.\n");
            }
            else if (NULL != strstr(chTemp, "绿"))
            {
                size_t ilen = strlen(chTemp);
                if (chTemp[ilen - 1] == 'D' || chTemp[ilen - 1] == 'F')
                {
                    CamResult->iPlateColor = COLOR_YELLOW_GREEN;
                    printf("find plate color COLOR_YELLOW_GREEN.\n");
                }
                else
                {
                    CamResult->iPlateColor = COLOR_GRADIENT_CREEN;
                    printf("find plate color COLOR_GRADIENT_CREEN.\n");
                }
            }
            else if (NULL != strstr(chTemp, "蓝") && NULL != strstr(chTemp, "白"))
            {
                CamResult->iPlateColor = COLOR_BLUE_WHIETE;
                printf("find plate color COLOR_BLUE_WHIETE.\n");
            }
            else
            {
                CamResult->iPlateColor = COLOR_UNKNOW;
                printf("find plate color COLOR_UNKNOW.\n");
            }
        }
        else
        {
            CamResult->iPlateColor = COLOR_UNKNOW;
        }

    }
    else
    {
        sprintf_s(CamResult->chPlateNO, sizeof(CamResult->chPlateNO), "无车牌");
        CamResult->iPlateColor = COLOR_UNKNOW;
    }

}

int Camera6467_plate::AnalysisVelchType(const char* vehType)
{
    if (vehType == NULL)
    {
        return UNKOWN_TYPE;
    }
    if (strstr(vehType, "客1"))
    {
        return BUS_TYPE_1;
    }
    else if (strstr(vehType, "客2"))
    {
        return BUS_TYPE_2;
        //printf("the Vehicle type code is 2.\n");
    }
    else if (strstr(vehType, "客3"))
    {
        return BUS_TYPE_3;
    }
    else if (strstr(vehType, "客4"))
    {
        return BUS_TYPE_4;
    }
    else if (strstr(vehType, "客5"))
    {
        return BUS_TYPE_5;
    }
    else if (strstr(vehType, "货1"))
    {
        return TRUCK_TYPE_1;
    }
    else if (strstr(vehType, "货2"))
    {
        return TRUCK_TYPE_2;
    }
    else if (strstr(vehType, "货3"))
    {
        return TRUCK_TYPE_3;
    }
    else if (strstr(vehType, "货4"))
    {
        return TRUCK_TYPE_4;
    }
    else if (strstr(vehType, "货5"))
    {
        return TRUCK_TYPE_5;
    }
    else
    {
        return UNKOWN_TYPE;
    }
}

void Camera6467_plate::ReadConfig()
{
    char iniFileName[MAX_PATH] = { 0 };
    char iniDeviceInfoName[MAX_PATH] = { 0 };
#ifdef GUANGXI_DLL
    sprintf_s(iniFileName, "..\\DevInterfaces\\HVCR_Signalway_V%d_%d\\HVCR_Config\\HVCR_Signalway_V%d_%d.ini", PROTOCAL_VERSION, DLL_VERSION, PROTOCAL_VERSION, DLL_VERSION);
#else
    strcat_s(iniFileName, Tool_GetCurrentPath());
    strcat_s(iniFileName, INI_FILE_NAME);
#endif

    //读取可靠性配置文件
    int iValue = GetPrivateProfileIntA("SaveToBufferPath", "Enable", 0, iniFileName);
    m_bSaveToBuffer = (iValue == 1) ? true : false;

    char chTemp[256] = { 0 };
    //sprintf_s(chTemp, "%d", iLog);
    sprintf_s(chTemp, sizeof(chTemp), "%d", iValue);
    WritePrivateProfileStringA("SaveToBufferPath", "Enable", chTemp, iniFileName);

    //------------------------------Overlay setting  enable--------------
    //iValue = GetPrivateProfileIntA("Overlay", "Enable", 0, iniFileName);
    //m_bOverlay = (iValue == 1) ? true : false;

    //memset(chTemp, 0, sizeof(chTemp));
    //sprintf_s(chTemp, sizeof(chTemp), "%d", iValue);
    //WritePrivateProfileStringA("Overlay", "Enable", chTemp, iniFileName);

    //------------------------------Compress setting  enable--------------
    //iValue = GetPrivateProfileIntA("Compress", "Enable", 0, iniFileName);
    //m_bCompress = (iValue == 1) ? true : false;

    //memset(chTemp, 0, sizeof(chTemp));
    //sprintf_s(chTemp, sizeof(chTemp), "%d", iValue);
    //WritePrivateProfileStringA("Compress", "Enable", chTemp, iniFileName);

    //-----------------------Compress setting : Big image size--------------
    //iValue = GetPrivateProfileIntA("Compress", "BigImgSize", COMPRESS_BIG_IMG_SIZE, iniFileName);
    //m_iCompressBigImgSize = (iValue > 0) ? iValue : COMPRESS_BIG_IMG_SIZE;

    //memset(chTemp, 0, sizeof(chTemp));
    //sprintf_s(chTemp, sizeof(chTemp), "%d", iValue);
    //WritePrivateProfileStringA("Compress", "BigImgSize", chTemp, iniFileName);

    //-----------------------Compress setting : Small image size--------------
    //iValue = GetPrivateProfileIntA("Compress", "SmallImgSize", COMPRESS_BIG_IMG_SIZE, iniFileName);
    //m_iCompressSamllImgSize = (iValue > 0) ? iValue : COMPRESS_BIG_IMG_SIZE;

    //memset(chTemp, 0, sizeof(chTemp));
    //sprintf_s(chTemp, sizeof(chTemp), "%d", iValue);
    //WritePrivateProfileStringA("Compress", "SmallImgSize", chTemp, iniFileName);

    BaseCamera::ReadConfig();
}

#ifdef  USE_VIDEO
int Camera6467_plate::StartPlayVideo(int iChannelID, HANDLE& playHandle, const HWND winHandle)
{
    char szLog[256] = { 0 };
    sprintf_s(szLog, sizeof(szLog), "StartPlayVideoByChannel, iChannelID = %d, playHndle = %p, winHandle = %p", iChannelID, playHandle, winHandle);
    WriteLog(szLog);

    char chCMD[256] = { 0 };
    if (iChannelID == 0)
    {
        sprintf_s(chCMD, sizeof(chCMD), "rtsp://%s:554/h264ESVideoTest", m_strIP.c_str());
        //m_iVedioChannelID = 0;
    }
    else
    {
        sprintf_s(chCMD, sizeof(chCMD), "rtsp://%s:554/h264ESVideoTestSecond", m_strIP.c_str());
        //m_iVedioChannelID = 1;
    }
    WriteLog(chCMD);
    playHandle = H264_Play(winHandle, chCMD);
    WriteLog("StartPlayVideoByChannel , return 0.");
    return 0;
}

void Camera6467_plate::StopPlayVideo(HANDLE& playHandle)
{
    WriteLog("StopPlayVideo begin.");
    if (playHandle)
    {
        H264_SetExitStatus(playHandle);
        H264_Destroy(playHandle);
        playHandle = NULL;
    }
    WriteLog("StopPlayVideo end.");
}
#endif
void Camera6467_plate::SetCheckThreadExit(bool bExit)
{
    EnterCriticalSection(&m_csFuncCallback);
    m_bStatusCheckThreadExit = bExit;
    LeaveCriticalSection(&m_csFuncCallback);
}

bool Camera6467_plate::GetCheckThreadExit()
{
    bool bExit = false;
    EnterCriticalSection(&m_csFuncCallback);
    bExit = m_bStatusCheckThreadExit;
    LeaveCriticalSection(&m_csFuncCallback);
    return bExit;
}

void Camera6467_plate::SaveOverlayResult()
{
    char chLog[MAX_PATH] = {0};
    if (m_bOverlay)
    {
        if (m_pTempBig1 == NULL)
        {
            m_pTempBig1 = new BYTE[MAX_IMG_SIZE];
            memset(m_pTempBig1, 0, MAX_IMG_SIZE);
        }
        else
        {
            memset(m_pTempBig1, 0, MAX_IMG_SIZE);
        }

        char chOverlayInfo[256] = { 0 };
        if (strstr(m_CameraResult->chPlateNO, "无"))
        {
            sprintf_s(chOverlayInfo, sizeof(chOverlayInfo), "时间:%s   车牌号: *****", m_CameraResult->chPlateTime);
        }
        else
        {
            sprintf_s(chOverlayInfo, sizeof(chOverlayInfo), "时间:%s   车牌号: %s", m_CameraResult->chPlateTime, m_CameraResult->chPlateNO);
        }
        std::string strOverlayInfo(chOverlayInfo);
        std::wstring wstrOverlayIno = Img_string2wstring(strOverlayInfo);

        if (m_CameraResult->CIMG_BestSnapshot.dwImgSize > 0 && m_CameraResult->CIMG_BestSnapshot.pbImgData)
        {
            if (m_pTempBig1)
            {
                memset(m_pTempBig1, 0, MAX_IMG_SIZE);
                long iDestSize = MAX_IMG_SIZE;

                memset(chLog, 0, sizeof(chLog));
                sprintf_s(chLog, sizeof(chLog), "BestSnapshot开始字符叠加， text = %s", chOverlayInfo);
                WriteLog(chLog);
                bool bOverlay = Tool_OverlayStringToImg(&m_CameraResult->CIMG_BestSnapshot.pbImgData,
                    m_CameraResult->CIMG_BestSnapshot.dwImgSize,
                    &m_pTempBig1, iDestSize,
                    wstrOverlayIno.c_str(), 32,
                    10, 30, 255, 255, 255,
                    50);
                if (bOverlay)
                {
                    memset(chLog, 0, sizeof(chLog));
                    sprintf_s(chLog, sizeof(chLog), "字符叠加成功, size = %ld, begin copy..", iDestSize);
                    WriteLog(chLog);

                    delete[] m_CameraResult->CIMG_BestSnapshot.pbImgData;
                    m_CameraResult->CIMG_BestSnapshot.pbImgData = new BYTE[iDestSize];
                    memcpy(m_CameraResult->CIMG_BestSnapshot.pbImgData, m_pTempBig1, iDestSize);
                    m_CameraResult->CIMG_BestSnapshot.dwImgSize = iDestSize;

                    WriteLog("finish copy.");
                }
                else
                {
                    WriteLog("字符叠加失败,使用原图数据.");
                }

            }
        }

        if (m_CameraResult->CIMG_LastSnapshot.dwImgSize > 0 && m_CameraResult->CIMG_LastSnapshot.pbImgData)
        {
            if (m_pTempBig1)
            {
                memset(m_pTempBig1, 0, MAX_IMG_SIZE);
                long iDestSize = MAX_IMG_SIZE;

                memset(chLog, 0, sizeof(chLog));
                MY_SPRINTF(chLog, "LastSnapshot 开始字符叠加， text = %s", chOverlayInfo);
                WriteLog(chLog);
                bool bOverlay = Tool_OverlayStringToImg(&m_CameraResult->CIMG_LastSnapshot.pbImgData, 
                    m_CameraResult->CIMG_LastSnapshot.dwImgSize,
                    &m_pTempBig1, iDestSize,
                    wstrOverlayIno.c_str(), 32,
                    10, 10, 255, 255, 255,
                    50);
                if (bOverlay)
                {
                    memset(chLog, 0, sizeof(chLog));
                    MY_SPRINTF(chLog, "字符叠加成功, size = %ld, begin copy..", iDestSize);
                    WriteLog(chLog);

                    delete[] m_CameraResult->CIMG_LastSnapshot.pbImgData;
                    m_CameraResult->CIMG_LastSnapshot.pbImgData = new BYTE[iDestSize];
                    memcpy(m_CameraResult->CIMG_LastSnapshot.pbImgData, m_pTempBig1, iDestSize);
                    m_CameraResult->CIMG_LastSnapshot.dwImgSize = iDestSize;

                    WriteLog("finish copy.");
                }
                else
                {
                    WriteLog("字符叠加失败,使用原图数据.");
                }
            }
        }
    }
}

int Camera6467_plate::RecordInfoBegin(DWORD dwCarID)
{
    char chLog[MAX_PATH] = { 0 };
    sprintf_s(chLog, sizeof(chLog), "RecordInfoBegin -begin- dwCarID = %lu", dwCarID);
    WriteLog(chLog);
    if (dwCarID == m_dwLastCarID)
    {
        WriteLog("相同carID,丢弃该结果");
        return 0;
    }
    SetResultComplete(false);
    if (NULL != m_CameraResult)
    {
        delete m_CameraResult;
        m_CameraResult = NULL;
    }
    m_CameraResult = new CameraResult();
    if (m_CameraResult)
    {
        sprintf_s(m_CameraResult->chDeviceIp, sizeof(m_CameraResult->chDeviceIp), "%s", m_strIP.c_str());
        m_CameraResult->dwCarID = dwCarID;
    }

    memset(chLog, 0, sizeof(chLog));
    sprintf_s(chLog, sizeof(chLog), "RecordInfoBegin -end- dwCarID = %lu", dwCarID);
    WriteLog(chLog);
    return 0;
}

int Camera6467_plate::RecordInfoEnd(DWORD dwCarID)
{
    char chLog[MAX_PATH] = { 0 };
    sprintf_s(chLog, sizeof(chLog), "RecordInfoEnd begin, dwCarID = %d", dwCarID);
    WriteLog(chLog);
    if (dwCarID != m_dwLastCarID)
    {
        m_dwLastCarID = dwCarID;
    }
    else
    {
        WriteLog("相同CarID, 丢弃该结果");
        return 0;
    }

    if (NULL == m_CameraResult)
    {
        return 0;
    }

    //CTime tm(m_CameraResult->dw64TimeMS / 1000);
    //SaveModeInfo TempSaveModeInfo;
    //sprintf_s(TempSaveModeInfo.chBeginTime, sizeof(TempSaveModeInfo.chBeginTime),
    //    "%d.%02d.%02d_%02d",
    //    tm.GetYear(),
    //    tm.GetMonth(),
    //    tm.GetDay(),
    //    tm.GetHour());
    //WriteHistoryInfo(TempSaveModeInfo);

    /**************************************队列模式*****begin*****************************************

    if (m_CameraResult->fVehLenth > (float)m_iSuperLenth)
    {
    char chLog[260] = {0};
    sprintf_s(chLog, "接收到车长%f的车， 超过设定超长车长%d, 清除当前车辆队列",m_CameraResult->fVehLenth, m_iSuperLenth );
    WriteLog(chLog);
    ClearAllResult();
    }

    if (m_CameraResult->bBackUpVeh)
    {
    ClearAllResult();
    delete m_CameraResult;
    m_CameraResult = NULL;
    WriteLog("接收倒车结果， 清除包括倒车结果在内的当前车辆队列。");
    }

    if (NULL != m_CameraResult)
    {
    EnterCriticalSection(&m_csResult);
    if (m_ResultList.size() < 255)
    {
    char chLogList[260] = {0};
    sprintf_s(chLogList, "now the size is %d, put one result in list.", m_ResultList.size());
    WriteLog(chLogList);
    m_ResultList.push_back(m_CameraResult);
    LeaveCriticalSection(&m_csResult);
    }
    else
    {
    LeaveCriticalSection(&m_csResult);
    //todo 将结果缓存的本地缓存目录
    //SaveResultToBuffer(m_CameraResult);

    delete m_CameraResult;
    //WriteLog("The list is full , save the result to buffer path.");
    WriteLog("The list is full , delete the result.");
    }
    m_CameraResult = NULL;
    }
    **************************************队列模式*****begin*****************************************/

    //SaveOverlayResult();

    //if (m_bSaveToBuffer)
    //{
    //    SaveResultToBufferPath(m_CameraResult);
    //}

    //if (m_bCompress)
    //{
    //    WriteLog("开始压缩车牌小图.");
    //    CompressImg(m_CameraResult->CIMG_PlateImage, m_iCompressSamllImgSize);
    //    WriteLog("开始压缩最清晰大图.");
    //    CompressImg(m_CameraResult->CIMG_BestSnapshot, m_iCompressBigImgSize);
    //    WriteLog("开始压缩最后大图.");
    //    CompressImg(m_CameraResult->CIMG_LastSnapshot, m_iCompressBigImgSize);
    //    WriteLog("全部压缩结束.");
    //}

    EnterCriticalSection(&m_csResult);
    if (NULL != m_BufferResult)
    {
        delete m_BufferResult;
        m_BufferResult = NULL;
    }
    m_BufferResult = new CameraResult(*m_CameraResult);
    m_bResultComplete = true;
    LeaveCriticalSection(&m_csResult);

    if (NULL != m_hWnd)
    {
        WriteLog("PostMessage");
        //::PostMessage(*((HWND*)m_hWnd),m_iMsg, 1, 0);
        ::PostMessage(m_hWnd, m_iMsg, (WPARAM)1, 0);
    }

    memset(chLog, 0, sizeof(chLog));
    sprintf_s(chLog, sizeof(chLog), "RecordInfoEnd end, dwCarID = %lu", dwCarID);
    WriteLog(chLog);
    return 0;
}

int Camera6467_plate::RecordInfoPlate(DWORD dwCarID,
    LPCSTR pcPlateNo,
    LPCSTR pcAppendInfo,
    DWORD dwRecordType,
    DWORD64 dw64TimeMS)
{
    SetResultComplete(false);

    if (NULL == m_CameraResult)
    {
        return -1;
    }
    char chLog[260] = { 0 };
    sprintf_s(chLog, sizeof(chLog), "RecordInfoPlate -begin- dwCarID = %d", dwCarID);
    WriteLog(chLog);
    if (dwCarID == m_dwLastCarID)
    {
        WriteLog("相同carID,丢弃该结果");
        return 0;
    }

    if (m_CameraResult->dwCarID == dwCarID)
    {
        m_CameraResult->dw64TimeMS = dw64TimeMS;
        sprintf_s(m_CameraResult->chPlateNO, sizeof(m_CameraResult->chPlateNO), "%s", pcPlateNo);
        WriteLog(m_CameraResult->chPlateNO);
  
        WriteLog(pcAppendInfo);
        //memset(m_CameraResult->pcAppendInfo, 0, sizeof(m_CameraResult->pcAppendInfo));
        //if (strlen(pcAppendInfo) < sizeof(m_CameraResult->pcAppendInfo))
        //{
        //    memcpy(m_CameraResult->pcAppendInfo, pcAppendInfo, strlen(pcAppendInfo));
        //    m_CameraResult->pcAppendInfo[strlen(pcAppendInfo)] = '\0';
        //}
        //else
        //{
        //    WriteLog("RecordInfoPlate , 预留附加信息空间不足.");
        //}
        //HVAPIUTILS_ParsePlateXmlStringEx(pcAppendInfo, m_CameraResult->pcAppendInfo, 2048);        

        //AnalysisAppendXML(m_CameraResult);

        //char chTemp[BUFFERLENTH] = { 0 };
        //int iLenth = BUFFERLENTH;
        //memset(chTemp, '\0', BUFFERLENTH);
        //if (Tool_GetDataFromAppenedInfo((char*)pcAppendInfo, "Confidence", chTemp, &iLenth))
        //{
        //    float fConfidence = 0.0;
        //    sscanf_s(chTemp, "%f", &fConfidence);
        //    m_CameraResult->iReliability = (int)(fConfidence * 10000);
        //}

        //memset(chLog, 0, sizeof(chLog));
        //sprintf_s(chLog, sizeof(chLog), "the plate time is %s, %I64u", m_CameraResult->chPlateTime, dw64TimeMS);
        //WriteLog(chLog);
    }
    memset(chLog, 0, sizeof(chLog));
    sprintf_s(chLog, sizeof(chLog), "RecordInfoPlate -end- dwCarID = %lu", dwCarID);
    WriteLog(chLog);
    return 0;
}

int Camera6467_plate::RecordInfoBigImage(DWORD dwCarID,
    WORD wImgType,
    WORD wWidth,
    WORD wHeight,
    PBYTE pbPicData,
    DWORD dwImgDataLen,
    DWORD dwRecordType,
    DWORD64 dw64TimeMS)
{
    SetResultComplete(false);

    if (NULL == m_CameraResult)
    {
        return -1;
    }
    char chLog[MAX_PATH] = { 0 };
    sprintf_s(chLog, sizeof(chLog), "RecordInfoBigImage -begin- dwCarID = %ld, dwRecordType = %#x， ImgType=%d, size = %ld",
        dwCarID, 
        dwRecordType, 
        wImgType,
        dwImgDataLen);
    WriteLog(chLog);

    if (dwCarID == m_dwLastCarID)
    {
        WriteLog("相同carID,丢弃该结果");
        return 0;
    }
    if (m_CameraResult->dwCarID == dwCarID)
    {
        if (wImgType == RECORD_BIGIMG_BEST_SNAPSHOT)
        {
            WriteLog("RecordInfoBigImage BEST_SNAPSHO  ");

            CopyDataToIMG(m_CameraResult->CIMG_BestSnapshot, pbPicData, wWidth, wHeight, dwImgDataLen, wImgType);
        }
        else if (wImgType == RECORD_BIGIMG_LAST_SNAPSHOT)
        {
            WriteLog("RecordInfoBigImage LAST_SNAPSHOT  ");

            CopyDataToIMG(m_CameraResult->CIMG_LastSnapshot, pbPicData, wWidth, wHeight, dwImgDataLen, wImgType);
        }
        else if (wImgType == RECORD_BIGIMG_BEGIN_CAPTURE)
        {
            WriteLog("RecordInfoBigImage BEGIN_CAPTURE  ");

            CopyDataToIMG(m_CameraResult->CIMG_BeginCapture, pbPicData, wWidth, wHeight, dwImgDataLen, wImgType);
        }
        else if (wImgType == RECORD_BIGIMG_BEST_CAPTURE)
        {
            WriteLog("RecordInfoBigImage BEST_CAPTURE  ");

            CopyDataToIMG(m_CameraResult->CIMG_BestCapture, pbPicData, wWidth, wHeight, dwImgDataLen, wImgType);
        }
        else if (wImgType == RECORD_BIGIMG_LAST_CAPTURE)
        {
            WriteLog("RecordInfoBigImage LAST_CAPTURE  ");

            CopyDataToIMG(m_CameraResult->CIMG_LastCapture, pbPicData, wWidth, wHeight, dwImgDataLen, wImgType);
        }
        else
        {
            WriteLog("RecordInfoBigImage other Image, put it to  LAST_CAPTURE .");
            CopyDataToIMG(m_CameraResult->CIMG_LastCapture, pbPicData, wWidth, wHeight, dwImgDataLen, wImgType);
        }
    }
    memset(chLog, 0, sizeof(chLog));
    sprintf_s(chLog, sizeof(chLog), "RecordInfoBigImage -end- dwCarID = %d", dwCarID);
    WriteLog(chLog);
    return 0;
}

int Camera6467_plate::RecordInfoSmallImage(DWORD dwCarID,
    WORD wWidth,
    WORD wHeight,
    PBYTE pbPicData,
    DWORD dwImgDataLen,
    DWORD dwRecordType,
    DWORD64 dw64TimeMS)
{
    SetResultComplete(false);
    if (NULL == m_CameraResult)
    {
        return -1;
    }
    char chlogInfo[MAX_PATH] = { 0 };
    sprintf_s(chlogInfo, sizeof(chlogInfo), "RecordInfoSmallImage  -begin- dwCarID = %lu", dwCarID);
    WriteLog(chlogInfo);
    if (dwCarID == m_dwLastCarID)
    {
        WriteLog("相同carID,丢弃该结果");
        return 0;
    }

    int iBuffLen = 1024 * 1024;
    if (m_CameraResult->dwCarID == dwCarID)
    {
        if (NULL != m_CameraResult->CIMG_PlateImage.pbImgData)
        {
            delete[] m_CameraResult->CIMG_PlateImage.pbImgData;
            m_CameraResult->CIMG_PlateImage.pbImgData = NULL;
        }
        m_CameraResult->CIMG_PlateImage.pbImgData = new BYTE[iBuffLen];
        WriteLog("RecordInfoSmallImage 内存申请.");
        if (m_CameraResult->CIMG_PlateImage.pbImgData != NULL)
        {
            WriteLog("RecordInfoSmallImage 内存申请成功.");
            memset(m_CameraResult->CIMG_PlateImage.pbImgData, 0, iBuffLen);
            HRESULT Hr = HVAPIUTILS_SmallImageToBitmapEx(pbPicData,
                wWidth,
                wHeight,
                m_CameraResult->CIMG_PlateImage.pbImgData,
                &iBuffLen);
            if (Hr == S_OK)
            {
                m_CameraResult->CIMG_PlateImage.wImgWidth = wWidth;
                m_CameraResult->CIMG_PlateImage.wImgHeight = wHeight;
                m_CameraResult->CIMG_PlateImage.dwImgSize = iBuffLen;
                if (m_Small_IMG_Temp.pbImgData == NULL)
                {
                    m_Small_IMG_Temp.pbImgData = new BYTE[MAX_IMG_SIZE];
                    memset(m_Small_IMG_Temp.pbImgData, 0, MAX_IMG_SIZE);
                }
                if (m_Small_IMG_Temp.pbImgData)
                {
                    size_t iDestLenth = MAX_IMG_SIZE;
                    memset(m_Small_IMG_Temp.pbImgData, 0, MAX_IMG_SIZE);
                    WriteLog("convert bmp to jpeg , begin .");
                    bool bScale = Tool_Img_ScaleJpg(m_CameraResult->CIMG_PlateImage.pbImgData,
                        m_CameraResult->CIMG_PlateImage.dwImgSize,
                        m_Small_IMG_Temp.pbImgData,
                        &iDestLenth,
                        m_CameraResult->CIMG_PlateImage.wImgWidth,
                        m_CameraResult->CIMG_PlateImage.wImgHeight,
                        80);
                    if (bScale)
                    {
                        WriteLog("convert bmp to jpeg success, begin copy.");
                        memset(m_CameraResult->CIMG_PlateImage.pbImgData, 0, m_CameraResult->CIMG_PlateImage.dwImgSize);
                        memcpy(m_CameraResult->CIMG_PlateImage.pbImgData, m_Small_IMG_Temp.pbImgData, iDestLenth);
                        m_CameraResult->CIMG_PlateImage.dwImgSize = iDestLenth;
                        WriteLog("convert bmp to jpeg success, finish copy.");
                    }
                    else
                    {
                        WriteLog("convert bmp to jpeg failed, use default.");
                    }
                }
            }
            else
            {
                WriteLog("HVAPIUTILS_SmallImageToBitmapEx 失败.");
            }
        }
        else
        {
            WriteLog("RecordInfoSmallImage 内存申请失败.");
        }
    }
    memset(chlogInfo, 0, sizeof(chlogInfo));
    sprintf_s(chlogInfo, sizeof(chlogInfo), "RecordInfoSmallImage  -end- dwCarID = %lu", dwCarID);
    WriteLog(chlogInfo);
    return 0;
}

int Camera6467_plate::RecordInfoBinaryImage(DWORD dwCarID,
    WORD wWidth,
    WORD wHeight,
    PBYTE pbPicData,
    DWORD dwImgDataLen,
    DWORD dwRecordType,
    DWORD64 dw64TimeMS)
{
    SetResultComplete(false);

    if (NULL == m_CameraResult)
    {
        return -1;
    }
    char chlogInfo[MAX_PATH] = { 0 };
    sprintf_s(chlogInfo, sizeof(chlogInfo), "RecordInfoBinaryImage -begin- dwCarID = %lu", dwCarID);
    WriteLog(chlogInfo);

    if (dwCarID == m_dwLastCarID)
    {
        WriteLog("相同carID,丢弃该结果");
        return 0;
    }
    //int iBufferlength = 1024 * 1024;
    //if (m_pTempBin == NULL)
    //{
    //    m_pTempBin = new BYTE[1024 * 1024];
    //    memset(m_pTempBin, 0x00, iBufferlength);
    //}
    //if (m_pTempBin)
    //{
    //    memset(m_pTempBin, 0x00, iBufferlength);

    //    HRESULT hRet = HVAPIUTILS_BinImageToBitmapEx(pbPicData, m_pTempBin, &iBufferlength);
    //    if (hRet == S_OK)
    //    {
    //        if (m_Bin_IMG_Temp.pbImgData == NULL)
    //        {
    //            m_Bin_IMG_Temp.pbImgData = new BYTE[MAX_IMG_SIZE];
    //            memset(m_Bin_IMG_Temp.pbImgData, 0x00, MAX_IMG_SIZE);
    //        }
    //        if (m_Bin_IMG_Temp.pbImgData)
    //        {
    //            DWORD iDestLenth = MAX_IMG_SIZE;
    //            memset(m_Bin_IMG_Temp.pbImgData, 0x00, MAX_IMG_SIZE);
    //            WriteLog("bin, convert bmp to jpeg , begin .");
    //            bool bScale = Tool_Img_ScaleJpg(m_pTempBin,
    //                iBufferlength,
    //                m_Bin_IMG_Temp.pbImgData,
    //                &iDestLenth,
    //                wWidth,
    //                wHeight,
    //                90);
    //            if (bScale)
    //            {
    //                WriteLog("bin, convert bmp to jpeg success, begin copy.");
    //                CopyDataToIMG(m_CameraResult->CIMG_BinImage, m_Bin_IMG_Temp.pbImgData, wWidth, wHeight, iDestLenth, 0);
    //                WriteLog("bin, convert bmp to jpeg success, finish copy.");
    //            }
    //            else
    //            {
    //                WriteLog("bin, convert bmp to jpeg failed, use default.");
    //            }
    //        }
    //        else
    //        {
    //            WriteLog("m_Bin_IMG_Temp  is null.");
    //        }
    //    }
    //    else
    //    {
    //        WriteLog("HVAPIUTILS_BinImageToBitmapEx, failed, use default.");
    //        CopyDataToIMG(m_CameraResult->CIMG_BinImage, pbPicData, wWidth, wHeight, dwImgDataLen, 0);
    //    }
    //}
    //else
    {
        //WriteLog("m_pTempBin is NULL ,  use default.");
        CopyDataToIMG(m_CameraResult->CIMG_BinImage, pbPicData, wWidth, wHeight, dwImgDataLen, 0);
    }
    memset(chlogInfo, 0, sizeof(chlogInfo));
    sprintf_s(chlogInfo, sizeof(chlogInfo), "RecordInfoBinaryImage -end- dwCarID = %lu", dwCarID);
    WriteLog(chlogInfo);
    return 0;
}

int Camera6467_plate::DeviceJPEGStream(PBYTE pbImageData,
    DWORD dwImageDataLen,
    DWORD dwImageType,
    LPCSTR szImageExtInfo)
{
    static int iCout = 0;
    if (iCout++ > 100)
    {
        WriteLog("receive one jpeg frame.");
        iCout = 0;
    }

    EnterCriticalSection(&m_csResult);
    m_bJpegComplete = false;

    m_CIMG_StreamJPEG.dwImgSize = dwImageDataLen;
    m_CIMG_StreamJPEG.wImgWidth = 1920;
    m_CIMG_StreamJPEG.wImgHeight = 1080;
    if (NULL == m_CIMG_StreamJPEG.pbImgData)
    {
        m_CIMG_StreamJPEG.pbImgData = new unsigned char[MAX_IMG_SIZE];
        memset(m_CIMG_StreamJPEG.pbImgData, 0, MAX_IMG_SIZE);
    }
    if (m_CIMG_StreamJPEG.pbImgData)
    {
        memset(m_CIMG_StreamJPEG.pbImgData, 0, MAX_IMG_SIZE);
        memcpy(m_CIMG_StreamJPEG.pbImgData, pbImageData, dwImageDataLen);
        m_bJpegComplete = true;
    }
    LeaveCriticalSection(&m_csResult);

    return 0;
}

void Camera6467_plate::SetDisConnectCallback(void* funcDisc, void* pUser)
{
    EnterCriticalSection(&m_csFuncCallback);
    g_func_DisconnectCallback = funcDisc;
    g_pUser = pUser;
    LeaveCriticalSection(&m_csFuncCallback);
}

void Camera6467_plate::SetReConnectCallback(void* funcReco, void* pUser)
{
    EnterCriticalSection(&m_csFuncCallback);
    g_func_ReconnectCallback = funcReco;
    g_pUser = pUser;
    LeaveCriticalSection(&m_csFuncCallback);
}

bool Camera6467_plate::GetOneJpegImg(CameraIMG &destImg)
{
    WriteLog("GetOneJpegImg::begin.");
    bool bRet = false;

    if (!destImg.pbImgData)
    {
        WriteLog("GetOneJpegImg:: allocate memory.");
        destImg.pbImgData = new unsigned char[MAX_IMG_SIZE];
        memset(destImg.pbImgData, 0, MAX_IMG_SIZE);
        WriteLog("GetOneJpegImg:: allocate memory success.");
    }

    EnterCriticalSection(&m_csResult);
    if (m_bJpegComplete)
    {
        if (destImg.pbImgData)
        {
            memset(destImg.pbImgData, 0, MAX_IMG_SIZE);
            memcpy(destImg.pbImgData, m_CIMG_StreamJPEG.pbImgData, m_CIMG_StreamJPEG.dwImgSize);

            destImg.dwImgSize = m_CIMG_StreamJPEG.dwImgSize;
            destImg.wImgHeight = m_CIMG_StreamJPEG.wImgHeight;
            destImg.wImgWidth = m_CIMG_StreamJPEG.wImgWidth;
            bRet = true;
            WriteLog("GetOneJpegImg success.");
            m_bJpegComplete = false;
        }
        else
        {
            WriteLog("GetOneJpegImg:: allocate memory failed.");
        }
    }
    else
    {
        WriteLog("GetOneJpegImg the image is not ready.");
    }
    LeaveCriticalSection(&m_csResult);
    WriteLog("GetOneJpegImg:: end.");

    return bRet;
}

void Camera6467_plate::SendConnetStateMsg(bool isConnect)
{
    //if (m_hWnd == NULL)
    //	return;

    if (isConnect)
    {
        //EnterCriticalSection(&m_csFuncCallback);
        //if (g_ConnectStatusCallback)
        //{
        //    LeaveCriticalSection(&m_csFuncCallback);
        //    //char chIP[32] = { 0 };
        //    //sprintf_s(chIP, "%s", m_strIP.c_str());
        //    //g_ConnectStatusCallback(m_iIndex, 0, g_pUser);
        //}
        //else
        //{
        //    LeaveCriticalSection(&m_csFuncCallback);
        //}
        if (m_hWnd)
        {
            EnterCriticalSection(&m_csFuncCallback);
            ::PostMessage(m_hWnd, m_iConnectMsg, NULL, NULL);
            LeaveCriticalSection(&m_csFuncCallback);
        }

    }
    else
    {
        //EnterCriticalSection(&m_csFuncCallback);
        //if (g_ConnectStatusCallback)
        //{
        //    LeaveCriticalSection(&m_csFuncCallback);
        //    char chIP[32] = { 0 };
        //    sprintf_s(chIP, "%s", m_strIP.c_str());
        //    //g_ConnectStatusCallback(m_iIndex, -100, g_pUser);
        //}
        //else
        //{
        //    LeaveCriticalSection(&m_csFuncCallback);
        //}
        if (m_hWnd)
        {
            EnterCriticalSection(&m_csFuncCallback);
            ::PostMessage(m_hWnd, m_iDisConMsg, NULL, NULL);
            LeaveCriticalSection(&m_csFuncCallback);
        }
    }
}

void Camera6467_plate::SetConnectStatus_Callback(void* func, void* pUser, int TimeInterval)
{
    EnterCriticalSection(&m_csFuncCallback);
    //g_ConnectStatusCallback = (CBFun_GetDevStatus)func;
    g_pUser = pUser;
    m_iTimeInvl = TimeInterval;
    LeaveCriticalSection(&m_csFuncCallback);
}

bool Camera6467_plate::GetResultComplete()
{
    bool bFinish = false;
    EnterCriticalSection(&m_csResult);
    bFinish = m_bResultComplete;
    LeaveCriticalSection(&m_csResult);
    return bFinish;
}

CameraResult* Camera6467_plate::GetOneResult()
{
    CameraResult* tempResult = NULL;
    //EnterCriticalSection(&m_csResult);	
    //if (m_ResultList.size() > 0)
    //{
    //	tempResult = m_ResultList.front();
    //	m_ResultList.pop_front();
    //}
    //LeaveCriticalSection(&m_csResult);

    if (GetResultComplete())
    {
        EnterCriticalSection(&m_csResult);
        tempResult = new CameraResult(*m_BufferResult);
        LeaveCriticalSection(&m_csResult);
    }
    return tempResult;
}

#ifdef USE_VIDEO
void Camera6467_plate::StartPlayVideoByChannel(int iChannelID, const HWND winHandle)
{
    if (iChannelID == 0)
    {
        StartPlayVideo(iChannelID, m_hPlayFirstH264, winHandle);
    }
    else
    {
        StartPlayVideo(iChannelID, m_hPlaySecondh264, winHandle);
    }
}


int Camera6467_plate::StopPlayVideoByChannel(int iChannelID)
{
    if (iChannelID == 0)
    {
        WriteLog("StopPlayVideoByChannel 0, begin.");
        StopPlayVideo(m_hPlayFirstH264);
    }
    else if (iChannelID == 1)
    {
        WriteLog("StopPlayVideoByChannel 1, begin.");
        StopPlayVideo(m_hPlaySecondh264);
    }
    WriteLog("StopPlayVideoByChannel, end.");
    return 0;
}

void* Camera6467_plate::GetVideoHandleByChannel(int iChannerlID)
{
    if (iChannerlID == 0)
    {
        return m_hPlayFirstH264;
    }
    else
    {
        return m_hPlaySecondh264;
    }
}

int Camera6467_plate::GetChannelIDByHandle(void* handle)
{
    if (handle == NULL)
    {
        return -1;
    }
    else if (handle == m_hPlayFirstH264)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

bool Camera6467_plate::TakeOnePictureFromVedio(int channelID, CameraIMG& camImg, int iImgType /*= 0*/)
{
    bool bRet = false;

    if (m_pCaptureImg == NULL)
    {
        m_pCaptureImg = new BYTE[MAX_IMG_SIZE];
    }
    if (m_pCaptureImg)
    {
        memset(m_pCaptureImg, 0, MAX_IMG_SIZE);
        int iLength = MAX_IMG_SIZE;
        int iWidth = 1920;
        int iHeight = 1080;
        int iTryTime = 5;
        while (iTryTime--)
        {
            if (channelID == 0 && m_hPlayFirstH264)
            {
                bRet = H264_GetOneBmpImg(m_hPlayFirstH264, m_pCaptureImg, iLength, iWidth, iHeight);
            }
            else if (channelID == 1 && m_hPlaySecondh264)
            {
                bRet = H264_GetOneBmpImg(m_hPlaySecondh264, m_pCaptureImg, iLength, iWidth, iHeight);
            }
            else
            {
                WriteLog("TakeOnePictureFromVedio , 参数错误.");
                break;
            }
            //bRet = H264_GetOneImg( m_hPlayH264, m_pCaptureImg, iLength, iWidth, iHeight );
            if (bRet)
            {
                break;
            }
            Sleep(50);
        }
        if (bRet)
        {
            if (iImgType == 0)
            {
                //BMP format
                if (camImg.pbImgData == NULL)
                {
                    camImg.pbImgData = new BYTE[MAX_IMG_SIZE];
                }
                if (camImg.pbImgData)
                {
                    memset(camImg.pbImgData, 0, MAX_IMG_SIZE);
                    memcpy(camImg.pbImgData, m_pCaptureImg, iLength);
                    camImg.dwImgSize = iLength;
                    camImg.wImgWidth = iWidth;
                    camImg.wImgHeight = iHeight;
                }
                else
                {
                    WriteLog("iput image data is null.");
                }
            }
            else
            {
                //JPEG format
                if (m_pTempBig == NULL)
                {
                    m_pTempBig = new BYTE[MAX_IMG_SIZE];
                }
                if (m_pTempBig)
                {
                    memset(m_pTempBig, 0, MAX_IMG_SIZE);
                    size_t iDestLength = MAX_IMG_SIZE;
                    bool bScale = Tool_Img_ScaleJpg(m_pCaptureImg, iLength, m_pTempBig, &iDestLength, iWidth, iHeight, 80);
                    if (bScale)
                    {
                        WriteLog("Img_ScaleJpg success.");
                        if (camImg.pbImgData == NULL)
                        {
                            camImg.pbImgData = new BYTE[MAX_IMG_SIZE];
                        }
                        if (camImg.pbImgData)
                        {
                            memset(camImg.pbImgData, 0, MAX_IMG_SIZE);
                            memcpy(camImg.pbImgData, m_pTempBig, iDestLength);
                            camImg.dwImgSize = iDestLength;
                            camImg.wImgWidth = iWidth;
                            camImg.wImgHeight = iHeight;
                        }
                        else
                        {
                            WriteLog("iput image data is null.");
                        }
                    }
                    else
                    {
                        memset(camImg.pbImgData, 0, MAX_IMG_SIZE);
                        memcpy(camImg.pbImgData, m_pCaptureImg, iLength);
                        camImg.dwImgSize = iLength;
                        camImg.wImgWidth = iWidth;
                        camImg.wImgHeight = iHeight;
                        WriteLog("Img_ScaleJpg failed, use default");
                    }
                }
                else
                {
                    WriteLog("Ig_funcBigImg_OSD_Callback =null or  m_pTempBig = null.");
                }
            }
        }
        else
        {
            WriteLog("H264_GetOneImg failed.");
        }
    }
    else
    {
        WriteLog("m_CaptureImg = null");
    }

    return bRet;
}

int Camera6467_plate::GetChanelIDByWinHandle(void* handle)
{
    if (handle == NULL)
    {
        return -1;
    }
    if (handle == m_hFirstWinHandle)
    {
        return 0;
    }
    else if (handle == m_hSecondWinHandle)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

void Camera6467_plate::recordChannelWinHandle(int channelID, void* handle)
{
    if (channelID == 0)
    {
        m_hFirstWinHandle = handle;
    }
    else
    {
        m_hSecondWinHandle = handle;
    }
}
#endif


void Camera6467_plate::SetResultComplete(bool bfinish)
{
    EnterCriticalSection(&m_csResult);
    m_bResultComplete = bfinish;
    LeaveCriticalSection(&m_csResult);
}

int Camera6467_plate::GetTimeInterval()
{
    int iTimeInterval = 1;
    EnterCriticalSection(&m_csFuncCallback);
    iTimeInterval = m_iTimeInvl;
    LeaveCriticalSection(&m_csFuncCallback);
    return iTimeInterval;
}

void Camera6467_plate::CheckStatus()
{
    int iLastStatus = -1;
    INT64 iLastTick = 0, iCurrentTick = 0;
    int iFirstConnctSuccess = -1;

    while (!GetCheckThreadExit())
    {
        Sleep(50);
        iCurrentTick = GetTickCount();
        int iTimeInterval = GetTimeInterval();
        if ((iCurrentTick - iLastTick) >= (iTimeInterval * 1000))
        {
            int iStatus = GetCamStatus();
            if (iStatus != iLastStatus)
            {
                if (iStatus == 0)
                {
                    //if (iStatus != iLastStatus)
                    //{
                    //	pThis->SendConnetStateMsg(true);
                    //}
                    //SendConnetStateMsg(true);
                    WriteLog("设备连接正常.");
                    iFirstConnctSuccess = 0;
                }
                else
                {
                    //SendConnetStateMsg(false);
                    WriteLog("设备连接失败, 尝试重连");

                    if (iFirstConnctSuccess == -1)
                    {
                        //pThis->ConnectToCamera();
                    }
                }
            }
            iLastStatus = iStatus;

            iLastTick = iCurrentTick;
        }
    }
}
