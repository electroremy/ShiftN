!IFNDEF _WIN32_MAK_
_WIN32_MAK_ = 1
!IF "$(PROCESSOR_ARCHITECTURE)" == ""
CPU=i386
PROCESSOR_ARCHITECTURE=x86
!endif
!IF !DEFINED(CPU) || "$(CPU)" == ""
CPU = $(PROCESSOR_ARCHITECTURE)
!ENDIF # CPU
!IF ( "$(CPU)" == "X86" ) || ( "$(CPU)" == "x86" )
CPU = i386
!ENDIF # CPU == X86
!IF "$(CPU)" != "i386"
!IF "$(CPU)" != "IA64"
!IF "$(CPU)" != "AMD64"
!ERROR  Must specify CPU environment variable ( CPU=i386, CPU=IA64, CPU=AMD64)
!ENDIF
!ENDIF
!ENDIF
!IFNDEF TARGETOS
TARGETOS = WINNT
!ENDIF
!IF "$(TARGETOS)" != "WINNT"
!IF "$(TARGETOS)" != "WIN95"
!IF "$(TARGETOS)" != "BOTH"
!ERROR Must specify TARGETOS environment variable (BOTH, WIN95, WINNT)
!ENDIF
!ENDIF
!ENDIF
!IFNDEF APPVER
APPVER = 5.0
!ENDIF
!IF "$(APPVER)" != "6.1"
!IF "$(APPVER)" != "6.0"
!IF "$(APPVER)" != "5.02"
!IF "$(APPVER)" != "5.01"
!IF "$(APPVER)" != "5.0"
!IF "$(APPVER)" != "4.0"
!ERROR Must specify APPVER environment variable (4.0, 5.0, 5.01, 5.02, 6.0, 6.1)
!ENDIF
!ENDIF
!ENDIF
!ENDIF
!ENDIF
!ENDIF
!IF "$(APPVER)" =="6.1"
!IFNDEF _WIN32_IE
_WIN32_IE = 0x0800
!ENDIF # _WIN32_IE
!ENDIF # APPVER == 6.1
!IF "$(APPVER)" =="6.0"
!IFNDEF _WIN32_IE
_WIN32_IE = 0x0700
!ENDIF # _WIN32_IE
!ENDIF # APPVER == 6.0
!IF "$(APPVER)" =="5.0"
!IFNDEF _WIN32_IE
_WIN32_IE = 0x0500
!ENDIF # _WIN32_IE
!ENDIF # APPVER == 5.0
!IF "$(APPVER)" =="5.01"
!IFNDEF _WIN32_IE
_WIN32_IE = 0x0600
!ENDIF # _WIN32_IE
!ENDIF # APPVER == 5.01
!IF "$(APPVER)" =="5.02"
!IFNDEF _WIN32_IE
_WIN32_IE = 0x0600
!ENDIF # _WIN32_IE
!ENDIF # APPVER == 5.02
!IFNDEF _WIN32_IE
_WIN32_IE = 0x0400
!ENDIF
cc     = cl
link   = link
implib = lib
midl   = midl
rc     = Rc
hc     = Start /Wait Hcrtf
mc     = Mc
hcvars = -xn
ccommon = -c -DCRTAPI1=_cdecl -DCRTAPI2=_cdecl -nologo -GS
!IFDEF SEHMAP
ccommon = $(ccommon) -FIsehmap.h
!ENDIF
!IF "$(TARGETLANG)" == "LANG_JAPANESE"
ccommon = $(ccommon) -DJAPAN -DDBCS -DFE_IME
!ENDIF
!IF "$(TARGETLANG)" == "LANG_CHINESE"
ccommon = $(ccommon) -DDBCS -DFE_IME
!ENDIF
!IF "$(TARGETLANG)" == "LANG_KOREAN"
ccommon = $(ccommon) -DDBCS -DFE_IME
!ENDIF
!IF "$(CPU)" == "i386"
cflags = $(ccommon) -D_X86_=1  -DWIN32 -D_WIN32 -W3
scall  = -Gz
!ELSEIF "$(CPU)" == "IA64"
cflags = $(ccommon) -D_IA64_=1 -DWIN64 -D_WIN64  -DWIN32 -D_WIN32
cflags = $(cflags) -W4
scall  =
!ELSEIF "$(CPU)" == "AMD64"
cflags = $(ccommon) -D_AMD64_=1 -DWIN64 -D_WIN64  -DWIN32 -D_WIN32
cflags = $(cflags) -W4
scall  =
!ENDIF
!IF "$(APPVER)" == "4.0"
NMAKE_WINVER = 0x0400
!ELSEIF "$(APPVER)" == "5.0"
NMAKE_WINVER = 0x0500
!ELSEIF "$(APPVER)" == "5.01"
NMAKE_WINVER = 0x0501
!ELSEIF "$(APPVER)" == "5.02"
NMAKE_WINVER = 0x0502
!ELSEIF "$(APPVER)" == "6.0"
NMAKE_WINVER = 0x0600
!ELSEIF "$(APPVER)" == "6.1"
NMAKE_WINVER = 0x0601
!ENDIF
!IF "$(TARGETOS)" == "WINNT"
cflags = $(cflags) -D_WINNT -D_WIN32_WINNT=$(NMAKE_WINVER) -DNTDDI_VERSION=$(NMAKE_WINVER)0000
!ENDIF
!IF "$(TARGETOS)" == "WIN95"
cflags = $(cflags) -D_WIN95 -D_WIN32_WINDOWS=$(NMAKE_WINVER) /D_WIN32_DCOM
!ENDIF
cflags = $(cflags) -D_WIN32_IE=$(_WIN32_IE) -DWINVER=$(NMAKE_WINVER)
!IF "$(CPU)" != "IA64"
!IFDEF NODEBUG
cdebug = -Ox -DNDEBUG 
!ELSE IFDEF PROFILE
cdebug = -Gh -Ox -DNDEBUG
!ELSE IFDEF TUNE
cdebug = -Gh -Ox -DNDEBUG
!ELSE
cdebug = -Zi -Od -DDEBUG
!ENDIF
!ELSE
!IFDEF NODEBUG
cdebug = -Ox -DNDEBUG
!ELSE
cdebug = -Zi -Od -DDEBUG
!ENDIF
!ENDIF
!IFDEF NO_ANSI
noansi = -DNULL=0
!ENDIF
!IFDEF NODEBUG
cvarsmt    = $(noansi) -D_MT -MT
cvars      = $(cvarsmt)
cvarsdll   = $(noansi) -D_MT -D_DLL -MD
!ELSE
cvarsmt    = $(noansi) -D_MT -MTd
cvars      = $(cvarsmt)
cvarsdll   = $(noansi) -D_MT -D_DLL -MDd
!ENDIF
cvarsmtdll = $(cvarsdll)
psxvars    = -D_POSIX_
rcflags = /r
!ifdef NODEBUG
rcvars = -DWIN32 -D_WIN32 -DWINVER=$(NMAKE_WINVER) $(noansi)
!else
rcvars = -DWIN32 -D_WIN32 -DWINVER=$(NMAKE_WINVER) -DDEBUG -D_DEBUG $(noansi)
!endif
!IF "$(TARGETLANG)" == "LANG_JAPANESE"
rcflags = $(rcflags) /c932
rcvars = $(rcvars) -DJAPAN -DDBCS -DFE_IME
!ENDIF
!IF "$(TARGETLANG)" == "LANG_CHINESE"
rcvars = $(rcvars) -DDBCS -DFE_IME
!ENDIF
!IF "$(TARGETLANG)" == "LANG_KOREAN"
rcvars = $(rcvars) -DDBCS -DFE_IME
!ENDIF
!IF "$(TARGETOS)" == "WIN95"
MIDL_OPTIMIZATION=-target NT40
!ELSEIF "$(TARGETOS)" == "WINNT"
!IF "$(APPVER)" == "5.0"
MIDL_OPTIMIZATION=-target NT50
!ELSEIF "$(APPVER)" == "6.0"
MIDL_OPTIMIZATION=-target NT60
!ELSEIF "$(APPVER)" == "6.1"
MIDL_OPTIMIZATION=-target NT61
!ELSEIF "$(APPVER)" == "5.01"
MIDL_OPTIMIZATION=-target NT51
!ELSEIF "$(APPVER)" == "5.02"
MIDL_OPTIMIZATION=-target NT51
!ELSEIF "$(APPVER)" == "4.0"
MIDL_OPTIMIZATION=-target NT40
!ENDIF
!ENDIF
!IF "$(CPU)" == "IA64"
MIDL_OPTIMIZATION = $(MIDL_OPTIMIZATION) /ia64
!ELSEIF "$(CPU)" == "AMD64"
MIDL_OPTIMIZATION = $(MIDL_OPTIMIZATION) /x64
!ELSE
MIDL_OPTIMIZATION = $(MIDL_OPTIMIZATION) /win32
!ENDIF
!IF ("$(TARGETOS)" == "WINNT" ) && ("$(APPVER)" != "4.0")
MIDL_OPTIMIZATION = $(MIDL_OPTIMIZATION) /robust
!ENDIF
lflags  = $(lflags) /INCREMENTAL:NO /NOLOGO
!IF "$(CPU)" == "i386"
DLLENTRY = @12
!ENDIF
!IF "$(CPU)" == "IA64"
DLLENTRY =
!ENDIF
!IF "$(CPU)" == "AMD64"
DLLENTRY =
!ENDIF
!IF "$(CPU)" == "i386"
!IFDEF NODEBUG
ldebug = /RELEASE
!ELSE
ldebug = /DEBUG /DEBUGTYPE:cv
!ENDIF
!ELSE
!IFDEF NODEBUG
ldebug = /RELEASE
!ELSE IFDEF PROFILE
ldebug = /DEBUG:mapped,partial /DEBUGTYPE:coff
!ELSE IFDEF TUNE
ldebug = /DEBUG:mapped,partial /DEBUGTYPE:coff
!ELSE
ldebug = /DEBUG /DEBUGTYPE:cv
!ENDIF
!ENDIF
linkdebug = $(ldebug)
!IF ("$(APPVER)" == "5.0") && (("$(TARGETOS)" == "BOTH") || ("$(TARGETOS)" == "WIN95"))
EXEVER = 4.10
!ELSE
EXEVER = $(APPVER)
!ENDIF
conlflags = $(lflags) -subsystem:console,$(EXEVER)
guilflags = $(lflags) -subsystem:windows,$(EXEVER)
dlllflags = $(lflags) -entry:_DllMainCRTStartup$(DLLENTRY) -dll
!IF "$(CPU)" == "i386"
savlflags = $(lflags) -subsystem:windows,$(EXEVER) -entry:WinMainCRTStartup
!ELSE
savlflags = $(lflags) -subsystem:windows,$(EXEVER) -entry:mainCRTStartup
!ENDIF
psxlflags = $(lflags) -subsystem:posix -entry:__PosixProcessStartup
conflags  = $(conlflags)
guiflags  = $(guilflags)
psxflags  = $(psxlflags)
psxlibs    = libcpsx.lib psxdll.lib psxrtl.lib oldnames.lib
!IF "$(CPU)" != "IA64"
!IFDEF PROFILE
optlibs =  cap.lib
!ELSE IFDEF TUNE
optlibs = wst.lib
!ELSE
optlibs =
!ENDIF
!ELSE
optlibs =
!ENDIF
!IF "$(TARGETOS)" == "WIN95"
!IF "$(APPVER)" == "4.0"
winsocklibs = wsock32.lib
!ELSE
winsocklibs = ws2_32.lib mswsock.lib
!ENDIF
!ELSE
winsocklibs = ws2_32.lib mswsock.lib
!ENDIF
baselibs    = kernel32.lib $(optlibs) $(winsocklibs) advapi32.lib
winlibs     = $(baselibs) user32.lib gdi32.lib comdlg32.lib winspool.lib
conlibs     = $(baselibs)
guilibs     = $(winlibs)
olelibs     = ole32.lib uuid.lib oleaut32.lib $(guilibs)
conlibsmt   = $(conlibs)
conlibsdll  = $(conlibs)
guilibsmt   = $(guilibs)
guilibsdll  = $(guilibs)
olelibsmt   = $(olelibs)
olelibsdll  = $(olelibs)
ole2libs    = $(olelibs)
ole2libsmt  = $(olelibsmt)
ole2libsdll = $(olelibsdll)
bc     = vb6
bc_exe = /Make
bc_dll = /Makedll
!IF ("$(APPVER)" == "6.1") 
OUTDIR=WIN7
!ELSEIF ("$(APPVER)" == "6.0") 
OUTDIR=Vista
!ELSEIF "$(APPVER)" == "5.0"
OUTDIR=WIN2000
!ELSEIF "$(APPVER)" == "5.01" 
OUTDIR=XP32
!ELSEIF "$(APPVER)" == "5.02"
OUTDIR=SRV2003
!ELSEIF "$(APPVER)" == "4.0"
OUTDIR=NT4
!ENDIF
!IF "$(CPU)" == "AMD64"
OUTDIR=$(OUTDIR)_X64
!ELSEIF "$(CPU)" == "IA64"
OUTDIR=$(OUTDIR)_64
!ENDIF
!IF "$(SDKPRERELEASE)" == "1"
OUTDIR=PRE_$(OUTDIR)
!ENDIF
!IF "$(NODEBUG)" == ""
OUTDIR=$(OUTDIR)_DEBUG
!ELSE
OUTDIR=$(OUTDIR)_RETAIL
!ENDIF
!IF "$(OS)" == "Windows_NT"
CLEANUP=if exist $(OUTDIR)/$(NULL) rd /s /q $(OUTDIR)
!ELSE
CLEANUP=deltree /y $(OUTDIR)
!ENDIF
VC6MSG=Microsoft Visual C++ 6.0.
WIN64MSG=not supported on 64 bit.
!ENDIF
