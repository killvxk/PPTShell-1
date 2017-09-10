# Microsoft Developer Studio Project File - Name="Update" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Update - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Update.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Update.mak" CFG="Update - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Update - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Update - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Update - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "RARDLL" /D "UNRAR" /D "SILENT" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ws2_32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Update - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "RARDLL" /D "UNRAR" /D "SILENT" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ws2_32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Update - Win32 Release"
# Name "Update - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AutoUpdate.cpp
# End Source File
# Begin Source File

SOURCE=.\BmpProgCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\MD5Encrypt.cpp
# End Source File
# Begin Source File

SOURCE=.\Net.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\UnRarAuto.cpp
# End Source File
# Begin Source File

SOURCE=.\Update.cpp
# End Source File
# Begin Source File

SOURCE=.\Update.rc
# End Source File
# Begin Source File

SOURCE=.\UpdateDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Util.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AutoUpdate.h
# End Source File
# Begin Source File

SOURCE=.\BmpProgCtrl.h
# End Source File
# Begin Source File

SOURCE=.\MD5Encrypt.h
# End Source File
# Begin Source File

SOURCE=.\Net.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\UnRarAuto.h
# End Source File
# Begin Source File

SOURCE=.\Update.h
# End Source File
# Begin Source File

SOURCE=.\UpdateDlg.h
# End Source File
# Begin Source File

SOURCE=.\Util.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\dlg.bmp
# End Source File
# Begin Source File

SOURCE=.\res\per.bmp
# End Source File
# Begin Source File

SOURCE=.\res\perbk.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Update.ico
# End Source File
# Begin Source File

SOURCE=.\res\Update.rc2
# End Source File
# End Group
# Begin Group "unrar"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\unrar\archive.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\archive.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\arcread.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\array.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\cmddata.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\cmddata.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\coder.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\compress.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\consio.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\consio.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\crc.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\crc.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\crypt.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\crypt.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\dll.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\dll.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\encname.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\encname.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\errhnd.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\errhnd.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\extinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\extinfo.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\extract.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\extract.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\filcreat.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\filcreat.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\file.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\file.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\filefn.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\filefn.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\filestr.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\filestr.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\find.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\find.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\getbits.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\getbits.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\global.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\global.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\headers.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\isnt.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\isnt.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\list.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\list.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\loclang.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\log.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\match.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\match.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\model.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\options.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\options.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\os.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\pathfn.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\pathfn.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\rar.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\rar.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\rardefs.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\rarlang.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\raros.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\rartypes.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\rarvm.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\rarvm.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\rarvmtbl.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\rawread.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\rawread.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\rdwrfn.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\rdwrfn.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\recvol.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\recvol.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\resource.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\resource.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\rijndael.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\rijndael.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\rs.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\rs.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\savepos.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\savepos.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\scantree.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\scantree.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\sha1.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\sha1.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\smallfn.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\smallfn.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\strfn.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\strfn.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\strlist.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\strlist.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\suballoc.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\system.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\system.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\timefn.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\timefn.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\ulinks.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\ulinks.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\unicode.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\unicode.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\unpack.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\unpack.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\version.hpp
# End Source File
# Begin Source File

SOURCE=.\unrar\volume.cpp
# End Source File
# Begin Source File

SOURCE=.\unrar\volume.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=".\res\vc6-xp-uac.txt"
# End Source File
# End Target
# End Project
