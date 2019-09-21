# Microsoft Developer Studio Project File - Name="Qoole" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Qoole - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Qoole.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Qoole.mak" CFG="Qoole - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Qoole - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Qoole - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/QooleMFC/Qoole", KAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Qoole - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "." /I ".res" /I "basic" /I "objects" /I "render" /I "interface" /I "interface2" /I "qoole" /I "res" /I "game" /I "res/mgl" /I "interface\glide3" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "__MSC__" /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc" /out:"exec/Qoole99.exe" /libpath:"lib" /libpath:"interface"
# SUBTRACT LINK32 /pdb:none /debug

!ELSEIF  "$(CFG)" == "Qoole - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GX /ZI /Od /I "." /I "basic" /I "objects" /I "render" /I "game" /I "interface" /I "interface2" /I "qoole" /I "res" /I "interface\glide3" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "__MSC__" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 mgl.lib winmm.lib glide3x.lib opengl32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc" /nodefaultlib:"LIBCMT" /out:"Debug/Qoole99.exe" /pdbtype:sept /libpath:"interface" /libpath:"lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "Qoole - Win32 Release"
# Name "Qoole - Win32 Debug"
# Begin Group "Qoole"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Qoole\Qoole.cpp
# End Source File
# Begin Source File

SOURCE=.\Qoole\Qoole.h
# End Source File
# Begin Source File

SOURCE=.\Qoole\QooleDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\Qoole\QooleDoc.h
# End Source File
# Begin Source File

SOURCE=.\Qoole\QooleOp.cpp
# End Source File
# Begin Source File

SOURCE=.\Qoole\QooleOp.h
# End Source File
# Begin Source File

SOURCE=.\Qoole\QVersion.h
# End Source File
# End Group
# Begin Group "Interface"

# PROP Default_Filter ""
# Begin Group "QConfigs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Interface\QConf2DV.cpp

!IF  "$(CFG)" == "Qoole - Win32 Release"

# ADD CPP /I ".."

!ELSEIF  "$(CFG)" == "Qoole - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Interface\QConf2DV.h
# End Source File
# Begin Source File

SOURCE=.\Interface\QConf3DV.cpp

!IF  "$(CFG)" == "Qoole - Win32 Release"

# ADD CPP /I ".."

!ELSEIF  "$(CFG)" == "Qoole - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Interface\QConf3DV.h
# End Source File
# Begin Source File

SOURCE=.\Interface\QConfDrw.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\QConfDrw.h
# End Source File
# Begin Source File

SOURCE=.\Interface\QConfGen.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\Interface\QConfGen.h
# End Source File
# Begin Source File

SOURCE=.\Interface\QConfGms.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\QConfGms.h
# End Source File
# Begin Source File

SOURCE=.\Interface\QConfSht.cpp

!IF  "$(CFG)" == "Qoole - Win32 Release"

# ADD CPP /I ".."

!ELSEIF  "$(CFG)" == "Qoole - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Interface\QConfSht.h
# End Source File
# End Group
# Begin Group "QProperties"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Interface\QPropCnt.cpp

!IF  "$(CFG)" == "Qoole - Win32 Release"

# ADD CPP /I ".."

!ELSEIF  "$(CFG)" == "Qoole - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Interface\QPropCnt.h
# End Source File
# Begin Source File

SOURCE=.\Interface\QPropEnt.cpp

!IF  "$(CFG)" == "Qoole - Win32 Release"

# ADD CPP /I ".."

!ELSEIF  "$(CFG)" == "Qoole - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Interface\QPropEnt.h
# End Source File
# Begin Source File

SOURCE=.\Interface\QPropLst.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\Interface\QPropLst.h
# End Source File
# Begin Source File

SOURCE=.\Interface\QPropMsc.cpp

!IF  "$(CFG)" == "Qoole - Win32 Release"

# ADD CPP /I ".."

!ELSEIF  "$(CFG)" == "Qoole - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Interface\QPropMsc.h
# End Source File
# Begin Source File

SOURCE=.\Interface\QPropPg.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\QPropPg.h
# End Source File
# Begin Source File

