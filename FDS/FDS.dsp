# Microsoft Developer Studio Project File - Name="FDS" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=FDS - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FDS.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FDS.mak" CFG="FDS - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FDS - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "FDS - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FDS - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# SUBTRACT CPP /Z<none>
# ADD BASE RSC /l 0x40d /d "NDEBUG"
# ADD RSC /l 0x40d /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "FDS - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x40d /d "_DEBUG"
# ADD RSC /l 0x40d /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "FDS - Win32 Release"
# Name "FDS - Win32 Debug"
# Begin Group "3DS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\3DS\3DS_READ.CPP
# End Source File
# End Group
# Begin Group "Cameras"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CAMERAS\CAMERAS.CPP
# End Source File
# End Group
# Begin Group "Fillers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FILLERS\BIHI.ASM

!IF  "$(CFG)" == "FDS - Win32 Release"

# Begin Custom Build
InputPath=.\FILLERS\BIHI.ASM

"bihi.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	wasm -fp5 Fillers\bihi.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "FDS - Win32 Debug"

# Begin Custom Build
InputPath=.\FILLERS\BIHI.ASM

"bihi.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	wasm -fp5 Fillers\bihi.asm

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FILLERS\BILINEAR.ASM

!IF  "$(CFG)" == "FDS - Win32 Release"

# Begin Custom Build
InputPath=.\FILLERS\BILINEAR.ASM

"bilinear.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	wasm -fp5 Fillers\bilinear.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "FDS - Win32 Debug"

# Begin Custom Build
InputPath=.\FILLERS\BILINEAR.ASM

"bilinear.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	wasm -fp5 Fillers\bilinear.asm

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FILLERS\BITRFLAT.ASM

!IF  "$(CFG)" == "FDS - Win32 Release"

# Begin Custom Build
InputPath=.\FILLERS\BITRFLAT.ASM

"bitrflat.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	wasm -fp5  Fillers\bitrflat.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "FDS - Win32 Debug"

# Begin Custom Build
InputPath=.\FILLERS\BITRFLAT.ASM

"bitrflat.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	wasm -fp5  Fillers\bitrflat.asm

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FILLERS\BITRUE.ASM

!IF  "$(CFG)" == "FDS - Win32 Release"

# Begin Custom Build
InputPath=.\FILLERS\BITRUE.ASM

"bitrue.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	wasm -fp5 Fillers\bitrue.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "FDS - Win32 Debug"

# Begin Custom Build
InputPath=.\FILLERS\BITRUE.ASM

"bitrue.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	wasm -d1 -fp5 Fillers\bitrue.asm

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FILLERS\BITTRUE.ASM

!IF  "$(CFG)" == "FDS - Win32 Release"

# Begin Custom Build
InputPath=.\FILLERS\BITTRUE.ASM

"bittrue.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	wasm -fp5 Fillers\bittrue.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "FDS - Win32 Debug"

# Begin Custom Build
InputPath=.\FILLERS\BITTRUE.ASM

"bittrue.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	wasm -fp5 Fillers\bittrue.asm

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FILLERS\F4Vec.cpp
# End Source File
# Begin Source File

SOURCE=.\FILLERS\F4Vec.h
# End Source File
# Begin Source File

SOURCE=.\FILLERS\FILLERS.CPP
# End Source File
# Begin Source File

SOURCE=.\FILLERS\GOURAUD.ASM

!IF  "$(CFG)" == "FDS - Win32 Release"

# Begin Custom Build
InputPath=.\FILLERS\GOURAUD.ASM

"gouraud.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	wasm -fp5 Fillers\gouraud.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "FDS - Win32 Debug"

# Begin Custom Build
InputPath=.\FILLERS\GOURAUD.ASM

"gouraud.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	wasm -fp5 Fillers\gouraud.asm

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FILLERS\GOURMMX.ASM

!IF  "$(CFG)" == "FDS - Win32 Release"

# Begin Custom Build
InputPath=.\FILLERS\GOURMMX.ASM

