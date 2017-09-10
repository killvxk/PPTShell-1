;7Z打开空白
!system '>blank set/p=MSCF<nul'
!packhdr temp.dat 'cmd /c Copy /b temp.dat /b +blank&&del blank'

#========================================================================================================================
#
#         PS: 当前脚本没有生成Uninstall,安装包要放上已生成的Unistall.exe      搜索【 WriteUninstaller "$INSTDIR\Uninstall.exe" 】判断是否生成
#
#========================================================================================================================

Var MSG     ;MSG变量必须定义，而且在最前面，否则WndProc::onCallback不工作，插件中需要这个消息变量,用于记录消息信息
Var Dialog  ;Dialog变量也需要定义，他可能是NSIS默认的对话框变量用于保存窗体中控件的信息
Var DialogWidth
Var DialogHight

Var IconImage ;图标图片
Var BGImage  ;背景大图
Var ImageHandle
Var IconHandle
Var DefaultPage

Var WarningForm

Var Btn_QuickInstall
Var Btn_CustomInstall
Var Btn_Install
Var Btn_Done
Var Btn_Confirm
Var Btn_Back

Var CurrentPage ;当前页到退出页的间隔页数

Var Txt_License
Var Bool_License ;安装协议状态
Var Bool_LicenseShow
Var Chk_License
Var Btn_Agreement
Var Lbl_License

Var Lbl_Sumary
Var PPercent
Var PB_ProgressBar
Var SubProgress

Var Bool_UnInstall

Var Btn_No
Var Btn_Yes

Var Lbl_AppName
Var Lbl_Title
Var Lbl_Version

Var Txt_Browser
Var Btn_TxtBrowser
Var btn_Browser
var Txt_SpaceValue

Var Bool_Desktop ;安装到桌面
Var Bool_QuickRun  ;添加快速启动
Var Chk_Desktop
Var Chk_QuickRun


;---------------------------全局编译脚本预定义的常量-----------------------------------------------------
; -- 安装文件所放目录 为 【PROGRAM_NAME + PRODUCT_VERSION】 本例为  101教育PPT0.0.0.27
!define PRODUCT_NAME "101教育PPT"
!define PRODUCT_DIR "101教育PPT"
!define PROGRAM_NAME "101PPT"
!define MUST_PACKAGE_FOLDER_NAME "ImportantFile"

!define CACHE_DIR "Cache"
!define TEMP_DIR "Temp"

!define BIN_DIR "bin"
!define FLASH_OCX "flash.ocx"
!define KAITI "kaiti简.ttf"

!define PPTSHELL_EXE_NAME "${PROGRAM_NAME}.exe"
!define OLD_PPTSHELL_EXE_NAME "PPTShell.exe"
!define PPT_EXE_NAME "POWERPNT.exe"
!define UPDATE_EXE_NAME "update.exe"
!define COURSE_EXE_NAME "CoursePlayer.exe"
!define NODE_EXE_NAME "node.exe"

;!define MyMutex_Install     "PPTShell_Install"
;!define MyMutex_UnInstall   "PPTShell_UnInstall"


;!define ADBMGR_NAME "助手管理"
;!define ADBMGR_DIR "AssistantMgr"
;!define ADBMGR_PROGRAM_NAME "AssistantManager"

!define PRODUCT_VERSION "1.1.3.8"
!define PRODUCT_PUBLISHER "华渔教育科技有限公司"
!define PRODUCT_WEB_SITE "http://ppt.101.com"

!define PRODUCT_WEB_LINK_NAME "101教育PPT官网"

!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\${PROGRAM_NAME}.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

ShowInstDetails nevershow ;设置是否显示安装详细信息。
ShowUnInstDetails nevershow ;设置是否显示删除详细信息。


;应用程序显示名字
Name "${PRODUCT_NAME}"
;应用程序输出文件名
OutFile "${PRODUCT_NAME}${PRODUCT_VERSION}.exe"
;默认安装目录
InstallDir "$PROGRAMFILES\${PRODUCT_DIR}"
InstallDirRegKey HKLM "${PRODUCT_UNINST_KEY}" "UninstallString"

; MUI 预定义常量
;!define MUI_ABORTWARNING ;退出提示
;安装图标的路径名字
!define MUI_ICON "Icon\Install.ico"
;卸载图标的路径名字
!define MUI_UNICON "Icon\UnInstall.ico"
;使用的UI
!define MUI_UI "UI\mod.exe" ;本UI是修改过的

;---------------------------设置软件压缩类型（也可以通过外面编译脚本控制）------------------------------------
RequestExecutionLevel		admin

; 设置压缩选项
SetCompress auto
; 选择压缩方式
SetCompressor lzma
;设置数据块优化
SetDatablockOptimize on
;XPStyle on
; ------ MUI 现代界面定义 (1.67 版本以上兼容) ------
!include "MUI2.nsh"
!include "WinCore.nsh"
!include "nsWindows.nsh"
!include "LogicLib.nsh"
!include "FileFunc.nsh"
!include "nsDialogs.nsh"
!include "LoadRTF.nsh"
!include "WinVer.nsh"
!include "VersionCheckNew.nsh"
!include "WordFunc.nsh"

;!include "WndSubclass.nsh"  ;进度可能使用

!define /math PBM_SETRANGE32 ${WM_USER} + 6 #定义nsis未包含的进度条相关常量
;!define WS_BORDER 0x800000

!define MUI_CUSTOMFUNCTION_GUIINIT onGUIInit

;自定义页面
Page custom Page.1

Page custom Page.2

; 许可协议页面
;!define MUI_LICENSEPAGE_CHECKBOX
; 安装目录选择页面
;!insertmacro MUI_PAGE_DIRECTORY
; 安装过程页面
;!define MUI_PAGE_CUSTOMFUNCTION_PRO InstFilesPagePRO
!define MUI_PAGE_CUSTOMFUNCTION_SHOW InstFilesPageShow
!define MUI_PAGE_CUSTOMFUNCTION_LEAVE InstFileLeave
!insertmacro MUI_PAGE_INSTFILES

Page custom Page.3

Page custom Page.4

Page custom Page.Exit
; 安装完成页面
;!insertmacro MUI_PAGE_FINISH
; 安装卸载过程页面
UninstPage custom un.Page.5

UninstPage instfiles un.InstFiles.PRO un.InstFiles.Show un.InstFiles.Leave

UninstPage custom un.Page.6

UninstPage custom un.Page.7


; 安装界面包含的语言设置
!insertmacro MUI_LANGUAGE "SimpChinese"

;------------------------------------------------------MUI 现代界面定义以及函数结束------------------------

Function .onInit
  SetOverwrite on
  InitPluginsDir
  ;Call MutexCreate
  ;禁止多个安装程序实例
  System::Call 'kernel32::CreateMutexA(i 0, i 0, t "PPTShell_Install") i .r1 ?e'
  Pop $R0
  StrCmp $R0 0 +3
  MessageBox MB_OK|MB_ICONEXCLAMATION "安装或者卸载程序正在运行"
  Abort
  
  ;InstallDir "$PROGRAMFILES\${PRODUCT_DIR}"
  
  ;Call UnInstallDirver  ;必要的时候添加卸载驱动  zcs
  FindProcDLL::FindProc  "${PPTSHELL_EXE_NAME}" ;"${PROGRAM_NAME}.exe"
  Pop $R0
  IntCmp $R0 1 kill_app no_run
  kill_app:
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON1 "${PRODUCT_NAME}正在运行！点“是”将关闭继续安装，点“否”退出安装程序。" IDYES kill IDNO quit
  kill:
    ;Call KillPPT
    KillProcDLL::KillProc "${PPT_EXE_NAME}"
    KillProcDLL::KillProc "${PPTSHELL_EXE_NAME}"
    KillProcDLL::KillProc "${PPTSHELL_EXE_NAME}"
    Sleep 500
    goto install
  quit:
    Quit
  no_run:
    ;Call UninstallSoft ;调用卸载判断
  install:
    ;Call KillUpdate
    ;Call KillCoursePlayer
    KillProcDll::KillProc "${OLD_PPTSHELL_EXE_NAME}"
    KillProcDLL::KillProc "${UPDATE_EXE_NAME}"
    KillProcDLL::KillProc "${COURSE_EXE_NAME}"
    KillProcDLL::KillProc "${NODE_EXE_NAME}"
    Call CheckOldProc
  
  ;Kill Adb

  ;FindProcDLL::FindProc "adb.exe"
  ;Pop $R0
  ;IntCmp $R0 1 kill_adb adb_no_run
  ;kill_adb:
  ;  KillProcDLL::KillProc "adb.exe"
  ;  Sleep 100
  ;  goto install
  ;adb_no_run:

    StrCpy $Bool_License 1
    StrCpy $Bool_Desktop 1
    StrCpy $Bool_QuickRun 1
    
    StrCpy $DialogWidth 602 ;588
    StrCpy $DialogHight 428 ;291
    
    #===================================================================
    #           un.onInit 也记得要释放图片
    #===================================================================
    
    File `/oname=$PLUGINSDIR\license.rtf` `license\license.rtf`
    
    File `/ONAME=$PLUGINSDIR\bg.bmp` `images\StPad.bmp`
    File `/ONAME=$PLUGINSDIR\icon.bmp` `images\icon.bmp`
    File `/ONAME=$PLUGINSDIR\IconP.bmp` `images\IconP.bmp`
    
    File `/ONAME=$PLUGINSDIR\QuickInstall.bmp` `images\QuickInstall.bmp`
    File `/ONAME=$PLUGINSDIR\CustomInstall.bmp` `images\CustomInstall.bmp`
    File `/ONAME=$PLUGINSDIR\License_Checked.bmp` `images\License_Checked.bmp`
    File `/ONAME=$PLUGINSDIR\License_NoChecked.bmp` `images\License_NoChecked.bmp`
    File `/ONAME=$PLUGINSDIR\Agreement.bmp` `images\Agreement.bmp`
    File `/ONAME=$PLUGINSDIR\Confirm.bmp` `images\Confirm.bmp`
    File `/ONAME=$PLUGINSDIR\Back.bmp` `images\Back.bmp`
    
    File `/ONAME=$PLUGINSDIR\Install.bmp` `images\Install.bmp`
    File `/ONAME=$PLUGINSDIR\InstallDir.bmp` `images\InstallDir.bmp`
    File `/ONAME=$PLUGINSDIR\Browse.bmp` `images\Browse.bmp`
    
    File `/ONAME=$PLUGINSDIR\Yes.bmp` `images\Yes.bmp`
    File `/ONAME=$PLUGINSDIR\No.bmp` `images\No.bmp`
    File `/ONAME=$PLUGINSDIR\Done.bmp` `images\Done.bmp`

    File `/oname=$PLUGINSDIR\btnClose.bmp` `images\close.bmp`
    File `/oname=$PLUGINSDIR\btnmini.bmp` `images\mini.bmp`
    
    File `/oname=$PLUGINSDIR\VersionBgLeft.bmp` `images\VersionBgLeft.bmp`
    File `/oname=$PLUGINSDIR\VersionBgRight.bmp` `images\VersionBgRight.bmp`
    ;安装页面
    File `/oname=$PLUGINSDIR\Intro.bmp` `images\Install_Intro.bmp`
    File `/oname=$PLUGINSDIR\IconDone.bmp` `images\IconDone.bmp`
    ;卸载页面
    File `/oname=$PLUGINSDIR\IconUnInstall.bmp` `images\IconUnInstall.bmp`
    
    ;进度条皮肤
    File `/oname=$PLUGINSDIR\Progress.bmp` `images\Progress.bmp`
    File `/oname=$PLUGINSDIR\ProgressBar.bmp` `images\ProgressBar.bmp`
    ;File `/ONAME=$PLUGINSDIR\PPercent.bmp` `images\PPercent.bmp`
    
    ;自定义安装 CheckBox
    File `/ONAME=$PLUGINSDIR\Desktop_Checked.bmp` `images\Desktop_Checked.bmp`
    File `/ONAME=$PLUGINSDIR\Desktop_NoChecked.bmp` `images\Desktop_NoChecked.bmp`
    
    File `/ONAME=$PLUGINSDIR\QuickRun_Checked.bmp` `images\QuickRun_Checked.bmp`
    File `/ONAME=$PLUGINSDIR\QuickRun_NoChecked.bmp` `images\QuickRun_NoChecked.bmp`
    
    SkinBtn::Init "$PLUGINSDIR\QuickInstall.bmp"
    SkinBtn::Init "$PLUGINSDIR\close.bmp"
FunctionEnd

Function KillPPT
  FindProcDLL::FindProc "${PPT_NAME}.EXE"
  Pop $R0
  IntCmp $R0 1 kill_ppt ppt_no_run
  kill_ppt:
    KillProcDLL::KillProc "${PPT_NAME}.EXE"
    Sleep 500
  ppt_no_run:
FunctionEnd

Function KillUpdate
  FindProcDLL::FindProc "${UPDATE_NAME}.EXE"
  Pop $R0
  IntCmp $R0 1 kill_update update_no_run
  kill_update:
    KillProcDLL::KillProc "${UPDATE_NAME}.EXE"
    Sleep 500
  update_no_run:
