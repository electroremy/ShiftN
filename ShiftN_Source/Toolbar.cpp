// Toolbar.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "ShiftN.h"
#include "Toolbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld Toolbar 


Toolbar::Toolbar(CWnd* pParent /*=NULL*/)
	: CDialog(Toolbar::IDD, pParent)
{
	//{{AFX_DATA_INIT(Toolbar)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void Toolbar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Toolbar)
		// HINWEIS: Der Klassen-Assistent fügt hier DDX- und DDV-Aufrufe ein
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Toolbar, CDialog)
	//{{AFX_MSG_MAP(Toolbar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten Toolbar 


