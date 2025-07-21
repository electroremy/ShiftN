// SprachDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "ShiftN.h"
#include "SprachDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld SprachDlg 


SprachDlg::SprachDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SprachDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SprachDlg)
	m_sprach = -1;
	//}}AFX_DATA_INIT
}


void SprachDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SprachDlg)
	DDX_Radio(pDX, IDC_RADIO1, m_sprach);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SprachDlg, CDialog)
	//{{AFX_MSG_MAP(SprachDlg)
		// HINWEIS: Der Klassen-Assistent fügt hier Zuordnungsmakros für Nachrichten ein
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten SprachDlg 
