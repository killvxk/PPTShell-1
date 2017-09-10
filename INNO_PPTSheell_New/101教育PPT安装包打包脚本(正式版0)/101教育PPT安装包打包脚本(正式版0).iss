//
#define DS_APP_TITLE "101教育PPT"
//APP名称
#define DS_APP_NAME "101教育PPT"
//安装目录名
#define DS_DIR_NAME "101教育PPT"
//组名
#define DS_GROUP_NAME "101教育PPT"
//开发版名字
#define DS_APP_DEV_NAME "101教育PPT开发版"
//版本信息
#define DS_APP_VER_NAME "1.1.6.0"
//主软件名
#define DS_MAIN_FILE_NAME "101PPT.exe"
//打包文件目录名
#define DS_PACKAGE_DIR_NAME "101PPT"
//打包文件目录名开发版
#define DS_PACKAGE_DEV_DIR_NAME "101PPT开发版"
//程序互斥名
#define MyProgramsMutexName "PPTShell_Install”
//安装包升级程序
#define MyUpdateAppName "Update.exe"
//软件所属公司
#define DS_COMPANY_Name  "华渔未来教育科技有限公司"
//官网
#define DS_URL  "http://ppt.101.com/"
//卸载程序名称
#define DS_UNINSTALL_NAME  "卸载101教育PPT"
//开发板程序名字
#define DS_UNINSTALL_DEV_NAME  "卸载101教育PPT开发版"
//官网快捷方式名称
#define DS_URL_NAME "101教育PPT官网"
//统计安装次数地址
#define DS_POST_SEVER_URL    "http://p.101.com/101ppt/pptEvent.php"
//注册表Uninstall路径
#define DS_UNINSTALL_REGEDIT_PATH    "Software\Microsoft\Windows\CurrentVersion\Uninstall\"
//注册表软件路径
#define DS_REGEDIT_APP_PATH "SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\"
//
#define DS_REGEDIT_USERCHOISE_PPT_PATH "Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\"
//快捷方式名称
#define DS_PINLINK_NAME "101教育PPT.lnk"
//快捷方式名称开发版
#define DS_PINLINK_DEV_NAME "101教育PPT开发版.lnk"
//版本标志0:正式版;1:开发版;
#define DS_SOFT_VERSIONS 0
//打包文件下官网文件
#define DS_URL_FILE "101教育PPT.url"