"gourmmx.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	wasm -fp5 Fillers\gourmmx.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "FDS - Win32 Debug"

# Begin Custom Build
InputPath=.\FILLERS\GOURMMX.ASM

"gourmmx.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	wasm -fp5 Fillers\gourmmx.asm

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FILLERS\GOURMMXT.ASM

!IF  "$(CFG)" == "FDS - Win32 Release"

# Begin Custom Build
InputPath=.\FILLERS\GOURMMXT.ASM

"gourmmxt.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	wasm -fp5 Fillers\gourmmxt.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "FDS - Win32 Debug"

# Begin Custom Build
InputPath=.\FILLERS\GOURMMXT.ASM

"gourmmxt.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	wasm -fp5 Fillers\gourmmxt.asm

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FILLERS\IX.cpp
# End Source File
# Begin Source File

SOURCE=.\FILLERS\IX.h
# End Source File
# Begin Source File

SOURCE=.\FILLERS\IXFZ.cpp
# End Source File
# Begin Source File

SOURCE=.\FILLERS\IXGZ.cpp
# End Source File
# Begin Source File

SOURCE=.\FILLERS\IXTGZ.cpp
# End Source File
# Begin Source File

SOURCE=.\FILLERS\IXTGZM.ASM

!IF  "$(CFG)" == "FDS - Win32 Release"

# Begin Custom Build
InputPath=.\FILLERS\IXTGZM.ASM
InputName=IXTGZM

"$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ml /c /Cp /Cx /coff /Fl /nologo /Sa /Sc $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "FDS - Win32 Debug"

# Begin Custom Build
InputPath=.\FILLERS\IXTGZM.ASM
InputName=IXTGZM

"$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ml /c /Cp /Cx /coff /Fl /nologo /Sa /Sc /Zi /Zd $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FILLERS\IXTGZTM.ASM

!IF  "$(CFG)" == "FDS - Win32 Release"

# Begin Custom Build
InputPath=.\FILLERS\IXTGZTM.ASM
InputName=IXTGZTM

"$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ml /c /Cp /Cx /coff /Fl /nologo /Sa /Sc $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "FDS - Win32 Debug"

# Begin Custom Build
InputPath=.\FILLERS\IXTGZTM.ASM
InputName=IXTGZTM

"$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ml /c /Cp /Cx /coff /Fl /nologo /Sa /Sc /Zi /Zd $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FILLERS\IXTZ.cpp
# End Source File
# Begin Source File

SOURCE=.\FILLERS\SCALE.ASM

!IF  "$(CFG)" == "FDS - Win32 Release"

# Begin Custom Build
InputPath=.\FILLERS\SCALE.ASM

"scale.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	wasm -fp5 Fillers\scale.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "FDS - Win32 Debug"

# Begin Custom Build
InputPath=.\FILLERS\SCALE.ASM

"scale.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	wasm -fp5 Fillers\scale.asm

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FILLERS\SCALE64.ASM

!IF  "$(CFG)" == "FDS - Win32 Release"

# Begin Custom Build
InputPath=.\FILLERS\SCALE64.ASM

"scale64.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	wasm -fp5 Fillers\scale64.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "FDS - Win32 Debug"

# Begin Custom Build
InputPath=.\FILLERS\SCALE64.ASM

"scale64.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	wasm -fp5 Fillers\scale64.asm

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FILLERS\SCALEMMX.ASM

!IF  "$(CFG)" == "FDS - Win32 Release"

# Begin Custom Build
InputPath=.\FILLERS\SCALEMMX.ASM

"scalemmx.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	wasm -fp5 Fillers\scalemmx.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "FDS - Win32 Debug"

# Begin Custom Build
InputPath=.\FILLERS\SCALEMMX.ASM

"scalemmx.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	wasm -fp5  -d1 Fillers\scalemmx.asm

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FILLERS\SclMMX32.asm

!IF  "$(CFG)" == "FDS - Win32 Release"

# Begin Custom Build
InputPath=.\FILLERS\SclMMX32.asm

