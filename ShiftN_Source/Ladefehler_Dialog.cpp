// Ladefehler_Dialog.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "ShiftN.h"
#include "Ladefehler_Dialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld Ladefehler_Dialog 


Ladefehler_Dialog::Ladefehler_Dialog(CWnd* pParent /*=NULL*/)
	: CDialog(Ladefehler_Dialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(Ladefehler_Dialog)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void Ladefehler_Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Ladefehler_Dialog)
		// HINWEIS: Der Klassen-Assistent fügt hier DDX- und DDV-Aufrufe ein
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Ladefehler_Dialog, CDialog)
	//{{AFX_MSG_MAP(Ladefehler_Dialog)
		// HINWEIS: Der Klassen-Assistent fügt hier Zuordnungsmakros für Nachrichten ein
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten Ladefehler_Dialog 