FunctionEnd

Function KillCoursePlayer
  FindProcDLL::FindProc "${COURSE_NAME}.EXE"
  Pop $R0
  IntCmp $R0 1 kill_course course_no_run
  kill_course:
    KillProcDLL::KillProc "${COURSE_NAME}.EXE"
    Sleep 500
  course_no_run:
FunctionEnd

Function KillAllApp
    KillProcDLL::KillProc "${PPT_EXE_NAME}"
    KillProcDLL::KillProc "${PPTSHELL_EXE_NAME}"
    KillProcDll::KillProc "${OLD_PPTSHELL_EXE_NAME}"
    KillProcDLL::KillProc "${UPDATE_EXE_NAME}"
    KillProcDLL::KillProc "${COURSE_EXE_NAME}"
    KillProcDLL::KillProc "${NODE_EXE_NAME}"
FunctionEnd

;-------------检测计算机上安装程序已运行----------
;Function MutexCreate
;  System::Call 'kernel32::CreateMutexA(i 0, i 0, t"${MyMutex_Install}") i.R3 ?e'  ;#检查安装互斥：#
;  Pop $R2
;  System::Call 'kernel32::CloseHandle(i R1) i.s'

;  IntCmp $R2 0 CheckUnMutex MutexMsg   ;#判断安装互斥的存在#
;MutexMsg:
;  MessageBox MB_OK|MB_ICONEXCLAMATION "安装程序已经运行！"
;  Quit
;CheckUnMutex:
;  System::Call 'kernel32::CreateMutexA(i 0, i 0, t"${MyMutex_UnInstall}") i.R3 ?e'    ;#检查卸载互斥：#
;  Pop $R2
;  System::Call 'kernel32::CloseHandle(i R3) i.s'

;  IntCmp $R2 0 ExitCheckMutex UnMutexMsg      ;#判断卸载互斥的存在#
;UnMutexMsg:
;  MessageBox MB_OK|MB_ICONEXCLAMATION "卸载程序已经运行！"
;  Quit
;ExitCheckMutex:
;FunctionEnd

;Section Install
;  SetOutPath $EXEDIR
;  WriteUninstaller "$EXEDIR\Uninstall.exe"
;SectionEnd

;-------------检测计算机上是否已安装此程序及版本情况----------

Function CheckOldProc

  ClearErrors

  ReadRegStr $0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion"
  ReadRegStr $1 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName"
  ReadRegStr $2 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher"
  ReadRegStr $6 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString"
  ${GetParent} "$6" $R6

  IfErrors ExitCheckOldProc
  
  StrCmp $1 ${PRODUCT_NAME} 0 ExitCheckOldProc    ;解决【1.0.1.1】版本改进程名的问题

  ${VersionCheckNew} $0 ${PRODUCT_VERSION} "$R0"
  StrCmp $R0 0 0 ExistProcCmp ;当前计算机上安装版本与安装盘版本一致
  MessageBox MB_OKCANCEL|MB_DEFBUTTON2|MB_TOPMOST "您已安装的$1版本，与当前版本一致。您确定要覆盖此程序吗？" IDCANCEL DoCancel
  IfFileExists "$R6" SetInstDir ExitCheckOldProc ;如果 $R1软件位置 还有文件则跳转到 DIR: 删除里面的所有文件
SetInstDir:
   StrCpy $INSTDIR $R6
   Goto ExitCheckOldProc

ExistProcCmp:
  StrCmp $R0 1 0 ExitCheckOldProc ;当前计算上的版本比安装盘版本高       ;ExistProcL
  ;MessageBox MB_OK "您已安装的$1版本，高于当前版本${PRODUCT_VERSION}。不能安装此程序，请卸载后再安装。"
  ;Quit
  ;运行卸载程序
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_TOPMOST "您已安装的$1版本，高于当前版本${PRODUCT_VERSION}。不能继续安装，是否卸载后继续安装？" IDNo DoCancel
  ;ReadRegStr $R0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "UninstallString"
  ;${GetParent} "$6" $R6
  ;StrCmp $R0 "" ExitCheckOldProc
  ;IfFileExists $R0 UnInstallApp
	;Goto ExitCheckOldProc
;UnInstallApp:
  ;ExecWait "$R0 /S _?=$R1" ;这里$R0为读取到的卸载程序名称， /S是静默卸载参数使用NSIS生成的卸载程序必须要加上 _? 才能等待卸载。$R1是软件位置
  IfFileExists "$R6" DelFile ;如果 $R1软件位置 还有文件则跳转到 DIR: 删除里面的所有文件
  Goto ExitCheckOldProc
DelFile: ;如果文件夹存在
	;Delete "$R1\*.*" ;即删除里面所有文件,请谨慎使用
  StrCpy $INSTDIR $R6
  Delete "$INSTDIR\*.exe"
  Delete "$INSTDIR\*.dll"
  Delete "$INSTDIR\*.dat"
  Delete "$INSTDIR\*.log"

  RMDir /r /REBOOTOK "$INSTDIR\bin"
  RMDir /r /REBOOTOK "$INSTDIR\Skins"
  RMDir /r /REBOOTOK "$INSTDIR\template"
  RMDir /r /REBOOTOK "$INSTDIR\Cache"
  RMDir /r /REBOOTOK "$INSTDIR\log"
  RMDir /r /REBOOTOK "$INSTDIR\temp"
  
  Call InstallDelContent
  
  Goto ExitCheckOldProc
ExistProcL:
  MessageBox MB_OKCANCEL|MB_DEFBUTTON2|MB_TOPMOST "您已安装了 $1 的较低版本 $0 ，当前安装版本为${PRODUCT_VERSION}。您确定要用新版程序覆盖旧版程序吗？" IDOK ExitCheckOldProc IDCANCEL DoCancel
DoCancel:
  Quit
ExitCheckOldProc:
FunctionEnd

Function InstallDelContent
  SetShellVarContext all
  Delete "$DESKTOP\${PRODUCT_NAME}.lnk"
  Delete "$SMPROGRAMS\${PRODUCT_NAME}.lnk"
  Delete "$DESKTOP\${PRODUCT_WEB_LINK_NAME}.lnk"
  RMDir /r /REBOOTOK "$SMPROGRAMS\${PRODUCT_NAME}"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"

  ;恢复注册表
 #------------------ Win 8 -------------------------------------
  DeleteRegKey HKCR ".ppt\PPTShell"
  DeleteRegKey HKCR ".pptx\PPTShells"

  WriteRegStr HKCR ".ppt" "" "PowerPoint.Show.8"
  WriteRegStr HKCR ".ppt\PowerPoint.Show.8\ShellNew" "" ""

  WriteRegStr HKCR ".pptx" "" "PowerPoint.Show.12"
  WriteRegStr HKCR ".pptx\PowerPoint.Show.12\ShellNew" "" ""

 #------------------ 其它 -------------------------------------

 DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.pptx\UserChoice"
 DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.ppt\UserChoice"

 DeleteRegKey HKCR "PPTShell\\Shell\\Open\\Command"
 DeleteRegKey HKCU "Software\\Classes\\Applications\\PPTShell\\shell\\open\\command"

 WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.pptx\UserChoice" "Progid" "PowerPoint.Show.12"
 WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.ppt\UserChoice" "Progid" "PowerPoint.Show.8"
FunctionEnd

Function onGUIInit

    ;消除边框
    System::Call `user32::SetWindowLong(i$HWNDPARENT,i${GWL_STYLE},0x9480084C)i.R0`
    ;设置阴影
    System::Call `user32::SetClassLong(i$HWNDPARENT,i${GCL_STYLE},i${CS_DROPSHADOW})`
    ;隐藏一些既有控件
    GetDlgItem $0 $HWNDPARENT 1034
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1035
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1036
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1037
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1038
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1039
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1256
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1028
    ShowWindow $0 ${SW_HIDE}

    ${NSW_SetWindowSize} $HWNDPARENT $DialogWidth $DialogHight ;改变主窗体大小
    System::Call User32::GetDesktopWindow()i.R0
    ;圆角
    System::Alloc 16
    System::Call user32::GetWindowRect(i$HWNDPARENT,isR0)
    System::Call *$R0(i.R1,i.R2,i.R3,i.R4)
    IntOp $R3 $R3 - $R1
    IntOp $R4 $R4 - $R2
    System::Call gdi32::CreateRoundRectRgn(i0,i0,iR3,iR4,i4,i4)i.r0
    System::Call user32::SetWindowRgn(i$HWNDPARENT,ir0,i1)
    System::Free $R0
    
    
FunctionEnd

Function .onInstFailed
  Call OnExit
FunctionEnd

;处理无边框移动
Function onGUICallback
  ${If} $MSG = ${WM_LBUTTONDOWN}
        SendMessage $HWNDPARENT ${WM_NCLBUTTONDOWN} ${HTCAPTION} $0
  ${EndIf}
FunctionEnd

;弹出对话框移动
Function onWarningGUICallback
  ${If} $MSG = ${WM_LBUTTONDOWN}
    SendMessage $WarningForm ${WM_NCLBUTTONDOWN} ${HTCAPTION} $0
  ${EndIf}
FunctionEnd

#=======================================================================#
#                             安装包 -- 首页                            #
#=======================================================================#
Function Page.1

    GetDlgItem $0 $HWNDPARENT 1
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 2
    ShowWindow $0 ${SW_HIDE}
    EnableWindow $0 0       ;禁止取消按钮，达到屏蔽ESC的效果
    GetDlgItem $0 $HWNDPARENT 3
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1990
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1991
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1992
    ShowWindow $0 ${SW_HIDE}
    
    StrCpy $Bool_LicenseShow 0
    ;设置当前页面位置与退出页面的相隔数
    StrCpy $CurrentPage 6
    
    nsDialogs::Create 1044
    Pop $0
    ${If} $0 == error
        Abort
    ${EndIf}
    SetCtlColors $0 ""  transparent ;背景设成透明
    ${NSW_SetWindowSize} $0 601 427 ;改变Page大小   588 291

    ;应用名     zcs
;    ;${NSD_CreateLabel} 270 70 300 60 "${PRODUCT_NAME}"        ;--原位置 左边有图标时
;    ${NSD_CreateLabel} 200 70 300 60 "${PRODUCT_NAME}"         ;--无图标 居中
;    Pop $Lbl_AppName
;    SetCtlColors $Lbl_AppName "FFFFFF" transparent ;背景设成透明
;    CreateFont $1 "微软雅黑" "28" "400"
;    SendMessage $Lbl_AppName ${WM_SETFONT} $1 0
;    ;;${NSD_AddStyle} $Lbl_AppName ${ES_CENTER}
;    GetFunctionAddress $0 onGUICallback
;    WndProc::onCallback $Lbl_AppName $0 ;处理无边框窗体移动

    ;标题文字
    ${NSD_CreateLabel} 5u 5u 115u 12u "${PRODUCT_NAME}" ;安装向导"
    Pop $Lbl_Title
    ;SetCtlColors $Lbl_Title "" 0xFFFFFF ;蓝色
    SetCtlColors $Lbl_Title "FFFFFF"  transparent ;背景设成透明
    CreateFont $1 "微软雅黑" "11" "800"
    SendMessage $Lbl_Title ${WM_SETFONT} $1 0
    ;${NSD_AddStyle} $Lbl_Title ${ES_CENTER}
    GetFunctionAddress $0 onGUICallback
    WndProc::onCallback $Lbl_Title $0 ;处理无边框窗体移动

    ;程序版本
    ${NSD_CreateLabel} 305 180 115u 12u "当前版本" ;  ${PRODUCT_VERSION}"
    Pop $Lbl_Version
    ;SetCtlColors $Lbl_Title "" 0xFFFFFF ;蓝色
    SetCtlColors $Lbl_Version "FFFFFF"  transparent ;背景设成透明
    CreateFont $1 "宋体" "10" "120"
    SendMessage $Lbl_Version ${WM_SETFONT} $1 0
    ;${NSD_AddStyle} $Lbl_Title ${ES_CENTER}
    GetFunctionAddress $0 onGUICallback
    WndProc::onCallback $Lbl_Version $0 ;处理无边框窗体移动
    
    ;快速安装按钮
    ${NSD_CreateButton} 204 249 191 54 ""
    Pop $Btn_QuickInstall
    SkinBtn::Set /IMGID=$PLUGINSDIR\QuickInstall.bmp $Btn_QuickInstall
    GetFunctionAddress $3 OnQucikInstall
    SkinBtn::onClick $Btn_QuickInstall $3
    ${NSD_SetFocus} $Btn_QuickInstall

    ;自定义安装按钮
    ${NSD_CreateButton} 505 390 85 16 ""
    Pop $Btn_CustomInstall
    SkinBtn::Set /IMGID=$PLUGINSDIR\CustomInstall.bmp $Btn_CustomInstall
    GetFunctionAddress $3 OnNext
    SkinBtn::onClick $Btn_CustomInstall $3
    
    ;同意协议CheckBox
    ${NSD_CreateButton} 16 390 17 17 ""
    Pop $Chk_License
    SkinBtn::Set /IMGID=$PLUGINSDIR\License_Checked.bmp $Chk_License
    GetFunctionAddress $3 SetLicense
    SkinBtn::onClick $Chk_License $3
    
    Call InitLicense
    
    ;同意学生派文字
    ${NSD_CreateLabel} 42 388 205 20 "您已阅读并同意${PRODUCT_NAME}的"
    Pop $Lbl_License
    ;SetCtlColors $Lbl_Title "" 0xFFFFFF ;蓝色
    SetCtlColors $Lbl_License "676666"  transparent ;背景设成透明
    CreateFont $1 "微软雅黑" "11" "200"
    SendMessage $Lbl_License ${WM_SETFONT} $1 0
    ;${NSD_AddStyle} $Lbl_License ${ES_CENTER}
    
    ;读取RTF的文本框
		nsDialogs::CreateControl "RichEdit20A" \
    ${ES_READONLY}|${WS_VISIBLE}|${WS_CHILD}|${WS_TABSTOP}|${WS_VSCROLL}|${ES_MULTILINE}|${ES_WANTRETURN} \
		${WS_EX_STATICEDGE} 38 45 525 300 ''
    Pop $Txt_License
		${LoadRTF} '$PLUGINSDIR\license.rtf' $Txt_License
    ShowWindow $Txt_License ${SW_HIDE}
    
    ;用户协议按钮
    ${NSD_CreateButton} 247 390 85 16 ""
    Pop $Btn_Agreement
    SkinBtn::Set /IMGID=$PLUGINSDIR\Agreement.bmp $Btn_Agreement
    GetFunctionAddress $3 OnClickAgreement
    SkinBtn::onClick $Btn_Agreement $3

    ;协议确定按钮
    ;${NSD_CreateButton} 470 375 122 47 ""
    ${NSD_CreateButton} 470 379 105 38 ""
    Pop $Btn_Confirm
    SkinBtn::Set /IMGID=$PLUGINSDIR\Confirm.bmp $Btn_Confirm
    GetFunctionAddress $3 OnClickAgreement
    SkinBtn::onClick $Btn_Confirm $3
    ShowWindow $Btn_Confirm ${SW_HIDE}

    ;最小化按钮
    ${NSD_CreateButton} 546 1 27 27 ""
    Pop $0
    SkinBtn::Set /IMGID=$PLUGINSDIR\btnmini.bmp $0
    GetFunctionAddress $3 OnMini
    SkinBtn::onClick $0 $3

    ;关闭按钮
    ${NSD_CreateButton} 573 1 27 27 ""
    Pop $0
    SkinBtn::Set /IMGID=$PLUGINSDIR\btnClose.bmp $0
    GetFunctionAddress $3 OnClickClose
    SkinBtn::onClick $0 $3
    
    ;贴版本图标左部分
    ${NSD_CreateBitmap} 364 176 9 22 ""
    Pop $IconImage
    ${NSD_SetImage} $IconImage $PLUGINSDIR\VersionBgLeft.bmp $IconHandle
    GetFunctionAddress $0 onGUICallback
    WndProc::onCallback $IconImage $0 ;处理无边框窗体移动
    
    ;贴版到本图标右部分
