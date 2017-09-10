
SETLOCAL ENABLEDELAYEDEXPANSION

set curdir=%~sdp0
::set curdir=%cd%
::echo %curdir%
cd /d %curdir%
::echo !curdir!
::echo %~sdp0
set Dirtmp=%curdir%AutoBuildStr.h

set FilePath=%Dirtmp:\=\\%

set  user1=#define _STR_BUILD_TIME _T
set  user2=^("Beta.%date:~0,4%.%date:~5,2%%date:~8,2%.%time:~0,2%%time:~3,2%"^)

set  final1=#define _STR_FINAL_TIME _T
set  final2=^("Final.%date:~0,4%.%date:~5,2%%date:~8,2%.%time:~0,2%%time:~3,2%"^)

set user2=%user2: =0%
set final2=%final2: =0%
set user=%user1%%user2%
set final=%final1%%final2%
	

if not exist %FilePath% set "first=true"
if "%first%"=="true" echo !user!>AutoBuildStr.h
if "%first%"=="true" echo !final!>>AutoBuildStr.h

:loop
for /f "skip=1 tokens=1 delims=. " %%a in ('wmic datafile where name^=^"%FilePath%^" get LastModified') do (
IF %%a LSS 0 GOTO end
set a=%%a

SET /a th=%time:~0,2%
SET /a ah=!a:~8,1!*10+!a:~9,1!
::SET /a h=%time:~0,2%-!a:~8,2!
SET /a h=!th!-!ah!

::SET /a m=%time:~3,2%-!a:~10,2!
SET /a tm=%time:~3,1%*10+%time:~4,1%
SET /a am=!a:~10,1!*10+!a:~11,1!
SET /a m=!tm!-!am!

::SET /a d=%date:~8,2%-!a:~6,2!
SET /a td=%date:~8,1%*10+%date:~9,1%
SET /a ad=!a:~6,1!*10+!a:~7,1!
SET /a d=!td!-!ad!


::SET /a y=%date:~5,2%-!a:~4,2!
SET /a ty=%date:~5,1%*10+%date:~6,1%
SET /a ay=!a:~4,1!*10+!a:~5,1!
SET /a y=!ty!-!ay!


SET /a n=%date:~0,4%-!a:~0,4!

IF !n! gtr 0 (echo !user!>AutoBuildStr.h)
IF !n! gtr 0 (echo !final!>>AutoBuildStr.h)

IF !y! gtr 0 (echo !user!>AutoBuildStr.h)
IF !y! gtr 0 (echo !final!>>AutoBuildStr.h)

IF !d! gtr 0 (echo !user!>AutoBuildStr.h)
IF !d! gtr 0 (echo !final!>>AutoBuildStr.h)

::IF !h! gtr 0 (IF !m! leq 0 (SET /a h=!h!-1 & SET /a m=%time:~3,2%+60-!a:~10,2!)) 
IF !h! gtr 0 (echo !user!>AutoBuildStr.h)
IF !h! gtr 0 (echo !final!>>AutoBuildStr.h)

IF !m! gtr 9 (echo !user!>AutoBuildStr.h)
IF !m! gtr 9 (echo !final!>>AutoBuildStr.h)
)

:END