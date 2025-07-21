// abspDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "ShiftN.h"
#include "abspDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld abspDlg 


abspDlg::abspDlg(CWnd* pParent /*=NULL*/)
	: CDialog(abspDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(abspDlg)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void abspDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(abspDlg)
		// HINWEIS: Der Klassen-Assistent fügt hier DDX- und DDV-Aufrufe ein
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(abspDlg, CDialog)
	//{{AFX_MSG_MAP(abspDlg)
		// HINWEIS: Der Klassen-Assistent fügt hier Zuordnungsmakros für Nachrichten ein
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten abspDlg 