;    ${NSD_CreateBitmap} 373 176 10 22 ""
;    Pop $IconImage
;    ${NSD_SetImage} $IconImage $PLUGINSDIR\VersionBgRight.bmp $IconHandle
;    GetFunctionAddress $0 onGUICallback
;    WndProc::onCallback $IconImage $0 ;处理无边框窗体移动
    
    ${NSD_CreateButton} 365 176 70 22 "${PRODUCT_VERSION}"
    Pop $0
    SetCtlColors $0 "FFFFFF" transparent ;透明
    CreateFont $1 "宋体" "10" "120"
    SendMessage $0 ${WM_SETFONT} $1 0
    ${NSD_AddStyle} $0 ${ES_LEFT}
    SkinBtn::Set /IMGID=$PLUGINSDIR\VersionBgRight.bmp $0
    GetFunctionAddress $3 onGUICallback
    WndProc::onCallback $0 $3 ;处理无边框窗体移动
    

    ;贴程序图标
    ${NSD_CreateBitmap} 255 80 101 116 ""
    Pop $IconImage
    ${NSD_SetImage} $IconImage $PLUGINSDIR\IconP.bmp $IconHandle
    GetFunctionAddress $0 onGUICallback
    WndProc::onCallback $IconImage $0 ;处理无边框窗体移动

    ;贴背景大图
    ${NSD_CreateBitmap} 0 0 100% 100% ""
    Pop $BGImage
    ${NSD_SetImage} $BGImage $PLUGINSDIR\bg.bmp $ImageHandle
    GetFunctionAddress $0 onGUICallback
    WndProc::onCallback $BGImage $0 ;处理无边框窗体移动
    nsDialogs::Show
    ${NSD_FreeImage} $IconHandle
    ${NSD_FreeImage} $ImageHandle
    
FunctionEnd

;CheckBox点击事件
Function SetLicense
         ${If} $Bool_License == 1
               Call License_NoChecked
               Strcpy $Bool_License 0
        ${Else}
               Call License_Checked
               Strcpy $Bool_License 1
        ${EndIf}
FunctionEnd

Function InitLicense
         ${If} $Bool_License == 1
               Call License_Checked
        ${Else}
               Call License_NoChecked
        ${EndIf}
FunctionEnd

;设置选中状态时的背景图片
Function License_Checked
  SkinBtn::Set /IMGID=$PLUGINSDIR\License_Checked.bmp $Chk_License
  EnableWindow $Btn_CustomInstall 1
  EnableWindow $Btn_QuickInstall 1
FunctionEnd

;设置未选中状态时的背景图片
Function License_NoChecked
  SkinBtn::Set /IMGID=$PLUGINSDIR\License_NoChecked.bmp $Chk_License
  EnableWindow $Btn_CustomInstall 0
  EnableWindow $Btn_QuickInstall 0
FunctionEnd

;协议按钮事件
Function OnClickAgreement
	${IF} $Bool_LicenseShow == 1
		ShowWindow $Btn_QuickInstall ${SW_SHOW}
		ShowWindow $Btn_CustomInstall ${SW_SHOW}
		ShowWindow $Btn_Agreement ${SW_SHOW}
		ShowWindow $Chk_License ${SW_SHOW}
		ShowWindow $Lbl_AppName ${SW_SHOW}
		ShowWindow $Lbl_License ${SW_SHOW}
    ShowWindow $Lbl_Version ${SW_SHOW}
		
    ShowWindow $Btn_Confirm ${SW_HIDE}
    ShowWindow $Txt_License ${SW_HIDE}
		IntOp $Bool_LicenseShow $Bool_LicenseShow - 1
	${ELSE}
	  ShowWindow $Btn_QuickInstall ${SW_HIDE}
	  ShowWindow $Btn_CustomInstall ${SW_HIDE}
		ShowWindow $Btn_Agreement ${SW_HIDE}
		ShowWindow $Chk_License ${SW_HIDE}
	  ShowWindow $Lbl_AppName ${SW_HIDE}
		ShowWindow $Lbl_License ${SW_HIDE}
		ShowWindow $Lbl_Version ${SW_HIDE}
		
		ShowWindow $Btn_Confirm ${SW_SHOW}
	  ShowWindow $Txt_License ${SW_SHOW}
		IntOp $Bool_LicenseShow $Bool_LicenseShow + 1
	${EndIf}
FunctionEnd


#=======================================================================#
#                        安装包 -- 自定义安装页                         #
#=======================================================================#
Function Page.2
    GetDlgItem $0 $HWNDPARENT 1
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 2
    ShowWindow $0 ${SW_HIDE}
    EnableWindow $0 0       ;禁止取消按钮，达到屏蔽ESC的效果
    GetDlgItem $0 $HWNDPARENT 3
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1990
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1991
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1992
    ShowWindow $0 ${SW_HIDE}


    ;设置当前页面位置与退出页面的相隔数
    StrCpy $CurrentPage 5

    nsDialogs::Create 1044
    Pop $0
    ${If} $0 == error
        Abort
    ${EndIf}
    SetCtlColors $0 ""  transparent ;背景设成透明

    ;${NSW_SetWindowSize} $0 $DialogWidth $DialogHight ;改变Page大小   588 291
    ${NSW_SetWindowSize} $0 601 427 ;改变Page大小   588 291

    ;应用名
;    ${NSD_CreateLabel} 270 70 300 60 "${PRODUCT_NAME}"
;    Pop $Lbl_AppName
;    SetCtlColors $Lbl_AppName "FFFFFF" transparent ;背景设成透明
;    CreateFont $1 "微软雅黑" "28" "400"
;    SendMessage $Lbl_AppName ${WM_SETFONT} $1 0
    ;;${NSD_AddStyle} $Lbl_AppName ${ES_CENTER}
;    GetFunctionAddress $0 onGUICallback
;    WndProc::onCallback $Lbl_AppName $0 ;处理无边框窗体移动
;    GetFunctionAddress $3 HideBrowserEdit
;    SkinBtn::onClick $Lbl_AppName $3

    ;标题文字
    ${NSD_CreateLabel} 5u 5u 115u 12u "${PRODUCT_NAME}" ; 安装向导"
    Pop $Lbl_Title
    ;SetCtlColors $Lbl_Title "" 0xFFFFFF ;蓝色
    SetCtlColors $Lbl_Title "FFFFFF"  transparent ;背景设成透明
    CreateFont $1 "微软雅黑" "11" "800"
    SendMessage $Lbl_Title ${WM_SETFONT} $1 0
    ;${NSD_AddStyle} $Lbl_Title ${ES_CENTER}
    GetFunctionAddress $0 onGUICallback
    WndProc::onCallback $Lbl_Title $0 ;处理无边框窗体移动
    GetFunctionAddress $3 HideBrowserEdit
    SkinBtn::onClick $Lbl_Title $3

    ;立即安装按钮
    ${NSD_CreateButton} 470 379 105 38 ""
    Pop $Btn_Install
    SkinBtn::Set /IMGID=$PLUGINSDIR\Install.bmp $Btn_Install
    GetFunctionAddress $3 OnInstall
    SkinBtn::onClick $Btn_Install $3
    ${NSD_SetFocus} $Btn_Install
    
    ;返回快速安装按钮
    ${NSD_CreateButton} 365 379 84 38 ""
    Pop $Btn_Back
    SkinBtn::Set /IMGID=$PLUGINSDIR\Back.bmp $Btn_Back
    GetFunctionAddress $3 OnPre
    SkinBtn::onClick $Btn_Back $3

    ;安装位置文字
    ${NSD_CreateLabel} 45 65 80 20 "安装位置："
    Pop $2
    SetCtlColors $2 "FFFFFF" transparent ;背景设成透明
    CreateFont $1 "微软雅黑" "12" "800"
    SendMessage $2 ${WM_SETFONT} $1 0
    ;;${NSD_AddStyle} $Lbl_AppName ${ES_CENTER}
    GetFunctionAddress $0 onGUICallback
    WndProc::onCallback $2 $0 ;处理无边框窗体移动
    
    ;可用空间  Txt_SpaceValue
    ${NSD_CreateLabel} 125 66 160 20 ""
    Pop $Txt_SpaceValue
    SetCtlColors $Txt_SpaceValue "FFFFFF" "54BBC2" ;背景设成透明
    CreateFont $1 "微软雅黑" "10" "100"
    SendMessage $Txt_SpaceValue ${WM_SETFONT} $1 0
    ${NSD_AddStyle} $Txt_SpaceValue ${ES_LEFT}
    GetFunctionAddress $0 onGUICallback
    WndProc::onCallback $2 $0 ;处理无边框窗体移动
    Call GetDriveSpace
    
    
    ;创建安装目录输入文本框
    ;${NSD_CreateText} 24 150 350 24 $INSTDIR
    ${NSD_CreateText} 45 95 440 38 $INSTDIR
    Pop $Txt_Browser
    ${NSD_SetTextLimit} $Txt_Browser 255
    ;SkinBtn::Set /IMGID=$PLUGINSDIR\InstallDir.bmp $Txt_Browser
    SetCtlColors $Txt_Browser "FFFFFF" "49AAAF"  ;背景设成透明   "096FA9" ; transparent;
    CreateFont $1 "微软雅黑" "14" "150"
    SendMessage $Txt_Browser ${WM_SETFONT} $1 0
    ShowWindow $Txt_Browser ${SW_HIDE}
    ${NSD_OnChange} $Txt_Browser EditTextChanged
    
    ;创建安装目录按钮
    ;${NSD_CreateText} 24 150 350 24 $INSTDIR
    ${NSD_CreateButton} 45 95 440 40 $INSTDIR
    Pop $Btn_TxtBrowser
    SkinBtn::Set /IMGID=$PLUGINSDIR\InstallDir.bmp $Btn_TxtBrowser
    SetCtlColors $Btn_TxtBrowser "FFFFFF" transparent ;背景设成透明   "096FA9" ; transparent;
    CreateFont $1 "微软雅黑" "14" "150"
    SendMessage $Btn_TxtBrowser ${WM_SETFONT} $1 0
    GetFunctionAddress $3 ShowBrowserEdit
    SkinBtn::onClick $Btn_TxtBrowser $3
    
    ;;圆角
