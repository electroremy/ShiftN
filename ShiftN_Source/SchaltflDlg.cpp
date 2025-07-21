// SchaltflDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "ShiftN.h"
#include "SchaltflDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld SchaltflDlg 


SchaltflDlg::SchaltflDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SchaltflDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SchaltflDlg)
		m_saveexit = -1;
		m_saveexit_check = FALSE;
	//}}AFX_DATA_INIT
}


void SchaltflDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SchaltflDlg)
	DDX_Check(pDX, IDC_CHECK1, m_saveexit_check);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SchaltflDlg, CDialog)
	//{{AFX_MSG_MAP(SchaltflDlg)
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten SchaltflDlg 

void SchaltflDlg::OnCheck1() 
{
	UpdateData(TRUE);
	if (m_saveexit_check) m_saveexit=1;
	else m_saveexit=0;
	
}
