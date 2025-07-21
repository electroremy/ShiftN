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
#include "ShiftNDoc.h"
#include "ShiftNView.h"
#include "Splash.h"
#include "Hinweise.h"

CRect wrect;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShiftNApp

BEGIN_MESSAGE_MAP(CShiftNApp, CWinApp)
	//{{AFX_MSG_MAP(CShiftNApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_ACKNOW, OnHinweise)
	//}}AFX_MSG_MAP
	// Dateibasierte Standard-Dokumentbefehle
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShiftNApp Konstruktion

CShiftNApp::CShiftNApp()
{
	EnableHtmlHelp();
}

/////////////////////////////////////////////////////////////////////////////
// Das einzige CShiftNApp-Objekt

CShiftNApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CShiftNApp Initialisierung

class MySingleDocTemplate : public CSingleDocTemplate {
	public: MySingleDocTemplate( UINT nIDResource, CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass ) : CSingleDocTemplate(nIDResource,pDocClass,pFrameClass,pViewClass) {
	}
	virtual ~MySingleDocTemplate() {
	}
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszPathName, BOOL bMakeVisible);
};

CDocument* MySingleDocTemplate::OpenDocumentFile(LPCTSTR lpszPathName, BOOL bMakeVisible) {
	CDocument* pDocument = NULL;
	CFrameWnd* pFrame = NULL;
	BOOL bCreated = FALSE; // => doc and frame created
	BOOL bWasModified = FALSE;
	if (m_pOnlyDoc != NULL) {
		// already have a document - reinit it
		pDocument = m_pOnlyDoc;
		if (!pDocument->SaveModified())
		return NULL; // leave the original one
		pFrame = (CFrameWnd*)AfxGetMainWnd();
		ASSERT(pFrame != NULL);
		ASSERT_KINDOF(CFrameWnd, pFrame);
		ASSERT_VALID(pFrame);
	}
	else {
		// create a new document
		pDocument = CreateNewDocument();
		ASSERT(pFrame == NULL); // will be created below
		bCreated = TRUE;
	}

	if (pDocument == NULL) {
		AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
		return NULL;
	}
	ASSERT(pDocument == m_pOnlyDoc);
	if (pFrame == NULL) {
		ASSERT(bCreated);
		// create frame - set as main document frame
		BOOL bAutoDelete = pDocument->m_bAutoDelete;
		pDocument->m_bAutoDelete = FALSE;
		// don't destroy if something goes wrong
		pFrame = CreateNewFrame(pDocument, NULL);
		pDocument->m_bAutoDelete = bAutoDelete;
		if (pFrame == NULL) {
			AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
			delete pDocument; // explicit delete on error
			return NULL;
		}
	}
	if (lpszPathName==NULL) {
		// create a new document
		SetDefaultTitle(pDocument);
		// avoid creating temporary compound file when starting up invisible
		if (!bMakeVisible)
		pDocument->m_bEmbedded = TRUE;
		if (!pDocument->OnNewDocument()) {
			// user has been alerted to what failed in OnNewDocument
			TRACE0("CDocument::OnNewDocument returned FALSE.\n");
			if (bCreated) pFrame->DestroyWindow(); // will destroy document
			return NULL;
		}
	}
	else {
		lpszPathName=NULL;
		// create a new document
		SetDefaultTitle(pDocument);
		// avoid creating temporary compound file when starting up invisible
		if (!bMakeVisible)
		pDocument->m_bEmbedded = TRUE;
		if (!pDocument->OnNewDocument()) {
			// user has been alerted to what failed in OnNewDocument
			TRACE0("CDocument::OnNewDocument returned FALSE.\n");
			if (bCreated) pFrame->DestroyWindow(); // will destroy document
			return NULL;
		}
	}
	/*
	else {
		CWaitCursor wait;
		// open an existing document
		bWasModified=pDocument->IsModified();
		pDocument->SetModifiedFlag(FALSE); // not dirty for open

		if (!pDocument->OnOpenDocument(lpszPathName)) {
			// user has been alerted to what failed in OnOpenDocument
			TRACE0("CDocument::OnOpenDocument returned FALSE.\n");
			if (bCreated) {
				pFrame->DestroyWindow(); // will destroy document
			}
			else if (!pDocument->IsModified()) {
				// original document is untouched
				pDocument->SetModifiedFlag(bWasModified);
			}
			else {
				// we corrupted the original document
				SetDefaultTitle(pDocument);
				if (!pDocument->OnNewDocument()) {
					TRACE0("Error: OnNewDocument failed after trying to open a document - trying to continue.\n");
					// assume we can continue
				}
			}
			return NULL; // open failed
		}
		pDocument->SetPathName(lpszPathName);
	}
	*/

	CWinThread* pThread = AfxGetThread();
	if (bCreated && pThread->m_pMainWnd == NULL) {
		// set as main frame (InitialUpdateFrame will show the window)
		pThread->m_pMainWnd = pFrame;
	}
	InitialUpdateFrame(pFrame, pDocument, bMakeVisible);
	return pDocument;
}

