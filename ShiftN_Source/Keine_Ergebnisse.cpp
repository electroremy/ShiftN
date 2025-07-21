// Keine_Ergebnisse.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "ShiftN.h"
#include "Keine_Ergebnisse.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld Keine_Ergebnisse 


Keine_Ergebnisse::Keine_Ergebnisse(CWnd* pParent /*=NULL*/)
	: CDialog(Keine_Ergebnisse::IDD, pParent)
{
	//{{AFX_DATA_INIT(Keine_Ergebnisse)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void Keine_Ergebnisse::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Keine_Ergebnisse)
		// HINWEIS: Der Klassen-Assistent fügt hier DDX- und DDV-Aufrufe ein
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Keine_Ergebnisse, CDialog)
	//{{AFX_MSG_MAP(Keine_Ergebnisse)
		// HINWEIS: Der Klassen-Assistent fügt hier Zuordnungsmakros für Nachrichten ein
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten Keine_Ergebnisse 