;    System::Alloc 16
;    System::Call user32::GetWindowRect(i$Txt_Browser,isR0)
;    System::Call *$R0(i.R1,i.R2,i.R3,i.R4)
;    IntOp $R3 $R3 - $R1
;    IntOp $R4 $R4 - $R2
;    System::Call gdi32::CreateRoundRectRgn(i0,i0,iR3,iR4,i8,i8)i.r0
;    System::Call user32::SetWindowRgn($Txt_Browser, ir0, i1)
;    System::Free $R0

    ;浏览按钮
    ${NSD_CreateButton} 484 95 71 38  ""
    Pop $btn_Browser
    SkinBtn::Set /IMGID=$PLUGINSDIR\Browse.bmp $btn_Browser
    GetFunctionAddress $3 onButtonClickSelectPath
    SkinBtn::onClick $btn_Browser $3
    
    ;创建桌面快捷方式CheckBox
    ${NSD_CreateButton} 45 150 17 17 ""
    Pop $Chk_Desktop
    SkinBtn::Set /IMGID=$PLUGINSDIR\Desktop_Checked.bmp $Chk_Desktop
    GetFunctionAddress $3 SetDesktop
    SkinBtn::onClick $Chk_Desktop $3
    
    ${NSD_CreateLabel} 70 149 160 20 "添加至桌面快捷方式"
    Pop $2
    SetCtlColors $2 "FFFFFF" transparent ;背景设成透明
    CreateFont $1 "微软雅黑" "10" "100"
    SendMessage $2 ${WM_SETFONT} $1 0
    ${NSD_AddStyle} $2 ${ES_LEFT}
    GetFunctionAddress $0 onGUICallback
    WndProc::onCallback $2 $0 ;处理无边框窗体移动
    
    Call InitDesktop
    
    ;创建快速启动栏CheckBox
    ${NSD_CreateButton} 45 180 17 17 ""
    Pop $Chk_QuickRun
    SkinBtn::Set /IMGID=$PLUGINSDIR\QuickRun_Checked.bmp $Chk_QuickRun
    GetFunctionAddress $3 SetQuickRun
    SkinBtn::onClick $Chk_QuickRun $3
    
    ${NSD_CreateLabel} 70 179 160 20 "添加至快速启动栏"
    Pop $2
    SetCtlColors $2 "FFFFFF" transparent ;背景设成透明
    CreateFont $1 "微软雅黑" "10" "100"
    SendMessage $2 ${WM_SETFONT} $1 0
    ${NSD_AddStyle} $2 ${ES_LEFT}
    GetFunctionAddress $0 onGUICallback
    WndProc::onCallback $2 $0 ;处理无边框窗体移动
    
    Call InitQuickRun

    ;最小化按钮
    ${NSD_CreateButton} 546 1 27 27 ""
    Pop $0
    SkinBtn::Set /IMGID=$PLUGINSDIR\btnmini.bmp $0
    GetFunctionAddress $3 OnMini
    SkinBtn::onClick $0 $3
    
    ;关闭按钮
    ${NSD_CreateButton} 573 1 27 27 ""
    Pop $0
    SkinBtn::Set /IMGID=$PLUGINSDIR\btnClose.bmp $0
    GetFunctionAddress $3 OnClickClose
    SkinBtn::onClick $0 $3

    ;贴程序图标
;    ${NSD_CreateBitmap} 195 64 55 57 ""
;    Pop $IconImage
;    ${NSD_SetImage} $IconImage $PLUGINSDIR\icon.bmp $IconHandle
;    GetFunctionAddress $0 onGUICallback
;    WndProc::onCallback $IconImage $0 ;处理无边框窗体移动
;    GetFunctionAddress $3 HideBrowserEdit
;    SkinBtn::onClick $IconImage $3

    ;贴背景大图
    ${NSD_CreateBitmap} 0 0 100% 100% ""
    Pop $BGImage
    ${NSD_SetImage} $BGImage $PLUGINSDIR\bg.bmp $ImageHandle
    GetFunctionAddress $0 onGUICallback
    WndProc::onCallback $BGImage $0 ;处理无边框窗体移动
    GetFunctionAddress $3 HideBrowserEdit
    SkinBtn::onClick $BGImage $3
    
    nsDialogs::Show
    ${NSD_FreeImage} $IconHandle
    ${NSD_FreeImage} $ImageHandle
FunctionEnd

Function ShowBrowserEdit
  ShowWindow $Btn_TxtBrowser ${SW_HIDE}
  ShowWindow $Txt_Browser ${SW_SHOW}
FunctionEnd

Function HideBrowserEdit
  ShowWindow $Btn_TxtBrowser ${SW_SHOW}
  ShowWindow $Txt_Browser ${SW_HIDE}
FunctionEnd

Function EditTextChanged
  ${NSD_GetText} $Txt_Browser $0
  StrCpy $INSTDIR $0
  ${NSD_SetText} $Btn_TxtBrowser $0

  ${GetRoot} "$INSTDIR" $R3   ;获取安装根目录
  StrCpy $R0 "$R3\"
  StrCpy $R1 "invalid"
  ${GetDrives} "ALL" "HDDDetection"            ;获取将要安装的根目录磁盘类型
  ${If} $R1 == HDD              ;是硬盘
     EnableWindow $Btn_Install 1
     Call GetDriveSpace
  ${Else}
     EnableWindow $Btn_Install 0
  ${Endif}
  ;system::Call `user32::SetWindowText(i $Btn_TxtBrowser, t "$INSTDIR")`
FunctionEnd

Function HDDDetection
  ${if} $R0 == $9    ;$9是盘符;$8是属性
    StrCpy $R1 $8
  ${endif}
  Push $0
FunctionEnd

Function GetDriveSpace
  ${GetRoot} "$INSTDIR" $R3   ;获取安装根目录
  StrCpy $R0 "$R3\"
  ${DriveSpace}  "$R0" "/D=F /S=M" $R5
  ${If} $R5 < 1024
    ${NSD_SetText} $Txt_SpaceValue "($R3 盘可用空间:$R5M)"
  ${Else}
    IntOp $R6 $R5 / 1024
    ${NSD_SetText} $Txt_SpaceValue "($R3 盘可用空间:$R6G)"
  ${Endif}
FunctionEnd

;添加桌面快捷方式点击事件
Function SetDesktop
         ${If} $Bool_Desktop == 1
               Call Desktop_NoChecked
               Strcpy $Bool_Desktop 0
        ${Else}
               Call Desktop_Checked
               Strcpy $Bool_Desktop 1
        ${EndIf}
FunctionEnd

Function InitDesktop
         ${If} $Bool_Desktop == 1
               Call Desktop_Checked
        ${Else}
               Call Desktop_NoChecked
        ${EndIf}
FunctionEnd

;设置选中状态时的背景图片
Function Desktop_Checked
  SkinBtn::Set /IMGID=$PLUGINSDIR\Desktop_Checked.bmp $Chk_Desktop
FunctionEnd

;设置未选中状态时的背景图片
Function Desktop_NoChecked
  SkinBtn::Set /IMGID=$PLUGINSDIR\Desktop_NoChecked.bmp $Chk_Desktop

FunctionEnd

;添加快捷启动点击事件
Function SetQuickRun
         ${If} $Bool_QuickRun == 1
               Call QuickRun_NoChecked
               Strcpy $Bool_QuickRun 0
        ${Else}
               Call QuickRun_Checked
               Strcpy $Bool_QuickRun 1
        ${EndIf}
FunctionEnd

Function InitQuickRun
         ${If} $Bool_QuickRun == 1
               Call QuickRun_Checked
        ${Else}
               Call QuickRun_NoChecked
        ${EndIf}
FunctionEnd

;设置选中状态时的背景图片
Function QuickRun_Checked
  SkinBtn::Set /IMGID=$PLUGINSDIR\QuickRun_Checked.bmp $Chk_QuickRun
FunctionEnd

;设置未选中状态时的背景图片
Function QuickRun_NoChecked
  SkinBtn::Set /IMGID=$PLUGINSDIR\QuickRun_NoChecked.bmp $Chk_QuickRun
FunctionEnd

#----------------------------------------------
#第2个页面完成  -- 安装进度页面
#----------------------------------------------

Function  InstFilesPageShow
    FindWindow $R2 "#32770" "" $HWNDPARENT
    StrCpy $DefaultPage $R2
    GetDlgItem $0 $HWNDPARENT 1
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 2
    ShowWindow $0 ${SW_HIDE}
    EnableWindow $0 0       ;禁止取消按钮，达到屏蔽ESC的效果
    GetDlgItem $0 $HWNDPARENT 3
    ShowWindow $0 ${SW_HIDE}
    
    GetDlgItem $1 $R2 1027
    ShowWindow $1 ${SW_HIDE}
    
    GetDlgItem $1 $R2 1990  ;最小化
    ShowWindow $1 ${SW_HIDE}
    GetDlgItem $1 $R2 1991 ;关闭
    ShowWindow $1 ${SW_HIDE}
    EnableWindow $1 0
    GetDlgItem $1 $R2 1994  ;安装
    ShowWindow $1 ${SW_HIDE}
    ;EnableWindow $1 0
    
    ;w7tbp::Start

    StrCpy $R0 $R2 ;改变页面大小,不然贴图不能全页
    System::Call "user32::MoveWindow(i R0, i 0, i 0, i 601, i 427) i R2"        ;修改大小   588 291
    GetFunctionAddress $0 onGUICallback
    WndProc::onCallback $R0 $0 ;处理无边框窗体移动

    GetDlgItem $R0 $R2 1004  ;设置进度条位置
    System::Call "user32::MoveWindow(i R0, i 15, i 400, i 562, i 6) i R2"
    Strcpy $PB_ProgressBar $R0
    SkinProgress::Set $PB_ProgressBar "$PLUGINSDIR\Progress.bmp" "$PLUGINSDIR\ProgressBar.bmp"
    
    ;标题文字
    GetDlgItem $R0 $R2 1993  
    System::Call "user32::MoveWindow(i R0, i 8, i 8, i 160, i 18) i R2"    ;注意：调用系统API，必须用 R0 ~ R9, 才有效果
    StrCpy $Lbl_Title $R0
    SetCtlColors $Lbl_Title "FFFFFF"  transparent ;背景设成透明
    ${NSD_SetText} $Lbl_Title "${PRODUCT_NAME}" ; 安装向导"
    CreateFont $1 "微软雅黑" "11" "800"
    SendMessage $Lbl_Title ${WM_SETFONT} $1 0
    
    ;应用名
;    ;${NSD_CreateLabel} 270 70 300 60 "${PRODUCT_NAME}"
;    GetDlgItem $R0 $R2 1997  ;设置应用名位置
;    System::Call "user32::MoveWindow(i R0, i 250, i 70, i 300, i 60) i R2"
;    StrCpy $Lbl_AppName $R0
;    SetCtlColors $Lbl_AppName "FFFFFF" transparent ;背景设成透明
;    ${NSD_SetText} $Lbl_AppName "${PRODUCT_NAME}"
;    CreateFont $1 "微软雅黑" "28" "400"
;    SendMessage $Lbl_AppName ${WM_SETFONT} $1 0

    ;DetailPrint 文字 -- "正在安装"
    GetDlgItem $R1 $R2 1006  ;获取1006控件设置颜色并改变位置
    System::Call "user32::MoveWindow(i R1, i 15, i 376, i 200, i 40) i R2"
    SetCtlColors $R1 "888888"  transparent ;背景设成F6F6F6,注意颜色不能设为透明，否则重叠
    CreateFont $1 "微软雅黑" "10" "50"
    SendMessage $R1 ${WM_SETFONT} $1 0
    
    GetDlgItem $R0 $R2 1992  ;获取1992控件设置颜色并改变位置   [用按钮来显示]      1997 是文本
    ShowWindow $R0 ${SW_HIDE}
    ;进度文字
    GetDlgItem $R0 $R2 1997  ;获取1992控件设置颜色并改变位置   [用按钮来显示]      1997 是文本
    ShowWindow $R0 ${SW_HIDE}
    System::Call "user32::MoveWindow(i R0, i 90, i 376, i 40, i 20, i 1) i R2"
    SetCtlColors $R0 "888888" "FFFFFF" ;背景设成F6F6F6,注意颜色不能设为透明，否则重叠
    CreateFont $1 "微软雅黑" "10" "50"
    SendMessage $R0 ${WM_SETFONT} $1 0
    Strcpy $PPercent $R0
    ${NSD_SetText} $PPercent "0%"

    ;介绍图标
    GetDlgItem $R0 $R2 1996
    System::Call "user32::MoveWindow(i R0, i 72, i 49, i 311, i 306) i R2"
    ${NSD_SetImage} $R0 $PLUGINSDIR\Intro.bmp $IconHandle
    ;贴背景大图
    GetDlgItem $R0 $R2 1995
    System::Call "user32::MoveWindow(i R0, i 0, i 0, i 601, i 427) i R2"      ;修改大小   588 291
    ${NSD_SetImage} $R0 $PLUGINSDIR\bg.bmp $ImageHandle

    #借用WndSubclass插件设定timer函数地址（尝试过直接为settimer设定函数地址，但没有插件支持很困难）
    ;${WndSubclass_Subclass} $HWNDPARENT GetProgressPos $SubProgress $SubProgress
    ;System::Call user32::SetTimer(i $DefaultPage, i 10, i 20, i 0)
    ;GetFunctionAddress $2 GetProgressPos
    ;ThreadTimer::Start 200 0 $2
