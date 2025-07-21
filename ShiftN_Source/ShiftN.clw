; CLW-Datei enthält Informationen für den MFC-Klassen-Assistenten

[General Info]
Version=1
LastClass=CMainFrame
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "ShiftN.h"
LastPage=0

ClassCount=18
Class1=CShiftNApp
Class2=CShiftNDoc
Class3=CShiftNView
Class4=CMainFrame

ResourceCount=36
Resource1=IDR_MAINFRAME (Englisch (USA))
Class5=CAboutDlg
Resource2=IDD_DIALOG4 (Englisch (USA))
Class6=Ladefehler_Dialog
Resource3=IDD_DIALOG10
Class7=Keine_Ergebnisse
Resource4=IDR_MAINFRAME (Neutral)
Class8=Kein_Bild
Resource5=CG_IDD_PROGRESS
Class9=DarstDlg
Resource6=IDD_DIALOG3
Class10=SprachDlg
Resource7=IDD_DIALOG6
Class11=ParamDlg
Resource8=IDR_ACCELERATOR1
Class12=AdjustDlg
Resource9=IDD_DIALOG12
Class13=abspDlg
Resource10=IDD_DIALOG9 (Englisch (USA))
Resource11=IDD_ABOUTBOX (Englisch (USA))
Resource12=IDD_DIALOG6 (Englisch (USA))
Class14=CProgressDlg
Resource13=IDD_DIALOG1
Resource14=CG_IDD_MYDIALOGBAR_SAVE
Resource15=IDR_MAINFRAME
Resource16=IDD_DIALOG7 (Englisch (USA))
Resource17=IDD_ABOUTBOX (Deutsch (Deutschland))
Resource18=IDD_DIALOG3 (Englisch (USA))
Resource19=IDD_DIALOG5 (Englisch (USA))
Resource20=CG_IDD_MYDIALOGBAR_SAVE (Englisch (USA))
Resource21=CG_IDD_MYDIALOGBAR (Englisch (USA))
Resource22=IDD_DIALOG2
Resource23=IDD_DIALOG1 (Englisch (USA))
Resource24=IDD_DIALOG8
Resource25=CG_IDD_PROGRESS (Englisch (USA))
Class15=CSplashWnd
Resource26=IDD_DIALOG2 (Englisch (USA))
Class16=Toolbar
Resource27=IDD_DIALOG11 (Englisch (USA))
Resource28=IDD_DIALOG8 (Englisch (USA))
Resource29=IDD_DIALOG10 (Englisch (USA))
Class17=Hinweise
Resource30=IDD_DIALOG11
Resource31=IDD_DIALOG12 (Englisch (USA))
Resource32=IDD_DIALOG4
Resource33=IDD_ABOUTBOX
Class18=SchaltflDlg
Resource34=IDD_DIALOG13
Resource35=CG_IDD_MYDIALOGBAR
Resource36=IDD_DIALOG13 (Englisch (USA))

[CLS:CShiftNApp]
Type=0
HeaderFile=ShiftN.h
ImplementationFile=ShiftN.cpp
Filter=N
LastObject=CG_ID_VIEW_MYDIALOGBAR
BaseClass=CWinApp
VirtualFilter=AC

[CLS:CShiftNDoc]
Type=0
HeaderFile=ShiftNDoc.h
ImplementationFile=ShiftNDoc.cpp
Filter=N

[CLS:CShiftNView]
Type=0
HeaderFile=ShiftNView.h
ImplementationFile=ShiftNView.cpp
Filter=C
BaseClass=CView
VirtualFilter=VWC
LastObject=ID_CTRLC


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
LastObject=ID__HELP
BaseClass=CFrameWnd
VirtualFilter=fWC




[CLS:CAboutDlg]
Type=0
HeaderFile=ShiftN.cpp
ImplementationFile=ShiftN.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDOK,button,1342242817
Control3=IDC_STATIC,button,1342177287
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352

[DLG:IDD_DIALOG1]
Type=1
Class=Ladefehler_Dialog
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,button,1342177287
Control3=IDC_STATIC,static,1342308352