[Setup]
AppName={#DS_APP_NAME}
AppVerName={#DS_APP_VER_NAME}
AppPublisher={#DS_COMPANY_Name}
AppPublisherURL={#DS_URL}
AppSupportURL={#DS_URL}
AppUpdatesURL={#DS_URL}
AppVersion={#DS_APP_VER_NAME}
//DefaultDirName={pf}\{#DS_APP_NAME}
DefaultDirName={code:GetDefaultInstallPath}
DefaultGroupName={#DS_GROUP_NAME}
OutputBaseFilename={#DS_APP_NAME}{#DS_APP_VER_NAME}
Outputdir=.
AllowNoIcons=yes
AppMutex=MyProgramsMutexName
//压缩级别 算法
Compression=lzma2/ultra64
UninstallIconFile={tmp}\Uninstall.ico
SolidCompression=yes
SetupIconFile={tmp}\Setup.ico
Uninstallable=yes
DirExistsWarning=no
UninstallDisplayIcon=Uninstall.ico
UninstallDisplayName={#DS_APP_NAME}
PrivilegesRequired=admin
SignedUninstaller=no
UsePreviousAppDir=yes
DisableDirPage=yes
//DisableFinishedPage=yes
DisableProgramGroupPage=yes
DisableReadyMemo=yes
DisableStartupPrompt =yes
//DisableWelcomePage=yes
DisableReadyPage=yes
AppId={{F59653BA-036B-4E55-A19A-9395AE8F970B}


[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; OnlyBelowVersion: 0,8.1
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}";


[Icons]
;开始菜单快捷方式{code:GetDefaultInstallRoot}    
Name: "{group}\{#DS_APP_NAME}"; Filename: "{app}\{#DS_MAIN_FILE_NAME}"; WorkingDir: {app}; Check: StartMenu;
Name: "{group}\{#DS_UNINSTALL_NAME}"; Filename: "{app}\unins000.exe"; Parameters: /SILENT;
//Name: "{group}\{#DS_UNINSTALL_NAME}"; Filename: "{app}\unins000.exe";
//不能用{#DS_URL}，要依赖载体101教育PPT.url才能加载图标
Name: "{group}\{#DS_URL_NAME}"; Filename: "{app}\UninstallSkin\{#DS_URL_FILE}"; IconFilename: "{app}\UninstallSkin\Browser.ico"; Check: StartMenu;
;桌面快捷方式
Name: "{commondesktop}\{#DS_APP_NAME}"; Filename: "{app}\{#DS_MAIN_FILE_NAME}"; WorkingDir: {app}; Check: desktop;
//绑定任务栏
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#DS_APP_NAME}"; Filename: "{app}\{#DS_MAIN_FILE_NAME}"; Tasks: quicklaunchicon; Check: desktop;


[Dirs]
Name:"{app}\Plugin";
Name:"{app}\UninstallSkin";


//开机自启动注册表
[registry]
;Root:HKLM;Subkey:"SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths"; ValueType:string; ValueName:"101PPT"; ValueData:"{app}\101PPT.exe"; check:regedit;


[Files]
Source: {tmp}\*; DestDir: {tmp}; Flags: dontcopy solidbreak
Source: ".\Common\*"; DestDir: "{app}\UninstallSkin"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{tmp}\botva2.dll"; DestDir: "{app}\Plugin"; Flags: ignoreversion
Source: "{tmp}\InnoCallback.dll"; DestDir: "{app}\Plugin"; Flags: ignoreversion
Source: "{tmp}\KP.dll"; DestDir: {app}\Plugin; Flags: ignoreversion
Source: "{tmp}\psvince.dll"; DestDir: "{app}\Plugin"; Flags: ignoreversion
Source: "{tmp}\UpdateIcon.dll"; DestDir: {app}\Plugin; Flags: ignoreversion
Source: "{#DS_PACKAGE_DIR_NAME}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
//以上是包含文件夹的目录
//包含文件夹recursesubdirs createallsubdirs   卸载时不删除uninsneveruninstall      卸载时删除ignoreversion

[UninstallDelete]
//Type: files; Name: "{app}\plugins\Uninstall\botva2.dll"
//Name: "{app}\DuiLib.dll"; Type: filesandordirs
Name: "{app}\*.dll"; Type: filesandordirs
Name: "{app}\*.exe"; Type: filesandordirs
Name: "{app}\*.dat"; Type: filesandordirs
Name: "{app}\Skins"; Type: filesandordirs
Name: "{app}\Cache"; Type: filesandordirs
Name: "{app}\log"; Type: filesandordirs
Name: "{app}\Package"; Type: filesandordirs
Name: "{app}\temp"; Type: filesandordirs
//或者： Source: embedded\md5.exe; DestDir: {tmp}; Flags: overwritereadonly deleteafterinstall dontcopy

[Code]
type
  TBtnEventProc = procedure (h:HWND);
  TPBProc = function(h:hWnd;Msg,wParam,lParam:Longint):Longint;  //百分比

Const
  GUID = '{F59653BA-036B-4E55-A19A-9395AE8F970B}';
  //鼠标指向
  MyCursor1 = 101;
  MyCursor2 = 102;
  Radius  = 5;
  GWL_EXSTYLE = (-20);
  GCL_STYLE = (-26);
  CS_DROPSHADOW = $20000;
  //窗口移动
  WM_SYSCOMMAND = $0112;
  //卸载
  //WS_EX_APPWINDOW = $40000;
  //按钮
  BtnClickEventID      = 1;
  BtnMouseEnterEventID = 2;
  //botva2
  const WM_CLOSE=$0010;
  bidDelAll = 1;
  bidSkipAll = 2;
  //最小化
  SC_MINIMIZE = 61472;//61472;
  
      INTERNET_CONNECTION_MODEM=1;
  INTERNET_CONNECTION_LAN=2;
  INTERNET_CONNECTION_PROXY=4;
  INTERNET_CONNECTION_MODEM_BUSY=8;
  
  // 过程函数
  function ImgLoad(Wnd :HWND; FileName :PAnsiChar; Left, Top, Width, Height :integer; Stretch, IsBkg :boolean) :Longint;
  external 'ImgLoad@{tmp}\botva2.dll stdcall delayload';
  function ImgLoad1(Wnd: HWND; FileName: PAnsiChar; Left, Top, Width, Height: integer; Stretch, IsBkg: boolean) :Longint;
  external 'ImgLoad@{tmp}\botva2.dll stdcall delayload uninstallonly';

  function WrapBtnCallback(Callback: TBtnEventProc; ParamCount: Integer): Longword;
  external 'wrapcallback@{tmp}\innocallback.dll stdcall delayload';
  function WrapBtnCallback1(Callback: TBtnEventProc; ParamCount: Integer): Longword;
  external 'wrapcallback@{tmp}\innocallback.dll stdcall delayload uninstallonly';

  function BtnCreate(hParent:HWND; Left,Top,Width,Height:integer; FileName:PAnsiChar; ShadowWidth:integer; IsCheckBtn:boolean):HWND;
  external 'BtnCreate@{tmp}\botva2.dll stdcall delayload';
  function BtnCreate1(hParent:HWND; Left,Top,Width,Height:integer; FileName:PAnsiChar; ShadowWidth:integer; IsCheckBtn:boolean):HWND;
  external 'BtnCreate@{tmp}\botva2.dll stdcall delayload uninstallonly';

  procedure BtnSetText(h:HWND; Text:PAnsiChar);
  external 'BtnSetText@{tmp}\botva2.dll stdcall delayload';
  procedure BtnSetTextU(h:HWND; Text:PAnsiChar);
  external 'BtnSetText@{tmp}\botva2.dll stdcall delayload uninstallonly';

  procedure BtnSetFont(h:HWND; Font:Cardinal);
  external 'BtnSetFont@{tmp}\botva2.dll stdcall delayload';
  procedure BtnSetFontU(h:HWND; Font:Cardinal);
  external 'BtnSetFont@{tmp}\botva2.dll stdcall delayload uninstallonly';

  procedure BtnSetEvent(h:HWND; EventID:integer; Event:Longword);
  external 'BtnSetEvent@{tmp}\botva2.dll stdcall delayload';
  procedure BtnSetEvent1(h:HWND; EventID:integer; Event:Longword);
  external 'BtnSetEvent@{tmp}\botva2.dll stdcall delayload uninstallonly';

  procedure ImgSetPosition(img :Longint; NewLeft, NewTop, NewWidth, NewHeight :integer);
  external 'ImgSetPosition@files:botva2.dll stdcall';
  procedure ImgSetPositionU(img: Longint; NewLeft, NewTop, NewWidth, NewHeight: integer);
  external 'ImgSetPosition@{tmp}\botva2.dll stdcall delayload uninstallonly';

  function PBCallBack(P:TPBProc;ParamCount:integer):LongWord;
  external 'wrapcallback@files:innocallback.dll stdcall';
  function PBCallBackU(P: TPBProc; ParamCount: integer): LongWord;
  external 'wrapcallback@{tmp}\innocallback.dll stdcall delayload uninstallonly';

  procedure ImgSetVisiblePart(img:Longint; NewLeft, NewTop, NewWidth, NewHeight : integer);
  external 'ImgSetVisiblePart@files:botva2.dll stdcall';
  procedure ImgSetVisiblePart1(img:Longint; NewLeft, NewTop, NewWidth, NewHeight : integer);
  external 'ImgSetVisiblePart@{tmp}\botva2.dll stdcall delayload uninstallonly';

  function IsModuleLoaded(modulename: String ): Boolean;
  external 'IsModuleLoaded@files:psvince.dll stdcall';
  function IsModuleLoadedU(modulename: String ): Boolean;
  external 'IsModuleLoaded@{tmp}\psvince.dll stdcall delayload uninstallonly';

  procedure ImgApplyChanges(h:HWND);
  external 'ImgApplyChanges@{tmp}\botva2.dll stdcall delayload';
  procedure ImgApplyChanges1(h: HWND);
  external 'ImgApplyChanges@{tmp}\botva2.dll stdcall delayload uninstallonly';

  procedure CreateFormFromImage(h:HWND; FileName:PAnsiChar);
  external 'CreateFormFromImage@{tmp}\botva2.dll stdcall delayload';
  procedure CreateFormFromImageU(h:HWND; FileName:PAnsiChar);
  external 'CreateFormFromImage@{tmp}\botva2.dll stdcall delayload uninstallonly';
  
  // 连网
  function InternetGetConnectedState(var dwFlag: Integer; dwReserved: Integer): Boolean;
  external 'InternetGetConnectedState@winInet.dll stdcall';
  
  //进程查杀
  function GetPIDByName(pname:string):Integer;
  external 'GetPIDByName@files:KP.dll cdecl';
  
  function GetPIDByNameU(pname:string):Integer;
  external 'GetPIDByName@{tmp}\KP.dll cdecl delayload uninstallonly';

  function KillProcess(pid:DWORD):Integer;
  external 'KillProcess@files:KP.dll cdecl';
  
  function KillProcessU(pid:DWORD):Integer;
  external 'KillProcess@{tmp}\KP.dll cdecl delayload uninstallonly';
  
  procedure ImgSetVisibility(img :Longint; Visible :boolean);
  external 'ImgSetVisibility@{tmp}\botva2.dll stdcall delayload';

  procedure ImgSetTransparent(img:Longint; Value:integer);
  external 'ImgSetTransparent@{tmp}\botva2.dll stdcall delayload';

  procedure ImgRelease(img :Longint);
  external 'ImgRelease@{tmp}\botva2.dll stdcall delayload';

  procedure gdipShutdown;
  external 'gdipShutdown@{tmp}\botva2.dll stdcall delayload';

  procedure BtnSetVisibility(h:HWND; Value:boolean);
  external 'BtnSetVisibility@{tmp}\botva2.dll stdcall delayload';

  procedure BtnSetFontColor(h:HWND; NormalFontColor, FocusedFontColor, PressedFontColor, DisabledFontColor: Cardinal);
  external 'BtnSetFontColor@{tmp}\botva2.dll stdcall delayload';

  procedure BtnSetCursor(h:HWND; hCur:Cardinal);
  external 'BtnSetCursor@{tmp}\botva2.dll stdcall delayload';

  procedure BtnSetEnabled(h:HWND; Value:boolean);
  external 'BtnSetEnabled@{tmp}\botva2.dll stdcall delayload';

  function GetSysCursorHandle(id:integer):Cardinal;
  external 'GetSysCursorHandle@{tmp}\botva2.dll stdcall delayload';

  function BtnGetChecked(h:HWND):boolean;
  external 'BtnGetChecked@{tmp}\botva2.dll stdcall delayload';

  function SetWindowLong(Wnd: HWnd; Index: Integer; NewLong: Longint): Longint;
  external 'SetWindowLongA@user32.dll stdcall';

  function CallWindowProc(lpPrevWndFunc: Longint; hWnd: HWND; Msg: UINT; wParam, lParam: Longint): Longint;
  external 'CallWindowProcA@user32.dll stdcall';

  function SetLayeredWindowAttributes(hwnd:HWND; crKey:Longint; bAlpha:byte; dwFlags:longint ):longint;
  external 'SetLayeredWindowAttributes@user32 stdcall';

  function SetClassLong(hWnd: HWND; nlndex: integer; dwNewLong: integer ): integer;
  external 'SetClassLongA@user32 stdcall';

  function GetClassLong(IntPtr:hwnd;nIndex:integer ):integer;
  external 'GetClassLongA@user32 stdcall';

  function CreateRoundRectRgn(p1, p2, p3, p4, p5, p6: Integer): THandle;
  external 'CreateRoundRectRgn@gdi32 stdcall';

  function SetWindowRgn(hWnd: HWND; hRgn: THandle; bRedraw: Boolean): Integer;
  external 'SetWindowRgn@user32 stdcall';

  function ReleaseCapture(): Longint;
  external 'ReleaseCapture@user32.dll stdcall';

  function UpdateIcon(const hWnd: Integer; const exeFileName, exeIcon, IcoFileName: String; wlangID: DWORD): Boolean;
  external 'UpdateIcon@files:UpdateIcon.dll stdcall';

  //发送卸载原因消息
  function SendBuffSetup(msg:String; len:Integer): Integer;
  external 'SendBuff@{tmp}\uninstallreport.dll stdcall delayload uninstallonly';
  
  procedure BtnSetChecked(h:HWND; Value:boolean);
  external 'BtnSetChecked@{tmp}\botva2.dll stdcall delayload';

  function LoadLibrary(lpFilename : String) : LongInt;
  external 'LoadLibraryW@kernel32.dll stdcall delayload';  //if Non Unicode Inno Setup, use LoadLibraryA
  
  function FreeLibrary(hModule : LongInt) : BOOL;
  external 'FreeLibrary@kernel32.dll stdcall delayload';
  
  function LoadString(hInstance:LongInt; uId:UINT; lpBuffer:String; nBufferMax:Integer) : Integer;
  external 'LoadStringW@user32.dll stdcall delayload';  //if Non Unicode Inno Setup, use LoadStringA


  //替换卸载程序的图标
  function UpdateUninstIcon(const IcoFileName: String): Boolean;
  begin
  //要替换图标的exe文件路径名称留空，则插件会自动替换掉Inno卸载程序的图标,其它参数类似！
  Result:= UpdateIcon(MainForm.Handle, '', '', IcoFileName, 0);
  //替换卸载图标
  end;

  
//根据版本获取安装目录
  function GetDefaultInstallRoot(Param: String): String;
  begin
 if ( {#DS_SOFT_VERSIONS} = 0 ) then
    begin
        Result := ExpandConstant('{#DS_APP_NAME}');
        end
    else
    begin

        Result := ExpandConstant('{#DS_APP_DEV_NAME}');
        end;
end;


  function GetDefaultInstallPath(Param: String): String;
  var
  szInstallPath : String;
  i : integer;
  szTemp : String;
begin
    if RegQueryStringValue(HKLM, '{#DS_UNINSTALL_REGEDIT_PATH}'+'101教育PPT', 'DisplayIcon', szInstallPath) then
      begin
     i := pos('101PPT.exe',szInstallPath)
     Delete(szInstallPath,i-1,11)
     Result := ExpandConstant(szInstallPath);
      end
    else
    begin
      Result := ExpandConstant('{pf}\{#DS_APP_NAME}');
    end;
end;


//根据版本获取卸载程序名称
function GetUninstallExeNa(Param: String): String;
begin
    if ( {#DS_SOFT_VERSIONS} = 0 ) then
    begin
      Result := ExpandConstant('{#DS_UNINSTALL_NAME}');
      end
    else
    begin
      Result := ExpandConstant('{#DS_UNINSTALL_DEV_NAME}');
      end;
end;


//根据版本选择文件夹压缩
  function GetPackDirByVer(Param: String): String;
begin
    if ( {#DS_SOFT_VERSIONS} = 0 ) then
        Result := ExpandConstant('{#DS_PINLINK_NAME}')
    else
        Result := ExpandConstant('{#DS_PINLINK_DEV_NAME}');
end;


//窗口圆角
procedure ShapeForm(aForm: TForm; edgeSize: integer);
var
  FormRegion:LongWord;
begin
  FormRegion:=CreateRoundRectRgn(0,0,aForm.Width+1,aForm.Height+1,edgeSize,edgeSize);
  SetWindowRgn(aForm.Handle,FormRegion,True);
end;


//判断连网
function InternetConnected():Boolean;
var
dwConnectionTypes:integer;
begin
dwConnectionTypes:=INTERNET_CONNECTION_MODEM+INTERNET_CONNECTION_LAN+INTERNET_CONNECTION_PROXY;
Result:=InternetGetConnectedState(dwConnectionTypes,0);
end;



//统计安装次数
function ReportInstall(): Boolean;
var
WinHttpReq: Variant;
btmp: Boolean;
begin
  Result := false;
  Result := InternetConnected;
  if  Result = true then
  begin
  
  // POST服务器 统计安装次数
  WinHttpReq := CreateOleObject('WinHttp.WinHttpRequest.5.1');
  btmp := WinHttpReq.Open('post', '{#DS_POST_SEVER_URL}', false);
  if  btmp = true then
  begin
  
     WinHttpReq.SetRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
    WinHttpReq.Send('<"eventId":1005,"eventTime":0,"params":[],"userId":0,"versionCode":"{#DS_APP_VER_NAME}","ver":0>');

    if WinHttpReq.Status <> 200 then
    begin
      Result := true;
    end
    else
    begin
      Result := false;
    end;
  end
 
  end
  else
  begin
    Result := false;
  end
end;


var
  //取消窗口
  CancelImage: Longint;
  CancelForm: TSetupForm;
  OKButton: TButton;
  
  //右上角按钮+取消窗口
  CloseBtn,CancelMinBtn,CancelCloseBtn: HWND;
  IconI:HWND;
  
  //最小化窗口按钮
  MinBtn:HWND;
  HasRun:HWND;
  
  //按钮
  OKBtn,CancelFormBtn:HWND;
  
  //窗口
  WizardFormImage: Longint;
  FormImage: Longint;
  gddh,pbImg,pbbgImg: Longint;
  
  XYFormImage: Longint;
  
  //按钮
  CancelBtn,backBtn,Startbtn,NextBtn,nextbtn1,Okbtn1,DirBrowseBtn,Finbtn: HWND;
  
  //勾选框
  A2Check,A3Check,A4Check,A5Check,A6Check,A7Check,A8Check: HWND;
  
  //文字标签
  Label1,Label2,Label3,Label4,Label5,Label6,Label7,Label8,Label9,Label20,Label21,Label23,LabelDir,Label22,Label10:tlabel;
  Label11,Label12,Label13:tlabel;
  Edit1:Tedit;
  WelTx2,PrLabel,PrLabel1:Tlabel;
  Xkxy,Yhty,NewYhty:TRichEditViewer;
  bOpenExe: boolean;                                                                      //立即体验标识
  WFButtonFont:tfont;                                                                     //进度条
  WFUnInsButtonFont:tfont; //卸载字体
  UninstallImage, UninstallIng: Longint;//卸载主窗口图片, 正在卸载窗口, 完成标识
  bUnInstall: boolean;//卸载标识
  UnProForm, UnStartForm, UFrame, Frame1, UProFrame, UnFinishedFrame, XYFormFrame: TForm;//卸载窗口
  UninstalledForm: TSetupForm;//完成页面
  XYFrom: TSetupForm;//协议页面
  labUning, labUning1: TLabel;//正在卸载
  //硬盘显示
  FreeMB,TotalMB: Cardinal;
  PBOldProcU, PBOldProc : Longint;
  //
  IsFrameDragging : boolean;
  dx,dy,dh1 : integer;
  Frame : TForm;
  
  IsInstallPathExit : boolean;
  //是否删除配置
  IsUninstallCfg:boolean;

  Form: TSetupForm;
  CheckListBox: TNewCheckListBox;
  AllButton, UnAllButton: TButton;
  CancelButton, DelButton: TButton;
  XYFromMinBtn, XYFromCancelBtn:HWND;
  StaticText: TNewStaticText;

  MsgForm: TSetupForm;
  MsgAllButton, MsgSkipAllButton: TButton;
  MsgCancelButton, MsgDelButton: TButton;

  DelAllReadOnly: Boolean;
  SkipAllReadOnly: Boolean;
    intUninstallMsg: String;//卸载发送消息整理
  CheckDirs, CheckFiles: Boolean;

  UnCloseButton: TButton;//关闭（卸载）
  UnConfirmButton: TButton;
  progressbgImg, progressbgImg1, PaoPaoImgU: Longint;//进度条
  btnUninstallClose, btnUninstallMin, checkAgree, btnUninstallYes, btnUninstallNo, btnUninstallFinish: HWND;//关闭，最小化，删除复选框, 卸载，取消，完成

  bReadTreatyCheck: boolean;//删除配置文件完成标识
  checkCause: array of HWND;//卸载原因
  OtherEdit: TRichEditViewer;//卸载原因编辑框
  CauseID: integer;//当前卸载原因ID
  CancelCloseButton: TButton;//关闭(取消对话框)


//安装进度百分比
function PBProc(h:hWnd;Msg,wParam,lParam:Longint):Longint;
var
  pr,i1,i2 : Extended;
  w : integer;
begin
  Result:=CallWindowProc(PBOldProc,h,Msg,wParam,lParam);
  if (Msg=$402) and (WizardForm.ProgressGauge.Position>WizardForm.ProgressGauge.Min) then begin
    i1:=WizardForm.ProgressGauge.Position-WizardForm.ProgressGauge.Min;
    i2:=WizardForm.ProgressGauge.Max-WizardForm.ProgressGauge.Min;
    pr:=i1*100/i2;
    PrLabel.Caption:='('+IntToStr((Round(pr)))+'%)';
    w:=Round(555*pr/100);
    ImgSetVisiblePart(pbImg,0,0,w,16);
    ImgSetPosition(pbImg,26,403,w,16);
    ImgApplyChanges(WizardForm.Handle);
  end;
end;


//卸载进度百分比
function PBProcU(h: hWnd; Msg, wParam, lParam:Longint): Longint;
var
  pr, i1, i2 : Extended;
  w: integer;
begin
  Result := CallWindowProc(PBOldProcU, h, Msg, wParam, lParam);
  if (Msg=$402) and (UninstallProgressForm.ProgressBar.Position > UninstallProgressForm.ProgressBar.Min) then
  begin
    i1 := UninstallProgressForm.ProgressBar.Position - UninstallProgressForm.ProgressBar.Min;
    i2 := UninstallProgressForm.ProgressBar.Max - UninstallProgressForm.ProgressBar.Min;
    pr := i1 * 100 / i2;
    labUning.Caption := '正在卸载' +'('+IntToStr((Round(pr)))+'%)';
    w := Round(555 * pr / 100);
    ImgSetVisiblePart1(progressbgImg,0,0,w,16);
    ImgSetPositionU(progressbgImg, 26,403,w,16);
    ImgApplyChanges1(UninstallProgressForm.Handle);
  end;
end;


//磁盘
function NumToStr(Float: Extended): string;
begin
  Result:=Format('%.2n', [Float]);
end;
function MbOrTB(Float: Extended): string;
begin
  if Float<1024 then Result:=NumToStr(Float)+' MB'
  else if (Float/1024)<1024 then Result:=NumToStr(Float/1024)+' GB'
  else if (Float/(1024*1024))<1024 then Result:=NumToStr(Float/(1024*1024))+' TB'
end;


//获取磁盘空余空间
procedure GetFreeSpaceCaption(Sender: TObject);
begin
  WizardForm.DirEdit.Text:=WizardForm.DirEdit.Text;
  if GetSpaceOnDisk(ExtractFileDrive(WizardForm.DirEdit.Text),True,FreeMB,TotalMB) then
  begin
    IsInstallPathExit := true;
    WelTx2.Font.Color :=$EFEDD5;
    WelTx2.Caption:= ExpandConstant('/可用空间：') + MbOrTB(FreeMB) ;
    BtnSetEnabled(nextbtn1,true);
    BtnSetEnabled(Startbtn,true);
  end else
  begin
    WelTx2.Font.Color:=$EFEDD5
    WelTx2.Caption:= '错误安装路径或者空间不足';
    if WelTx2.Caption='错误安装路径或者空间不足'then
    begin
     IsInstallPathExit := false;
    BtnSetEnabled(nextbtn1,false);
    BtnSetEnabled(Startbtn,false);
   end
 end
end;


//*************外框**************
procedure WizardMouseDown(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  ReleaseCapture
  SendMessage(WizardForm.Handle, WM_SYSCOMMAND, $F012, 0)
end;


procedure WizardFormMouseDown(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  IsFrameDragging:=True;
  dx:=X;
  dy:=Y;
end;


procedure UWizardFormMouseDown(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  IsFrameDragging:=True;
  dx:=X;
  dy:=Y;
end;


procedure UWizardFormMouseUp(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  IsFrameDragging:=False;
  //UFrame.Show;
  UnStartForm.Show;
end;


procedure UWizardFormMouseMove(Sender: TObject; Shift: TShiftState; X,Y: Integer);
begin
if IsFrameDragging then begin
    UnStartForm.Left:=UnStartForm.Left+X-dx;
    UnStartForm.Top:=UnStartForm.Top+Y-dy;
    UFrame.Left:=UnStartForm.Left-10;
    UFrame.Top:=UnStartForm.Top-10;
  end;
end;


procedure CancelWizardFormMouseDown(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  IsFrameDragging:=True;
  dx:=X;
  dy:=Y;
end;


procedure CancelWizardFormMouseUp(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  IsFrameDragging:=False;
  //Frame.Show;
  CancelForm.Show;
end;


procedure CancelWizardFormMouseMove(Sender: TObject; Shift: TShiftState; X,Y: Integer);
begin
if IsFrameDragging then begin
    CancelForm.Left:=CancelForm.Left+X-dx;
    CancelForm.Top:=CancelForm.Top+Y-dy;
    Frame1.Left:=CancelForm.Left-10;
    Frame1.Top:=CancelForm.Top-10;
  end;
end;


procedure UProgressWizardFormMouseDown(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  IsFrameDragging:=True;
  dx:=X;
  dy:=Y;
end;


procedure UProgressWizardFormMouseUp(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  IsFrameDragging:=False;
  UninstallProgressForm.Show;
end;


procedure UProgressWizardFormMouseMove(Sender: TObject; Shift: TShiftState; X,Y: Integer);
begin
if IsFrameDragging then begin
    UninstallProgressForm.Left:=UninstallProgressForm.Left+X-dx;
    UninstallProgressForm.Top:=UninstallProgressForm.Top+Y-dy;
    UProFrame.Left:=UninstallProgressForm.Left-10;
    UProFrame.Top:=UninstallProgressForm.Top-10;
  end;
end;


//卸载完成界面
procedure UFinishWizardFormMouseDown(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  IsFrameDragging:=True;
  dx:=X;
  dy:=Y;
end;


procedure UFinishWizardFormMouseUp(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  IsFrameDragging:=False;
  UninstalledForm.Show;
end;


procedure UFinishWizardFormMouseMove(Sender: TObject; Shift: TShiftState; X,Y: Integer);
begin
if IsFrameDragging then begin
    UninstalledForm.Left:=UninstalledForm.Left+X-dx;
    UninstalledForm.Top:=UninstalledForm.Top+Y-dy;
    UnFinishedFrame.Left:=UninstalledForm.Left-10;
    UnFinishedFrame.Top:=UninstalledForm.Top-10;
  end;
end;
  
  
//协议页面
procedure XYFormMouseDown(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  IsFrameDragging:=True;
  dx:=X;
  dy:=Y;
end;


procedure XYFormMouseUp(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  IsFrameDragging:=False;
  XYFrom.Show;
end;


procedure XYFormMouseMove(Sender: TObject; Shift: TShiftState; X,Y: Integer);
begin
if IsFrameDragging then begin
    XYFrom.Left:=XYFrom.Left+X-dx;
    XYFrom.Top:=XYFrom.Top+Y-dy;
    XYFormFrame.Left:=XYFrom.Left-10;
    XYFormFrame.Top:=XYFrom.Top-10;
  end;
end;


procedure frameUnInstFormMouseUp(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  UnStartForm.Show;
end;


procedure frameCancelFormMouseUp(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  CancelForm.Show;
end;


procedure frameUnInstedFormMouseUp(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  UninstalledForm.Show;
end;


  procedure frameXYFormMouseUp(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  XYFrom.Show;
end;


procedure frameFormMouseUp(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  WizardForm.Show;
end;


procedure CreateFrame;
begin
  IsFrameDragging:=False;
  Frame:=TForm.Create(nil);;
  Frame.BorderStyle:=bsNone;
  CreateFormFromImage(Frame.Handle,ExpandConstant('{tmp}\window0.png'));
  with TLabel.Create(Frame) do begin
    Parent:=Frame;
    AutoSize:=false;
    Left:=0;
    Top:=0;
    Width:=Frame.CLientWidth;
    Height:=Frame.ClientHeight;
    OnMouseDown:=@frameFormMouseUp;
  end;
  WizardForm.Left:=Frame.Left+10;
  WizardForm.Top:=Frame.Top+10;
  //Frame.Show;
  Frame.Hide;
end;


procedure UCreateFrame0;
begin
  IsFrameDragging:=False;
  UFrame:=TForm.Create(nil);
  UFrame.BorderStyle:=bsNone;
  CreateFormFromImageU(UFrame.Handle,ExpandConstant('{tmp}\window0.png'));
  with TLabel.Create(UFrame) do begin
    Parent:=UFrame;
    AutoSize:=False;
    Left:=0;
    Top:=0;
    Width:=UFrame.CLientWidth;
    Height:=UFrame.ClientHeight;
    OnMouseDown:=@frameUnInstFormMouseUp;
  end;

  UnStartForm.Left := UFrame.Left+10;
  UnStartForm.Top := UFrame.Top+10;

  UFrame.Show;
end;


// 取消对话框
procedure CancelFrame;
begin
  IsFrameDragging:=False;
  Frame1:=TForm.Create(nil);
  Frame1.BorderStyle:=bsNone;
  CreateFormFromImage(Frame1.Handle,ExpandConstant('{tmp}\window0.png'));
  with TLabel.Create(Frame1) do begin
    Parent:=Frame1;
    AutoSize:=False;
    Left:=0;
    Top:=0;
    Width:=Frame1.CLientWidth;
    Height:=Frame1.ClientHeight;
    OnMouseDown:=@frameCancelFormMouseUp;
  end;
  CancelForm.Left := Frame1.Left+10;
  CancelForm.Top := Frame1.Top+10;
  
  //Frame1.Show;
  Frame1.Hide;
end;


procedure UCreateProgressFrame;
begin

  IsFrameDragging:=False;
  UProFrame:=TForm.Create(nil);
  UProFrame.BorderStyle:=bsNone;
  CreateFormFromImageU(UProFrame.Handle,ExpandConstant('{tmp}\window0.png'));
  with TLabel.Create(UProFrame) do begin
    Parent:=UProFrame;
    AutoSize:=False;
    Left:=0;
    Top:=0;
    Width:=UProFrame.CLientWidth;
    Height:=UProFrame.ClientHeight;
    OnMouseDown:=@frameUnInstFormMouseUp;
  end;

  UninstallProgressForm.Left := UProFrame.Left+10;
  UninstallProgressForm.Top := UProFrame.Top+10;

  //UProFrame.Show;
  UProFrame.Hide;
end;


procedure UCreateFinshFrame;
begin

  IsFrameDragging:=False;
  UnFinishedFrame:=TForm.Create(nil);
  UnFinishedFrame.BorderStyle:=bsNone;
  CreateFormFromImageU(UnFinishedFrame.Handle,ExpandConstant('{tmp}\window0.png'));
  with TLabel.Create(UnFinishedFrame) do begin
    Parent:=UnFinishedFrame;
    AutoSize:=False;
    Left:=0;
    Top:=0;
    Width:=UnFinishedFrame.CLientWidth;
    Height:=UnFinishedFrame.ClientHeight;
    OnMouseDown:=@frameUnInstedFormMouseUp;
  end;

  UninstalledForm.Left := UnFinishedFrame.Left+10;
  UninstalledForm.Top := UnFinishedFrame.Top+10;

  UnFinishedFrame.Show;
end;
  
  
// 协议透明
procedure CreateXYFrame;
begin

  IsFrameDragging:=False;
  XYFormFrame:=TForm.Create(nil);
  XYFormFrame.BorderStyle:=bsNone;
  CreateFormFromImage(XYFormFrame.Handle,ExpandConstant('{tmp}\window0.png'));
  with TLabel.Create(XYFormFrame) do begin
    Parent:=XYFormFrame;
    AutoSize:=False;
    Left:=0;
    Top:=0;
    Width:=XYFormFrame.CLientWidth;
    Height:=XYFormFrame.ClientHeight;
    OnMouseDown:=@frameXYFormMouseUp;
  end;

  XYFrom.Left := XYFormFrame.Left+10;
  XYFrom.Top := XYFormFrame.Top+10;

  XYFormFrame.Show;
end;


// 卸载弹窗鼠标按下
procedure FormMouseDown(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  IsFrameDragging:=True;
  dx:=X;
  dy:=Y;
end;


//关闭
procedure ExitUninstallBtnClick(hBtn:HWND);
begin
UFrame.Hide;
    UnCloseButton.Click;

end;


//关闭
procedure ExitUninstallCancelBtnClick(hBtn:HWND);
begin
  UnStartForm.Close;
  UFrame.Close;
end;


procedure WizardFormMouseUp(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  IsFrameDragging:=False;
  WizardForm.Show;
end;


// 卸载弹窗鼠标弹起
procedure FormMouseUp(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  IsFrameDragging:=True;
  Form.Show;
end;


procedure WizardFormMouseMove(Sender: TObject; Shift: TShiftState; X,Y: Integer);
begin
  if IsFrameDragging then begin
    WizardForm.Left:=WizardForm.Left+X-dx;
    WizardForm.Top:=WizardForm.Top+Y-dy;
    Frame.Left:=WizardForm.Left-10;
    Frame.Top:=WizardForm.Top-10;
  end;
end;


// 卸载弹窗鼠标弹起
procedure FormMouseMove(Sender: TObject; Shift: TShiftState; X,Y: Integer);
begin
  if IsFrameDragging then begin
    Form.Left:=Form.Left+X-dx;
    Form.Top:=Form.Top+Y-dy;
    Frame.Left:=Form.Left-10;
    Frame.Top:=Form.Top-10;
  end;
end;


procedure WizardFormcc;
begin
    WizardForm.OnMouseDown:=@WizardFormMouseDown;
    WizardForm.OnMouseUp:=@WizardFormMouseUp;
    WizardForm.OnMouseMove:=@WizardFormMouseMove;
end;


// 卸载窗口
procedure Formcc;
begin
    Form.OnMouseDown:=@FormMouseDown;
    Form.OnMouseUp:=@FormMouseUp;
    Form.OnMouseMove:=@FormMouseMove;
end;


//最小化
procedure UnShowMin(hBtn:HWND);
begin
  PostMessage(UnStartForm.Handle,WM_SYSCOMMAND, SC_MINIMIZE,0);
end;


//*************外框**************
//***********第二页界面显示**************
procedure NextBtnClick(hBtn:HWND);

begin
WizardFormImage:=ImgLoad(WizardForm.Handle,ExpandConstant('{tmp}\window3c.png'),-10,-10,622,448,True,True);
WizardFormImage:=ImgLoad(WizardForm.Handle,ExpandConstant('{tmp}\DirImg.png'),53,110,445,36,True,True);
BtnSetVisibility(backBtn,true);BtnSetVisibility(NextBtn,false);
BtnSetVisibility(Startbtn,false);BtnSetVisibility(DirBrowseBtn,true);
BtnSetVisibility(MinBtn,true);
BtnSetVisibility(nextbtn1,true);
BtnSetVisibility(A2Check,true);
BtnSetVisibility(A3Check,false);BtnSetVisibility(A4Check,true);
BtnSetVisibility(A5Check,false);
Label2.Hide;Label3.Hide;
Edit1.show;
//LabelDir.Show;
Label4.show;
Label5.show;Label6.show;Label7.Hide;
Label8.hide;Label9.hide;
Label10.hide;
WelTx2.show;
Label20.Hide;
ImgApplyChanges(WizardForm.Handle);
end;


//返回按钮，返回第一页
procedure backBtnClick(hBtn:HWND);
begin
WizardFormImage:=ImgLoad(WizardForm.Handle,ExpandConstant('{tmp}\windowc.png'),-10, -10,622,448,True,True);
BtnSetVisibility(backBtn,false);BtnSetVisibility(Startbtn,true);
BtnSetVisibility(NextBtn,true);BtnSetVisibility(DirBrowseBtn,false);
BtnSetVisibility(MinBtn,True);BtnSetVisibility(nextbtn1,false);
WelTx2.hide;
Label2.show;Label3.show;
Edit1.hide;
//LabelDir.Hide;
Label4.hide;
Label8.hide;Label9.hide;
Label10.hide;
Label20.Show;
//Label21.Show;
Label5.hide;Label6.hide;Label7.hide;
BtnSetVisibility(A2Check,false);
BtnSetVisibility(A3Check,true);BtnSetVisibility(A4Check,false);
BtnSetVisibility(A5Check,false);
ImgApplyChanges(WizardForm.Handle)
end;


//协议确定按钮
procedure OkBtnClick(hBtn:HWND);
begin
Frame.Show;
WizardForm.Show;
XYFrom.Hide;
XYFormFrame.Hide;
end;


//全局取消按钮点击事件
procedure CancelbtnClick(hBtn:HWND);
begin
  bOpenExe := false;
  WizardForm.CancelButton.Click;
end;


//协议页面最小化按钮
procedure XYFromMinBtnOnClick(hBtn:HWND);
begin
SendMessage(XYFrom.Handle,WM_SYSCOMMAND,61472,0);
end;


procedure XYFromCancelBtnClick(hBtn:HWND);
begin
XYFormFrame.Hide;
XYFrom.Hide;
Frame.Show;
WizardForm.Show;
end;
  
//“许可协议”按钮点击石事件
procedure XYClick(Sender: TObject);
//var
//ErrorCode: Integer;
begin
  //ShellExec('open','http://www.xunlei.com/xieyi/xl7.html','','',SW_SHOWNORMAL,ewNoWait,ErrorCode);
  Frame.Hide;
  WizardForm.Hide;
  
//协议页面
XYFrom := CreateCustomForm();
XYFrom.Width := 602;
XYFrom.Height := 426;
XYFrom.BorderStyle := bsNone;

//圆角
ShapeForm(XYFrom, radius);

XYFrom.OnMouseDown := @XYFormMouseDown;
XYFrom.OnMouseUp := @XYFormMouseUp;
XYFrom.OnMouseMove := @XYFormMouseMove;

// 透明
CreateXYFrame;

Label23 := TLabel.Create(XYFrom);
Label23.Parent := XYFrom;
Label23.Caption := '{#DS_APP_NAME}';
Label23.Transparent := true;
Label23.Font.Size:=11
Label23.Font.Name:='微软雅黑'
Label23.Font.Style := [fsBold];
Label23.Font.Color:=$F4F2E0
Label23.Left := (15);
Label23.Top := (10);

Yhty:= TRichEditViewer.Create(XYFrom);
Yhty.Parent := XYFrom;
Yhty.ReadOnly:= true;
Yhty.SCROLLBARS:= ssVertical;
Yhty.Font.Size := 9
Yhty.SetBounds((25),(45), (470),(250));
Yhty.Lines.LoadFromFile(ExpandConstant('{tmp}\license.rtf'));
Yhty.SetBounds((-1000),(45), (470),(2));
Yhty.SetBounds((38),(45), (525),(300));

//框架
//UninstallIng:=ImgLoad1(UninstalledForm.Handle,ExpandConstant('{tmp}\windows1.png'),(-10), (-10), UninstalledForm.ClientWidth,UninstalledForm.ClientHeight,True,True);
XYFormImage:=ImgLoad(XYFrom.Handle,ExpandConstant('{tmp}\window3c.png'),-10, -10,622,448,True,True);

//确定
Okbtn:=BtnCreate(XYFrom.Handle,257,377,88,40,ExpandConstant('{tmp}\btn_ok.png'),1,False);
BtnSetEvent(Okbtn,BtnClickEventID,WrapBtnCallback(@OkbtnClick,1));

//最小化按钮
XYFromMinBtn:=BtnCreate(XYFrom.Handle,548,0,26,26,ExpandConstant('{tmp}\Btn_min.png'),3,False);
BtnSetEvent(XYFromMinBtn,BtnClickEventID,WrapBtnCallback(@XYFromMinBtnOnClick,1));

//关闭按钮
XYFromCancelBtn:=BtnCreate(XYFrom.Handle,574,0,26,26,ExpandConstant('{tmp}\btn_close.png'),1,False)
BtnSetEvent(XYFromCancelBtn,BtnClickEventID,WrapBtnCallback(@XYFromCancelBtnClick,1));
//BtnSetEvent(XYFromCancelBtn,BtnClickEventID,WrapBtnCallback(@CancelBtnClick,1));

ImgApplyChanges(XYFrom.Handle);
XYFrom.ShowModal;
end;


//确定卸载
procedure UnConfirmBtnOnClick(hBtn: HWND);
begin
   UnConfirmButton.Click;
end;


//协议勾选框点击事件
procedure A1onoff(hBtn:HWND);
begin
if  IsInstallPathExit then
begin
    if BtnGetChecked(A3Check) then
      begin
      Label9.hide;
      BtnSetEnabled(Startbtn,true);BtnSetEnabled(NextBtn,true);
      end  else
      begin
      Label9.hide;
      BtnSetEnabled(Startbtn,false);BtnSetEnabled(NextBtn,false);
      end
 end
end;
  
  
//删除快捷方式
procedure DelTempDesk();
var
  StartMenu:String;
  StartMenuUrl:String;
  StartMenuUninstall:String;
  StartMenuFolder:String;
  commondesktop:String;
  commondesktopUrl:String;
  userappdata:String;
  InstallLocation:String;
  
begin
  //删除开始菜单项
  
  if IsWin64 then
  begin
  RegQueryStringValue(HKLM64,'{#DS_UNINSTALL_REGEDIT_PATH}', 'Start Menu', StartMenu);
  RegQueryStringValue(HKLM64,'{#DS_UNINSTALL_REGEDIT_PATH}', 'Start Menu', StartMenuUrl);
  RegQueryStringValue(HKLM64,'{#DS_UNINSTALL_REGEDIT_PATH}', 'Start Menu Uninstall', StartMenuUninstall);
  RegQueryStringValue(HKLM64,'{#DS_UNINSTALL_REGEDIT_PATH}', 'Start Menu Folder', StartMenuFolder);
  RegQueryStringValue(HKLM64,'{#DS_UNINSTALL_REGEDIT_PATH}', 'commondesktop', commondesktop); //这个路径正确
  RegQueryStringValue(HKLM64,'{#DS_UNINSTALL_REGEDIT_PATH}', 'commondesktop Url', commondesktopUrl); //这个路径正确
  RegQueryStringValue(HKLM64,'{#DS_UNINSTALL_REGEDIT_PATH}', 'userappdata', userappdata);
  RegQueryStringValue(HKLM64,'{#DS_UNINSTALL_REGEDIT_PATH}', 'InstallLocation', InstallLocation);
  end
  else
  begin
  RegQueryStringValue(HKLM32,'{#DS_UNINSTALL_REGEDIT_PATH}}', 'Start Menu',StartMenu);
  RegQueryStringValue(HKLM32,'{#DS_UNINSTALL_REGEDIT_PATH}', 'Start Menu', StartMenuUrl);
  RegQueryStringValue(HKLM32,'{#DS_UNINSTALL_REGEDIT_PATH}}', 'Start Menu Uninstall',StartMenuUninstall);
  RegQueryStringValue(HKLM32,'{#DS_UNINSTALL_REGEDIT_PATH}}', 'Start Menu Folder',StartMenuFolder);
  RegQueryStringValue(HKLM32,'{#DS_UNINSTALL_REGEDIT_PATH}}', 'commondesktop',commondesktop); //这个路径正确
   RegQueryStringValue(HKLM32,'{#DS_UNINSTALL_REGEDIT_PATH}', 'commondesktop Url', commondesktopUrl); //这个路径正确
  RegQueryStringValue(HKLM32,'{#DS_UNINSTALL_REGEDIT_PATH}}', 'userappdata',userappdata);
  RegQueryStringValue(HKLM32,'{#DS_UNINSTALL_REGEDIT_PATH}}', 'InstallLocation',InstallLocation);
  end
        
  DeleteFile(StartMenu);
  DeleteFile(StartMenuUrl);
  DeleteFile(StartMenuUninstall);
  RemoveDir(StartMenuFolder);
  DeleteFile(commondesktop);
  DeleteFile(commondesktopUrl);
  DeleteFile(userappdata);
  DeleteFile(InstallLocation);
end;


//“快速安装”按钮点击事件
procedure NextBtn1Click(hBtn:HWND);
begin
//DelTempDesk();
WizardForm.NextButton.Click;
WizardForm.NextButton.Click;
end;


//“浏览”按钮点击事件
procedure EditdirOnClick(hBtn:HWND);
begin
WizardForm.DirBrowseButton.Click;
Edit1.Text:=WizardForm.DirEdit.Text;
//LabelDir.Caption :=  WizardForm.DirEdit.Text;
end;


//“自定义目录”修改
procedure Editchanged(Sender: TObject);
begin
WizardForm.DirEdit.Text:=Edit1.Text;
end;


//最小化按钮
procedure MinBtnOnClick(hBtn:HWND);
begin
SendMessage(WizardForm.Handle,WM_SYSCOMMAND,61472,0);
end;

//取消窗口
procedure CancelFormMouseDown(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  bOpenExe := false;
  ReleaseCapture
  SendMessage(CancelForm.Handle, WM_SYSCOMMAND, $F012, 0)
end;


//卸载程序确认弹窗
procedure UninstallFormMouseDown(Sender: TObject; Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  ReleaseCapture
  SendMessage(UnStartForm.Handle, WM_SYSCOMMAND, $F012, 0)
end;


//“立即体验”按钮点击事件
procedure FinbtnClick(hBtn:HWND);
begin
bOpenExe := true;
WizardForm.NextButton.Click;
end;


//“安装完成关闭”按钮点击事件
procedure FinCloseClick(hBtn:HWND);
begin
// 避免透明框遗留
Frame.Hide;
bOpenExe := false;
WizardForm.NextButton.Click;
end;


procedure checkAgreeDel(hBtn:HWND);
begin
  if BtnGetChecked(checkAgree) then
  begin
    bReadTreatyCheck := True
  end;
end;


//取消窗口关闭按钮
procedure CancelCloseBtnOnClick(hBtn:HWND);
begin
bOpenExe := false;
CancelForm.close;
end;


//确定按钮
procedure OKBtnOnClick(hBtn:HWND);
begin
Frame1.Hide;
OKButton.Click;
end;


//取消窗口取消按钮
procedure CancelBtnOnClick(hBtn:HWND);
begin
Frame1.close;
CancelForm.close;
Frame.Show;
WizardForm.Show;
end;

procedure label2click(sender :TObject);
begin
BtnSetChecked(A3Check, not BtnGetChecked(A3Check));
if BtnGetChecked(A3Check) then
  begin
  BtnSetEnabled(StartBtn,true);BtnSetEnabled(NextBtn,true);
  end  else
  begin
  BtnSetEnabled(StartBtn,false);BtnSetEnabled(NextBtn,false);
  end
end;


//勾选框文字点击事件
procedure label4click(sender :TObject);
begin
BtnSetChecked(A2Check, not BtnGetChecked(A2Check));
end;


procedure label6click(sender :TObject);
begin
BtnSetChecked(A4Check, not BtnGetChecked(A4Check));
end;


procedure label7click(sender :TObject);
begin
BtnSetChecked(A5Check, not BtnGetChecked(A5Check));
end;


procedure label8click(sender :TObject);
begin
BtnSetChecked(A6Check, not BtnGetChecked(A6Check));
end;


procedure label9click(sender :TObject);
begin
BtnSetChecked(A7Check, not BtnGetChecked(A7Check));
end;


procedure label10click(sender :TObject);
begin
BtnSetChecked(A8Check, not BtnGetChecked(A8Check));
end;


//************注册表*********************
Function regedit(): Boolean;
begin
if BtngetChecked(A5Check) = True then
  Result :=BtngetChecked(A5Check)
end;


//***********添加桌面快捷方式***************
Function desktop(): Boolean;
begin
  if BtngetChecked(A2Check) = True then
  Result :=BtngetChecked(A2Check)
end;


//************开始菜单栏*************
Function StartMenu(): Boolean;
begin

  if BtngetChecked(A4Check) = True then
  Result :=BtngetChecked(A4Check)
end;


function ShouldSkipPage(PageID: Integer): Boolean;
begin
if PageID=wpSelectComponents then    //跳过组件安装界面
  result := true;
end;


//****************页面切换****************
procedure CurPageChanged(CurPageID: Integer);
var fileVer: String;

begin
  WizardForm.NextButton.Width:=0;
  WizardForm.NextButton.TabStop:=false
  WizardForm.NextButton.Hide;
  
  WizardForm.CancelButton.Width:=0;
  WizardForm.CancelButton.TabStop:=false
  WizardForm.BackButton.Width:=0;
  WizardForm.BackButton.TabStop:=false
  BtnSetVisibility(backBtn,false);BtnSetVisibility(DirBrowseBtn,false);
  BtnSetVisibility(NextBtn,false);BtnSetVisibility(Startbtn,false);
  BtnSetVisibility(A2Check,false);
  BtnSetVisibility(A3Check,false);BtnSetVisibility(A4Check,false);
  BtnSetVisibility(A5Check,false);
  BtnSetVisibility(MinBtn,True);
  WelTx2.hide;
  Edit1.hide;
  // LabelDir.Hide;
  PrLabel.hide;PrLabel1.hide;
  Label4.hide;Label5.hide;
  Label6.hide;Label7.hide;
  Label8.hide;Label9.hide;
  Label10.hide;Label11.hide;
  Label12.hide;Label13.hide;
  BtnSetVisibility(nextbtn1,false);
  PrLabel.hide;PrLabel1.hide;

if CurPageID = wpWelcome then
  begin
  WizardFormImage:=ImgLoad(WizardForm.Handle,ExpandConstant('{tmp}\windowc.png'),-10, -10,622,448,True,True);
  Frame.Show;
  
  BtnSetVisibility(A3Check,true);
  BtnSetVisibility(NextBtn,true)
  BtnSetVisibility(Startbtn,true)
  Label20.show;
  //Label21.show;
  end

if CurPageID = wpSelectDir then
  begin
  WizardFormImage:=ImgLoad(WizardForm.Handle,ExpandConstant('{tmp}\window3c.png'),(-10),(-10),622,448,True,True);
  WizardFormImage:=ImgLoad(WizardForm.Handle,ExpandConstant('{tmp}\DirImg.png'),53,110,445,36,True,True);
  BtnSetVisibility(backBtn,true);BtnSetVisibility(NextBtn,false);
  BtnSetVisibility(Startbtn,false);BtnSetVisibility(DirBrowseBtn,true);
  //BtnSetVisibility(MinBtn,true);
  BtnSetVisibility(nextbtn1,true);
  BtnSetVisibility(A2Check,true);
  BtnSetVisibility(A3Check,false);BtnSetVisibility(A4Check,true);
  BtnSetVisibility(A5Check,true);
  Label2.Hide;Label3.Hide;
  Edit1.show;
  //LabelDir.Show;
  WelTx2.show;
  Label4.show;Label5.show;
  Label6.show;Label7.show;
  Label8.hide;Label9.hide;
  Label10.hide;Label11.hide;
  Label12.hide;Label13.hide;
  ImgApplyChanges(WizardForm.Handle);
  Label20.Hide;
  //Label21.Hide;
  /// result:= true;
  end;
  
if CurPageID = wpInstalling then
  begin
  BtnSetVisibility(MinBtn,false);
  BtnSetVisibility(CancelBtn,false);
  WizardFormImage:=ImgLoad(WizardForm.Handle,ExpandConstant('{tmp}\window2c.png'),-10, -10,622,448,True,True);

  pbbgImg:=ImgLoad(WizardForm.Handle,ExpandConstant('{tmp}\Pbbg.png'),26,403,552,6,True,True);
  pbImg:=ImgLoad(WizardForm.Handle,ExpandConstant('{tmp}\Pbjd.png'),0,0,0,0,TRUE,True);
  
  //统计安装次数
  ReportInstall();

  Label2.hide;
  Label3.Hide;
  Label20.Hide;
  //Label21.Hide
  PrLabel.show;PrLabel1.show;
  end;
  
if CurPageID = wpFinished then
  begin
  BtnSetVisibility(CancelBtn,true);

  WizardFormImage:=ImgLoad(WizardForm.Handle,ExpandConstant('{tmp}\Installed.png'),-10, -10,622,448,True,True);
  Finbtn:=BtnCreate(WizardForm.Handle,475,377,106,40,ExpandConstant('{tmp}\btn_finished.png'),1,False) //完成按钮
  BtnSetEvent(Finbtn,BtnClickEventID,WrapBtnCallback(@FinbtnClick,1));

  BtnSetChecked(A6Check,True);
  BtnSetChecked(A7Check,True);
  BtnSetChecked(A8Check,True);
  Label8.Hide;Label9.Hide;
  Label10.Hide;Label11.Hide;
  Label12.Hide;Label13.Hide;
  Label20.Hide;
  //Label21.Hide;
  BtnSetEvent(CancelBtn,BtnClickEventID,WrapBtnCallback(@FinCloseClick,1));
  end;
  ImgApplyChanges(WizardForm.Handle);
  
end;


  // 获取安装文件夹路径
  function GetInstalledDirPath(): String;
  var
  InstalledDirPath: String;
  DefaultDir: String;
  Param:String;
  cIsys:Integer;
  
  begin
  InstalledDirPath :='';
  cIsys := HKLM;

  //Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}
  //RegQueryStringValue(HKLM, 'SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths', 'Installed version', InstalledVersion);
  //RegQueryStringValue(HKLM, '{#DS_UNINSTALL_REGEDIT_PATH}'+'101教育PPT', 'UninstallString', InstalledDirPath);
  DefaultDir := GetDefaultInstallRoot(Param);

  if IsWin64 then
        begin
           RegQueryStringValue(cIsys, '{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir, 'DisplayIcon', InstalledDirPath);
        end
    else
       begin
           RegQueryStringValue(cIsys, '{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir, 'DisplayIcon', InstalledDirPath);
       end

  Result := InstalledDirPath;
  end;
  
  
procedure WizardFormKeyDown(Sender: TObject; var Key: Word; Shift: TShiftState);
begin
     //Msgbox('WizardFormKeyDown',MBInformation,MB_OK);
//if (key=VK_RETURN) then  
 if (key=13) then
 
//Msgbox('11粘贴无效，请手动输入',MBInformation,MB_OK);

//Edit1.text:='';

end;


procedure WizardFormKeyUp(Sender: TObject; var Key: Word; Shift: TShiftState);

begin
    // Msgbox('WizardFormKeyUp',MBInformation,MB_OK);
//if (key=VK_RETURN) then
 if (key=13) then
//Msgbox('22粘贴无效up，请手动输入',MBInformation,MB_OK);
  //  BtnSetVisibility(A3Check,true);

//Edit1.text:='';
end;
  
  
procedure InitializeWizard();
//var
//InstallDir :String;

begin

            //屏蔽重复打开安装包
  CreateMutex('MyProgramsMutexName');

//ExtractTemporaryFile('PTASKBAR.dll');
//ExtractTemporaryFile('UpdateIcon.dll'); 
// ExtractTemporaryFile('psvince.dll'); 

ExtractTemporaryFile('Browser.ico');
// ExtractTemporaryFile('Uninstall.ico');
ExtractTemporaryFile('btn_by.png');
ExtractTemporaryFile('btn_n.png');
ExtractTemporaryFile('CheckButton.png');
ExtractTemporaryFile('QuickCheckButton.png');
ExtractTemporaryFile('backbtn.png');
ExtractTemporaryFile('btn_cancel.png');
ExtractTemporaryFile('btn_s1.png');
ExtractTemporaryFile('btn_s2.png');
ExtractTemporaryFile('btn_min.png');
ExtractTemporaryFile('btn_close.png');
ExtractTemporaryFile('window0.png');
ExtractTemporaryFile('windows1.png');
//ExtractTemporaryFile('window.png');
ExtractTemporaryFile('windowc.png');
//ExtractTemporaryFile('window1.png');
//ExtractTemporaryFile('window1c.png');
//ExtractTemporaryFile('window2.png');
ExtractTemporaryFile('window2c.png');
ExtractTemporaryFile('Installed.png');
ExtractTemporaryFile('window3c.png');
ExtractTemporaryFile('windows4c.png');
ExtractTemporaryFile('DirImg.png');
ExtractTemporaryFile('Pbbg.png');
ExtractTemporaryFile('Pbjd.png');
ExtractTemporaryFile('btn_ok.png');
//ExtractTemporaryFile('MessageDlgBkg.png');
//ExtractTemporaryFile('MessageDlgBkgc.png');
//ExtractTemporaryFile('MessageDlgAlert.png');
ExtractTemporaryFile('version.png');
ExtractTemporaryFile('btn_finished.png');
WizardForm.BorderStyle:=bsnone
ExtractTemporaryFile('license.rtf')
//ExtractTemporaryFile('license1.txt')
//主界面框大小
//WizardForm.Width:=587  602 428
//WizardForm.Height:=437
WizardForm.Width:=602
//WizardForm.Height:=426
WizardForm.Height:=428
WizardForm.OnMouseDown:=@WizardFormMouseDown;
WizardForm.OnKeyDown :=@WizardFormKeyDown;
//WizardForm.OnKeyPress :=@WizardFormKeyDown;
WizardForm.OnKeyUp := @WizardFormKeyUp;
//EditKeyDown;
WizardFormcc;
//透明
CreateFrame;
ShapeForm(WizardForm, radius);//圆角  radius

WFButtonFont:=TFont.Create;
WFButtonFont.Name:='宋体'
WFButtonFont.Size:=9;

//SetClassLong:=(Wizard.Handle,-26,GetWindowLong(WinzardForm.Handle,-26)+$20000);
//WizardFormImage:=ImgLoad(WizardForm.Handle,ExpandConstant('{tmp}\windowc.png'),-10,-10,WizardForm.ClientWidth,WizardForm.ClientHeight,True,True);

//卸载界面左下角文字
WizardForm.OuterNotebook.Hide;
WizardForm.Bevel.Hide;
WizardForm.BeveledLabel.Width := 0
WizardForm.BeveledLabel.Height := 0

//****************全局界面按钮****************
//最小化按钮
MinBtn:=BtnCreate(WizardForm.Handle,548,0,26,26,ExpandConstant('{tmp}\Btn_min.png'),3,False);
BtnSetEvent(MinBtn,BtnClickEventID,WrapBtnCallback(@MinBtnOnClick,1));

//关闭按钮
CancelBtn:=BtnCreate(WizardForm.Handle,574,0,26,26,ExpandConstant('{tmp}\btn_close.png'),1,False)
BtnSetEvent(CancelBtn,BtnClickEventID,WrapBtnCallback(@CancelBtnClick,1));


//协议页面确定按钮
Okbtn:=BtnCreate(WizardForm.Handle,257,377,88,40,ExpandConstant('{tmp}\btn_ok.png'),1,False);
BtnSetEvent(Okbtn,BtnClickEventID,WrapBtnCallback(@OkbtnClick,1));
//BtnSetFont(Okbtn,WFButtonFont.Handle);
//BtnSetFontColor(Okbtn,clBlack,clBlack,clBlack,clGray);
//BtnSetText(Okbtn,ExpandConstant('确定'));

//**************第一页字体和按钮******************
 // 版本号
  Label20:= TLabel.Create(WizardForm);
  Label20.AutoSize:=False;
  Label20.Font.Name :='微软雅黑'
  Label20.Font.Size :=9
  Label20.Font.Color:=clWhite
  Label20.Parent:=WizardForm;
  Label20.WordWrap := True;
  Label20.SetBounds(ScaleX(347), ScaleY(191), ScaleY(225), ScaleY(50));
  Label20.Transparent:=True;
  //Label20.Caption := '1.3.0.8';
  Label20.Caption := '{#DS_APP_VER_NAME}';

Label1 := TLabel.Create(WizardForm);
Label1.Parent := WizardForm;
Label1.Caption := '{#DS_APP_NAME}';
Label1.Transparent := true;
Label1.Font.Size:=11
Label1.Font.Name:='微软雅黑'
Label1.Font.Style := [fsBold];
//Label1.Font.Color:=clSilver
Label1.Font.Color:=$F4F2E0
Label1.Left := (15);
Label1.Top := (10);

Label2 := TLabel.Create(WizardForm);
Label2.Parent := WizardForm;
Label2.Caption := '您已阅读并同意101教育PPT的';
Label2.Transparent := true;
Label2.Font.Size:=11
Label2.Font.Name:='微软雅黑'
Label2.Font.Color:=$002A2A2A
Label2.Left := (45);
Label2.Top := (384);
Label2.OnClick:=@label2click;

Label3 := TLabel.Create(WizardForm);
Label3.Parent := WizardForm;
Label3.Caption := '用户许可协议';
Label3.Transparent := true;
Label3.Font.Size:=11
Label3.Font.Name:='微软雅黑'
Label3.Font.Color:=$00EBAD44
Label3.Left := 249;
Label3.Top := Label2.Top;
Label3.OnClick:=@XYClick;

//“快速安装”按钮
Startbtn:=BtnCreate(WizardForm.Handle,200,253,200,62,ExpandConstant('{tmp}\btn_s1.png'),1,False);
BtnSetEvent(Startbtn,BtnClickEventID,WrapBtnCallback(@NextBtn1Click,1));

//协议前勾选按钮
A3Check:=BtnCreate(WizardForm.Handle,20,385,17,19,ExpandConstant('{tmp}\CheckButton.png'),1,true);
BtnSetEvent(A3Check,BtnClickEventID,WrapBtnCallback(@A1onoff,1));
BtnSetChecked(A3Check,true);

//自定义安装按钮
NextBtn:=BtnCreate(WizardForm.Handle,495,384,86,16,ExpandConstant('{tmp}\btn_n.png'),1,False);
BtnSetEvent(NextBtn,BtnClickEventID,WrapBtnCallback(@NextBtnClick,1));
//BtnSetFontColor(NextBtn,clBlack,clBlack,clBlack,clGray);


  //**************第一页字体和按钮******************

  //**************磁盘大小显示*******************
  WelTx2 := TLabel.Create(WizardForm);
  WelTx2.AutoSize:=False;
  WelTx2.SetBounds(122,79, ScaleX(500), ScaleY(17));
  WelTx2.Transparent:=True;
  WelTx2.Font.Color :=$EFEDD5;
  WelTx2.Font.Size := 9
  WelTx2.Font.Name:= '微软雅黑';
  WelTx2.Parent := WizardForm;
  if GetSpaceOnDisk(ExtractFileDrive(WizardForm.DirEdit.Text), True, FreeMB, TotalMB) then
  begin
  WelTx2.Caption:=ExpandConstant('/可用空间：') + MbOrTB(FreeMB);
  end;
  WizardForm.DirEdit.OnChange := @GetFreeSpaceCaption; //更换磁盘后相应事件
  //**************磁盘大小显示*******************

  //**************第二页字体和按钮******************
  Edit1 := TEdit.Create(WizardForm);
  Edit1.Parent := WizardForm;
  Edit1.Text := WizardForm.DirEdit.Text;//自定义目录文字
  Edit1.Font.Size:=10
  Edit1.Left := 55;
  Edit1.Top := 118 ;
  Edit1.Width :=440
  Edit1.Height  :=20
  Edit1.BorderStyle :=bsnone;
  Edit1.Font.Color:=clBlack
  Edit1.Font.Name:='微软雅黑'
  Edit1.OnChange:=@Editchanged;

  Label4 := TLabel.Create(WizardForm);
  Label4.Parent := WizardForm;
  Label4.Caption := '添加桌面快捷方式';
  Label4.Transparent := true;
  Label4.Font.Size:=9
  Label4.Font.Name:='微软雅黑'
  Label4.Font.Color:=clWhite
  Label4.Left := 75;
  Label4.Top := 161;
  Label4.OnClick:=@label4click;

  Label5 :=TLabel.Create(WizardForm);
  Label5.Parent := WizardForm;
  Label5.Caption := '安装位置:';
  Label5.Transparent := true;
  Label5.Font.Style := [fsBold];
  Label5.Font.Size:=11
  Label5.Font.Name:='微软雅黑'
  Label5.Font.Color:=clWhite;
  Label5.Left := 53;
  Label5.Top := 79;

  Label6 := TLabel.Create(WizardForm);
  Label6.Parent := WizardForm;
  //Label6.Caption := '添加多浏览器支持';
  Label6.Caption := '添加至快速启动栏';
  Label6.Transparent := true;
  Label6.Font.Size:=9
  Label6.Font.Name:='微软雅黑'
  Label6.Font.Color:=clWhite
  Label6.Left := 75;
  Label6.Top := 181;
  Label6.OnClick:=@label6click;

  Label7 := TLabel.Create(WizardForm);
  Label7.Parent := WizardForm;
  Label7.Caption := '开机启动迅雷7';
  Label7.Transparent := true;
  Label7.Font.Size:=9
  Label7.Font.Name:='微软雅黑'
  Label7.Font.Color:=$002A2A2A
  Label7.Left := 382;
  Label7.Top := 322;
  Label7.OnClick:=@label7click;


  //“立即安装”按钮
  nextbtn1:=BtnCreate(WizardForm.Handle,475,375,106,40,ExpandConstant('{tmp}\btn_s2.png'),1,False);
  BtnSetEvent(nextbtn1,BtnClickEventID,WrapBtnCallback(@NextBtn1Click,1));
  BtnSetFontColor(nextbtn1,clWhite,clWhite,clWhite,clWhite);

  //浏览
  DirBrowseBtn:=BtnCreate(WizardForm.Handle,498,108,72,40,ExpandConstant('{tmp}\btn_by.png'),1,False);
  BtnSetEvent(DirBrowseBtn,BtnClickEventID,WrapBtnCallback(@EditdirOnClick,1));
  BtnSetFontColor(DirBrowseBtn,Clblack,Clblack,Clblack,$B6B6B6);

  //返回按钮
  backBtn:=BtnCreate(WizardForm.Handle,372,377,84,40,ExpandConstant('{tmp}\backBtn.png'),1,False);
  BtnSetEvent(backBtn,BtnClickEventID,WrapBtnCallback(@backBtnClick,1));
  BtnSetFontColor(backBtn,clBlack,clBlack,clBlack,clGray);

  //快捷方式够选框
  A2Check:=BtnCreate(WizardForm.Handle,53,160,17,19,ExpandConstant('{tmp}\QuickCheckButton.png'),1,True);
  A4Check:=BtnCreate(WizardForm.Handle,53,180,17,19,ExpandConstant('{tmp}\QuickCheckButton.png'),1,True);
  A5Check:=BtnCreate(WizardForm.Handle,359,324,17,19,ExpandConstant('{tmp}\QuickCheckButton.png'),1,True);
  //快捷方式够选框，默认勾选
  BtnSetChecked(A2Check,true);
  BtnSetChecked(A4Check,true);
  BtnSetChecked(A5Check,true);
  ImgApplyChanges(WizardForm.Handle);
  //**************第二页字体和按钮******************
  //百分比
  PrLabel:=TLabel.Create(WizardForm);
  PrLabel.Parent:=WizardForm;
  PrLabel.Left:=27+48;
  PrLabel.Top:=380;                  //百分比
  PrLabel.Transparent:=True;
  PrLabel.Font.Name:='微软雅黑';
  PrLabel.Font.Color :=  $888888;
  PrLabel1:=TLabel.Create(WizardForm);
  PrLabel1.Parent:=WizardForm;
  PrLabel1.Left:=27;
  PrLabel1.Top:=380;
  PrLabel1.Caption:='正在安装';
  PrLabel1.Transparent:=True;
  PrLabel1.Font.Name:='微软雅黑';
  PrLabel1.Font.Color := $888888;
  PBOldProc:=SetWindowLong(WizardForm.ProgressGauge.Handle,-4,PBCallBack(@PBProc,4));

  //**********安装完成界面*****************
  Label8 := TLabel.Create(WizardForm);
  Label8.Parent := WizardForm;
  Label8.Caption := '360安全导航';
  Label8.Transparent := true;
  Label8.Font.Size:=10
  Label8.Font.Name:='微软雅黑'
  Label8.Font.Color:=$002A2A2A
  Label8.Left := 55;
  Label8.Top := 281;
  Label8.OnClick:=@label8click;

  Label9 := TLabel.Create(WizardForm);
  Label9.Parent := WizardForm;
  Label9.Caption := '360安全卫士';
  Label9.Transparent := true;
  Label9.Font.Size:=10
  Label9.Font.Name:='微软雅黑'
  Label9.Font.Color:=$002A2A2A
  Label9.Left := 55+140;
  Label9.Top := 281;
  Label9.OnClick:=@label9click;

  Label10 := TLabel.Create(WizardForm);
  Label10.Parent := WizardForm;
  Label10.Caption := '百度7工具栏';
  Label10.Transparent := true;
  Label10.Font.Size:=10
  Label10.Font.Name:='微软雅黑'
  Label10.Font.Color:=$002A2A2A
  Label10.Left := 55+140+144;
  Label10.Top := 281;
  Label10.OnClick:=@label10click;

  Label11 := TLabel.Create(WizardForm);
  Label11.Parent := WizardForm;
  Label11.Caption := '安全上网导航';
  Label11.Transparent := true;
  Label11.Font.Size:=10
  Label11.Font.Name:='微软雅黑'
  Label11.Font.Color:=clGray
  Label11.Left := 55;
  Label11.Top := 303;

  Label12 := TLabel.Create(WizardForm);
  Label12.Parent := WizardForm;
  Label12.Caption := '永久加固保护XP';
  Label12.Transparent := true;
  Label12.Font.Size:=10
  Label12.Font.Name:='微软雅黑'
  Label12.Font.Color:=clGray
  Label12.Left := 55+140;
  Label12.Top := 303;

  Label13 := TLabel.Create(WizardForm);
  Label13.Parent := WizardForm;
  Label13.Caption := '让IE更强大！';
  Label13.Transparent := true;
  Label13.Font.Size:=10
  Label13.Font.Name:='微软雅黑'
  Label13.Font.Color:=clGray
  Label13.Left := 55+140+144;
  Label13.Top := 303;
  
  //Yhty.SetBounds((-1000),(45), (470),(2));
  BtnSetVisibility(Okbtn,false);BtnSetVisibility(Okbtn1,false);

  end;


  //最小化
  procedure CancelInsShowMin(hBtn:HWND);
  begin
    PostMessage(CancelForm.Handle,WM_SYSCOMMAND, SC_MINIMIZE,0);
  end;


  //**************取消弹窗************************
  procedure CancelButtonClick(CurPageID: Integer; var Cancel, Confirm: Boolean);
  var
    CancelLabel2: Tlabel;
    CancelLabel3: Tlabel;
  begin

    Frame.Hide;
    WizardForm.Hide;

    Cancel := False;
    Confirm := False;

    CancelForm := CreateCustomForm();
    CancelForm.Width := 602;        //退出安装对话框大小
    CancelForm.Height := 428;
    CancelForm.BorderStyle:=bsNone;

    //CancelForm.CenterInsideControl(WizardForm, False);

    ShapeForm(CancelForm, radius);
     //CancelForm.OnMouseDown := @CancelWizardFormMouseDown;

    CancelForm.OnMouseDown := @CancelWizardFormMouseDown;
    //CancelForm.OnMouseDown:=@CancelFormMouseDown;
    CancelForm.OnMouseUp:=@CancelWizardFormMouseUp;
    CancelForm.OnMouseMove:=@CancelWizardFormMouseMove;

    //透明
    CancelFrame;

    //CancelImage := ImgLoad(CancelForm.Handle,ExpandConstant('{tmp}\window3c.png'),(-10), (-10), CancelForm.ClientWidth,CancelForm.ClientHeight,True,True);   //退出安装对话框 图片
    CancelImage := ImgLoad(CancelForm.Handle,ExpandConstant('{tmp}\windows4c.png'),-10,-10,622,448,True,True);
    //IconI:=ImgLoad(CancelForm.Handle,ExpandConstant('{tmp}\logo.ico'),10,44,54,54,True,True);
    //IconI:=ImgLoad(CancelForm.Handle,ExpandConstant('{tmp}\MessageDlgAlert.png'),10,44,54,54,True,True);

     // 最小化
    //CancelMinBtn:=BtnCreate(CancelForm.Handle,548,0,26,26,ExpandConstant('{tmp}\Btn_min.png'),3,False); //退出安装对话框右上角 按钮
    //BtnSetEvent(CancelMinBtn,BtnClickEventID,WrapBtnCallback(@CancelInsShowMin,1));

    //关闭按钮
    CancelCloseBtn:=BtnCreate(CancelForm.Handle,574,0,26,26,ExpandConstant('{tmp}\btn_close.png'),3,False); //退出安装对话框右上角 按钮
    //BtnSetEvent(CancelCloseBtn,BtnClickEventID,WrapBtnCallback(@OKBtnOnClick,1));
    BtnSetEvent(CancelCloseBtn,BtnClickEventID,WrapBtnCallback(@CancelBtnOnClick,1));

    //退出框中文字
    CancelLabel2:= TLabel.Create(CancelForm);
    CancelLabel2.AutoSize:=False;
    CancelLabel2.Font.Name :='微软雅黑'
    CancelLabel2.Font.Size :=15
    CancelLabel2.Font.Color:=clWhite
    CancelLabel2.Parent:=CancelForm;
    CancelLabel2.WordWrap := True;
    CancelLabel2.SetBounds(ScaleX(114), ScaleY(258), ScaleY(500), ScaleY(50));
    CancelLabel2.OnMouseDown:=@CancelFormMouseDown;
    CancelLabel2.Transparent:=True;
    CancelLabel2.Caption := '您确定要退出101教育PPT的安装程序吗？';

  //  //确认键反馈
    OKButton := TButton.Create(CancelForm);
    OKButton.Parent := CancelForm;
    OKButton.ModalResult := mrOk;
    OKButton.SetBounds(0, 0, 0, 0);

    CancelCloseButton := TButton.Create(CancelForm);
    CancelCloseButton.Parent := CancelForm;
    CancelCloseButton.ModalResult := mrCancel;
    CancelCloseButton.SetBounds(0, 0, 0, 0);

    //退出安装“确定”按钮
    OKBtn:=BtnCreate(CancelForm.Handle,495,378,88,40,ExpandConstant('{tmp}\btn_ok.png'),1,False);
    BtnSetEvent(OKBtn,BtnClickEventID,WrapBtnCallback(@OKBtnOnClick,1));
    //BtnSetFontColor(OKBtn,clBlack,clBlack,clBlack,$B6B6B6);       //字体颜色

    //退出安装“取消”按钮
    CancelFormBtn:=BtnCreate(CancelForm.Handle,392,378,84,40,ExpandConstant('{tmp}\btn_cancel.png'),1,False);
    BtnSetEvent(CancelFormBtn,BtnClickEventID,WrapBtnCallback(@CancelBtnOnClick,1));
    //BtnSetFontColor(CancelFormBtn,clBlack,clBlack,clBlack,$B6B6B6);        //字体颜色

    ImgApplyChanges(CancelForm.Handle);
    //CancelForm.ShowModal;

    Frame1.Show;
    if CancelForm.ShowModal = mrOK then
    begin
     Cancel := True;
    end
  end;


  procedure DecodeVersion(verstr: String; var verint: array of Integer);
  var
  i,p: Integer; s: string;
  begin
  // initialize array
  verint := [0,0,0,0];
  i := 0;
  while ( (Length(verstr) > 0) and (i < 4) ) do
  begin
  p := pos('.', verstr);
  if p > 0 then
  begin
  if p = 1 then s:= '0' else s:= Copy( verstr, 1, p - 1 );
  verint[i] := StrToInt(s);
  i := i + 1;
  verstr := Copy( verstr, p+1, Length(verstr));
  end
  else
  begin
  verint[i] := StrToInt( verstr );
  verstr := '';
  end;
  end;
  end;
  // This function compares version string
  // return -1 if ver1 < ver2
  // return 0 if ver1 = ver2
  // return 1 if ver1 > ver2
  function CompareVersion( ver1, ver2: String ) : Integer;
  var
  verint1, verint2: array of Integer;
  i: integer;
  begin
  SetArrayLength( verint1, 4 );
  DecodeVersion( ver1, verint1 );
  SetArrayLength( verint2, 4 );
  DecodeVersion( ver2, verint2 );
  Result := 0; i := 0;
  while ( (Result = 0) and ( i < 4 ) ) do
  begin
  if verint1[i] > verint2[i] then
  Result := 1
  else
  if verint1[i] < verint2[i] then
  Result := -1
  else
  Result := 0;
  i := i + 1;
  end;
  end;

  function SysBitsIsWin64(): Boolean;
  begin
  Result := False;
        if IsWin64 then
        begin
         Result := True;
        end
        else
        begin
         Result := False;
        end
  end;


  // 获取安装版本号
  function GetInstalledVersion(): String;
  var
  InstalledVersion: String;
  DefaultDir: String;
  Param:String;
  cIsys: integer;
  
  begin
  InstalledVersion :='';
  //Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}
  //RegQueryStringValue(HKLM, 'SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths', 'Installed version', InstalledVersion);
  //RegQueryStringValue(HKLM, '{#DS_UNINSTALL_REGEDIT_PATH}'+'101教育PPT', 'DisplayVersion', InstalledVersion);
  
  DefaultDir := GetDefaultInstallRoot(Param);
  
  cIsys := HKLM;

   if IsWin64 then
   begin
   //cIsys := HKLM64;
   RegQueryStringValue(cIsys, '{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir, 'DisplayVersion', InstalledVersion);
   end
   else
   begin
   //cIsys := HKLM32;
   RegQueryStringValue(cIsys, '{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir, 'DisplayVersion', InstalledVersion);
   end
        
  Result := InstalledVersion;
  end;





  // 判断APP是否运行中
  function IsMyAppRunning(const FileName : string): Boolean;
  var
      FSWbemLocator: Variant;
      FWMIService   : Variant;
      FWbemObjectSet: Variant;
      
  begin
      Result := false;
      try
        FSWbemLocator := CreateOleObject('WBEMScripting.SWBEMLocator');
        FWMIService := FSWbemLocator.ConnectServer('', 'root\CIMV2', '', '');
        FWbemObjectSet := FWMIService.ExecQuery(Format('SELECT Name FROM Win32_Process Where Name="%s"',[FileName]));
        Result := (FWbemObjectSet.Count > 0);
        FWbemObjectSet := Unassigned;
        FWMIService := Unassigned;
        FSWbemLocator := Unassigned;
      except
        if (IsModuleLoaded(FileName)) then
          begin
            Result := false;
          end
        else
          begin
            Result := true;
          end
        end;
  end;
  
  
  function DefSysDeleteRegedit(Isys: Integer): Boolean;
  var
    cIsys: integer;
    cIsysCR: integer;

    StartMenu1:String;
    StartMenuUrl:String;
    StartMenuUninstall:String;
    StartMenuFolder:String;
    commondesktop:String;
    commondesktopUrl:String;
    userappdata:String;
    InstallLocation:String;
    
    DefaultDir:String;
    Param:String;

  begin

    if Isys = 64 then
    begin
     cIsys :=  HKLM64;
     cIsysCR := HKCR64;
    end
    else
    begin
     cIsys := HKLM32;
     cIsysCR := HKCR32;
    end
    
    DefaultDir := GetDefaultInstallRoot(Param);
    
    cIsys := HKLM;
    cIsysCR := HKCR;

    //删除开始菜单项
    RegQueryStringValue(cIsys, '{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir, 'Start Menu', StartMenu1);
    RegQueryStringValue(cIsys, '{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir, 'Start Menu Url', StartMenuUrl);
    RegQueryStringValue(cIsys, '{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir, 'Start Menu Uninstall', StartMenuUninstall);
    RegQueryStringValue(cIsys, '{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir, 'Start Menu Folder', StartMenuFolder);
    RegQueryStringValue(cIsys, '{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir, 'commondesktop', commondesktop); //这个路径正确
    RegQueryStringValue(cIsys, '{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir, 'commondesktop Url', commondesktopUrl); //这个路径正确
    RegQueryStringValue(cIsys, '{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir, 'userappdata', userappdata);
    RegQueryStringValue(cIsys, '{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir, 'InstallLocation', InstallLocation);

    DeleteFile(StartMenu1);
    DeleteFile(StartMenuUrl);
    DeleteFile(StartMenuUninstall);
    RemoveDir(StartMenuFolder);
    DeleteFile(commondesktop);
    DeleteFile(commondesktopUrl);
    DeleteFile(userappdata);
    DeleteFile(InstallLocation);
    
    //删除注册表记录的安装信息
//  if(RegValueExists(HKLM,'Software\{#DS_GROUP_NAME}\Settings','Path')) then
//  begin
//      RegDeleteValue(HKLM,'Software\{#DS_GROUP_NAME}\Settings','Path');
//  end;
 //     RegWriteStringValue(cIsys,'{#DS_UNINSTALL_REGEDIT_PATH}', 'Start Menu Url', ExpandConstant('{group}\101教育PPT官网.lnk'));
//      RegWriteStringValue(cIsys,'{#DS_UNINSTALL_REGEDIT_PATH}', 'Start Menu Uninstall', ExpandConstant('{group}\卸载101教育PPT.lnk'));
//      RegWriteStringValue(cIsys,'{#DS_UNINSTALL_REGEDIT_PATH}', 'Start Menu Folder', ExpandConstant('{group}\{code:GetDefaultInstallRoot}'));
//      RegWriteStringValue(cIsys,'{#DS_UNINSTALL_REGEDIT_PATH}', 'commondesktop', ExpandConstant('{commondesktop}\{code:GetDefaultInstallRoot}.lnk'));
//      RegWriteStringValue(cIsys,'{#DS_UNINSTALL_REGEDIT_PATH}', 'commondesktop Url', ExpandConstant('{commondesktop}\101教育PPT官网.lnk'));
//      RegWriteStringValue(cIsys,'{#DS_UNINSTALL_REGEDIT_PATH}', 'userappdata', ExpandConstant('{userappdata}\Microsoft\Internet Explorer\Quick Launch\{code:GetDefaultInstallRoot}.lnk'));

  //  RegDeleteKeyIfEmpty(cIsys,'{#DS_UNINSTALL_REGEDIT_PATH}');
    

    //DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
    //RegDeleteKeyIncludingSubkeys(HKLM, '{#DS_UNINSTALL_REGEDIT_PATH}'+'101教育PPT');
    RegDeleteKeyIncludingSubkeys(cIsys, '{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir);
    //DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
    RegDeleteKeyIncludingSubkeys(cIsys, '{#DS_REGEDIT_APP_PATH}'+DefaultDir+'.exe');
    
    //恢复注册表
   //#------------------ Win 8 -------------------------------------
    //DeleteRegKey HKCR ".ppt\PPTShell"
    RegDeleteKeyIncludingSubkeys(cIsys, '.ppt\PPTShell');
    //DeleteRegKey HKCR ".pptx\PPTShells"
    RegDeleteKeyIncludingSubkeys(cIsys, '.pptx\PPTShells');
    //WriteRegStr HKCR ".ppt" "" "PowerPoint.Show.8"
    RegWriteStringValue(cIsys,'.ppt', '','PowerPoint.Show.8');
    //WriteRegStr HKCR ".ppt\PowerPoint.Show.8\ShellNew" "" ""
    RegWriteStringValue(cIsys,'.ppt\PowerPoint.Show.8\ShellNew', '', '');
    //WriteRegStr HKCR ".pptx" "" "PowerPoint.Show.12"
    RegWriteStringValue(cIsys,'.pptx', '', 'PowerPoint.Show.12');
    //WriteRegStr HKCR ".pptx\PowerPoint.Show.12\ShellNew" "" ""
    RegWriteStringValue(cIsys,'.pptx\PowerPoint.Show.12\ShellNew', '', '');

   //#------------------ 其它 -------------------------------------
   //DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.pptx\UserChoice"
   RegDeleteKeyIncludingSubkeys(cIsys, '{#DS_REGEDIT_USERCHOISE_PPT_PATH}'+'.pptx\UserChoice');
   //DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.ppt\UserChoice"
   RegDeleteKeyIncludingSubkeys(cIsys, '{#DS_REGEDIT_USERCHOISE_PPT_PATH}'+'.ppt\UserChoice');
   //DeleteRegKey HKCR "PPTShell\\Shell\\Open\\Command"
   RegDeleteKeyIncludingSubkeys(cIsysCR, 'PPTShell\\Shell\\Open\\Command');
   //DeleteRegKey HKCU "Software\\Classes\\Applications\\PPTShell\\shell\\open\\command"
   RegDeleteKeyIncludingSubkeys(cIsys, 'Software\\Classes\\Applications\\PPTShell\\shell\\open\\command');
   //WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.pptx\UserChoice" "Progid" "PowerPoint.Show.12"
   RegWriteStringValue(cIsys,'{#DS_REGEDIT_USERCHOISE_PPT_PATH}'+'.pptx\UserChoice', 'Progid', 'PowerPoint.Show.12');
   //WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.ppt\UserChoice" "Progid" "PowerPoint.Show.8"
   RegWriteStringValue(cIsys,'{#DS_REGEDIT_USERCHOISE_PPT_PATH}'+'.ppt\UserChoice', 'Progid', 'PowerPoint.Show.8');

  end;
  

  function InstallDelContentRegedit(Isys: Integer): Boolean;
  var
    cIsys: integer;
    cIsysCR: integer;
    cIsysCU: integer;

    StartMenu1:String;
    StartMenuUrl:String;
    StartMenuUninstall:String;
    StartMenuFolder:String;
    commondesktop:String;
    commondesktopUrl:String;
    userappdata:String;
    InstallLocation:String;
    
    Param:String;
    DefaultDir:String;
  begin

    if Isys = 64 then
    begin
    cIsys :=  HKLM64;
    cIsysCR := HKCR64;
    cIsysCU := HKCU64;
    end
    else
    begin
    cIsys := HKLM32;
    cIsysCR := HKCR32;
    cIsysCU := HKCU32;
    end
    
    DefaultDir := GetDefaultInstallRoot(Param);
    
    cIsys := HKLM;
    cIsysCR := HKCR;
    cIsysCU := HKCU;
    
    //删除开始菜单项
    RegQueryStringValue(cIsys, '{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir, 'Start Menu', StartMenu1);
    RegQueryStringValue(cIsys, '{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir, 'Start Menu Url', StartMenuUrl);
    RegQueryStringValue(cIsys, '{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir, 'Start Menu Uninstall', StartMenuUninstall);
    RegQueryStringValue(cIsys, '{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir, 'Start Menu Folder', StartMenuFolder);
    RegQueryStringValue(cIsys, '{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir, 'commondesktop', commondesktop); //这个路径正确
    RegQueryStringValue(cIsys, '{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir, 'commondesktop Url', commondesktopUrl); //这个路径正确
    RegQueryStringValue(cIsys, '{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir, 'userappdata', userappdata);
    RegQueryStringValue(cIsys, '{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir, 'InstallLocation', InstallLocation);

    DeleteFile(StartMenu1);
    DeleteFile(StartMenuUrl);
    DeleteFile(StartMenuUninstall);
    RemoveDir(StartMenuFolder);
    DeleteFile(commondesktop);
    DeleteFile(commondesktopUrl);
    DeleteFile(userappdata);
    DeleteFile(InstallLocation);

     // 删除桌面链接快捷方式
    //Delete "$DESKTOP\${PRODUCT_NAME}.lnk"
    //Delete "$SMPROGRAMS\${PRODUCT_NAME}.lnk"
    //Delete "$DESKTOP\${PRODUCT_WEB_LINK_NAME}.lnk"
    //RMDir /r /REBOOTOK "$SMPROGRAMS\${PRODUCT_NAME}"

    //DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
    //RegDeleteKeyIncludingSubkeys(HKLM, '{#DS_UNINSTALL_REGEDIT_PATH}'+'101教育PPT');
    RegDeleteKeyIncludingSubkeys(cIsys, '{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir);
    //DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
    RegDeleteKeyIncludingSubkeys(cIsys, '{#DS_REGEDIT_APP_PATH}'+DefaultDir+'.exe');

    //恢复注册表
   //#------------------ Win 8 -------------------------------------
    RegDeleteKeyIncludingSubkeys(cIsysCR, '.ppt\PPTShell');
    RegDeleteKeyIncludingSubkeys(cIsysCR, '.pptx\PPTShells');
    RegWriteStringValue(cIsysCR, '.ppt', '','PowerPoint.Show.8');
    RegWriteStringValue(cIsysCR, '.ppt\PowerPoint.Show.8\ShellNew', '', '');
    RegWriteStringValue(cIsysCR, '.pptx', '', 'PowerPoint.Show.12');
    RegWriteStringValue(cIsysCR, '.pptx\PowerPoint.Show.12\ShellNew', '', '');

     //#------------------ 其它 -------------------------------------
     RegDeleteKeyIncludingSubkeys(cIsysCU, '{#DS_REGEDIT_USERCHOISE_PPT_PATH}'+'.pptx\UserChoice');
     //DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.ppt\UserChoice"
     RegDeleteKeyIncludingSubkeys(cIsysCU, '{#DS_REGEDIT_USERCHOISE_PPT_PATH}'+'.ppt\UserChoice');
     //DeleteRegKey HKCR "PPTShell\\Shell\\Open\\Command"
     RegDeleteKeyIncludingSubkeys(cIsysCU, 'PPTShell\Shell\Open\Command');
     //DeleteRegKey HKCU "Software\\Classes\\Applications\\PPTShell\\shell\\open\\command"
     RegDeleteKeyIncludingSubkeys(cIsysCU, 'Software\Classes\Applications\PPTShell\shell\open\command');
     //WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.pptx\UserChoice" "Progid" "PowerPoint.Show.12"
     RegWriteStringValue(cIsysCU,'{#DS_REGEDIT_USERCHOISE_PPT_PATH}'+'.pptx\UserChoice', 'Progid', 'PowerPoint.Show.12');
     //WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.ppt\UserChoice" "Progid" "PowerPoint.Show.8"
     RegWriteStringValue(cIsysCU,'{#DS_REGEDIT_USERCHOISE_PPT_PATH}'+'.ppt\UserChoice', 'Progid', 'PowerPoint.Show.8');
   
  end;


  // 安装前注册表操作
  function InstallDelContent(): Boolean;
  begin
    if IsWin64 then
    begin
    InstallDelContentRegedit(64);
     end
  else
    begin
    InstallDelContentRegedit(32);
    end
  end;


  // 卸载后注册表操作
  function UnInstallDelContent(): Boolean;
  begin
     // 删除桌面链接快捷方式
    //Delete "$DESKTOP\${PRODUCT_NAME}.lnk"
    //Delete "$SMPROGRAMS\${PRODUCT_NAME}.lnk"
    //Delete "$DESKTOP\${PRODUCT_WEB_LINK_NAME}.lnk"
    //RMDir /r /REBOOTOK "$SMPROGRAMS\${PRODUCT_NAME}"

    // 删除任务栏
    if IsWin64 then
    begin
     DefSysDeleteRegedit(64);
    end
    else
    begin
     DefSysDeleteRegedit(32);
    end
  end;



  //注册表操作3 删除注册表信息
  procedure DelRegeditSilent();
  var
    ResultCode: boolean;
    ResultStr: String;
  begin
    //删除启动项
  //  if(RegValueExists(HKEY_LOCAL_MACHINE,'SOFTWARE\Microsoft\Windows\CurrentVersion\Run','{#MyAppName}')) then
  //  begin
  //      RegDeleteValue(HKEY_LOCAL_MACHINE,'SOFTWARE\Microsoft\Windows\CurrentVersion\Run','{#MyAppName}');
  //  end;
  //
    //删除注册表记录的安装信息
//    if(RegValueExists(HKLM,'Software\{#MyAppGroupName}\Settings','Path')) then
//    begin
//        RegDeleteValue(HKLM,'Software\{#MyAppGroupName}\Settings','Path');
//    end;
  //
  //  //删除最早老版本的唯威注册表
  //  ResultCode := RegQueryStringValue(HKLM,'Software\深圳唯威\VV\Settings', 'Path',ResultStr);
  //  if  DirExists(ResultStr) then
  //  begin
  //    DelTree(ResultStr, True, True, True);
  //  end;
  //
  //  //删除外网老版本安装目录
  //  ResultCode := RegQueryStringValue(HKLM,'Software\唯威\VV\Settings', 'Path',ResultStr);
  //  DelTree(ResultStr, True, True, True);
  //
  //  //删除安装目录
  //  ResultCode := RegQueryStringValue(HKLM,'Software\唯威\Settings', 'Path',ResultStr);
  //  ResultCode := DirExists(ResultStr);
  //  DelTree(ResultStr, True, True, True);

    //删除卸载信息

     // ;解锁开始菜单快捷方式           先解锁，再删除快捷，最后删除应用，不然会导致解锁失败
   // ExecShell "startunpin" "$QUICKLAUNCH\${PRODUCT_NAME}.lnk"
   // ;解锁任务栏快捷方式
   // ExecShell "taskbarunpin" "$QUICKLAUNCH\${PRODUCT_NAME}.lnk"

   // Delete "$DESKTOP\${PRODUCT_NAME}.lnk"
   // Delete "$SMPROGRAMS\${PRODUCT_NAME}.lnk"
   // Delete "$DESKTOP\${PRODUCT_WEB_LINK_NAME}.lnk"
   // RMDir /r /REBOOTOK "$SMPROGRAMS\${PRODUCT_NAME}"

  //  Delete "$QUICKLAUNCH\User Pinned\TaskBar\${PRODUCT_NAME}.lnk"
   // Delete "$QUICKLAUNCH\${PRODUCT_NAME}.lnk"
  //  Delete "$STARTMENU\${PRODUCT_NAME}.lnk"

  //  Delete "$INSTDIR\*.exe"
  //  Delete "$INSTDIR\*.dll"
  //  Delete "$INSTDIR\*.dat"
  //  Delete "$INSTDIR\*.log"

    //RMDir /r /REBOOTOK "$INSTDIR\bin"
   // RMDir /r /REBOOTOK "$INSTDIR\Skins"
   // RMDir /r /REBOOTOK "$INSTDIR\template"
   // RMDir /r /REBOOTOK "$INSTDIR\Cache"
   // RMDir /r /REBOOTOK "$INSTDIR\log"
   // RMDir /r /REBOOTOK "$INSTDIR\temp"
   // RMDir /r /REBOOTOK "$INSTDIR\package"

    //Messagebox MB_YESNO "是否删除配置文件?" IDYES true IDNO false
    //true:
      //RMDir /r /REBOOTOK "$INSTDIR\settting"
     // RMDir /r /REBOOTOK "$INSTDIR"
    //false:
  end;
  
  
  //安装前判断
  function InitializeSetup(): Boolean;
  var
  PrevVer: String;
  IsAppRunning: boolean;
  UninstallStringDirPath: String;
  nErrCode : Integer;
  //RCode: Integer;
  
  //UninstallNa:String;
  Param:String;
  i : Integer;
  
  ResultStr: String;
ResultCode: Integer;

  
  begin
  
  //UninstallNa := GetUninstallExeNa(Param);
  
    IsInstallPathExit := true;
    //判断101PPT.exe 是否在运行
    //FileCopy(ExpandConstant('{src}\KP.dll'), ExpandConstant('{tmp}\KP.dll'), True);
    
    //101PPTDaemon服务安装操作
    //bin\CoursePlayer\101PPTDaemon.exe -u
    //ShellExec('open', ExpandConstant('D:\\桌面\\101PPT修改中(0122)\\101PPTDaemon.exe'), '-i', '', SW_HIDE, ewNoWait, RCode);
    
    
    // 0.解决【1.0.1.1】版本改进程名的问题
    // 1.当前计算机上安装版本与安装盘版本一致
    PrevVer := GetInstalledVersion();
    if CompareVersion(PrevVer, '{#DS_APP_VER_NAME}') = 0  then
    begin
      if MsgBox('您已安装的版本，与当前版本一致。您确定要覆盖此程序吗？'#13''#13'单击“是”按钮覆盖程序安装；'#13''#13'单击“否”按钮退出安装！', mbConfirmation, MB_YESNO) = IDYES then
        begin
        // 读注册表安装路径 UninstallString
        UninstallStringDirPath := '';
        UninstallStringDirPath := GetInstalledDirPath();
        // 解绑任务栏
        ShellExec('taskbarunpin', ExpandConstant('{userappdata}\\Microsoft\\Internet Explorer\\Quick Launch\\{#DS_PINLINK_NAME}'), '', '', SW_SHOWNORMAL, ewNoWait, nErrCode);
        // 删除注册表
        //UnInstallDelContent();
        // 删除以前安装包
        //DelTree(ExpandConstant(UninstallStringDirPath), True, True, True);
        end
      else
        begin
        Exit;
        end
    end;


    // 2.当前计算上的版本比安装盘版本高
    PrevVer := GetInstalledVersion();
    if CompareVersion(PrevVer, '{#DS_APP_VER_NAME}') > 0 then
    begin
     if MsgBox('你现有程序的版本高于 {#DS_APP_VER_NAME}，是否卸载后继续安装？'#13''#13'单击“是”按钮卸载程序；'#13''#13'单击“否”按钮退出安装！', mbInformation, MB_YESNO)  = IDYES then
      begin
      // 读注册表安装路径 UninstallString
//      UninstallStringDirPath := '';
//      UninstallStringDirPath := GetInstalledDirPath();
//      // 解绑任务栏
//      ShellExec('taskbarunpin', ExpandConstant('{userappdata}\\Microsoft\\Internet Explorer\\Quick Launch\\{#DS_PINLINK_NAME}'), '', '', SW_SHOWNORMAL, ewNoWait, nErrCode);
//      // 删除注册表
//      UnInstallDelContent();
//      // 删除以前安装包
//      DelTree(ExpandConstant(UninstallStringDirPath), True, True, True);
      //PrevVer := GetInstalledDirPath();

      //if RegQueryStringValue(HKLM, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\APP_NAME_is1', 'UninstallString', ResultStr) then
      if RegQueryStringValue(HKLM, '{#DS_UNINSTALL_REGEDIT_PATH}'+'101教育PPT', 'UninstallString', ResultStr) then
      begin
      
     //加处理
     //i := pos(' /SILENT',ResultStr)
     //Delete(ResultStr,i-1,9)
     
      
      ResultStr := RemoveQuotes(ResultStr);
      Exec(ResultStr, '/silent', '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
      end;
      Exit;

      end
     else
      begin
      Exit;
      end
    end;
    
    
    //判断是否已经打开卸载程序
    IsAppRunning:= IsMyAppRunning('unins000.exe');
    while IsAppRunning do
    begin
      if MsgBox('101教育PPT卸载程序正在运行！'#13''#13'单击“是”按钮关闭程序并继续安装；'#13''#13'单击“否”按钮退出安装！', mbConfirmation, MB_YESNO) = IDYES then
        begin
        KillProcess(GetPIDByName('unins000.exe')); 
        KillProcess(GetPIDByName('_iu14D2N.tmp'));
        //IsAppRunning:= IsMyAppRunning('unins000.exe');
        IsAppRunning:= false;
        end
      else
        begin
        IsAppRunning:= false;
        Exit;
        end;
    end;
    
    
    //判断是否已经打开101教育PPT程序
    IsAppRunning:= IsModuleLoaded('{#DS_MAIN_FILE_NAME}');
    while IsAppRunning do
    begin
      if MsgBox('{#DS_MAIN_FILE_NAME}正在运行！'#13''#13'单击“是”按钮关闭程序并继续安装；'#13''#13'单击“否”按钮退出安装！', mbConfirmation, MB_YESNO) = IDYES then
      begin
        //IsAppRunning:= IsModuleLoaded('{#DS_MAIN_FILE_NAME}');
        KillProcess(GetPIDByName('101PPT.exe'));
        //KillProcess(GetPIDByName('POWERPNT.EXE'));
        IsAppRunning:= false;
      end
    else
      begin
      IsAppRunning:= false;
      Exit;
      end
    end;
  
  
   // KillProcess(GetPIDByName('YodaoDict.exe'));
    KillProcess(GetPIDByName('POWERPNT.exe'));
    KillProcess(GetPIDByName('CoursePlayer.exe'));
    KillProcess(GetPIDByName('{#MyUpdateAppName}'));
    KillProcess(GetPIDByName('node.exe'));
    KillProcess(GetPIDByName('{#DS_MAIN_FILE_NAME}'));
    KillProcess(GetPIDByName('PPTShell.exe'));
    
    
    //必须为真，否则安装页面无法启动
    Result:= true;

    //安装前注册表操作
    //InstallDelContent();
  
    
  end;
  

procedure CopyFileToTmpDir();
begin
  FileCopy(ExpandConstant('{app}\Plugin\botva2.dll'), ExpandConstant('{tmp}\botva2.dll'), True);
  FileCopy(ExpandConstant('{app}\Plugin\InnoCallback.dll'), ExpandConstant('{tmp}\InnoCallback.dll'), True);
  FileCopy(ExpandConstant('{app}\Plugin\KP.dll'), ExpandConstant('{tmp}\KP.dll'), True);
  FileCopy(ExpandConstant('{app}\Plugin\InnoCallback.dll'), ExpandConstant('{tmp}\InnoCallback.dll'), True);
  FileCopy(ExpandConstant('{app}\Plugin\psvince.dll'), ExpandConstant('{tmp}\psvince.dll'), True);
  
  FileCopy(ExpandConstant('{app}\UninstallSkin\window0.png'), ExpandConstant('{tmp}\window0.png'), True);
  FileCopy(ExpandConstant('{app}\UninstallSkin\window.png'), ExpandConstant('{tmp}\window.png'), True);
  FileCopy(ExpandConstant('{app}\UninstallSkin\btn_min.png'), ExpandConstant('{tmp}\btn_min.png'), True);
  FileCopy(ExpandConstant('{app}\UninstallSkin\btn_close.png'), ExpandConstant('{tmp}\btn_close.png'), True);
  FileCopy(ExpandConstant('{app}\UninstallSkin\btn_ok.png'), ExpandConstant('{tmp}\btn_ok.png'), True);
  FileCopy(ExpandConstant('{app}\UninstallSkin\btn_cancel.png'), ExpandConstant('{tmp}\btn_cancel.png'), True);
  FileCopy(ExpandConstant('{app}\UninstallSkin\Pbbg.png'), ExpandConstant('{tmp}\Pbbg.png'), True);
  FileCopy(ExpandConstant('{app}\UninstallSkin\Pbjd.png'), ExpandConstant('{tmp}\Pbjd.png'), True);
  FileCopy(ExpandConstant('{app}\UninstallSkin\UnInstalled.png'), ExpandConstant('{tmp}\UnInstalled.png'), True);
end;

  //卸载前判断
  function InitializeUninstall(): boolean;
  var
    tempUninstallResult: integer;
    IsAppRunning: boolean;
    RCode: Integer;
    ResultCode: Integer;
    
     UninstallLabel: Tlabel;
    UninstallingLabe: Tlabel;
    MResult: Integer;
    
  begin
      Result := true;
      //资源备份
      CopyFileToTmpDir();
      
//    判断是否已经打开安装程序
//    IsAppRunning:= IsMyAppRunning('101教育PPT1.1.3.8.exe');
//    while IsAppRunning do
//    begin
//      if MsgBox('101教育PPT安装程序正在运行,请先关闭它!', mbConfirmation, MB_OKCANCEL) = IDOK then
//        begin
//        IsAppRunning:= IsMyAppRunning('101教育PPT1.1.3.8.exe');
//        end
//      else
//        begin
//        IsAppRunning:= false;
//        Exit;
//        end;
//    end;

    //读注册表获取先前安装路径，判断是否为空
//    UninstallStringDirPath := GetInstalledDirPath();
//    if  UninstallStringDirPath = '' then
//      begin
//      // 不存在退出
//      Result := false;
//      Exit;
//      end
//    else
//      begin
//      // 存在卸载
//      Result := true;
//      end;

    //判断是否已经打开101教育PPT程序
    IsAppRunning:= IsModuleLoadedU('{#DS_MAIN_FILE_NAME}');
    while IsAppRunning do
    begin
      if MsgBox('{#DS_MAIN_FILE_NAME}正在运行！'#13''#13'单击“是”按钮关闭程序并继续卸载；'#13''#13'单击“否”按钮退出卸载！', mbConfirmation, MB_YESNO) = IDYES then
      begin
        //IsAppRunning:= IsModuleLoaded('{#DS_MAIN_FILE_NAME}');
        KillProcessU(GetPIDByNameU('101PPT.exe'));
        //KillProcess(GetPIDByName('POWERPNT.EXE'));
        IsAppRunning:= false;
      end
      else
      begin
      IsAppRunning:= false;
      Result := false;
      Exit;
      end
    
    end;

    //101PPTDaemon服务卸载操作
    //bin\CoursePlayer\101PPTDaemon.exe -u
    //ShellExec('open', ExpandConstant('D:\\桌面\\101PPT修改中(0122)\\101PPTDaemon.exe'), '-u', '', SW_HIDE, ewNoWait, RCode);
    ShellExec('open', ExpandConstant('{app}\bin\CoursePlayer\101PPTDaemon.exe'), '-u', '', SW_HIDE, ewNoWait, RCode);

    KillProcessU(GetPIDByNameU('Update.exe'));
    KillProcessU(GetPIDByNameU('CoursePlayer.exe'));
    KillProcessU(GetPIDByNameU('{#MyUpdateAppName}'));
    KillProcessU(GetPIDByNameU('node.exe'));

//    //执行静默卸载
//    if Pos('silent', Lowercase(GetCmdTail)) > 0 then
//    begin
//      Result := True;
//    end
//    else begin
//      Exec(ExpandConstant('{uninstallexe}'), ' /Silent', '', SW_SHOW, ewNoWait, ResultCode)
//      Result := False;
//      exit;
//    end
      
    
  end;


  //完成
  procedure UninstalledBtnOnClick(hBtn: HWND);
  begin
    UnFinishedFrame.Hide;
    UninstalledForm.Close;
  end;
  
  
procedure InitializeUninstallProgressForm();
var
    UninstallLabel: Tlabel;
    UninstallingLabe: Tlabel;
    MResult: Integer;
    
begin
    UnStartForm := CreateCustomForm();
    try
      UnStartForm.Width := 602;
      UnStartForm.Height := 428;
      UnStartForm.BorderStyle := bsNone;

      UnStartForm.OnMouseDown := @UWizardFormMouseDown;
      UnStartForm.OnMouseUp:=@UWizardFormMouseUp;
      UnStartForm.OnMouseMove:=@UWizardFormMouseMove;

      //透明
      UCreateFrame0;

      //按钮公用字体样式
      WFButtonFont:=TFont.Create;
      WFButtonFont.Name:='宋体'
      WFButtonFont.Size:=9;

      //圆角
      ShapeForm(UnStartForm, radius);

      //主窗口
      //UninstallImage := ImgLoad1(UnStartForm.Handle, ExpandConstant('{tmp}\window.png'), (-10), (-10), UnStartForm.ClientWidth,UnStartForm.ClientHeight,True,True);
      UninstallImage := ImgLoad1(UnStartForm.Handle,ExpandConstant('{tmp}\window.png'),-10,-10,622,448,True,True);

      UnCloseButton := TButton.Create(UnStartForm);
      UnCloseButton.Parent := UnStartForm;
      UnCloseButton.ModalResult := mrCancel;
      UnCloseButton.Cancel := True;
      UnCloseButton.SetBounds(0, 0, 0, 0);
      UnCloseButton.Hide;

      //确定卸载
      UnConfirmButton := TButton.Create(UnStartForm);
      UnConfirmButton.Parent := UnStartForm;
      UnConfirmButton.ModalResult := mrOk;
      UnConfirmButton.SetBounds(0, 0, 0, 0);
      UnConfirmButton.Hide;

      Label21 := TLabel.Create(UnStartForm);
      Label21.Parent := UnStartForm;
      Label21.Caption := '{#DS_APP_NAME}';
      Label21.Transparent := true;
      Label21.Font.Size:=11
      Label21.Font.Name:='微软雅黑'
      Label21.Font.Style := [fsBold];
      //Label1.Font.Color:=clSilver
      Label21.Font.Color:=$F4F2E0
      Label21.Left := (15);
      Label21.Top := (10);

      UninstallLabel:= TLabel.Create(UnStartForm);
      UninstallLabel.AutoSize:=False;
      UninstallLabel.Font.Name :='微软雅黑'
      UninstallLabel.Font.Size :=15
      UninstallLabel.Font.Color:=clWhite
      UninstallLabel.Parent:=UnStartForm;
      UninstallLabel.WordWrap := True;
      UninstallLabel.SetBounds(ScaleX(114), ScaleY(258), ScaleY(500), ScaleY(50));
      UninstallLabel.OnMouseDown:=@UninstallFormMouseDown;
      UninstallLabel.Transparent:=True;
      UninstallLabel.Caption := '您确定要卸载101教育PPT的安装程序吗？';

      //最小化
      //btnUninstallMin:=BtnCreate1(UnStartForm.Handle,548,0,26,26,ExpandConstant('{tmp}\btn_min.png'),3,False);
      //BtnSetEvent1(btnUninstallMin,BtnClickEventID,WrapBtnCallback1(@UnShowMin,1));

      //关闭按钮
      btnUninstallClose:=BtnCreate1(UnStartForm.Handle,574,0,26,26,ExpandConstant('{tmp}\btn_close.png'),1,False);
      BtnSetEvent1(btnUninstallClose,BtnClickEventID,WrapBtnCallback1(@ExitUninstallBtnClick,1));

      //确定按钮
      btnUninstallYes:=BtnCreate1(UnStartForm.Handle,495,377,88,40,ExpandConstant('{tmp}\btn_ok.png'),1,False);
      BtnSetEvent1(btnUninstallYes,BtnClickEventID,WrapBtnCallback1(@UnConfirmBtnOnClick,1));

      //取消按钮
      btnUninstallNo := BtnCreate1(UnStartForm.Handle,392,378,84,40,ExpandConstant('{tmp}\btn_cancel.png'),1,False);
      BtnSetEvent1(btnUninstallNo,BtnClickEventID,WrapBtnCallback1(@ExitUninstallBtnClick,1));

      ImgApplyChanges1(UnStartForm.Handle);
      MResult := UnStartForm.ShowModal;

      finally
      UnStartForm.Hide;
      UnStartForm.Free;
      if MResult <> mrOk then Abort;
      end;

      UFrame.Hide;
      UninstallProgressForm.Visible := false;
      UninstallProgressForm.Width := 0;
      UninstallProgressForm.Height := 0;
      UninstallProgressForm.Hide;
      
       // UFrame.Hide;
    //UninstallProgressForm.Hide;
     //  MsgBox('CreateUninstallSetpForm', mbConfirmation, MB_YESNO);
      //UninstallProgressForm 卸载进度窗体
      UninstallProgressForm.Width := 602;
      UninstallProgressForm.Height := 426;
      UninstallProgressForm.BorderStyle := bsNone;
      UninstallProgressForm.OuterNotebook.Hide;
      UninstallProgressForm.MainPanel.Hide;
      UninstallProgressForm.Bevel1.Hide;
      UninstallProgressForm.StatusLabel.Hide;
      UninstallProgressForm.BeveledLabel.Hide;
      UninstallProgressForm.Bevel.Hide;
      UninstallProgressForm.CancelButton.Hide;

   //   MsgBox('CreateUninstallSetpForm2', mbConfirmation, MB_YESNO);

end;


  // 新建卸载窗口
  procedure CreateUninstallSetpForm();
  var
    UninstallingLabe: Tlabel;
    
  begin
  
      //圆角
      ShapeForm(UninstallProgressForm, radius);

      UninstallProgressForm.OnMouseDown := @UProgressWizardFormMouseDown;
      UninstallProgressForm.OnMouseUp := @UProgressWizardFormMouseUp;
      UninstallProgressForm.OnMouseMove := @UProgressWizardFormMouseMove;

      // 透明
      UCreateProgressFrame;
      
     // MsgBox('CreateUninstallSetpForm3333', mbConfirmation, MB_YESNO);
      //框架
      //UninstallIng:=ImgLoad1(UninstallProgressForm.Handle,ExpandConstant('{#DS_UNINSTALL_RESOURCES_PATH}\window.png'),(-10), (-10), UninstallProgressForm.ClientWidth,UninstallProgressForm.ClientHeight,True,True);
      UninstallIng:=ImgLoad1(UninstallProgressForm.Handle,ExpandConstant('{tmp}\window.png'),-10, -10,622,448,True,True);

      Label21 := TLabel.Create(UninstallProgressForm);
      Label21.Parent := UninstallProgressForm;
      Label21.Caption := '{#DS_APP_NAME}';
      Label21.Transparent := true;
      Label21.Font.Size:=11
      Label21.Font.Name:='微软雅黑'
      Label21.Font.Style := [fsBold];
      //Label1.Font.Color:=clSilver
      Label21.Font.Color:=$F4F2E0
      Label21.Left := (15);
      Label21.Top := (10);
      
      //感谢您使用101教育PPT!
      UninstallingLabe:= TLabel.Create(UninstallProgressForm);
      UninstallingLabe.AutoSize:=False;
      UninstallingLabe.Font.Name :='微软雅黑'
      UninstallingLabe.Font.Size :=15
      UninstallingLabe.Font.Color:=clWhite
      UninstallingLabe.Parent:=UninstallProgressForm;
      UninstallingLabe.WordWrap := True;
      UninstallingLabe.SetBounds(ScaleX(191), ScaleY(258), ScaleY(500), ScaleY(50));
      //UninstallingLabe.OnMouseDown:=@UninstallFormMouseDown;
      UninstallingLabe.Transparent:=True;
      UninstallingLabe.Caption := '感谢您使用101教育PPT！';
      
      //正在卸载
      labUning:=TLabel.create(UninstallProgressForm);
      with labUning do
      begin
      	parent:=UninstallProgressForm;
      	left:= 27;
      	top := 380;
      	width:=50;
      	caption:='正在卸载';
        Font.Color := $898989; //控件文字颜色
        Font.Name :='微软雅黑';
        Font.Size := 9;
        Transparent := True; //控件透明显示
      end;

      //进度条
      progressbgImg1 := ImgLoad1(UninstallProgressForm.Handle,ExpandConstant('{tmp}\Pbbg.png'),26,403,552,6,True,True);
      progressbgImg := ImgLoad1(UninstallProgressForm.Handle,ExpandConstant('{tmp}\Pbjd.png'),0,0,0,0,True,True);
      PBOldProcU := SetWindowLong(UninstallProgressForm.ProgressBar.Handle, -4, PBCallBackU(@PBProcU, 4));
      
      // MsgBox('CreateUninstallSetpForm44444', mbConfirmation, MB_YESNO);

      ImgApplyChanges1(UninstallProgressForm.Handle);
      UProFrame.Show;
      UnInstallProgressForm.Show;
       IsUninstallCfg := false;
       if MsgBox('是否删除配置文件?？'#13''#13'单击“是”删除配置文件；'#13''#13'单击“否”保留配置文件！', mbConfirmation, MB_YESNO) = IDYES then
        begin
           IsUninstallCfg := true;
        end

      // 卸载窗口创建(创建必须在这里，独立创建会显示不了)
      UninstalledForm := CreateCustomForm();
      UninstalledForm.Width := 602;
      UninstalledForm.Height := 426;
      UninstalledForm.BorderStyle := bsNone;
      //UninstalledForm.Position := poScreenCenter;
  
  end;


  //卸载完成界面
  procedure CreateUninstalledForm();
  begin
      Label21 := TLabel.Create(UninstalledForm);
      Label21.Parent := UninstalledForm;
      Label21.Caption := '{#DS_APP_NAME}';
      Label21.Transparent := true;
      Label21.Font.Size:=11
      Label21.Font.Name:='微软雅黑'
      Label21.Font.Style := [fsBold];
      //Label1.Font.Color:=clSilver
      Label21.Font.Color:=$F4F2E0
      Label21.Left := (15);
      Label21.Top := (10);

      //按钮公用字体样式
      WFButtonFont:=TFont.Create;
      WFButtonFont.Name:='宋体'
      WFButtonFont.Size:=9;

      //圆角
      ShapeForm(UninstalledForm, radius);

      UninstalledForm.OnMouseDown := @UFinishWizardFormMouseDown;
      UninstalledForm.OnMouseUp := @UFinishWizardFormMouseUp;
      UninstalledForm.OnMouseMove := @UFinishWizardFormMouseMove;

      // 透明
      UCreateFinshFrame;

      //框架
      //UninstallIng:=ImgLoad1(UninstalledForm.Handle,ExpandConstant('{tmp}\windows1.png'),(-10), (-10), UninstalledForm.ClientWidth,UninstalledForm.ClientHeight,True,True);
      UninstallIng:=ImgLoad1(UninstalledForm.Handle,ExpandConstant('{tmp}\UnInstalled.png'),-10, -10,622,448,True,True);

      //完成按钮
      btnUninstallFinish :=BtnCreate1(UninstalledForm.Handle,495,378,88,40,ExpandConstant('{tmp}\btn_ok.png'),1,False);

      BtnSetEvent1(btnUninstallFinish,BtnClickEventID,WrapBtnCallback1(@UninstalledBtnOnClick,1));

       UProFrame.Hide;
       
    ImgApplyChanges1(UninstalledForm.Handle);
    UninstalledForm.ShowModal;
    //DelTree(ExpandConstant('{app}'), True, True, True);

    if  (bReadTreatyCheck = true)  then
    begin
      //DelMydoc();
    end;
    
  end;
  
  
   //卸载完成后调用
procedure DeinitializeUninstall();
begin
  gdipShutdown;
  Application.Terminate;
end;


  // 卸载步骤
  procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
  begin
  
    case CurUninstallStep of
        //开始卸载
        usUninstall:
        begin
        CreateUninstallSetpForm();
        end;

        //卸载完成
        usPostUninstall:
        begin
        CreateUninstalledForm();
        //DelRegedit();
        //RemoveDir('C:\Program Files (x86)\101教育PPT\plugins\Uninstall');
        // DeleteFile('C:\Program Files (x86)\101教育PPT\plugins\Uninstall\botva2.dll');
        // 卸载后注册表操作
        UnInstallDelContent();
        end;
        
        usDone:
        begin
        if IsUninstallCfg = true then
      begin
        //MsgBox('IsUninstallCfg = true！', mbConfirmation, MB_YESNO);
        DelTree(ExpandConstant('{app}'), True, True, True);
      end
      else
      begin
      //MsgBox('IsUninstallCfg = false', mbConfirmation, MB_YESNO);
      //DeleteFile(ExpandConstant('{app}\101PPT.exe'));
      //DeleteFile(ExpandConstant('{app}\DuiLib.dll'));
      //DelTree(ExpandConstant('{app}\Skins'), True, True, True);
      end
        end
        
     end;
  end;
  

  procedure DeinitializeSetup();
  begin
  gdipShutdown;  //背景图
  if PBOldProc<>0 then SetWindowLong(WizardForm.ProgressGauge.Handle,-4,PBOldProc);
  end;

 
  function DefSysRegedit(Isys: Integer): Boolean;
  var
   cIsys: integer;
   Param: String;
   DefaultDir :  String;
   //UninstallNa : String;
  begin
    if Isys = 64 then
    begin
     cIsys :=  HKLM64;
    end
    else
    begin
     cIsys := HKLM32;
    end
    
     DefaultDir := GetDefaultInstallRoot(Param);
     
     cIsys := HKLM;
      
    // UninstallNa := GetUninstallExeNa(Param);
//    RegWriteStringValue(cIsys,'{#DS_UNINSTALL_REGEDIT_PATH}', 'Start Menu', ExpandConstant('{group}\{code:GetDefaultInstallRoot}.lnk'));
      RegWriteStringValue(cIsys,'{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir, 'Start Menu', ExpandConstant('{group}\{code:GetDefaultInstallRoot}.lnk'));
      RegWriteStringValue(cIsys,'{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir, 'Start Menu Url', ExpandConstant('{group}\101教育PPT官网.lnk'));
      RegWriteStringValue(cIsys,'{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir, 'Start Menu Uninstall', ExpandConstant('{group}\卸载101教育PPT.lnk'));
      RegWriteStringValue(cIsys,'{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir, 'Start Menu Folder', ExpandConstant('{group}\{code:GetDefaultInstallRoot}'));
      RegWriteStringValue(cIsys,'{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir, 'commondesktop', ExpandConstant('{commondesktop}\{code:GetDefaultInstallRoot}.lnk'));
      RegWriteStringValue(cIsys,'{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir, 'commondesktop Url', ExpandConstant('{commondesktop}\101教育PPT官网.lnk'));
      RegWriteStringValue(cIsys,'{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir, 'userappdata', ExpandConstant('{userappdata}\Microsoft\Internet Explorer\Quick Launch\{code:GetDefaultInstallRoot}.lnk'));

      RegWriteStringValue(cIsys,'{#DS_REGEDIT_APP_PATH}'+DefaultDir+'.exe', '', ExpandConstant('{app}\{#DS_MAIN_FILE_NAME}'));
      //RegWriteStringValue(HKLM, 'Software\Microsoft\Windows\CurrentVersion\Uninstall\101教育PPT', 'DisplayName', '{#DS_APP_NAME}');
      RegWriteStringValue(cIsys, '{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir, 'DisplayName', DefaultDir);
      //RegWriteStringValue(HKLM, 'Software\Microsoft\Windows\CurrentVersion\Uninstall\101教育PPT', 'UninstallString', ExpandConstant('{app}\Uninstall.exe'));
      RegWriteStringValue(cIsys, '{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir, 'UninstallString', '"' + ExpandConstant('{app}\unins000.exe')+'"'+' /SILENT');
      //RegWriteStringValue(cIsys, '{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir, 'UninstallString', ExpandConstant('{app}\unins000.exe'));
      
      //RegWriteStringValue(cIsys, '{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir, 'QuietUninstallString', '"' + ExpandConstant('{app}\unins000.exe')+'"'+' /SILENT');
      //RegWriteStringValue(HKLM, 'Software\Microsoft\Windows\CurrentVersion\Uninstall\101教育PPT', 'DisplayIcon', ExpandConstant('{app}\101PPT.exe'));
      RegWriteStringValue(cIsys, '{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir, 'DisplayIcon', ExpandConstant('{app}\{#DS_MAIN_FILE_NAME}'));
      //RegWriteStringValue(HKLM, 'Software\Microsoft\Windows\CurrentVersion\Uninstall\101教育PPT', 'DisplayVersion', '1.1.3.8' );
      RegWriteStringValue(cIsys, '{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir, 'DisplayVersion', '{#DS_APP_VER_NAME}' );
      //RegWriteStringValue(HKLM, 'Software\Microsoft\Windows\CurrentVersion\Uninstall\101教育PPT', 'URLInfoAbout', 'http://ppt.101.com/');
      RegWriteStringValue(cIsys, '{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir, 'URLInfoAbout', '{#DS_URL}');
      //RegWriteStringValue(HKLM, 'Software\Microsoft\Windows\CurrentVersion\Uninstall\101教育PPT', 'Publisher', '华渔教育公司');
      RegWriteStringValue(cIsys, '{#DS_UNINSTALL_REGEDIT_PATH}'+DefaultDir, 'Publisher', '{#DS_COMPANY_Name}');
      
      // 右键菜单
      
      //RegWriteStringValue(HKCR, '*\shell\101教育PPT', '', '上传101教育PPT网盘');
      //RegWriteStringValue(HKCR, '*\shell\101教育PPT', 'Icon', '"' + ExpandConstant('{app}\101PPT.exe')+'"');
      //RegWriteStringValue(HKCR, '*\shell\101教育PPT\command', '', '"' + ExpandConstant('{app}\101PPT.exe')+'%1'+'"');


  end;
  

  //安装后注册表操作
  procedure InsterRegedit();
  var
  ResultCode :Boolean;
  begin
    if IsWin64 then
    begin
    DefSysRegedit(64);
    end
    else
    begin
    DefSysRegedit(32);
    end
  end;


  //安装步骤
  procedure CurStepChanged(CurStep: TSetupStep);
  var
  RCode: Integer;
  sIcon: String;
  uninspath, uninsname, NewUninsName, MyAppName: string;
  uninstallExeNa :String;
  param :String;
  
  begin

  case CurStep of
  
      ssInstall:
      begin
      sIcon:= ExpandConstant('{tmp}\Uninstall.ico'); //定义卸载图标
      ExtractTemporaryFile(ExtractFileName(sIcon)); //释放卸载图标
      //要替换图标的exe文件路径名称留空，则插件会自动替换掉Inno卸载程序的图标！
      UpdateUninstIcon(sIcon);//替换卸载图标
      end

      ssDone:
      begin
      
        // 安装后注册表操作
        InsterRegedit();
      
//      // 指定新的卸载文件名（不包含扩展名），请相应修改！
//
//      uninstallExeNa := GetUninstallExeNa(param);
//      NewUninsName := uninstallExeNa;
//      // 应用程序名称，与 [SEUTP] 段的 AppName 必须一致，请相应修改！
//      MyAppName := '{#DS_APP_NAME}';
//      // 以下重命名卸载文件
//      uninspath:= ExtractFilePath(ExpandConstant('{uninstallexe}'));
//      uninsname:= Copy(ExtractFileName(ExpandConstant('{uninstallexe}')),1,8);
//      RenameFile(uninspath + uninsname + '.exe', uninspath + NewUninsName + '.exe');
//      RenameFile(uninspath + uninsname + '.dat', uninspath + NewUninsName + '.dat');
        //// 以下修改相应的注册表内容
        if RegKeyExists(HKEY_LOCAL_MACHINE, '{#DS_UNINSTALL_REGEDIT_PATH}' + GUID + '_is1') then
        begin
        //RegWriteStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall\' + GUID + '_is1', 'UninstallString', '"' + uninspath + NewUninsName + '.exe"');
        //RegWriteStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall\' + GUID + '_is1', 'QuietUninstallString', '"' + uninspath + NewUninsName + '.exe" /SILENT');
         RegDeleteValue(HKEY_LOCAL_MACHINE, '{#DS_UNINSTALL_REGEDIT_PATH}' + GUID + '_is1', 'UninstallString');
        end

        //锁定任务栏
        ShellExec('taskbarpin', ExpandConstant('{userappdata}\\Microsoft\\Internet Explorer\\Quick Launch\\{code:GetPackDirByVer}'), '', '', SW_SHOWNORMAL, ewNoWait, RCode);

        //启动主程序
        if  bOpenExe then
        begin
          Exec(ExpandConstant('{app}\101PPT.exe'), 'TaskbarPin', '', SW_SHOW, ewNoWait, RCode);
        end;
        
     end;
  end;
  end;

