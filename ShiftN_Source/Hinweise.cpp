// Hinweise.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "ShiftN.h"
#include "Hinweise.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld Hinweise 


Hinweise::Hinweise(CWnd* pParent /*=NULL*/)
	: CDialog(Hinweise::IDD, pParent)
{
	//{{AFX_DATA_INIT(Hinweise)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void Hinweise::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Hinweise)
		// HINWEIS: Der Klassen-Assistent fügt hier DDX- und DDV-Aufrufe ein
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Hinweise, CDialog)
	//{{AFX_MSG_MAP(Hinweise)
		// HINWEIS: Der Klassen-Assistent fügt hier Zuordnungsmakros für Nachrichten ein
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten Hinweise 