"sclmmx32.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	wasm -fp5 Fillers\sclmmx32.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "FDS - Win32 Debug"

# Begin Custom Build
InputPath=.\FILLERS\SclMMX32.asm

"sclmmx32.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	wasm -fp5  -d1 Fillers\sclmmx32.asm

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FILLERS\TEX.CPP
# End Source File
# Begin Source File

SOURCE=.\FILLERS\TEXTGOUR.ASM

!IF  "$(CFG)" == "FDS - Win32 Release"

# Begin Custom Build
InputPath=.\FILLERS\TEXTGOUR.ASM

"textgour.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	wasm -fp5 Fillers\textgour.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "FDS - Win32 Debug"

# Begin Custom Build
InputPath=.\FILLERS\TEXTGOUR.ASM

"textgour.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	wasm -fp5 Fillers\textgour.asm

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FILLERS\TRNSGOUR.ASM

!IF  "$(CFG)" == "FDS - Win32 Release"

# Begin Custom Build
InputPath=.\FILLERS\TRNSGOUR.ASM

"trnsgour.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	wasm -fp5 Fillers\trnsgour.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "FDS - Win32 Debug"

# Begin Custom Build
InputPath=.\FILLERS\TRNSGOUR.ASM

"trnsgour.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	wasm -fp5 Fillers\trnsgour.asm

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FILLERS\ZSCALE.ASM

!IF  "$(CFG)" == "FDS - Win32 Release"

# Begin Custom Build
InputPath=.\FILLERS\ZSCALE.ASM

"zscale.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	wasm -fp5 Fillers\zscale.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "FDS - Win32 Debug"

# Begin Custom Build
InputPath=.\FILLERS\ZSCALE.ASM

"zscale.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	wasm -fp5 Fillers\zscale.asm

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "FLD"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FLD\FLD_CONV.CPP
# End Source File
# Begin Source File

SOURCE=.\FLD\FLD_MAT.CPP
# End Source File
# Begin Source File

SOURCE=.\FLD\FLD_READ.CPP
# End Source File
# Begin Source File

SOURCE=.\FLD\FLD_READ.H
# End Source File
# Begin Source File

SOURCE=.\FLD\LWREAD.H
# End Source File
# End Group
# Begin Group "FRUSTRUM"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FRUSTRUM\FRUSTRUM.CPP
# End Source File
# End Group
# Begin Group "IMGCODE"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\IMGCODE\IMGCODE.CPP
# End Source File
# Begin Source File

SOURCE=.\IMGCODE\QUANTUM.H
# End Source File
# End Group
# Begin Group "IMGGENR"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\IMGGENR\IMGGENR.CPP
# End Source File
# Begin Source File

SOURCE=.\IMGGENR\WOBTR.ASM

!IF  "$(CFG)" == "FDS - Win32 Release"

# Begin Custom Build
InputPath=.\IMGGENR\WOBTR.ASM

"wobtr.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	wasm -fp5  Imggenr\wobtr.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "FDS - Win32 Debug"

# Begin Custom Build
InputPath=.\IMGGENR\WOBTR.ASM

"wobtr.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	wasm -fp5  Imggenr\wobtr.asm

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IMGGENR\WOBTR320.ASM

!IF  "$(CFG)" == "FDS - Win32 Release"

# Begin Custom Build
InputPath=.\IMGGENR\WOBTR320.ASM

"wobtr320.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	wasm -fp5  Imggenr\wobtr320.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "FDS - Win32 Debug"

# Begin Custom Build
InputPath=.\IMGGENR\WOBTR320.ASM

"wobtr320.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	wasm -fp5  Imggenr\wobtr320.asm

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IMGGENR\WOBTRMMX.asm
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\IMGGENR\WOBTRnew.asm

!IF  "$(CFG)" == "FDS - Win32 Release"

# Begin Custom Build
InputPath=.\IMGGENR\WOBTRnew.asm

"wobtrnew.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ml /c /Cp /Cx /coff /Fl /nologo /Sa /Sc Imggenr\wobtrnew.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "FDS - Win32 Debug"

