// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� VCR_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// VCR_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef VCR_EXPORTS
#define VCR_API  EXTERN_C  __declspec(dllexport)  
#else
#define VCR_API EXTERN_C   __declspec(dllimport) 
#endif

#define VCR_DECL __stdcall

//************************************
// Method:        InitAutoVehType
// Describe:        ��ʼ���豸
// FullName:      InitAutoVehType
// Access:          public 
// Returns:         BOOL
// Returns Describe: TRUE �ɹ� FALSE ʧ��
// Parameter:    LPCSTR lpszConnPara1  ���Ӳ���1�����ͨ���������ӣ����Ӳ�������ʽ������IP��ַ������˿ںţ�
//                                                          �硰192.168.0.1:9000�������ͨ���������ӣ����Ӳ�������Ϊ�˿ںţ��硰COM1����1��
//
// Parameter:    LPCSTR lpszConnPara2  ���Ӳ���2��һ��Ϊ���ַ�������������Ҫ���ö���������豸
//************************************
VCR_API  BOOL VCR_DECL InitAutoVehType(LPCSTR lpszConnPara1, LPCSTR lpszConnPara2);

//************************************
// Method:        RegisterMessage
// Describe:        ע�ᳵ��ʶ��֪ͨ��Ϣ�ͽ���ʶ����Ϣ�Ĵ��ھ��
// FullName:      RegisterMessage
// Access:          public 
// Returns:         int 
// Returns Describe:  0 �ɹ������� ʧ��
// Parameter:    HWND hWnd    ������Ϣ�Ĵ��ھ��
// Parameter:    UINT nMsgID    �û��Զ�����Ϣ����׼���ó���ʶ�����󣬶�̬����ָ�����ڷ��ʹ���Ϣ,֪ͨ�г���ʶ����
// ע��������صĽӿڹ��ܣ��豣�����ӿڣ�����֤����ֵΪ0��
//************************************
VCR_API   int VCR_DECL RegisterMessage(HWND hWnd, UINT nMsgID);

//************************************
// Method:        GetVehType
// Describe:        ȡ��һ������ʶ����
// FullName:      GetVehType
// Access:          public 
// Returns:         int 
// Returns Describe:  ����0����ֵ;  1~4���� ��1~��4
//                                                      11~15���� ��1~��5
//                                                      0  ����δ֪
//
// Parameter:    int nIndex    ����ʶ��������ţ�ȡֵΪ0��255��ȱʡֵΪ-1����ʾȡ��ǰ����
// ע�����ͱ��������ȡ���󣬶����еĳ��ͽ�����豸��̬������ɾ�������������������
//************************************
VCR_API   int VCR_DECL GetVehType(int nIndex = -1);

//************************************
// Method:        ExitAutoVeh
// Describe:        �ر��豸
// FullName:      ExitAutoVeh
// Access:          public 
// Returns:         void 
// Returns Describe:
//************************************
VCR_API   void VCR_DECL ExitAutoVeh();

//************************************
// Method:        DelVehType
// Describe:        ɾ�������һ���������ݣ������س���ֵ
// FullName:      DelVehType
// Access:          public 
// Returns:         int 
// Returns Describe:  ����0����ֵ
// ע��������صĽӿڹ��ܣ��豣�����ӿڣ�����֤����ֵΪ0��
//************************************
VCR_API   int VCR_DECL DelVehType();

//************************************
// Method:        FlushQueue
// Describe:         ���ʶ���豸��������ʶ��ĳ�������
// FullName:      FlushQueue
// Access:          public 
// Returns:         int 
// Returns Describe:  ɾ���ĳ�������
// ע��������صĽӿڹ��ܣ��豣�����ӿڣ�����֤����ֵΪ0��
//************************************
VCR_API   int  VCR_DECL FlushQueue(int index);

//************************************
// Method:        GetQueueLength
// Describe:        ȡ���Ѿ�ʶ��ĳ������ݵ�����
// FullName:      GetQueueLength
// Access:          public 
// Returns:         int 
// Returns Describe:  �������ݵ�����
// ע��������صĽӿڹ��ܣ��豣�����ӿڣ�����֤����ֵΪ0��
//************************************
VCR_API   int VCR_DECL GetQueueLength();

//************************************
// Method:        IsOccupied
// Describe:        �ж��ǵ�ǰʱ�̣���դ�Ƿ��г�
// FullName:      IsOccupied
// Access:          public 
// Returns:         int 
// Returns Describe:  0:�޳� �� ��0 :�г�
//************************************
VCR_API   int VCR_DECL IsOccupied();