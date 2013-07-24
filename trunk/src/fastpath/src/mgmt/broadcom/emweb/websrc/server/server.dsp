# Microsoft Developer Studio Project File - Name="server" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=server - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "server.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "server.mak" CFG="server - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "server - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "server - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
# PROP WCE_Configuration ""
# PROP WCE_FormatVersion ""
CPP=cl.exe

!IF  "$(CFG)" == "server - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\obj.x86-pc-win-vcc\release"
# PROP Intermediate_Dir "..\..\obj.x86-pc-win-vcc\release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\config" /I "..\include" /I "..\..\lib" /I "..\..\obj.x86-pc-win-vcc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "server - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\obj.x86-pc-win-vcc\debug"
# PROP Intermediate_Dir "..\..\obj.x86-pc-win-vcc\debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /I "..\..\config" /I "..\include" /I "..\..\lib" /I "..\..\obj.x86-pc-win-vcc" /D "WIN32" /D "_WINDOWS" /D "_DEBUG" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "server - Win32 Release"
# Name "server - Win32 Debug"
# Begin Source File

SOURCE=.\ews.c
# End Source File
# Begin Source File

SOURCE=.\ews_auth.c
# End Source File
# Begin Source File

SOURCE=.\ews_cgi.c
# End Source File
# Begin Source File

SOURCE=.\ews_ctxt.c
# End Source File
# Begin Source File

SOURCE=.\ews_dist.c
# End Source File
# Begin Source File

SOURCE=.\ews_doc.c
# End Source File
# Begin Source File

SOURCE=.\ews_file.c
# End Source File
# Begin Source File

SOURCE=.\ews_form.c
# End Source File
# Begin Source File

SOURCE=.\ews_net.c
# End Source File
# Begin Source File

SOURCE=.\ews_pars.c
# End Source File
# Begin Source File

SOURCE=.\ews_send.c
# End Source File
# Begin Source File

SOURCE=.\ews_serv.c
# End Source File
# Begin Source File

SOURCE=.\ews_skel.c
# End Source File
# Begin Source File

SOURCE=.\ews_str.c
# End Source File
# Begin Source File

SOURCE=.\ews_tah.c
# End Source File
# Begin Source File

SOURCE=.\ews_urlhooks.c
# End Source File
# Begin Source File

SOURCE=.\ews_xml.c
# End Source File
# End Target
# End Project