# Begin Custom Build
InputPath=.\IMGGENR\WOBTRnew.asm
InputName=WOBTRnew

"$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ml /c /Cp /Cx /Fl /coff /nologo /Zd /Zf /Zi $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IMGGENR\WOBTRXXX.ASM

!IF  "$(CFG)" == "FDS - Win32 Release"

# Begin Custom Build
InputPath=.\IMGGENR\WOBTRXXX.ASM

"wobtrXXX.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	wasm -fp5  Imggenr\wobtrxxx.asm

# End Custom Build

!ELSEIF  "$(CFG)" == "FDS - Win32 Debug"

# Begin Custom Build
InputPath=.\IMGGENR\WOBTRXXX.ASM

"wobtrXXX.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	wasm -fp5  Imggenr\wobtrxxx.asm -d1

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "IMGPROC"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\IMGPROC\IMGPROC.CPP
# End Source File
# End Group
# Begin Group "MATH"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MATH\BASEMATH.H
# End Source File
# Begin Source File

SOURCE=.\MATH\MATH.CPP
# End Source File
# End Group
# Begin Group "MISC"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MISC\Memmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\MISC\mmreg.inl
# End Source File
# Begin Source File

SOURCE=.\MISC\MOUSE.CPP
# End Source File
# Begin Source File

SOURCE=.\MISC\PREPROC.CPP
# End Source File
# Begin Source File

SOURCE=.\MISC\TABLES.CPP
# End Source File
# Begin Source File

SOURCE=.\MISC\TxtrLib.cpp
# End Source File
# End Group
# Begin Group "MIDAS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MDSPLAY\MDSPLAY.CPP
# End Source File
# Begin Source File

SOURCE=.\MDSPLAY\MIDASDLL.H
# End Source File
# End Group
# Begin Group "PCLSYS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\PCLSYS\PCLSYS.CPP
# End Source File
# End Group
# Begin Group "RADIO"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\RADIO\RADIO.CPP
# End Source File
# End Group
# Begin Group "RENDER"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\RENDER\GENERAL.H
# End Source File
# Begin Source File

SOURCE=.\RENDER\PackedVars.ASM

!IF  "$(CFG)" == "FDS - Win32 Release"

# Begin Custom Build
InputPath=.\RENDER\PackedVars.ASM
InputName=PackedVars

"$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ml /c /Cp /Cx /coff /Fl /nologo /Sa /Sc $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "FDS - Win32 Debug"

# Begin Custom Build
InputPath=.\RENDER\PackedVars.ASM
InputName=PackedVars

"$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	ml /c /Cp /Cx /coff /Fl /nologo /Sa /Sc $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RENDER\RENDER.CPP
# End Source File
# Begin Source File

SOURCE=.\RENDER\SORTS.H
# End Source File
# Begin Source File

SOURCE=.\RENDER\SQRTTBL.H
# End Source File
# End Group
# Begin Group "V3D"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\V3D\M_DEFS.H
# End Source File
# Begin Source File

SOURCE=.\V3D\OBJECTS.H
# End Source File
# Begin Source File

SOURCE=.\V3D\V3D_READ.CPP
# End Source File
# End Group
# Begin Group "VESA"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\VESA\VESA.CPP
# End Source File
# Begin Source File

SOURCE=.\VESA\VESA.H
# End Source File
# End Group
# Begin Group "ISR"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ISR\ISR.CPP
# End Source File
# End Group
# Begin Group "DPMI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DPMI\DPMI.CPP
# End Source File
# Begin Source File

SOURCE=.\DPMI\DPMI.H
# End Source File
# End Group
# Begin Group "Skycube"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SkyCube\SkyCube.cpp
# End Source File
# Begin Source File

SOURCE=.\SkyCube\SkyCube.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\FDS_DECS.H
# End Source File
# Begin Source File

SOURCE=.\FDS_DEFS.H
# End Source File
# Begin Source File

SOURCE=.\FDS_VARS.H
# End Source File
# End Target
# End Project
