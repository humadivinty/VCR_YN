unit VehType;

interface
uses
  AbstractDevUnit, Windows;
type
  // 初始化
  Func_VCR_InitDev = function(lpszConnPara1, lpszConnPara2 : PChar):Boolean;stdcall;
  //取得一条车型识别结果。
  Func_VCR_GetVehType = function(nIndex:Integer):Integer;stdcall;
  //关闭设备
  Func_VCR_CloseDev  = procedure();stdcall;
  //注册回调函数
  Func_VCR_RegMessage = function(hHandle:HWND;nDataMsgID:UINT):Integer;stdcall;
  //取得队列长度
  Func_VCR_GetQueueLength = function():Integer;stdcall;
  //清空队列
  Func_VCR_ClearQueue = function(nIndex:Integer):Integer;stdcall;
  //删除车型识别结果
  Func_VCR_DelVehType = function():Integer;stdcall;
  Func_VCR_GetCar = function():Integer;stdcall;

  TVehTypeVCR = class(TAbstractDev)
  protected
    function LoadDriver():Boolean; override;
    procedure ReleaseDriver();override;
  public
    function StartAVTDev():Boolean;
    function GetVehType(const nIndex :Integer = -1 ):Integer;
    function RemoveCurVeh():Integer;
    function GetQueueLength():Integer;
    function ClearQueue():Integer;
    function RegMessage(hWnd:HWND;nMsgID:UINT):Integer;
    function VehTypeGetCar :Integer;
  public
	  function StartDev():Boolean; override;
	  procedure CloseDev();override;
  public
    constructor Create; 
    destructor Destroy; override;
  published

  end;

implementation

uses
  LaneUtilUnit, SysUtils, LaneTypeUnit;

var
  m_bDriverLoaded: Boolean = FALSE;
  m_hLibModule: HMODULE = 0;
  VCR_InitDev: Func_VCR_InitDev =nil;
  VCR_GetVehType: Func_VCR_GetVehType =nil;
  VCR_CloseDev: Func_VCR_CloseDev =nil;
  VCR_RegMessage: Func_VCR_RegMessage =nil;
  VCR_GetQueueLength: Func_VCR_GetQueueLength =nil;
  VCR_ClearQueue: Func_VCR_ClearQueue =nil;
  VCR_DelVehType: Func_VCR_DelVehType =nil;
  VCR_GetCar: Func_VCR_GetCar =nil;

procedure TVehTypeVCR.CloseDev;
begin
  inherited;

  if not m_bDriverLoaded then exit;
  VCR_CloseDev();
end;

constructor TVehTypeVCR.Create;
begin
  inherited;
  
end;

destructor TVehTypeVCR.Destroy;
begin
  inherited;
  
end;

function TVehTypeVCR.StartDev : Boolean;
var
  bret : Boolean;
begin
  Result := false;
  if (not m_bDriverLoaded) then
  begin
    Exit;
  end;
  if(not VCR_InitDev( PChar(FConnStr1), PChar(FConnStr2))) then
  begin
    ChangeStatus(DEV_STATUS_CommErr);
    Logger.Error(Format('打开车型识别设备失败，System Error Code=%d',
        [Windows.GetLastError()]));
    Exit;
  end;
  Result := True;
  ChangeStatus(DEV_STATUS_OK);
  Logger.Error('打开车型识别设备成功.');
end;