[CLS:Ladefehler_Dialog]
Type=0
HeaderFile=Ladefehler_Dialog.h
ImplementationFile=Ladefehler_Dialog.cpp
BaseClass=CDialog
Filter=D
LastObject=ID_APP_ABOUT

[CLS:Keine_Ergebnisse]
Type=0
HeaderFile=keine_ergebnisse.h
ImplementationFile=keine_ergebnisse.cpp
BaseClass=CDialog
LastObject=Keine_Ergebnisse

[CLS:Kein_Bild]
Type=0
HeaderFile=Kein_Bild.h
ImplementationFile=Kein_Bild.cpp
BaseClass=CDialog
Filter=D
LastObject=Kein_Bild

[CLS:DarstDlg]
Type=0
HeaderFile=DarstDlg.h
ImplementationFile=DarstDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_CHECK10
VirtualFilter=dWC

[CLS:ParamDlg]
Type=0
HeaderFile=ParamDlg.h
ImplementationFile=ParamDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_EDIT13

[CLS:abspDlg]
Type=0
HeaderFile=abspDlg.h
ImplementationFile=abspDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=abspDlg

[DLG:IDD_ABOUTBOX (Englisch (USA))]
Type=1
Class=CAboutDlg
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDOK,button,1342242817
Control3=IDC_STATIC,button,1342177287
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352

[DLG:IDD_DIALOG1 (Englisch (USA))]
Type=1
Class=Ladefehler_Dialog
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,button,1342177287
Control3=IDC_STATIC,static,1342308352

[DLG:IDD_DIALOG2 (Englisch (USA))]
Type=1
Class=Keine_Ergebnisse
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,button,1342177287
Control3=IDC_STATIC,static,1342308352

[MNU:IDR_MAINFRAME (Englisch (USA))]
Type=1
Class=CMainFrame
Command1=MH_LOAD
Command2=MH_SAVE
Command3=ID_DATEI_SPEICHERNUNTER
Command4=MH_BATCH
Command5=ID_SAVE_CLOSE
Command6=ID_APP_EXIT
Command7=ID_CTRLC
Command8=ID_CTRLV
Command9=CG_ID_VIEW_MYDIALOGBAR
Command10=ID_GITTER
Command11=ID_VIEW_STATUS_BAR
Command12=MH_PARAM
Command13=MH_DARST
Command14=ID_OPTIONS_LANGUAGE
Command15=ID_EINSTELLUNGEN_SCHALTFLCHESPEICHERN
Command16=MH_Go
Command17=MH_Re
Command18=ID_ANWENDEN_KORREKTURANPASSEN
Command19=ID__HELP
Command20=ID__HINWEISE
Command21=ID_APP_ABOUT
CommandCount=21

[DLG:IDD_DIALOG3 (Englisch (USA))]
Type=1
Class=Kein_Bild
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,button,1342177287
Control3=IDC_STATIC,static,1342308352

[DLG:IDD_DIALOG4 (Englisch (USA))]
Type=1
Class=DarstDlg
ControlCount=16
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,button,1342177287
Control4=IDC_CHECK4,button,1342242819
Control5=IDC_CHECK5,button,1342242819
Control6=IDC_CHECK6,button,1342242819
Control7=IDC_CHECK7,button,1342242819
Control8=IDC_CHECK8,button,1342242819
Control9=IDC_CHECK9,button,1342242819
Control10=IDC_EDIT3,edit,1350631552
Control11=IDC_EDIT4,edit,1350631552
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STATIC,static,1342308352
Control14=IDC_EDIT6,edit,1350631552
Control15=IDC_STATIC,static,1342308352
Control16=IDC_CHECK10,button,1342242819

[DLG:IDD_DIALOG5 (Englisch (USA))]
Type=1
Class=?
ControlCount=9
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,button,1342177287
Control4=IDC_EDIT1,edit,1350631553
Control5=IDC_STATIC,static,1342308866
Control6=IDC_EDIT2,edit,1350631553
Control7=IDC_STATIC,static,1342308866
Control8=IDC_EDIT3,edit,1350631553
Control9=IDC_STATIC,static,1342308866

