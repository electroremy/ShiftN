/*

Copyright Marcus Hebel 1998-2010
http://www.shiftn.de
mhebel@gmx.de

This file is part of ShiftN 4.0.

ShiftN 4.0 is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published
by the Free Software Foundation, version 3 of the License.

ShiftN 4.0 is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with ShiftN 4.0. If not, see <http://www.gnu.org/licenses/lgpl-3.0.txt>.

*/

/*

EXIF handling of jpg files is done with jhead by Matthias Wandel
http://www.sentex.net/~mwandel/

This software is based in part on the work of the Independent
JPEG Group (JPEG library).

ShiftN incorporates the libraries "libtiff" and "zlib" for
processing and manipulating TIFF images.

Line detection is based on: J. Brian Burns, Allen R. Hanson, Edward
M. Riseman: "Extracting Straight Lines", IEEE Transactions on Pattern
Analysis and Machine Intelligence, Vol. PAMI-8, No. 4, July 1986.

*/

#include "stdafx.h"
#include "ShiftN.h"
#include "MainFrm.h"
#include "Splash.h"
#include "HtmlHelp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int lmhshowgui=1;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_COMMAND_EX(CG_ID_VIEW_MYDIALOGBAR, OnBarCheck)
	ON_UPDATE_COMMAND_UI(CG_ID_VIEW_MYDIALOGBAR, OnUpdateControlBarMenu)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_MOVE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_COMMAND(ID_HELPMH, OnHilfeMH)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // Statusleistenanzeige
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame Konstruktion/Zerstörung

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	UINT nl;
	int *lsave;
	int saveexit=1;
	if (AfxGetApp()->GetProfileBinary("SaveExit","SAVE", (LPBYTE*)&lsave, &nl)) {
		if (!(lsave[0])) saveexit=0;
		delete [] lsave;
	}
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Statusleiste konnte nicht erstellt werden\n");
		return -1;      // Fehler bei Erstellung
	}
	// CG: Die folgende Zeile wurde von der Komponente "Begrüßungsbildschirm" hinzugefügt.
	//CSplashWnd::ShowSplashScreen(this);
	
	// CG: Der folgende Block wurde von der  Komponente "Dialogfeldleiste" eingefügt
	{
		// Dialogfeldleiste initialisieren m_wndMyDialogBar
		if (saveexit) {
			if (!m_wndMyDialogBar.Create(this, CG_IDD_MYDIALOGBAR,
				CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_HIDE_INPLACE,
				CG_ID_VIEW_MYDIALOGBAR))
			{
				TRACE0("Erstellen der Dialogfeldleiste fehlgeschlagen m_wndMyDialogBar\n");
				return -1;		// Erstellen fehlgeschlagen
			}
		}
		else {
			if (!m_wndMyDialogBar.Create(this, CG_IDD_MYDIALOGBAR_SAVE,
				CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_HIDE_INPLACE,
				CG_ID_VIEW_MYDIALOGBAR))
			{
				TRACE0("Erstellen der Dialogfeldleiste fehlgeschlagen m_wndMyDialogBar\n");
				return -1;		// Erstellen fehlgeschlagen
			}
		}
		m_wndMyDialogBar.EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM);
		EnableDocking(CBRS_ALIGN_ANY);
		DockControlBar(&m_wndMyDialogBar);
	}
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	UINT nl;
	int *lsprach;
	HINSTANCE hRes = NULL;
	if (AfxGetApp()->GetProfileBinary("Language","SPRACH", (LPBYTE*)&lsprach, &nl)) {
		switch (lsprach[0]) {	
			case 1:
				hRes= LoadLibrary("shiftn_english.dll");
				if(hRes) AfxSetResourceHandle(hRes);
				break;
			case 2:
				hRes= LoadLibrary("shiftn_francais.dll");
				if(hRes) AfxSetResourceHandle(hRes);
				break;
			case 3:
				hRes= LoadLibrary("shiftn_espanol.dll");
				if(hRes) AfxSetResourceHandle(hRes);
				break;
			default:
				hRes= LoadLibrary("shiftn_deutsch.dll");
				if(hRes) AfxSetResourceHandle(hRes);
				break;
		}
		delete [] lsprach;
	}
	else {
		hRes= LoadLibrary("shiftn_english.dll");
		if(hRes) AfxSetResourceHandle(hRes);
	}
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame Diagnose

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame Nachrichten-Handler