BOOL CShiftNApp::InitInstance()
{
	BOOL mhshowsplash;
	CCommandLineInfo cmdInfo;
	int mhshowgui=1;
	int imhshowsplash=1;
	int i;
	if (__argc>=2) imhshowsplash=0;
	if (__argc>=3) {
		mhshowgui=0;
	}
	if (__argc>=4) {
		i=0;
		while ((__argv[3][i]!='\0') && (i<1000)) i++;
		if (i>0) {
			if ((__argv[3][0]==109) || (__argv[3][0]==77)) {
				mhshowgui=1;
			}
		}
	}
	// Befehlszeile parsen, um zu prüfen auf Standard-Umgebungsbefehle DDE, Datei offen
	ParseCommandLine(cmdInfo);
	mhshowsplash=cmdInfo.m_bShowSplash;
	if (!mhshowgui) mhshowsplash = FALSE;
	if (!imhshowsplash) mhshowsplash = FALSE;
	CSplashWnd::EnableSplashScreen(mhshowsplash);

	// Ändern des Registrierungsschlüssels, unter dem unsere Einstellungen gespeichert sind.
	SetRegistryKey(_T("Marcus Hebel Freeware"));

	LoadStdProfileSettings();  // Standard INI-Dateioptionen laden (einschließlich MRU)

	// Dokumentvorlagen registrieren

	MySingleDocTemplate* pDocTemplate;
	pDocTemplate = NULL;
	pDocTemplate = new MySingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CShiftNDoc),
		RUNTIME_CLASS(CMainFrame),       // Haupt-SDI-Rahmenfenster
		RUNTIME_CLASS(CShiftNView));
	AddDocTemplate(pDocTemplate);

	// Verteilung der in der Befehlszeile angegebenen Befehle
	if (!ProcessShellCommand(cmdInfo)) return FALSE;
	if (!mhshowgui) {
		m_pMainWnd->ShowWindow(SW_HIDE);
		m_pMainWnd->UpdateWindow();
		m_pMainWnd->PostMessage(WM_CLOSE,0,0);
	}
	else {
		m_pMainWnd->ShowWindow(SW_SHOW);
		m_pMainWnd->UpdateWindow();
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg-Dialog für Info über Anwendung

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialogdaten
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// Überladungen für virtuelle Funktionen, die vom Anwendungs-Assistenten erzeugt wurden
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	//{{AFX_MSG(CAboutDlg)
		// Keine Nachrichten-Handler
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// Keine Nachrichten-Handler
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Anwendungsbefehl zum Ausführen des Dialogfelds
void CShiftNApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CShiftNApp::OnHinweise()
{
	Hinweise aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CShiftNApp-Nachrichtenbehandlungsroutinen


void CShiftNApp::OnFinalRelease() 
{
	// TODO: Speziellen Code hier einfügen und/oder Basisklasse aufrufen
	CWinApp::OnFinalRelease();
}


BOOL CShiftNApp::PreTranslateMessage(MSG* pMsg)
{
	// CG: Die folgenden Zeilen wurden von der Komponente "Begrüßungsbildschirm" hinzugefügt.
	if (CSplashWnd::PreTranslateAppMessage(pMsg))
		return TRUE;

	return CWinApp::PreTranslateMessage(pMsg);
}
