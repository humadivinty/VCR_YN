// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 VCR_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// VCR_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef VCR_EXPORTS
#define VCR_API  EXTERN_C  __declspec(dllexport)  
#else
#define VCR_API EXTERN_C   __declspec(dllimport) 
#endif

#define VCR_DECL __stdcall

//************************************
// Method:        InitAutoVehType
// Describe:        初始化设备
// FullName:      InitAutoVehType
// Access:          public 
// Returns:         BOOL
// Returns Describe: TRUE 成功 FALSE 失败
// Parameter:    LPCSTR lpszConnPara1  连接参数1。如果通过网络连接，连接参数的形式可能是IP地址加网络端口号，
//                                                          如“192.168.0.1:9000”。如果通过串口连接，连接参数可能为端口号，如“COM1”或“1”
//
// Parameter:    LPCSTR lpszConnPara2  连接参数2。一般为空字符串，仅用于需要配置额外参数的设备
//************************************
VCR_API  BOOL VCR_DECL InitAutoVehType(LPCSTR lpszConnPara1, LPCSTR lpszConnPara2);

//************************************
// Method:        RegisterMessage
// Describe:        注册车型识别通知消息和接收识别消息的窗口句柄
// FullName:      RegisterMessage
// Access:          public 
// Returns:         int 
// Returns Describe:  0 成功，其他 失败
// Parameter:    HWND hWnd    接收消息的窗口句柄
// Parameter:    UINT nMsgID    用户自定义消息，当准备好车型识别结果后，动态库向指定窗口发送此消息,通知有车型识别结果
// 注：如无相关的接口功能，需保留本接口，并保证返回值为0。
//************************************
VCR_API   int VCR_DECL RegisterMessage(HWND hWnd, UINT nMsgID);

//************************************
// Method:        GetVehType
// Describe:        取得一条车型识别结果
// FullName:      GetVehType
// Access:          public 
// Returns:         int 
// Returns Describe:  返回0或车型值;  1~4代表 客1~客4
//                                                      11~15代表 货1~货5
//                                                      0  车型未知
//
// Parameter:    int nIndex    车型识别结果的序号，取值为0到255。缺省值为-1，表示取当前车辆
// 注：车型被车道软件取出后，队列中的车型结果由设备动态库自行删除，车道软件不做处理。
//************************************
VCR_API   int VCR_DECL GetVehType(int nIndex = -1);

//************************************
// Method:        ExitAutoVeh
// Describe:        关闭设备
// FullName:      ExitAutoVeh
// Access:          public 
// Returns:         void 
// Returns Describe:
//************************************
VCR_API   void VCR_DECL ExitAutoVeh();

//************************************
// Method:        DelVehType
// Describe:        删除最早的一条车型数据，并返回车型值
// FullName:      DelVehType
// Access:          public 
// Returns:         int 
// Returns Describe:  返回0或车型值
// 注：如无相关的接口功能，需保留本接口，并保证返回值为0。
//************************************
VCR_API   int VCR_DECL DelVehType();

//************************************
// Method:        FlushQueue
// Describe:         清空识别设备的所有已识别的车型数据
// FullName:      FlushQueue
// Access:          public 
// Returns:         int 
// Returns Describe:  删除的车型数量
// 注：如无相关的接口功能，需保留本接口，并保证返回值为0。
//************************************
VCR_API   int  VCR_DECL FlushQueue(int index);

//************************************
// Method:        GetQueueLength
// Describe:        取得已经识别的车型数据的数量
// FullName:      GetQueueLength
// Access:          public 
// Returns:         int 
// Returns Describe:  车型数据的数量
// 注：如无相关的接口功能，需保留本接口，并保证返回值为0。
//************************************
VCR_API   int VCR_DECL GetQueueLength();

//************************************
// Method:        IsOccupied
// Describe:        判断是当前时刻，光栅是否有车
// FullName:      IsOccupied
// Access:          public 
// Returns:         int 
// Returns Describe:  0:无车 ， 非0 :有车
//************************************
VCR_API   int VCR_DECL IsOccupied();