function TVehTypeVCR.LoadDriver :Boolean;
begin
  Result := False;
  if (m_bDriverLoaded) then
	begin
		Result := True;
    Exit;
	end;

	m_hLibModule := LoadLibrary(PChar(FDriverFileName));
	if (m_hLibModule = 0) then
	begin
		Logger.Error(Format('加载车型识别动态库[%s]失败，System Error Code=%d',
        [FDriverFileName, Windows.GetLastError()]));
		Exit;
	end;

	VCR_InitDev := Func_VCR_InitDev(GetProcAddress(m_hLibModule,'InitAutoVehType'));
	VCR_GetVehType := Func_VCR_GetVehType(GetProcAddress(m_hLibModule,'GetVehType'));
	VCR_CloseDev := Func_VCR_CloseDev(GetProcAddress(m_hLibModule,'ExitAutoVeh'));
  VCR_RegMessage := Func_VCR_RegMessage(GetProcAddress(m_hLibModule,'RegisterMessage'));
	VCR_GetQueueLength := Func_VCR_GetQueueLength(GetProcAddress(m_hLibModule,'GetQueueLength'));
	VCR_ClearQueue := Func_VCR_ClearQueue(GetProcAddress(m_hLibModule,'FlushQueue'));
  VCR_DelVehType := Func_VCR_DelVehType(GetProcAddress(m_hLibModule,'DelVehType'));
  VCR_GetCar := Func_VCR_GetCar(GetProcAddress(m_hLibModule,'IsOccupied'));
 { if (not Assigned(VCR_InitDev)) then
  begin
    Exit;
  end;
  if (not Assigned(VCR_GetVehType)) then
  begin
    Exit;
  end;
  if (not Assigned(VCR_CloseDev)) then
  begin
    Exit;
  end;
  if (not Assigned(VCR_RegMessage)) then
  begin
    Exit;
  end;
  if (not Assigned(VCR_GetQueueLength)) then
  begin
    Exit;
  end;
  if (not Assigned(VCR_ClearQueue)) then
  begin
    Exit;
  end;
  if (not Assigned(VCR_DelVehType)) then
  begin
    Exit;
  end; }
	if ((not Assigned(VCR_InitDev))or
		 (not Assigned(VCR_GetVehType)) or
     (not Assigned(VCR_CloseDev)) or
     (not Assigned(VCR_RegMessage)) or
		 (not Assigned(VCR_GetQueueLength)) or
		 (not Assigned(VCR_ClearQueue))or
     (not Assigned(VCR_DelVehType))or
     (not Assigned(VCR_GetCar))) then
	begin
    Logger.Error(Format('获取车型识别动态库[%s]中的一个或多个函数错误！',
        [FDriverFileName]));
    FreeLibrary(m_hLibModule);
		m_hLibModule := 0;
		Exit;
	end;
	m_bDriverLoaded := TRUE;
	Result := True;
end;

procedure TVehTypeVCR.ReleaseDriver();
begin
  inherited;

  if not m_bDriverLoaded then exit;
  if m_hLibModule = 0 then Exit;

  m_bDriverLoaded := FALSE;

	// 函数指针置为nil
	VCR_InitDev :=nil;
	VCR_GetVehType :=nil;
	VCR_CloseDev :=nil;
	VCR_RegMessage :=nil;
	VCR_GetQueueLength :=nil;
	VCR_ClearQueue :=nil;
	VCR_DelVehType :=nil;

	// 释放动态库句柄   try防止Dll文件错误
  try
    FreeLibrary(m_hLibModule);
  except
  end;
	m_hLibModule := 0;
end;  

function TVehTypeVCR.StartAVTDev():Boolean ;
begin
  Result := false;
  if (not m_bDriverLoaded) then
  begin
    Exit;
  end;
//  Logger.Error(Format('') ;
  if(not VCR_InitDev( PChar(FConnStr1), PChar(FConnStr2))) then
  begin
    ChangeStatus(DEV_STATUS_CommErr);
    Logger.Error(Format('打开车型识别设备失败，System Error Code=%d',
        [Windows.GetLastError()]));
    Exit;
  end;
  Result := True;
end;

function TVehTypeVCR.GetVehType(const nIndex :Integer):Integer;
begin
  Result := 0;
  if (not m_bDriverLoaded) then
  begin
    Exit;
  end;
  Result := VCR_GetVehType(nIndex);
end;

function TVehTypeVCR.RemoveCurVeh():Integer;
begin
  Result := 0;
  if (not m_bDriverLoaded) then
  begin
    Exit;
  end;
  Result := VCR_DelVehType;
end;

function TVehTypeVCR.GetQueueLength:Integer ;
begin
  Result := 0;
  if (not m_bDriverLoaded) then
  begin
    Exit;
  end;
  Result := VCR_GetQueueLength;
end;

function TVehTypeVCR.ClearQueue :Integer;
begin
  Result := 0;
  if (not m_bDriverLoaded) then
  begin
    Exit;
  end;
  Result := VCR_ClearQueue(0); 

end;

function  TVehTypeVCR.RegMessage(hWnd:HWND;nMsgID:UINT):Integer;
begin
  Result := 0;;
  if (not m_bDriverLoaded) then
  begin
    Exit;
  end;
  Result := VCR_RegMessage(hWnd,nMsgID);
end;
function TVehTypeVCR.VehTypeGetCar :Integer;
begin
  Result := 1;
  if (not m_bDriverLoaded) then
  begin
    Exit;
  end;
  Result := VCR_GetCar;

end;
end.