SOURCE=.\Interface\QPropSht.cpp

!IF  "$(CFG)" == "Qoole - Win32 Release"

# ADD CPP /I ".."

!ELSEIF  "$(CFG)" == "Qoole - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Interface\QPropSht.h
# End Source File
# Begin Source File

SOURCE=.\Interface\QPropSrf.cpp

!IF  "$(CFG)" == "Qoole - Win32 Release"

# ADD CPP /I ".."

!ELSEIF  "$(CFG)" == "Qoole - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Interface\QPropSrf.h
# End Source File
# Begin Source File

SOURCE=.\Interface\QPropTex.cpp

!IF  "$(CFG)" == "Qoole - Win32 Release"

# ADD CPP /I ".."

!ELSEIF  "$(CFG)" == "Qoole - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Interface\QPropTex.h
# End Source File
# End Group
# Begin Group "QDraw"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Interface\QDraw.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\QDraw.h
# End Source File
# Begin Source File

SOURCE=.\Interface\QDrawDX.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\QDrawDX.h
# End Source File
# Begin Source File

SOURCE=.\Interface\QDrawGld.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\QDrawGld.h
# End Source File
# Begin Source File

SOURCE=.\Interface\QDrawMFC.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\QDrawMFC.h
# End Source File
# Begin Source File

SOURCE=.\Interface\QDrawMGL.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\QDrawMGL.h
# End Source File
# Begin Source File

SOURCE=.\Interface\QDrawOGL.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\QDrawOGL.h
# End Source File
# Begin Source File

SOURCE=.\Interface\QDrawVir.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Interface\QColrWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\QColrWnd.h
# End Source File
# Begin Source File

SOURCE=.\Interface\QCompile.cpp

!IF  "$(CFG)" == "Qoole - Win32 Release"

# ADD CPP /I ".."

!ELSEIF  "$(CFG)" == "Qoole - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Interface\QCompile.h
# End Source File
# Begin Source File

SOURCE=.\Interface\QCtrlBar.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\QCtrlBar.h
# End Source File
# Begin Source File

SOURCE=.\Interface\QEntSel.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\QEntSel.h
# End Source File
# Begin Source File

SOURCE=.\Interface\QMainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\QMainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Interface\QPrefbVw.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\Interface\QPrefbVw.h
# End Source File
# Begin Source File

SOURCE=.\Interface\QPrefMgr.cpp

!IF  "$(CFG)" == "Qoole - Win32 Release"

# ADD CPP /I ".."

!ELSEIF  "$(CFG)" == "Qoole - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Interface\QPrefMgr.h
# End Source File
# Begin Source File

SOURCE=.\Interface\QProcWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\QProcWnd.h
# End Source File
# Begin Source File

SOURCE=.\Interface\QSetColor.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\QSetColor.h
# End Source File
# Begin Source File

SOURCE=.\Interface\QTexDraw.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\QTexDraw.h
# End Source File
# Begin Source File

SOURCE=.\Interface\QTexMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\QTexMgr.h
# End Source File
# Begin Source File

SOURCE=.\Interface\QTexRep.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\QTexRep.h
# End Source File
# Begin Source File

SOURCE=.\Interface\QTexView.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\QTexView.h
# End Source File
# Begin Source File

SOURCE=.\Interface\QTrView.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\QTrView.h
# End Source File
# Begin Source File

SOURCE=.\Interface\QVFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\QVFrame.h
# End Source File
# Begin Source File

SOURCE=.\Interface\QView.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface\QView.h
# End Source File
# End Group
# Begin Group "Interface2"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Interface2\CreditStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface2\CreditStatic.h
# End Source File
# Begin Source File

SOURCE=.\Interface2\DirDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface2\DirDialog.h
# End Source File
# Begin Source File

SOURCE=.\Interface2\dragdock.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface2\framewnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface2\mdifloat.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface2\mrcext.h
# End Source File
# Begin Source File

SOURCE=.\Interface2\mrcext.rc1
# End Source File
# Begin Source File

SOURCE=.\Interface2\mrcpriv.h
# End Source File
# Begin Source File