[DLG:IDD_DIALOG6 (Englisch (USA))]
Type=1
Class=ParamDlg
ControlCount=22
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,button,1342177287
Control4=IDC_EDIT2,edit,1350631553
Control5=IDC_STATIC,static,1342308866
Control6=IDC_EDIT6,edit,1350631553
Control7=IDC_STATIC,static,1342308866
Control8=IDC_EDIT9,edit,1350631553
Control9=IDC_STATIC,static,1342308866
Control10=IDC_EDIT11,edit,1350631553
Control11=IDC_STATIC,static,1342308866
Control12=IDC_CHECK1,button,1342242851
Control13=IDC_EDIT12,edit,1350631553
Control14=IDC_STATIC,static,1342308866
Control15=IDC_BUTTON1,button,1342242816
Control16=IDC_EDIT13,edit,1350631553
Control17=IDC_STATIC,static,1342308866
Control18=IDC_CHECK2,button,1342242851
Control19=IDC_EDIT14,edit,1350631553
Control20=IDC_STATIC,static,1342308866
Control21=IDC_EDIT15,edit,1350631553
Control22=IDC_STATIC,static,1342308866

[DLG:IDD_DIALOG7 (Englisch (USA))]
Type=1
Class=?
ControlCount=5
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,button,1342177287
Control4=IDC_CHECK1,button,1342242819
Control5=IDC_CHECK2,button,1342242819

[DLG:IDD_DIALOG8 (Englisch (USA))]
Type=1
Class=abspDlg
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,button,1342177287
Control3=IDC_STATIC,static,1342308352

[DLG:IDD_DIALOG9 (Englisch (USA))]
Type=1
Class=?
ControlCount=1
Control1=IDC_PROGRESS1,msctls_progress32,1350565888

[DLG:IDD_ABOUTBOX (Deutsch (Deutschland))]
Type=1
Class=CAboutDlg
ControlCount=7
Control1=IDC_STATIC,static,1342177283
Control2=IDOK,button,1342242817
Control3=IDC_STATIC,button,1342177287
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=MH_LOAD
Command2=MH_SAVE
Command3=ID_DATEI_SPEICHERNUNTER
Command4=MH_BATCH
Command5=ID_SAVE_CLOSE
Command6=ID_APP_EXIT
Command7=ID_CTRLC
Command8=ID_CTRLV
Command9=CG_ID_VIEW_MYDIALOGBAR
Command10=ID_GITTER
Command11=ID_VIEW_STATUS_BAR
Command12=MH_PARAM
Command13=MH_DARST
Command14=ID_EINSTELLUNGEN_SCHALTFLCHESPEICHERN
Command15=ID_OPTIONS_LANGUAGE
Command16=MH_Go
Command17=MH_Re
Command18=ID_ANWENDEN_KORREKTURANPASSEN
Command19=ID__HILFE
Command20=ID__HINWEISE
Command21=ID_APP_ABOUT
CommandCount=21

[DLG:IDD_DIALOG2]
Type=1
Class=Keine_Ergebnisse
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,button,1342177287
Control3=IDC_STATIC,static,1342308352

[DLG:IDD_DIALOG3]
Type=1
Class=Kein_Bild
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,button,1342177287
Control3=IDC_STATIC,static,1342308352

[DLG:IDD_DIALOG4]
Type=1
Class=DarstDlg
ControlCount=16
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,button,1342177287
Control4=IDC_CHECK4,button,1342242819
Control5=IDC_CHECK5,button,1342242819
Control6=IDC_CHECK6,button,1342242819
Control7=IDC_CHECK7,button,1342242819
Control8=IDC_CHECK8,button,1342242819
Control9=IDC_CHECK9,button,1342242819
Control10=IDC_EDIT3,edit,1350631552
Control11=IDC_EDIT4,edit,1350631552
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STATIC,static,1342308352
Control14=IDC_EDIT6,edit,1350631552
Control15=IDC_STATIC,static,1342308352
Control16=IDC_CHECK10,button,1342242819

