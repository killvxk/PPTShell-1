@color B
@echo off
@echo 开发版 - 升级文件自动复制批处理 - LSJ
@pause
@cd VerRoot
@echo 上两个对比版本如下： 
@dir /b
@set /p newVer=请输入新版本号(数字和点组成,格式 a.b.c.d):
md %newVer%
@cd %newVer%
@set /p tips=开始复制Skins目录，回车继续：
xcopy ..\..\..\PPTShell\PPTShell\Skins .\Skins\ /E /Y /D
@set /p tips=开始复制bin目录，回车继续：
xcopy ..\..\..\PPTShell\PPTShell\bin .\bin\ /E /Y /D /exclude:..\..\exclude.txt
xcopy ..\..\..\PPTShell\PPTShell\ReleaseCopy\updatelib\开发版 .\bin\CoursePlayer\ /E /Y /D
@set /p tips=开始复制程序文件，回车继续：
copy ..\..\..\bin\Release开发版\msftedit.dll .\ 
copy ..\..\..\bin\Release开发版\Update.exe .\
copy ..\..\..\bin\Release开发版\DuiLib.dll .\ 
copy ..\..\..\bin\Release开发版\101PPT.exe .\
>version.dat set /p=%newVer%<nul
@echo 恭喜，版本文件复制成功！
pause