// shiftn_espanol.cpp : Legt die Initialisierungsroutinen f�r die DLL fest.
//

#include "stdafx.h"
#include "shiftn_espanol.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Beachten Sie!
//
//		Wird diese DLL dynamisch an die MFC-DLLs gebunden,
//		muss bei allen von dieser DLL exportierten Funktionen,
//		die MFC-Aufrufe durchf�hren, das Makro AFX_MANAGE_STATE
//		direkt am Beginn der Funktion eingef�gt sein.
//
//		Beispiel:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//		// Hier normaler Funktionsrumpf
//		}
//
//		Es ist sehr wichtig, dass dieses Makro in jeder Funktion
//		vor allen MFC-Aufrufen erscheint. Dies bedeutet, dass es
//		als erste Anweisung innerhalb der Funktion ausgef�hrt werden
//		muss, sogar vor jeglichen Deklarationen von Objektvariablen,
//		da ihre Konstruktoren Aufrufe in die MFC-DLL generieren
//		k�nnten.
//
//		Siehe MFC Technical Notes 33 und 58 f�r weitere
//		Details.
//

/////////////////////////////////////////////////////////////////////////////
// Cshiftn_espanolApp

BEGIN_MESSAGE_MAP(Cshiftn_espanolApp, CWinApp)
	//{{AFX_MSG_MAP(Cshiftn_espanolApp)
		// HINWEIS - Hier werden Mapping-Makros vom Klassen-Assistenten eingef�gt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VER�NDERN!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Cshiftn_espanolApp Konstruktion

Cshiftn_espanolApp::Cshiftn_espanolApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// Das einzige Cshiftn_espanolApp-Objekt

Cshiftn_espanolApp theApp;