FunctionEnd

Function GetProgressPos
  SendMessage $PB_ProgressBar 0x0407 0 0 $1
  SendMessage $PB_ProgressBar 0x0408 0 0 $2
  ${If} $2 > 0
  ${If} $1 > 0
    IntOp $2 $2 * 100
    IntOp $1 $2 / $1
    ;SendMessage $PB_ProgressBar ${PBM_GETPOS} 0 0 $1
   ${NSD_SetText} $PPercent "$1%"
    ${if} $1 >= 99
      ThreadTimer::Stop
      ;System::Call user32::KillTimer(i $DefaultPage, i 10)
      ${NSD_SetText} $PPercent "100%"
    ${Endif}
  ${Endif}
  ${Endif}
FunctionEnd

Function InstFileLeave
  ${IfThen} ${Abort} ${|} Quit ${|}
FunctionEnd


#=======================================================================#
#                          安装包 -- 完成页面                           #
#=======================================================================#
Function Page.3
    GetDlgItem $0 $HWNDPARENT 1
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 2
    ShowWindow $0 ${SW_HIDE}
    EnableWindow $0 0       ;禁止取消按钮，达到屏蔽ESC的效果
    GetDlgItem $0 $HWNDPARENT 3
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1990
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1991
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1992
    ShowWindow $0 ${SW_HIDE}

    ;设置当前页面位置与退出页面的相隔数
    StrCpy $CurrentPage 2


    nsDialogs::Create 1044
    Pop $0
    ${If} $0 == error
        Abort
    ${EndIf}
    SetCtlColors $0 ""  transparent ;背景设成透明
    ;${NSW_SetWindowSize} $0 $DialogWidth $DialogHight ;改变Page大小   588 291
    ${NSW_SetWindowSize} $0 601 427 ;改变Page大小   588 291

    ;安装完成
    ${NSD_CreateLabel} 195 205 300 60 "安装完成"
    Pop $Lbl_AppName
    SetCtlColors $Lbl_AppName "FFFFFF" transparent ;背景设成透明
    CreateFont $1 "微软雅黑" "38" "400"
    SendMessage $Lbl_AppName ${WM_SETFONT} $1 0
    ;;${NSD_AddStyle} $Lbl_AppName ${ES_CENTER}
    GetFunctionAddress $0 onGUICallback
    WndProc::onCallback $Lbl_AppName $0 ;处理无边框窗体移动

    ;标题文字
    ${NSD_CreateLabel} 5u 5u 115u 12u "${PRODUCT_NAME}" ; 安装向导"
    Pop $Lbl_Title
    ;SetCtlColors $Lbl_Title "" 0xFFFFFF ;蓝色
    SetCtlColors $Lbl_Title "FFFFFF"  transparent ;背景设成透明
    CreateFont $1 "微软雅黑" "11" "800"
    SendMessage $Lbl_Title ${WM_SETFONT} $1 0
    ;${NSD_AddStyle} $Lbl_Title ${ES_CENTER}
    GetFunctionAddress $0 onGUICallback
    WndProc::onCallback $Lbl_Title $0 ;处理无边框窗体移动
    
    ;最小化按钮
    ${NSD_CreateButton} 546 1 27 27 ""
    Pop $0
    SkinBtn::Set /IMGID=$PLUGINSDIR\btnmini.bmp $0
    GetFunctionAddress $3 OnMini
    SkinBtn::onClick $0 $3

    ;关闭按钮
    ${NSD_CreateButton} 573 1 27 27 ""
    Pop $0
    SkinBtn::Set /IMGID=$PLUGINSDIR\btnClose.bmp $0
    GetFunctionAddress $3 OnClickClose
    SkinBtn::onClick $0 $3
    ;EnableWindow $0 0
    
        ;完成按钮    ;${NSD_CreateButton} 232 158 122 47 ""
    ${NSD_CreateButton} 470 379 105 38 ""
    Pop $Btn_Done
    ${NSD_SetFocus} $Btn_Done
    SkinBtn::Set /IMGID=$PLUGINSDIR\Done.bmp $Btn_Done
    GetFunctionAddress $5 OnInstallDoneRun
    SkinBtn::onClick $Btn_Done $5


    ;贴程序图标
    ${NSD_CreateBitmap} 254 90 93 96 ""
    Pop $BGImage
    ${NSD_SetImage} $BGImage $PLUGINSDIR\IconDone.bmp $ImageHandle
    GetFunctionAddress $0 onGUICallback
    WndProc::onCallback $BGImage $0 ;处理无边框窗体移动

    ;贴背景大图
    ${NSD_CreateBitmap} 0 0 100% 100% ""
    Pop $BGImage
    ${NSD_SetImage} $BGImage $PLUGINSDIR\bg.bmp $ImageHandle
    GetFunctionAddress $0 onGUICallback
    WndProc::onCallback $BGImage $0 ;处理无边框窗体移动
    
    nsDialogs::Show
    ${NSD_FreeImage} $ImageHandle
FunctionEnd


Function Page.4

FunctionEnd

#=======================================================================#
#                     安装包 -- 提示退出页面                            #
#=======================================================================#
Function Page.Exit
    GetDlgItem $0 $HWNDPARENT 1
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 2
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 3
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1990
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1991
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1992
    ShowWindow $0 ${SW_HIDE}

    nsDialogs::Create 1044
    Pop $0
    ${If} $0 == error
        Abort
    ${EndIf}
    SetCtlColors $0 ""  transparent ;背景设成透明

    ;${NSW_SetWindowSize} $0 $DialogWidth $DialogHight ;改变Page大小   588 291
    ${NSW_SetWindowSize} $0 601 427 ;改变Page大小   588 291

    ;退出提示文字
    ${NSD_CreateLabel} 55 249 493U 20u "您确定要退出${PRODUCT_NAME}的安装程序吗？"
    Pop $Lbl_AppName
    SetCtlColors $Lbl_AppName "FFFFFF" transparent ;背景设成透明
    CreateFont $1 "微软雅黑" "16" "220"
    SendMessage $Lbl_AppName ${WM_SETFONT} $1 0
    ${NSD_AddStyle} $Lbl_AppName ${ES_CENTER}
    
    GetFunctionAddress $0 onGUICallback
    WndProc::onCallback $Lbl_AppName $0 ;处理无边框窗体移动

    ;标题文字
    ${NSD_CreateLabel} 5u 5u 115u 12u "${PRODUCT_NAME}" ; 安装向导"
    Pop $Lbl_Title
    ;SetCtlColors $Lbl_Title "" 0xFFFFFF ;蓝色
    SetCtlColors $Lbl_Title "FFFFFF"  transparent ;背景设成透明
    CreateFont $1 "微软雅黑" "11" "800"
    SendMessage $Lbl_Title ${WM_SETFONT} $1 0
    ;${NSD_AddStyle} $Lbl_Title ${ES_CENTER}
    
    GetFunctionAddress $0 onGUICallback
    WndProc::onCallback $Lbl_Title $0 ;处理无边框窗体移动

    ;是(退出)按钮 ${NSD_CreateButton} 207 163 77 28 ""
    ${NSD_CreateButton} 470 379 105 38 ""
    Pop $Btn_Yes
    SkinBtn::Set /IMGID=$PLUGINSDIR\Yes.bmp $Btn_Yes
    SetCtlColors $Btn_Yes "808080"  transparent ;背景设成透明
    CreateFont $1 "微软雅黑" "16" "500"
    SendMessage $Btn_Yes ${WM_SETFONT} $1 0
    ${NSD_AddStyle} $Btn_Yes ${ES_CENTER}

    GetFunctionAddress $3 OnExit
    SkinBtn::onClick $Btn_Yes $3

    ;否(取消)按钮 ${NSD_CreateButton} 303 163 77 28 ""
    ${NSD_CreateButton} 365 379 84 38 ""
    Pop $Btn_No
    SkinBtn::Set /IMGID=$PLUGINSDIR\No.bmp $Btn_No
    SetCtlColors $Btn_No "0d6190"  transparent ;背景设成透明
    CreateFont $1 "微软雅黑" "16" "500" ;
    SendMessage $Btn_No ${WM_SETFONT} $1 0
    ${NSD_AddStyle} $Btn_No ${ES_CENTER}

    GetFunctionAddress $3 OnClickCancel
    SkinBtn::onClick $Btn_No $3
    
    ;最小化按钮
    ${NSD_CreateButton} 546 1 27 27 ""
    Pop $0
    SkinBtn::Set /IMGID=$PLUGINSDIR\btnmini.bmp $0
    GetFunctionAddress $3 OnMini
    SkinBtn::onClick $0 $3

    ;关闭按钮
    ${NSD_CreateButton} 573 1 27 27 ""
    Pop $0
    SkinBtn::Set /IMGID=$PLUGINSDIR\btnClose.bmp $0
    GetFunctionAddress $3 OnClickClose
    SkinBtn::onClick $0 $3
    EnableWindow $0 0

    ;贴程序图标
    ${NSD_CreateBitmap} 227 72 146 146 ""
    Pop $BGImage
    ${NSD_SetImage} $BGImage $PLUGINSDIR\Icon.bmp $ImageHandle
    GetFunctionAddress $0 onGUICallback
    WndProc::onCallback $BGImage $0 ;处理无边框窗体移动

    ;贴背景大图
    ${NSD_CreateBitmap} 0 0 100% 100% ""
    Pop $BGImage
    ${NSD_SetImage} $BGImage $PLUGINSDIR\bg.bmp $ImageHandle

    GetFunctionAddress $0 onGUICallback
    WndProc::onCallback $BGImage $0 ;处理无边框窗体移动
    nsDialogs::Show
    ${NSD_FreeImage} $ImageHandle
FunctionEnd

#----------------------------------------------
#卸载驱动
#----------------------------------------------
Function UnInstallDirver
  ReadRegStr $R0 HKLM \
  "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" \
  "UninstallString"
  ${GetParent} "$R0" $R1
  StrCmp $R0 "" undone
  IfFileExists $R0 uninstdirver
	Goto undone
;运行卸载程序
uninstdirver:
  ;MessageBox MB_ICONINFORMATION|MB_OK "$R1\${ADBMGR_DIR}\${ADBMGR_PROGRAM_NAME}.exe -uninstall "
  ExecWait "$R1\${ADBMGR_DIR}\${ADBMGR_PROGRAM_NAME}.exe -uninstall " ;这里$R0为读取到的卸载程序名称， /S是静默卸载参数使用NSIS生成的卸载程序必须要加上 _? 才能等待卸载。$R1是软件位置
undone:
FunctionEnd

#----------------------------------------------
#安装界面跳到下一界面
#----------------------------------------------
Function CloseInstallUI
         Sleep 100
         Call OnNext
         ;SetAutoClose true
FunctionEnd

;Function OnAutoClose
;  GetFunctionAddress $0 OnAutoClose
;  nsDialogs::KillTimer $0
;  GetFunctionAddress $0 CloseInstallUI
;  BgWorker::CallAndWait #启动一个低优先级的后台线程
;FunctionEnd

#----------------------------------------------
#安装完成界面按钮
#----------------------------------------------
Function OnInstallDoneRun
  ExecShell "open" "$INSTDIR\${PROGRAM_NAME}.exe"
  ;Exec "$INSTDIR\${PROGRAM_NAME}.exe"
  ;System::Alloc 4*18			;// $1 = struct STARTUPINFO
  ;Pop $1
  ;System::Alloc 4*4			;// $2 = struct PROCESS_INFORMATION
  ;Pop $2
  ;System::Call "*$1(i 4*18)"		;// StartUp.cb=sizeof(StartUp);
  ;System::Call \
  ;  'kernel32::CreateProcess(i 0, t "$INSTDIR\${PROGRAM_NAME}.exe", i 0, i 0, i 0, \
	;			i 0, i 0, i 0, i r1, i.r2)i.r0'
  ;System::Free $1
  ;System::Free $2
  Call OnExit
FunctionEnd


#----------------------------------------------
#执行安装事件
#----------------------------------------------
Section MainSetup
  ;Call KillAllApp
  Sleep 50
  SetDetailsPrint textonly
  DetailPrint "正在安装..."
  SetDetailsPrint none  ;不显示信息
  SetOverwrite on
  ;nsisSlideshow::Show /NOUNLOAD /auto=$PLUGINSDIR\Slides.dat
  ;CopyFiles "ImportantFile\*.*" "${PRODUCT_NAME}${PRODUCT_VERSION}"
  SetOutPath $INSTDIR
  File /r "${PRODUCT_NAME}${PRODUCT_VERSION}\*"
  File /r "${MUST_PACKAGE_FOLDER_NAME}\*"

  ;CopyFiles $INSTDIR\${BIN_DIR}\*.ttf $INSTDIR\${BIN_DIR}

  ;GetFunctionAddress $0 CloseInstallUI
  ;BgWorker::CallAndWait #启动一个低优先级的后台线程
  ;nsisSlideshow::Stop
  Call ReportInstall
  Sleep 100
  SetAutoClose true