SOURCE=.\Interface2\mrcresrc.h
# End Source File
# Begin Source File

SOURCE=.\Interface2\mrcstafx.h
# End Source File
# Begin Source File

SOURCE=.\Interface2\ProgressWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface2\ProgressWnd.h
# End Source File
# Begin Source File

SOURCE=.\Interface2\sizecont.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface2\sizedock.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface2\ToolBarEx.cpp
# End Source File
# Begin Source File

SOURCE=.\Interface2\ToolBarEx.h
# End Source File
# End Group
# Begin Group "Games"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Game\HalfLife.cpp
# End Source File
# Begin Source File

SOURCE=.\Game\HalfLife.h
# End Source File
# Begin Source File

SOURCE=.\Game\Heretic2.cpp
# End Source File
# Begin Source File

SOURCE=.\Game\Heretic2.h
# End Source File
# Begin Source File

SOURCE=.\Game\Hexen2.cpp
# End Source File
# Begin Source File

SOURCE=.\Game\Hexen2.h
# End Source File
# Begin Source File

SOURCE=.\Game\MapIO.cpp
# End Source File
# Begin Source File

SOURCE=.\Game\Mapio.h
# End Source File
# Begin Source File

SOURCE=.\Game\paks2wad.cpp
# End Source File
# Begin Source File

SOURCE=.\Game\Quake.cpp
# End Source File
# Begin Source File

SOURCE=.\Game\Quake.h
# End Source File
# Begin Source File

SOURCE=.\Game\Quake2.cpp
# End Source File
# Begin Source File

SOURCE=.\Game\Quake2.h
# End Source File
# Begin Source File

SOURCE=.\Game\Sin.cpp
# End Source File
# Begin Source File

SOURCE=.\Game\Sin.h
# End Source File
# End Group
# Begin Group "Render"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Render\Clip.cpp
# End Source File
# Begin Source File

SOURCE=.\Render\Clip.h
# End Source File
# Begin Source File

SOURCE=.\Render\Selector.cpp
# End Source File
# Begin Source File

SOURCE=.\Render\Selector.h
# End Source File
# Begin Source File

SOURCE=.\Render\View.cpp
# End Source File
# Begin Source File

SOURCE=.\Render\View.h
# End Source File
# End Group
# Begin Group "Objects"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Objects\Entity.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Entity.h
# End Source File
# Begin Source File

SOURCE=.\Objects\EntList.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\EntList.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Game.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Game.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Geometry.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Geometry.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Objects.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Objects.h
# End Source File
# Begin Source File

SOURCE=.\Objects\Texture.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\Texture.h
# End Source File
# End Group
# Begin Group "Basic"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Basic\LCommon.cpp
# End Source File
# Begin Source File

SOURCE=.\Basic\LCommon.h
# End Source File
# Begin Source File

SOURCE=.\Basic\List.h
# End Source File
# Begin Source File

SOURCE=.\Basic\Math3D.cpp

!IF  "$(CFG)" == "Qoole - Win32 Release"

!ELSEIF  "$(CFG)" == "Qoole - Win32 Debug"

# ADD CPP /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Basic\Math3D.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\pviewico.bmp
# End Source File
# Begin Source File

SOURCE=.\res\qcredits.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Qoole.ico
# End Source File
# Begin Source File

SOURCE=.\Qoole.rc
# End Source File
# Begin Source File

SOURCE=.\Qoole.rc2
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\res\Scissors.bmp
# End Source File
# Begin Source File

SOURCE=.\res\StdAfx.cpp
# ADD CPP /Yc
# End Source File
# Begin Source File

SOURCE=.\res\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\res\tbalign.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tbcmpl.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tbedit.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tbfile.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tbgroup.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tbguide.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tbmain.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tbmode.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tbrendr.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tbwnds.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tdown1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Timer.h
# End Source File
# Begin Source File

SOURCE=.\res\tleft1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tright1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tup1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tviewico.bmp
# End Source File
# Begin Source File

SOURCE=.\res\workspce.bmp
# End Source File
# End Group
# End Target
# End Project