void CMainFrame::OnClose() 
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen und/oder Standard aufrufen
	
	CFrameWnd::OnClose();
}

void CMainFrame::OnFinalRelease() 
{
	// TODO: Speziellen Code hier einfügen und/oder Basisklasse aufrufen
	
	CFrameWnd::OnFinalRelease();
}

void CMainFrame::OnMove(int x, int y) 
{
	CFrameWnd::OnMove(x, y);
	
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen
	
}

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);
	
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen
	
}

void CMainFrame::OnDestroy() 
{
	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);
	if (lmhshowgui) AfxGetApp()->WriteProfileBinary("MainFrameMH", "WP", (LPBYTE)&wp, sizeof(wp));
	CFrameWnd::OnDestroy();
		
}

void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	static bool bOnce = true;
	WINDOWPLACEMENT *lwp;
	WINDOWPLACEMENT lwp2;
	UINT nl;
	int i;
	if (__argc>=3) {
		lmhshowgui=0;
	}
	if (__argc>=4) {
		i=0;
		while ((__argv[3][i]!='\0') && (i<1000)) i++;
		if (i>0) {
			if ((__argv[3][0]==109) || (__argv[3][0]==77)) {
				lmhshowgui=1;
			}
		}
	}
	CFrameWnd::OnShowWindow(bShow, nStatus);
	if (lmhshowgui) {
		if (bShow && !IsWindowVisible() && bOnce) {
			bOnce = false;
			if (AfxGetApp()->GetProfileBinary("MainFrameMH", "WP", (LPBYTE*)&lwp, &nl)) {
				if (((lwp->rcNormalPosition.bottom<2) && (lwp->rcNormalPosition.top<2) && (lwp->rcNormalPosition.left<2) && (lwp->rcNormalPosition.right<2)) || ((lwp->rcNormalPosition.bottom-lwp->rcNormalPosition.top)<128) || ((lwp->rcNormalPosition.right-lwp->rcNormalPosition.left)<32) || (!(lwp->showCmd))) {
					lwp->flags=SW_SHOWNORMAL;
					lwp->showCmd=SW_SHOW;
					lwp->ptMinPosition.x=1;
					lwp->ptMinPosition.y=1;
					lwp->ptMaxPosition.x=500;
					lwp->ptMaxPosition.y=320;
					lwp->rcNormalPosition=CRect(lwp->ptMinPosition,lwp->ptMaxPosition);
				}
				SetWindowPlacement(lwp);
				delete [] lwp;
			}
		}
	}
	else {
		if (bShow && !IsWindowVisible() && bOnce) {
			bOnce=false;
			lwp2.length=sizeof(WINDOWPLACEMENT);
			lwp2.flags=SW_SHOWNORMAL;
			lwp2.showCmd=SW_HIDE;
			lwp2.ptMinPosition.x=0;
			lwp2.ptMinPosition.y=0;
			lwp2.ptMaxPosition.x=1;
			lwp2.ptMaxPosition.y=1;
			lwp2.rcNormalPosition = CRect(lwp2.ptMinPosition,lwp2.ptMaxPosition);
			SetWindowPlacement(&lwp2);
		}
	}
}

void CMainFrame::OnHilfeMH() 
{	
	char buff[512];
	CString helpname;
	UINT nl;
	int *lsprach;
	GetModuleFileName(NULL,buff,512);
	*(strrchr(buff,'\\')+1)='\0'; 
	helpname=buff;
	if (AfxGetApp()->GetProfileBinary("Language","SPRACH", (LPBYTE*)&lsprach, &nl)) {
		switch (lsprach[0]) {
			case 3 : helpname+="ShiftN_Ayuda.chm"; break;
			case 2 : helpname+="ShiftN_Aide.chm"; break;
			case 1 : helpname+="ShiftN_Help.chm"; break;
			default : helpname+="ShiftN_Hilfe.chm"; break;
		}
		delete [] lsprach;
	}
	else {
		helpname+="ShiftN_Help.chm";
	}
	::HtmlHelp(NULL,helpname.GetBuffer(),HH_DISPLAY_TOC,NULL);
}
