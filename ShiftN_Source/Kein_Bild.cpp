// Kein_Bild.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "ShiftN.h"
#include "Kein_Bild.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld Kein_Bild 


Kein_Bild::Kein_Bild(CWnd* pParent /*=NULL*/)
	: CDialog(Kein_Bild::IDD, pParent)
{
	//{{AFX_DATA_INIT(Kein_Bild)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void Kein_Bild::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Kein_Bild)
		// HINWEIS: Der Klassen-Assistent fügt hier DDX- und DDV-Aufrufe ein
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Kein_Bild, CDialog)
	//{{AFX_MSG_MAP(Kein_Bild)
		// HINWEIS: Der Klassen-Assistent fügt hier Zuordnungsmakros für Nachrichten ein
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten Kein_Bild