[DLG:IDD_DIALOG6]
Type=1
Class=ParamDlg
ControlCount=22
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,button,1342177287
Control4=IDC_EDIT2,edit,1350631553
Control5=IDC_STATIC,static,1342308866
Control6=IDC_EDIT6,edit,1350631553
Control7=IDC_STATIC,static,1342308866
Control8=IDC_EDIT9,edit,1350631553
Control9=IDC_STATIC,static,1342308866
Control10=IDC_EDIT11,edit,1350631553
Control11=IDC_STATIC,static,1342308866
Control12=IDC_CHECK1,button,1342242851
Control13=IDC_EDIT12,edit,1350631553
Control14=IDC_STATIC,static,1342308866
Control15=IDC_BUTTON1,button,1342242816
Control16=IDC_EDIT13,edit,1350631553
Control17=IDC_STATIC,static,1342308866
Control18=IDC_CHECK2,button,1342242851
Control19=IDC_EDIT14,edit,1350631553
Control20=IDC_STATIC,static,1342308866
Control21=IDC_EDIT15,edit,1350631553
Control22=IDC_STATIC,static,1342308866

[DLG:IDD_DIALOG8]
Type=1
Class=abspDlg
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,button,1342177287
Control3=IDC_STATIC,static,1342308352

[CLS:SprachDlg]
Type=0
HeaderFile=SprachDlg.h
ImplementationFile=SprachDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_RADIO1
VirtualFilter=dWC

[DLG:IDD_DIALOG10]
Type=1
Class=SprachDlg
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDC_RADIO1,button,1342308361
Control3=IDC_RADIO2,button,1342177289
Control4=IDC_STATIC,static,1350697473

[CLS:AdjustDlg]
Type=0
HeaderFile=AdjustDlg.h
ImplementationFile=AdjustDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_EDIT3

[DLG:IDD_DIALOG11 (Englisch (USA))]
Type=1
Class=AdjustDlg
ControlCount=21
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,static,1342308354
Control3=IDC_STATIC,static,1342308354
Control4=IDC_STATIC,button,1342177287
Control5=IDC_SLIDER1,msctls_trackbar32,1342242817
Control6=IDC_SLIDER2,msctls_trackbar32,1342242817
Control7=IDC_STATIC,static,1342308353
Control8=IDC_STATIC,static,1342308353
Control9=IDC_STATIC,static,1342308353
Control10=IDC_STATIC,static,1342308353
Control11=IDC_STATIC,static,1342308353
Control12=IDC_STATIC,static,1342308353
Control13=IDC_STATIC,static,1342308354
Control14=IDC_SLIDER3,msctls_trackbar32,1342242817
Control15=IDC_STATIC,static,1342308353
Control16=IDC_STATIC,static,1342308353
Control17=IDC_STATIC,static,1342308353
Control18=IDC_STATIC,button,1342177287
Control19=IDC_EDIT1,edit,1350631552
Control20=IDC_EDIT2,edit,1350631552
Control21=IDC_EDIT3,edit,1350631552

[CLS:CSplashWnd]
Type=0
HeaderFile=Splash.h
ImplementationFile=Splash.cpp
BaseClass=CWnd
LastObject=CSplashWnd

[DLG:CG_IDD_MYDIALOGBAR]
Type=1
Class=Toolbar
ControlCount=4
Control1=MH_Go,button,1342242816
Control2=ID_ANWENDEN_KORREKTURANPASSEN,button,1342242816
Control3=MH_Re,button,1342242816
Control4=ID_SAVE_CLOSE,button,1342242816

[DLG:CG_IDD_MYDIALOGBAR (Englisch (USA))]
Type=1
Class=Toolbar
ControlCount=4
Control1=MH_Go,button,1342242816
Control2=ID_ANWENDEN_KORREKTURANPASSEN,button,1342242816
Control3=MH_Re,button,1342242816
Control4=ID_SAVE_CLOSE,button,1342242816

[CLS:Toolbar]
Type=0
HeaderFile=Toolbar.h
ImplementationFile=Toolbar.cpp
BaseClass=CDialog
Filter=D
LastObject=MH_Go
VirtualFilter=dWC

[DLG:CG_IDD_PROGRESS]
Type=1
Class=CProgressDlg
ControlCount=1
Control1=CG_IDC_PROGDLG_PROGRESS,msctls_progress32,1350565888

[CLS:CProgressDlg]
Type=0
HeaderFile=ProgDlg.h
ImplementationFile=ProgDlg.cpp
BaseClass=CDialog
LastObject=CG_ID_VIEW_MYDIALOGBAR