SectionEnd

#----------------------------------------------
#创建控制面板卸载程序信息 ,下面的具体用法卡查看帮助  D.2 添加卸载信息到添加/删除程序面板  或者在帮助里搜索关键词，如：DisplayName
#----------------------------------------------

#----------------------------------------------------------------------------------------------------------------------------------
#
#             ;生成卸载文件  PS：如果【卸载文件】报毒，生成后添加签门后放入程序目录，并注释掉这句话 zcs
#
#----------------------------------------------------------------------------------------------------------------------------------

Section -Post
  ;WriteUninstaller "$INSTDIR\Uninstall.exe"     ;生成卸载文件  PS：如果【卸载文件】报毒，生成后添加签门后放入程序目录，并注释掉这句话 zcs

  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\${PROGRAM_NAME}.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\Uninstall.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\${PROGRAM_NAME}.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

Section -AdditionalIcons
  SetOverwrite on
  SetShellVarContext all   ;设置为所有用户 Win7下有影响      
  
  CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME}"     ;创建程序目录

  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"   ;创建官网快捷打开

  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\卸载${PRODUCT_NAME}.lnk" "$INSTDIR\Uninstall.exe"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}.lnk" "$INSTDIR\${PROGRAM_NAME}.exe"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk" "$INSTDIR\${PROGRAM_NAME}.exe"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_WEB_LINK_NAME}.lnk" "$INSTDIR\${PRODUCT_NAME}.url" "${PRODUCT_WEB_LINK_NAME}" "$INSTDIR\Browser.ico" ;"C:\Program Files\Internet Explorer\iexplore.exe"
  
  
  ;桌面快捷方式
  ${If} $Bool_Desktop == 1
    CreateShortCut "$DESKTOP\${PRODUCT_NAME}.lnk" "$INSTDIR\${PROGRAM_NAME}.exe"
    CreateShortCut "$DESKTOP\${PRODUCT_WEB_LINK_NAME}.lnk" "$INSTDIR\${PRODUCT_NAME}.url" "${PRODUCT_WEB_LINK_NAME}" "$INSTDIR\Browser.ico" ;"C:\Program Files\Internet Explorer\iexplore.exe"
  ${Endif}
  
  ;创建快速启动栏
  
  ${If} $Bool_QuickRun == 1
    ;CreateShortCut "$STARTMENU\${PRODUCT_NAME}.lnk" "$INSTDIR\${PROGRAM_NAME}.exe"
    CreateShortCut "$QUICKLAUNCH\${PRODUCT_NAME}.lnk" "$INSTDIR\${PROGRAM_NAME}.exe"
    ;CreateShortCut "$QUICKLAUNCH\User Pinned\TaskBar\${PRODUCT_NAME}.lnk" "$INSTDIR\${PROGRAM_NAME}.exe"  ;不用创建, taskbarpin 时会自动创建
;    GetVersion::WindowsVersion
;    Pop $R0
;    StrCpy $R2 $R0
;    GetVersion::WindowsServicePackBuild
;    Pop $R0
;    ${If} ${AtMostWin8.1}
    ${GetFileVersion} "$SYSDIR\kernel32.dll" $R1        ; x.x.x.x         6.2.7601.12345
    ${WordFind} "$R1" "." +3 $R0   ; 得到当前版本的整数串， 不能获得前面的，某些系统是升上去的会出问题
    
    ${If} $R0 < 10000
      ;锁定开始菜单快捷方式
      ExecShell "startpin" "$QUICKLAUNCH\${PRODUCT_NAME}.lnk"
      ;锁定任务栏快捷方式
      ExecShell "taskbarpin" "$QUICKLAUNCH\${PRODUCT_NAME}.lnk"
    ${EndIf}
  ${EndIf}
  System::Call 'shell32::SHChangeNotify(i, i, i, i) v (0x08000000, 0, 0, 0)'
  ;SendMessage $R0 ${WM_ERASEBKGND} $0 $0
  ;System::Call User32::RedrawWindow(i 0, i 0, i 0, i 133)
  ;CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\使用说明.lnk" "$INSTDIR\${PRODUCT_HELPFILE}"
SectionEnd

;处理页面跳转的命令
Function RelGotoPage
  IntCmp $R9 0 0 Move Move
    StrCmp $R9 "X" 0 Move
      StrCpy $R9 "120"
  Move:
  SendMessage $HWNDPARENT "0x408" "$R9" ""
FunctionEnd

Function OnNext
  StrCpy $R9 1 ;Goto the next page
  Call RelGotoPage
  Abort
FunctionEnd

Function OnPre
  StrCpy $R9 -1 ;Goto the Pre page
  Call RelGotoPage
  Abort
FunctionEnd

#----------------------------------------------
#跳转到关闭页面
#----------------------------------------------
Function OnClickClose
  StrCpy $R9 $CurrentPage ;Goto the next page
  Call RelGotoPage
  Abort
FunctionEnd

#----------------------------------------------
#关闭页面取消
#----------------------------------------------
Function OnClickCancel
  StrCpy $R9 -$CurrentPage ;Goto the pre page
  Call RelGotoPage
  Abort
FunctionEnd

Function OnQucikInstall
  StrCpy $R9 2 ;Goto the next page
  Call RelGotoPage
  Abort
FunctionEnd

Function OnInstall
  ${NSD_GetText} $Txt_Browser  $R0  ;获得设置的安装路径
  ;判断目录是否正确
	ClearErrors
	CreateDirectory "$R0"
	IfErrors 0 +3
  MessageBox MB_ICONINFORMATION|MB_OK "'$R0' 安装目录不存在，请重新设置。"
  Return

	StrCpy $INSTDIR  $R0  ;保存安装路径

	;跳到下一页， $R9是NavigationGotoPage 函数需要的跳转参数变量
  StrCpy $R9 1
  call RelGotoPage
FunctionEnd

#------------------------------------------
#最小化代码
#------------------------------------------
Function OnMini
System::Call user32::CloseWindow(i$HWNDPARENT) ;最小化
FunctionEnd

#------------------------------------------
#关闭代码
#------------------------------------------
Function OnExit
;SendMessage $hwndparent ${WM_CLOSE} 0 0 ;关闭
  RMDir /r /REBOOTOK "$PLUGINSDIR"
  ;DeleteRegKey  ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  System::Call kernel32::ExitProcess(0) ;关闭
FunctionEnd

#--------------------------------------------------------
# 路径选择按钮事件，打开Windows系统自带的目录选择对话框
#--------------------------------------------------------
Function onButtonClickSelectPath
  Pop $0
  Push $INSTDIR ; input string "C:\Program Files\ProgramName"
  Call GetParent
  Pop $R0 ; first part "C:\Program Files"

  Push $INSTDIR ; input string "C:\Program Files\ProgramName"
  Push "\" ; input chop char
  Call GetLastPart
  Pop $R1 ; last part "ProgramName"

  nsDialogs::SelectFolderDialog "请选择 $R1 安装的文件夹:" "$R0"
  Pop $0
  ${If} $0 == "error" # returns 'error' if 'cancel' was pressed?
    Return
  ${EndIf}
  ${If} $0 != ""
    StrLen $1 $0 ;判断是否是根目录
    ${If} $1 == 3
    StrCpy $INSTDIR "$0$R1"
    ${Else}
    StrCpy $INSTDIR "$0\$R1"
    ${EndIf}
    ;${NSD_SetText} $Txt_Browser ""
    ;SetCtlColors $Txt_Browser "FFFFFF" transparent ;背景设成透明
    ;CreateFont $1 "微软雅黑" "12" "100"
    ;SendMessage $Txt_Browser ${WM_SETFONT} $1 1
    ;system::Call `user32::SetWindowText(i $Txt_Browser, t "$INSTDIR")`
    ;system::Call `user32::SetWindowText(i $Btn_TxtBrowser, t "$INSTDIR")`
    ${NSD_SetText} $Txt_Browser $INSTDIR
    ${NSD_SetText} $Btn_TxtBrowser $INSTDIR
    ;SetCtlColors $Txt_Browser "FFFFFF" transparent ;背景设成透明
    ;CreateFont $1 "微软雅黑" "12" "100"
    ;SendMessage $Txt_Browser ${WM_SETFONT} $1 1
  ${EndIf}

   ;${NSD_GetText} $Txt_Browser  $0
   ;nsDialogs::SelectFolderDialog  "请选择 ${PRODUCT_NAME} 安装目录："  "$0"
   ;Pop $0
   ;${IfNot} $0 == error
   ;	${NSD_SetText} $Txt_Browser  $0
   ;${EndIf}
FunctionEnd

; Usage:
; Push "C:\Program Files\Directory\Whatever"
; Call GetParent
; Pop $R0 ; $R0 equal "C:\Program Files\Directory"
;得到选中目录用于拼接安装程序名称
Function GetParent
  Exch $R0 ; input string
  Push $R1
  Push $R2
  Push $R3
  StrCpy $R1 0
  StrLen $R2 $R0
  loop:
    IntOp $R1 $R1 + 1
    IntCmp $R1 $R2 get 0 get
    StrCpy $R3 $R0 1 -$R1
    StrCmp $R3 "\" get
    Goto loop
  get:
    StrCpy $R0 $R0 -$R1
    Pop $R3
    Pop $R2
    Pop $R1
    Exch $R0 ; output string
FunctionEnd

; Usage:
; Push $INSTDIR ; input string "C:\Program Files\ProgramName"
; Push "\" ; input chop char
; Call GetLastPart
; Pop $R1 ; last part "ProgramName"
;截取选中目录
Function GetLastPart
  Exch $0 ; chop char
  Exch
  Exch $1 ; input string
  Push $2
  Push $3
  StrCpy $2 0
  loop:
    IntOp $2 $2 - 1
    StrCpy $3 $1 1 $2
    StrCmp $3 "" 0 +3
      StrCpy $0 ""
      Goto exit2
    StrCmp $3 $0 exit1
    Goto loop
  exit1:
    IntOp $2 $2 + 1
    StrCpy $0 $1 "" $2
  exit2:
    Pop $3
    Pop $2
    Pop $1
    Exch $0 ; output string
FunctionEnd

/******************************
 *  以下是安装程序的字体  *
 ******************************/
;Section "Fonts"
  ;StrCpy $FONT_DIR $FONTS
;  Call InstallTTFFont
;  SendMessage ${HWND_BROADCAST} ${WM_FONTCHANGE} 0 0 /TIMEOUT=5000
;SectionEnd

Function InstallTTFFont
  CopyFiles "$INSTDIR\${BIN_DIR}\*.ttf" "$INSTDIR\${BIN_DIR}"
  StrCpy $0 "$INSTDIR\${BIN_DIR}\${KAITI}"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion\Fonts" "标准楷体简(TrueType)" "$0"
  System::Call "GDI32::AddFontResource(t) i ('$0') .s"
FunctionEnd

;Section "InstallReport"
;  Call ReportInstall
;SectionEnd

 /******************************
 *  统计安装次数部分 *
 ******************************/
Function ReportInstall
  StrCpy $R8 "data={$\"eventId$\":1005,$\"eventTime$\":0,$\"params$\":[],$\"userId$\":0,$\"versionCode$\":$\"${PRODUCT_VERSION}$\",$\"ver$\":0}"
  inetc::post "$R8" /SILENT "http://p.101.com/101ppt/pptEvent.php" "$INSTDIR\post.dat"
  Sleep 100
  Delete "$INSTDIR\post.dat"
FunctionEnd

/******************************
 *  以下是安装程序的卸载部分  *
 ******************************/
Function un.onInit
  SetOverwrite on
  InitPluginsDir
  ;Call un.CreateMutex
  ;禁止多个安装程序实例
  System::Call 'kernel32::CreateMutexA(i 0, i 0, t "PPTShell_Install") i .r1 ?e'
  Pop $R0
  StrCmp $R0 0 +3
  MessageBox MB_OK|MB_ICONEXCLAMATION "安装或者卸载程序正在运行"
  Abort

  FindProcDLL::FindProc "${PPTSHELL_EXE_NAME}"
  Pop $R0
  IntCmp $R0 1 kill_app no_run
  kill_app:
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON1 "${PRODUCT_NAME}正在运行！点“是”将关闭继续卸载，点“否”退出卸载程序。" IDYES kill IDNO quit
  kill:
    KillProcDLL::KillProc "${PPT_EXE_NAME}"
    KillProcDLL::KillProc "${PPTSHELL_EXE_NAME}"
    Sleep 500
    goto no_run
  quit:
    Quit
  no_run:
  
  ;Call un.KillUpdate
  ;Call un.KillCourse
  KillProcDLL::KillProc "${UPDATE_EXE_NAME}"
  KillProcDLL::KillProc "${COURSE_EXE_NAME}"
  KillProcDLL::KillProc "${NODE_EXE_NAME}"
  
  
  ;kill Adb
