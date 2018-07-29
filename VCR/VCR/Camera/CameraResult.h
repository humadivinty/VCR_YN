#ifndef CAMERRESULT
#define CAMERRESULT

/* 识别结果大图类型定义 */
#define RECORD_BIGIMG_BEST_SNAPSHOT			0x0001	/**< 最清晰识别图 */
#define RECORD_BIGIMG_LAST_SNAPSHOT			0x0002	/**< 最后识别图 */
#define RECORD_BIGIMG_BEGIN_CAPTURE			0x0003	/**< 开始抓拍图 */
#define RECORD_BIGIMG_BEST_CAPTURE			0x0004	/**< 最清晰抓拍图 */
#define RECORD_BIGIMG_LAST_CAPTURE			0x0005	/**<  最后抓拍图 */

#define  MAX_IMG_SIZE 10*1024*1024

#define UNKOWN_TYPE 0
#define BUS_TYPE_1 1
#define BUS_TYPE_2 2
#define BUS_TYPE_3 3
#define BUS_TYPE_4 4
#define BUS_TYPE_5 5
#define TRUCK_TYPE_1 11
#define TRUCK_TYPE_2 12
#define TRUCK_TYPE_3 13
#define TRUCK_TYPE_4 14
#define TRUCK_TYPE_5 15

class CameraIMG
{
public:
	CameraIMG();
	CameraIMG(const CameraIMG& CaIMG);
	~CameraIMG();
	unsigned long wImgWidth;
	unsigned long wImgHeight;
	DWORD dwImgSize;
	unsigned long  wImgType;
	char chSavePath[256];
	PBYTE pbImgData;

	CameraIMG& operator = (const CameraIMG& CaIMG);
};

class CameraResult
{
public:

	CameraResult();
	CameraResult(const CameraResult& CaRESULT);
	~CameraResult();

	DWORD dwCarID;
	int iDeviceID;
	int iPlateColor;
	int iPlateTypeNo;
	DWORD64 dw64TimeMS;
	int iSpeed;
	int iResultNo;
	int iVehTypeNo;		//车型代码: 客1--1 。。。客4--4， 货1--5  。。。货4--8
	int iVehBodyColorNo;	
	int iVehBodyDeepNo;	
	int iAreaNo;
	int iRoadNo;
	int iLaneNo;
	int iDirection;
	int iWheelCount;		//轮数
	long iAxletreeCount;		//轴数
	int iAxletreeGroupCount;//轴组数
	int iAxletreeType;		//轴型
    int iReliability;       //可信度
	float fVehLenth;			//车长
	float fDistanceBetweenAxles;		//轴距
	float fVehHeight;		//车高
    float fConfidenceLevel;     //可信度
	bool bBackUpVeh;		//是否倒车


	char chDeviceIp[64];
	char chPlateNO[64];
	char chPlateColor[64];
	char chListNo[256];
	char chPlateTime[256];
	char chSignStationID[256];
	char chSignStationName[256];
	char pcAppendInfo[10240];

	char chDeviceID[256];
	char chLaneID[256];

	CameraIMG CIMG_BestSnapshot;	/**< 最清晰识别图 */
	CameraIMG CIMG_LastSnapshot;	/**< 最后识别图 */
	CameraIMG CIMG_BeginCapture;	/**< 开始抓拍图 */
	CameraIMG CIMG_BestCapture;		/**< 最清晰抓拍图 */
	CameraIMG CIMG_LastCapture;		/**< 最后抓拍图 */
	CameraIMG CIMG_PlateImage;		/**< 车牌小图 */
	CameraIMG CIMG_BinImage;			/**< 车牌二值图 */

	CameraResult& operator = (const CameraResult& CaRESULT);

	friend bool SerializationResultToDisk(const char* chFilePath, const CameraResult& CaResult);
	friend bool SerializationFromDisk(const char* chFilePath, CameraResult& CaResult);

	friend bool SerializationAsConfigToDisk(const char* chFilePath, const CameraResult& CaResult);
	friend bool SerializationAsConfigFromDisk(const char* chFilePath, CameraResult& CaResult);
};

typedef struct _tagSafeModeInfo
{
	int iEableSafeMode;
	char chBeginTime[256];
	char chEndTime[256];
	int index;
	int DataInfo;
	_tagSafeModeInfo()
	{
		iEableSafeMode = 0;
		memset(chBeginTime, 0, sizeof(chBeginTime));
		memset(chEndTime, 0, sizeof(chEndTime));
		index = 0;
		DataInfo = 0;
	}
}_tagSafeModeInfo;

#endif