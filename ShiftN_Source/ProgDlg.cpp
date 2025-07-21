//  ProgDlg.cpp : Implementierungsdatei
// CG: Diese Datei wurde von der Komponente "Dialogfeld Fortschritt" hinzugefügt

#include "stdafx.h"
#include "resource.h"
#include "ProgDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg Dialogfeld

CProgressDlg::CProgressDlg(UINT nCaptionID)
{
	m_nCaptionID = CG_IDS_PROGRESS_CAPTION;
	if (nCaptionID != 0)
		m_nCaptionID = nCaptionID;

    m_nLower=0;
    m_nUpper=100;
    m_nStep=10;
    //{{AFX_DATA_INIT(CProgressDlg)
    // HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
    //}}AFX_DATA_INIT
    m_bParentDisabled = FALSE;
}

CProgressDlg::~CProgressDlg()
{
    if(m_hWnd!=NULL)
      DestroyWindow();
}

BOOL CProgressDlg::DestroyWindow()
{
    ReEnableParent();
    return CDialog::DestroyWindow();
}

void CProgressDlg::ReEnableParent()
{
    if(m_bParentDisabled && (m_pParentWnd!=NULL))
      m_pParentWnd->EnableWindow(TRUE);
    m_bParentDisabled=FALSE;
}

BOOL CProgressDlg::Create(CWnd *pParent)
{
    // Tatsächliches übergeordnetes Element des Dialogfelds bereitstellen
    m_pParentWnd = CWnd::GetSafeOwner(pParent);

    // m_bParentDisabled dient dem Neuaufbau des übergeordneten Fensters,
    // wenn das Dialogfeld entfernt wurde. Deshalb sollte sie nicht auf TRUE 
    // gesetzt werden, es sei denn das übergeordnete Fenster war bereits aktiviert.

    if((m_pParentWnd!=NULL) && m_pParentWnd->IsWindowEnabled())
    {
      m_pParentWnd->EnableWindow(FALSE);
      m_bParentDisabled = TRUE;
    }

    if(!CDialog::Create(CProgressDlg::IDD,pParent))
    {
      ReEnableParent();
      return FALSE;
    }

    return TRUE;
}

void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CProgressDlg)
    DDX_Control(pDX, CG_IDC_PROGDLG_PROGRESS, m_Progress);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
    //{{AFX_MSG_MAP(CProgressDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CProgressDlg::OnCancel()
{
}

void CProgressDlg::SetRange(int nLower,int nUpper)
{
    m_nLower = nLower;
    m_nUpper = nUpper;
    m_Progress.SetRange(nLower,nUpper);
}
  
int CProgressDlg::SetPos(int nPos)
{
    PumpMessages();
    return m_Progress.SetPos(nPos);
}

int CProgressDlg::SetStep(int nStep)
{
    m_nStep = nStep; // Für spätere Prozentberechnung speichern
    return m_Progress.SetStep(nStep);
}

int CProgressDlg::OffsetPos(int nPos)
{
    PumpMessages();
    return m_Progress.OffsetPos(nPos);
}

int CProgressDlg::StepIt()
{
    PumpMessages();
    return m_Progress.StepIt();
}

void CProgressDlg::SetCaptionText(CString strCapt) {
	SetWindowText(strCapt);
}

void CProgressDlg::PumpMessages()
{
    // Muss Create() aufrufen, bevor das Dialogfeld verwendet wird
    ASSERT(m_hWnd!=NULL);

    MSG msg;
    // Verarbeiten von Dialogfeld-Nachrichten
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      if(!IsDialogMessage(&msg))
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);  
      }
    }
}


    
/////////////////////////////////////////////////////////////////////////////
// CProgressDlg -Behandlungsroutinen

BOOL CProgressDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();
    m_Progress.SetRange(m_nLower,m_nUpper);
    m_Progress.SetStep(m_nStep);
    m_Progress.SetPos(m_nLower);

	CString strCaption;
	VERIFY(strCaption.LoadString(m_nCaptionID));
    SetWindowText(strCaption);

    return TRUE;  
}