;  FindProcDLL::FindProc "adb.exe"
;  Pop $R0
;  IntCmp $R0 1 kill_adb adb_no_run
;  kill_adb:
;    KillProcDLL::KillProc "adb.exe"
;    Sleep 100
;    goto no_run
;  adb_no_run:
  
    
    File `/ONAME=$PLUGINSDIR\bg.bmp` `images\StPad.bmp`
    File `/ONAME=$PLUGINSDIR\icon.bmp` `images\icon.bmp`

    File `/ONAME=$PLUGINSDIR\Yes.bmp` `images\Yes.bmp`
    File `/ONAME=$PLUGINSDIR\No.bmp` `images\No.bmp`
    File `/ONAME=$PLUGINSDIR\Done.bmp` `images\Done.bmp`
    
    File `/oname=$PLUGINSDIR\btnClose.bmp` `images\close.bmp`
    File `/oname=$PLUGINSDIR\btnmini.bmp` `images\mini.bmp`

    ;进度条皮肤
    File `/oname=$PLUGINSDIR\Progress.bmp` `images\Progress.bmp`
    File `/oname=$PLUGINSDIR\ProgressBar.bmp` `images\ProgressBar.bmp`
    File `/oname=$PLUGINSDIR\IconDone.bmp` `images\IconDone.bmp`
    File `/oname=$PLUGINSDIR\IconUnInstall.bmp` `images\IconUnInstall.bmp`
    ;File `/ONAME=$PLUGINSDIR\PPercent.bmp` `images\PPercent.bmp`

    SkinBtn::Init "$PLUGINSDIR\Yes.bmp"
    SkinBtn::Init "$PLUGINSDIR\No.bmp"
FunctionEnd

;-------------检测计算机上安装程序已运行----------
Function un.CreateMutex
#检查安装互斥：#
ReCheck:
  System::Call 'kernel32::CreateMutexA(i 0, i 0, t"${MyMutex_Install}") i.R1 ?e'
Pop $R0
  System::Call 'kernel32::CloseHandle(i R1) i.s'
#检查卸载互斥：#
  System::Call 'kernel32::CreateMutexA(i 0, i 0, t"${MyMutex_UnInstall}") i.R3 ?e'
Pop $R2
  System::Call 'kernel32::CloseHandle(i R3) i.s'
#判断安装/卸载互斥的存在#
${If} $R0 != 0
  MessageBox MB_RetryCancel|MB_ICONEXCLAMATION "安装程序已经运行！" IdRetry ReCheck
Quit
${ElseIf} $R2 != 0
  MessageBox MB_RetryCancel|MB_ICONEXCLAMATION "卸载程序已经运行！" IdRetry ReCheck
Quit
${Else}
#创建安装互斥：#
  System::Call 'kernel32::CreateMutexA(i 0, i 0, t"${MyMutex_Install}") i.R1 ?e'
Pop $R0
  StrCmp $R0 0 +2
Quit
${EndIf}
FunctionEnd

Function un.KillPPT
  FindProcDLL::FindProc "${PPT_NAME}.EXE"
  Pop $R0
  IntCmp $R0 1 kill_ppt ppt_no_run
  kill_ppt:
    KillProcDLL::KillProc "${PPT_NAME}.EXE"
    Sleep 500
  ppt_no_run:
FunctionEnd

Function un.KillUpdate
  FindProcDLL::FindProc "${UPDATE_NAME}.EXE"
  Pop $R0
  IntCmp $R0 1 kill_update update_no_run
  kill_update:
    KillProcDLL::KillProc "${UPDATE_NAME}.EXE"
    Sleep 500
  update_no_run:
FunctionEnd

Function un.KillCoursePlayer
  FindProcDLL::FindProc "${COURSE_NAME}.EXE"
  Pop $R0
  IntCmp $R0 1 kill_course course_no_run
  kill_course:
    KillProcDLL::KillProc "${COURSE_NAME}.EXE"
    Sleep 500
  course_no_run:
FunctionEnd

Function un.onGUIInit
    ;消除边框
    System::Call `user32::SetWindowLong(i$HWNDPARENT,i${GWL_STYLE},0x9480084C)i.R0`
    ;隐藏一些既有控件
    GetDlgItem $0 $HWNDPARENT 1034
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1035
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1036
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1037
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1038
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1039
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1256
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1028
    ShowWindow $0 ${SW_HIDE}

    ${NSW_SetWindowSize} $HWNDPARENT 602 428 ;改变Page大小   588 291
    ;${NSW_SetWindowSize} $HWNDPARENT $DialogWidth $DialogHight ;改变Page大小   588 291
    System::Call User32::GetDesktopWindow()i.R0
    ;圆角
    System::Alloc 16
    System::Call user32::GetWindowRect(i$HWNDPARENT,isR0)
    System::Call *$R0(i.R1,i.R2,i.R3,i.R4)
    IntOp $R3 $R3 - $R1
    IntOp $R4 $R4 - $R2
    System::Call gdi32::CreateRoundRectRgn(i0,i0,iR3,iR4,i4,i4)i.r0
    System::Call user32::SetWindowRgn(i$HWNDPARENT,ir0,i1)
    System::Free $R0

FunctionEnd

;处理无边框移动
Function un.onGUICallback
  ${If} $MSG = ${WM_LBUTTONDOWN}
    SendMessage $HWNDPARENT ${WM_NCLBUTTONDOWN} ${HTCAPTION} $0
  ${EndIf}
FunctionEnd
#----------------------------------------------
#执行卸载任务
#----------------------------------------------
Function UninstallSoft
  ReadRegStr $R0 HKLM \
  "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" \
  "UninstallString"
  ${GetParent} "$R0" $R1
  StrCmp $R0 "" done
  IfFileExists $R0 uninst
	Goto done
;运行卸载程序
uninst:
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_TOPMOST "系统已存在${PRODUCT_NAME}，是否卸载？" IDYES +2
  Goto done
  ExecWait "$R0 /S _?=$R1" ;这里$R0为读取到的卸载程序名称， /S是静默卸载参数使用NSIS生成的卸载程序必须要加上 _? 才能等待卸载。$R1是软件位置
  IfFileExists "$R1" dir ;如果 $R1软件位置 还有文件则跳转到 DIR: 删除里面的所有文件
  Goto done
dir: ;如果文件夹存在
	;Delete "$R1\*.*" ;即删除里面所有文件,请谨慎使用
  StrCpy $INSTDIR $R1
  Delete "$INSTDIR\*.exe"
  Delete "$INSTDIR\*.dll"
  Delete "$INSTDIR\*.dat"

  RMDir /r "$INSTDIR\framework"
  RMDir /r "$INSTDIR\bin"
  RMDir /r "$INSTDIR\Microsoft.VC90.CRT"
  RMDir /r "$INSTDIR\Microsoft.VC90.MFC"
  RMDir /r "$INSTDIR\MainSkin"
  RMDir /r "$INSTDIR\temp"
  RMDir /r "$INSTDIR\package"
done:

FunctionEnd


/******************************
 *  以下是安装程序的卸载部分  *
 ******************************/

Section Uninstall
;Call un.KillAllApp
Sleep 100
SetDetailsPrint textonly
DetailPrint "正在卸载..."
SetDetailsPrint none
  ;ReadRegStr $R0 HKLM \
  ;"Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" \
  ;"UninstallString"
  ;Messagebox MB_YESNO "$R0  + $INSTDIR"
  ;IfFileExists "$R0" uninstdone
  ;${GetParent} "$R0" $R1
  ;StrCpy $INSTDIR $R1
  ;SetOverwrite on
  SetShellVarContext all
  
  ;解锁开始菜单快捷方式           先解锁，再删除快捷，最后删除应用，不然会导致解锁失败
  ExecShell "startunpin" "$QUICKLAUNCH\${PRODUCT_NAME}.lnk"
  ;解锁任务栏快捷方式
  ExecShell "taskbarunpin" "$QUICKLAUNCH\${PRODUCT_NAME}.lnk"

  Delete "$DESKTOP\${PRODUCT_NAME}.lnk"
  Delete "$SMPROGRAMS\${PRODUCT_NAME}.lnk"
  Delete "$DESKTOP\${PRODUCT_WEB_LINK_NAME}.lnk"
  RMDir /r /REBOOTOK "$SMPROGRAMS\${PRODUCT_NAME}"
  
  Delete "$QUICKLAUNCH\User Pinned\TaskBar\${PRODUCT_NAME}.lnk"
  Delete "$QUICKLAUNCH\${PRODUCT_NAME}.lnk"
  Delete "$STARTMENU\${PRODUCT_NAME}.lnk"
  
  Delete "$INSTDIR\*.exe"
  Delete "$INSTDIR\*.dll"
  Delete "$INSTDIR\*.dat"
  Delete "$INSTDIR\*.log"
  
  RMDir /r /REBOOTOK "$INSTDIR\bin"
  RMDir /r /REBOOTOK "$INSTDIR\Skins"
  RMDir /r /REBOOTOK "$INSTDIR\template"
  RMDir /r /REBOOTOK "$INSTDIR\Cache"
  RMDir /r /REBOOTOK "$INSTDIR\log"
  RMDir /r /REBOOTOK "$INSTDIR\temp"
  RMDir /r /REBOOTOK "$INSTDIR\package"
  
  Messagebox MB_YESNO "是否删除配置文件?" IDYES true IDNO false
  true:
    RMDir /r /REBOOTOK "$INSTDIR\settting"
    RMDir /r /REBOOTOK "$INSTDIR"
  false:

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  
  ;恢复注册表
 #------------------ Win 8 -------------------------------------
  DeleteRegKey HKCR ".ppt\PPTShell"
  DeleteRegKey HKCR ".pptx\PPTShells"

  WriteRegStr HKCR ".ppt" "" "PowerPoint.Show.8"
  WriteRegStr HKCR ".ppt\PowerPoint.Show.8\ShellNew" "" ""
  
  WriteRegStr HKCR ".pptx" "" "PowerPoint.Show.12"
  WriteRegStr HKCR ".pptx\PowerPoint.Show.12\ShellNew" "" ""
  
 #------------------ 其它 -------------------------------------
 
 DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.pptx\UserChoice"
 DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.ppt\UserChoice"
 
 DeleteRegKey HKCR "PPTShell\\Shell\\Open\\Command"
 DeleteRegKey HKCU "Software\\Classes\\Applications\\PPTShell\\shell\\open\\command"
 
 WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.pptx\UserChoice" "Progid" "PowerPoint.Show.12"
 WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.ppt\UserChoice" "Progid" "PowerPoint.Show.8"
  
uninstdone:
  Sleep 10
  SetAutoClose true
SectionEnd

Function un.KillAllApp
    KillProcDLL::KillProc "${PPT_EXE_NAME}"
    KillProcDLL::KillProc "${PPTSHELL_EXE_NAME}"
    KillProcDll::KillProc "${OLD_PPTSHELL_EXE_NAME}"
    KillProcDLL::KillProc "${UPDATE_EXE_NAME}"
    KillProcDLL::KillProc "${COURSE_EXE_NAME}"
    KillProcDLL::KillProc "${NODE_EXE_NAME}"
FunctionEnd

#-- 根据 NSIS 脚本编辑规则，所有 Function 区段必须放置在 Section 区段之后编写，以避免安装程序出现未可预知的问题。--#

Function un.Page.5
   GetDlgItem $0 $HWNDPARENT 1
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 2
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 3
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1990
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1991
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1992
    ShowWindow $0 ${SW_HIDE}

    nsDialogs::Create 1044
    Pop $0
    ${If} $0 == error
        Abort
    ${EndIf}
    SetCtlColors $0 ""  transparent ;背景设成透明

    ;${NSW_SetWindowSize} $0 $DialogWidth $DialogHight ;改变Page大小   588 291
    ${NSW_SetWindowSize} $0 602 428 ;改变Page大小   588 291

    ;退出提示文字
    ${NSD_CreateLabel} 56 249 493U 20u "您确定要卸载${PRODUCT_NAME}程序吗？"
    Pop $Lbl_AppName
    SetCtlColors $Lbl_AppName "FFFFFF" transparent ;背景设成透明
    CreateFont $1 "微软雅黑" "16" "220"
    SendMessage $Lbl_AppName ${WM_SETFONT} $1 0
    ${NSD_AddStyle} $Lbl_AppName ${ES_CENTER}

    GetFunctionAddress $0 un.onGUICallback
    WndProc::onCallback $Lbl_AppName $0 ;处理无边框窗体移动

    ;标题文字
    ${NSD_CreateLabel} 5u 5u 115u 12u "${PRODUCT_NAME}" ; 卸载向导"
    Pop $Lbl_Title
    ;SetCtlColors $Lbl_Title "" 0xFFFFFF ;蓝色
    SetCtlColors $Lbl_Title "FFFFFF"  transparent ;背景设成透明
    CreateFont $1 "微软雅黑" "11" "800"
    SendMessage $Lbl_Title ${WM_SETFONT} $1 0
    ;${NSD_AddStyle} $Lbl_Title ${ES_LEFT}

    GetFunctionAddress $0 un.onGUICallback
    WndProc::onCallback $Lbl_Title $0 ;处理无边框窗体移动

    ;是(卸载)按钮
    ${NSD_CreateButton} 470 379 105 38 ""
    Pop $Btn_Yes
    SkinBtn::Set /IMGID=$PLUGINSDIR\Yes.bmp $Btn_Yes
    SetCtlColors $Btn_Yes "808080"  transparent ;背景设成透明
    CreateFont $1 "微软雅黑" "16" "500"
    SendMessage $Btn_Yes ${WM_SETFONT} $1 0
    ${NSD_AddStyle} $Btn_Yes ${ES_CENTER}

    GetFunctionAddress $3 un.OnNext
    SkinBtn::onClick $Btn_Yes $3

    ;否(退出)按钮
    ${NSD_CreateButton} 365 379 84 38 ""
    Pop $Btn_No
    SkinBtn::Set /IMGID=$PLUGINSDIR\No.bmp $Btn_No
    SetCtlColors $Btn_No "0d6190"  transparent ;背景设成透明
    CreateFont $1 "微软雅黑" "16" "500" ;
    SendMessage $Btn_No ${WM_SETFONT} $1 0
    ${NSD_AddStyle} $Btn_No ${ES_CENTER}

    GetFunctionAddress $3 un.OnExit
    SkinBtn::onClick $Btn_No $3

    ;贴程序图标
    ${NSD_CreateBitmap} 227 72 146 146 ""
    Pop $BGImage
    ${NSD_SetImage} $BGImage $PLUGINSDIR\Icon.bmp $ImageHandle
    GetFunctionAddress $0 un.onGUICallback
    WndProc::onCallback $BGImage $0 ;处理无边框窗体移动
    
    ;贴背景大图
    ${NSD_CreateBitmap} 0 0 100% 100% ""
    Pop $BGImage
    ${NSD_SetImage} $BGImage $PLUGINSDIR\bg.bmp $ImageHandle

    GetFunctionAddress $0 un.onGUICallback
    WndProc::onCallback $BGImage $0 ;处理无边框窗体移动
    nsDialogs::Show
    ${NSD_FreeImage} $ImageHandle
