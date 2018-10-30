# Microsoft Developer Studio Project File - Name="DEMO" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=DEMO - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DEMO.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DEMO.mak" CFG="DEMO - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DEMO - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DEMO - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DEMO - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40d /d "NDEBUG"
# ADD RSC /l 0x40d /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 d3dx8.lib kernel32.lib user32.lib gdi32.lib winmm.lib ddraw.lib dxguid.lib d3d8.lib advapi32.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"LIBC"
# SUBTRACT LINK32 /pdb:none /debug

!ELSEIF  "$(CFG)" == "DEMO - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40d /d "_DEBUG"
# ADD RSC /l 0x40d /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib d3dx8.lib kernel32.lib user32.lib gdi32.lib winmm.lib ddraw.lib dxguid.lib d3d8.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "DEMO - Win32 Release"
# Name "DEMO - Win32 Debug"
# Begin Group "D3D8"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\D3D8.CPP
# End Source File
# Begin Source File

SOURCE=.\D3D8.h
# End Source File
# Begin Source File

SOURCE=.\D3D8i.h
# End Source File
# Begin Source File

SOURCE=.\d3dutil.cpp
# End Source File
# Begin Source File

SOURCE=.\dxutil.cpp
# End Source File
# End Group
# Begin Group "DDRAW"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DDRAW.cpp
# End Source File
# Begin Source File

SOURCE=.\DDRAW.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\CHASE.CPP
# End Source File
# Begin Source File

SOURCE=.\CHASE.H
# End Source File
# Begin Source File

SOURCE=.\CITY.CPP
# End Source File
# Begin Source File

SOURCE=.\CITY.H
# End Source File
# Begin Source File

SOURCE=.\CRASH.CPP
# End Source File
# Begin Source File

SOURCE=.\CRASH.H
# End Source File
# Begin Source File

SOURCE=.\CREDITS.CPP
# End Source File
# Begin Source File

SOURCE=.\CREDITS.H
# End Source File
# Begin Source File

SOURCE=.\END.CPP
# End Source File
# Begin Source File

SOURCE=.\FillerTest.cpp
# End Source File
# Begin Source File

SOURCE=.\FillerTest.h
# End Source File
# Begin Source File

SOURCE=.\FOUNTAIN.CPP
# End Source File
# Begin Source File

SOURCE=.\FOUNTAIN.H
# End Source File
# Begin Source File

SOURCE=.\GDI.CPP
# End Source File
# Begin Source File

SOURCE=.\GDI.H
# End Source File
# Begin Source File

SOURCE=.\GLAT.CPP
# End Source File
# Begin Source File

SOURCE=.\GLAT.H
# End Source File
# Begin Source File

SOURCE=.\GREETS.CPP
# End Source File
# Begin Source File

SOURCE=.\GREETS.H
# End Source File
# Begin Source File

SOURCE=.\Koch.cpp
# End Source File
# Begin Source File

SOURCE=.\Koch.h
# End Source File
# Begin Source File

SOURCE=.\Nova.cpp
# End Source File
# Begin Source File

SOURCE=.\Nova.h
# End Source File
# Begin Source File

SOURCE=.\Raytracer.cpp
# End Source File
# Begin Source File

SOURCE=.\Raytracer.h
# End Source File
# Begin Source File

SOURCE=.\REV.CPP
# End Source File
# Begin Source File

SOURCE=.\REV.H
# End Source File
# Begin Source File

SOURCE=.\Win32Display.h
# End Source File
# End Target
# End Project