[DLG:CG_IDD_PROGRESS (Englisch (USA))]
Type=1
Class=CProgressDlg
ControlCount=1
Control1=CG_IDC_PROGDLG_PROGRESS,msctls_progress32,1350565888

[ACL:IDR_ACCELERATOR1]
Type=1
Class=CShiftNView
Command1=ID_APP_ABOUT
CommandCount=1

[ACL:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_CTRLC
Command2=ID_APP_ABOUT
Command3=ID_CTRLV
CommandCount=3

[DLG:IDD_DIALOG10 (Englisch (USA))]
Type=1
Class=SprachDlg
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDC_RADIO1,button,1342308361
Control3=IDC_RADIO2,button,1342177289
Control4=IDC_STATIC,static,1350697473

[DLG:IDD_DIALOG12]
Type=1
Class=Hinweise
ControlCount=8
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,static,1342177283
Control3=IDC_STATIC,button,1342177287
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352

[DLG:IDD_DIALOG12 (Englisch (USA))]
Type=1
Class=Hinweise
ControlCount=8
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,static,1342177283
Control3=IDC_STATIC,button,1342177287
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352

[CLS:Hinweise]
Type=0
HeaderFile=Hinweise.h
ImplementationFile=Hinweise.cpp
BaseClass=CDialog
Filter=D

[DLG:IDD_DIALOG11]
Type=1
Class=AdjustDlg
ControlCount=21
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,static,1342308354
Control3=IDC_STATIC,static,1342308354
Control4=IDC_STATIC,button,1342177287
Control5=IDC_SLIDER1,msctls_trackbar32,1342242817
Control6=IDC_SLIDER2,msctls_trackbar32,1342242817
Control7=IDC_STATIC,static,1342308353
Control8=IDC_STATIC,static,1342308353
Control9=IDC_STATIC,static,1342308353
Control10=IDC_STATIC,static,1342308353
Control11=IDC_STATIC,static,1342308353
Control12=IDC_STATIC,static,1342308353
Control13=IDC_STATIC,static,1342308354
Control14=IDC_SLIDER3,msctls_trackbar32,1342242817
Control15=IDC_STATIC,static,1342308353
Control16=IDC_STATIC,static,1342308353
Control17=IDC_STATIC,static,1342308353
Control18=IDC_STATIC,button,1342177287
Control19=IDC_EDIT1,edit,1350631552
Control20=IDC_EDIT2,edit,1350631552
Control21=IDC_EDIT3,edit,1350631552

[DLG:CG_IDD_MYDIALOGBAR_SAVE]
Type=1
Class=?
ControlCount=4
Control1=MH_Go,button,1342242816
Control2=ID_ANWENDEN_KORREKTURANPASSEN,button,1342242816
Control3=MH_Re,button,1342242816
Control4=MH_SAVE,button,1342242816

[DLG:CG_IDD_MYDIALOGBAR_SAVE (Englisch (USA))]
Type=1
Class=?
ControlCount=4
Control1=MH_Go,button,1342242816
Control2=ID_ANWENDEN_KORREKTURANPASSEN,button,1342242816
Control3=MH_Re,button,1342242816
Control4=MH_SAVE,button,1342242816

[DLG:]
Type=1
Class=?
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDC_RADIO1,button,1342308361
Control3=IDC_RADIO2,button,1342177289
Control4=IDC_STATIC,static,1350697473

[DLG:IDD_DIALOG13]
Type=1
Class=SchaltflDlg
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,static,1350697473
Control3=IDC_CHECK1,button,1342242819

[CLS:SchaltflDlg]
Type=0
HeaderFile=SchaltflDlg.h
ImplementationFile=SchaltflDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_CHECK1
VirtualFilter=dWC

[DLG:IDD_DIALOG13 (Englisch (USA))]
Type=1
Class=?
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,static,1350697473
Control3=IDC_CHECK1,button,1342242819

[ACL:IDR_MAINFRAME (Neutral)]
Type=1
Class=?
Command1=ID_CTRLC
Command2=ID_APP_ABOUT
Command3=ID_CTRLV
CommandCount=3