FunctionEnd

Function un.InstFiles.PRO

FunctionEnd

;卸载进度页面
Function un.InstFiles.Show

    StrCpy $Bool_UnInstall 0
    FindWindow $R2 "#32770" "" $HWNDPARENT
    StrCpy $DefaultPage $R2
    GetDlgItem $0 $HWNDPARENT 1
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 2
    ShowWindow $0 ${SW_HIDE}
    EnableWindow $0 0       ;禁止取消按钮，达到屏蔽ESC的效果
    GetDlgItem $0 $HWNDPARENT 3
    ShowWindow $0 ${SW_HIDE}

    GetDlgItem $1 $R2 1027
    ShowWindow $1 ${SW_HIDE}

    GetDlgItem $1 $R2 1990  ;最小化
    ShowWindow $1 ${SW_HIDE}
    GetDlgItem $1 $R2 1991 ;关闭
    ShowWindow $1 ${SW_HIDE}
    EnableWindow $1 0
    GetDlgItem $1 $R2 1994  ;安装
    ShowWindow $1 ${SW_HIDE}
    ;EnableWindow $1 0

    StrCpy $R0 $R2 ;改变页面大小,不然贴图不能全页
    System::Call "user32::MoveWindow(i R0, i 0, i 0, i 601, i 427) i R2"    ;修改大小   588 291
    GetFunctionAddress $0 un.onGUICallback
    WndProc::onCallback $R0 $0 ;处理无边框窗体移动

    GetDlgItem $R0 $R2 1004  ;设置进度条位置
    System::Call "user32::MoveWindow(i R0, i 15, i 400, i 562, i 6) i R2"
    Strcpy $PB_ProgressBar $R0
    SkinProgress::Set $PB_ProgressBar "$PLUGINSDIR\Progress.bmp" "$PLUGINSDIR\ProgressBar.bmp"

    ;标题文字
    GetDlgItem $R0 $R2 1993  ;
    System::Call "user32::MoveWindow(i R0, i 8, i 8, i 170, i 20) i R2"    ;注意：调用系统API，必须用 R0 ~ R9, 才有效果                   ;5u 5u 115u 12u
    StrCpy $Lbl_Title $R0
    SetCtlColors $Lbl_Title "FFFFFF"  transparent ;背景设成透明
    ${NSD_SetText} $Lbl_Title "${PRODUCT_NAME}" ; 卸载向导"
    CreateFont $1 "微软雅黑" "11" "800"
    SendMessage $Lbl_Title ${WM_SETFONT} $1 0

    ;应用名
    ;${NSD_CreateLabel} 270 70 300 60 "${PRODUCT_NAME}"
    GetDlgItem $R0 $R2 1997  ;设置应用名位置
    System::Call "user32::MoveWindow(i R0, i 130, i 95, i 360, i 60) i R2"
    StrCpy $Lbl_AppName $R0
    SetCtlColors $Lbl_AppName "FFFFFF" transparent ;背景设成透明
    ${NSD_SetText} $Lbl_AppName "感谢您使用${PRODUCT_NAME}!"
    CreateFont $1 "微软雅黑" "24" "300"
    SendMessage $Lbl_AppName ${WM_SETFONT} $1 0

    ;DetailPrint 文字 -- "正在卸载..." 在前面Section里输出的
    GetDlgItem $R1 $R2 1006  ;获取1006控件设置颜色并改变位置
    System::Call "user32::MoveWindow(i R1, i 15, i 376, i 500, i 40) i R2"
    SetCtlColors $R1 "888888"  transparent ;背景设成F6F6F6,注意颜色不能设为透明，否则重叠
    CreateFont $1 "微软雅黑" "10" "50"
    SendMessage $R1 ${WM_SETFONT} $1 0

    ;进度文字
    GetDlgItem $R0 $R2 1992  ;获取1992控件设置颜色并改变位置   [用按钮来显示]      1997 是文本
    ShowWindow $R0 ${SW_HIDE}
    ;进度文字
    ;GetDlgItem $R0 $R2 1997  ;获取1992控件设置颜色并改变位置   [用按钮来显示]      1997 是文本
    ;ShowWindow $R0 ${SW_HIDE}
    System::Call "user32::MoveWindow(i R0, i 90, i 376, i 40, i 20, i 1) i R2"
    SetCtlColors $R0 "888888" "FFFFFF" ;背景设成F6F6F6,注意颜色不能设为透明，否则重叠
    CreateFont $1 "微软雅黑" "10" "50"
    SendMessage $R0 ${WM_SETFONT} $1 0
    Strcpy $PPercent $R0
    ${NSD_SetText} $PPercent "0%"

    ;小图标
    GetDlgItem $R0 $R2 1996
    System::Call "user32::MoveWindow(i R0, i 227, i 155, i 146, i 146) i R2"
    ${NSD_SetImage} $R0 $PLUGINSDIR\IconUnInstall.bmp $IconHandle

    ;贴背景大图
    GetDlgItem $R0 $R2 1995
    System::Call "user32::MoveWindow(i R0, i 0, i 0, i 601, i 427) i R2"    ;修改大小   588 291
    ${NSD_SetImage} $R0 $PLUGINSDIR\bg.bmp $ImageHandle

    #借用WndSubclass插件设定timer函数地址（尝试过直接为settimer设定函数地址，但没有插件支持很困难）
    ;${WndSubclass_Subclass} $HWNDPARENT un.GetProgressPos $SubProgress $SubProgress
    ;System::Call user32::SetTimer(i $DefaultPage, i 10, i 50, i 0)
    ;GetFunctionAddress $2 un.GetProgressPos
    ;ThreadTimer::Start 200 0 $2
    
FunctionEnd

Function un.GetProgressPos                       ;GetProgressPos
  SendMessage $PB_ProgressBar 0x0407 0 0 $1
  SendMessage $PB_ProgressBar 0x0408 0 0 $2
  ${If} $2 > 0
  ${If} $1 > 0
    IntOp $2 $2 * 100
    IntOp $1 $2 / $1
    ${NSD_SetText} $PPercent "$1%"
    StrCpy $Bool_UnInstall 1
    ${If} $1 >= 99
      ThreadTimer::Stop
      ;System::Call user32::KillTimer(i $DefaultPage, i 10)
      ${NSD_SetText} $PPercent "100%"
      ;System::Call user32::InvalidateRect(i$DefaultPage, i 0, i 1)
    ${Endif}
  ;${ElseIf} $Bool_UnInstall = 1
  ;    System::Call user32::KillTimer(i $DefaultPage, i 10)
  ${Endif}
  ${Endif}
FunctionEnd


Function un.InstFiles.Leave
  ${IfThen} ${Abort} ${|} Quit ${|}
FunctionEnd

Function un.Page.6
    GetDlgItem $0 $HWNDPARENT 1
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 2
    ShowWindow $0 ${SW_HIDE}
    EnableWindow $0 0       ;禁止取消按钮，达到屏蔽ESC的效果
    GetDlgItem $0 $HWNDPARENT 3
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1990
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1991
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1992
    ShowWindow $0 ${SW_HIDE}

    ;设置当前页面位置与退出页面的相隔数
    StrCpy $CurrentPage 2

    nsDialogs::Create 1044
    Pop $0
    ${If} $0 == error
        Abort
    ${EndIf}
    SetCtlColors $0 ""  transparent ;背景设成透明
    ${NSW_SetWindowSize} $0 601 427 ;改变Page大小   588 291
    ;${NSW_SetWindowSize} $0 $DialogWidth $DialogHight ;改变Page大小   588 291

    ;完成按钮  ${NSD_CreateButton} 232 158 122 47 ""
    ${NSD_CreateButton} 470 379 105 38 ""
    Pop $Btn_Done
    SkinBtn::Set /IMGID=$PLUGINSDIR\Yes.bmp $Btn_Done

    GetFunctionAddress $3 un.OnExit
    SkinBtn::onClick $Btn_Done $3
    ${NSD_SetFocus} $Btn_Done

    ;卸载完成
    ${NSD_CreateLabel} 195 205 300 60 "卸载完成"
    Pop $Lbl_AppName
    SetCtlColors $Lbl_AppName "FFFFFF" transparent ;背景设成透明
    CreateFont $1 "微软雅黑" "38" "400"
    SendMessage $Lbl_AppName ${WM_SETFONT} $1 0
    ;;${NSD_AddStyle} $Lbl_AppName ${ES_CENTER}
    GetFunctionAddress $0 un.onGUICallback
    WndProc::onCallback $Lbl_AppName $0 ;处理无边框窗体移动

    ;标题文字
    ${NSD_CreateLabel} 5u 5u 115u 12u "${PRODUCT_NAME}" ; 卸载向导"
    Pop $Lbl_Title
    ;SetCtlColors $Lbl_Title "" 0xFFFFFF ;蓝色
    SetCtlColors $Lbl_Title "FFFFFF"  transparent ;背景设成透明
    CreateFont $1 "微软雅黑" "11" "800"
    SendMessage $Lbl_Title ${WM_SETFONT} $1 0
    ;${NSD_AddStyle} $Lbl_Title ${ES_CENTER}
    GetFunctionAddress $0 un.onGUICallback
    WndProc::onCallback $Lbl_Title $0 ;处理无边框窗体移动


    ;关闭按钮
    ;${NSD_CreateButton} 574 0 27 27 ""
    ;Pop $0
    ;SkinBtn::Set /IMGID=$PLUGINSDIR\btnClose.bmp $0
    ;GetFunctionAddress $3 onClickClose
    ;SkinBtn::onClick $0 $3

    ;贴程序图标
    ${NSD_CreateBitmap} 254 90 93 96 ""
    Pop $BGImage
    ${NSD_SetImage} $BGImage $PLUGINSDIR\IconDone.bmp $ImageHandle

    GetFunctionAddress $0 un.onGUICallback
    WndProc::onCallback $BGImage $0 ;处理无边框窗体移动

    ;贴背景大图
    ${NSD_CreateBitmap} 0 0 100% 100% ""
    Pop $BGImage
    ${NSD_SetImage} $BGImage $PLUGINSDIR\bg.bmp $ImageHandle

    GetFunctionAddress $0 un.onGUICallback
    WndProc::onCallback $BGImage $0 ;处理无边框窗体移动
    nsDialogs::Show
    ${NSD_FreeImage} $ImageHandle

FunctionEnd


Function un.Page.7

FunctionEnd

#------------------------------------------
#最小化代码
#------------------------------------------
Function un.OnMini
System::Call user32::CloseWindow(i$HWNDPARENT) ;最小化
FunctionEnd

#------------------------------------------
#卸载完成页使用独立区段方便操作，如打开某个网页
#------------------------------------------

;处理页面跳转的命令
Function un.RelGotoPage
  IntCmp $R9 0 0 Move Move
    StrCmp $R9 "X" 0 Move
      StrCpy $R9 "120"
  Move:
  SendMessage $HWNDPARENT "0x408" "$R9" ""
FunctionEnd

Function un.OnNext
  StrCpy $R9 1 ;Goto the next page
  Call un.RelGotoPage
  Abort
FunctionEnd

#------------------------------------------
#关闭代码
#------------------------------------------
Function un.OnExit
RMDir /r /REBOOTOK "$PLUGINSDIR"
System::Call kernel32::ExitProcess(0) ;关闭
FunctionEnd
