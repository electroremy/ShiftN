#if !defined(AFX_KEIN_BILD_H__14C458BC_D4EB_48B6_BAD1_32C3EE092F18__INCLUDED_)
#define AFX_KEIN_BILD_H__14C458BC_D4EB_48B6_BAD1_32C3EE092F18__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Kein_Bild.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld Kein_Bild 

class Kein_Bild : public CDialog
{
// Konstruktion
public:
	Kein_Bild(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(Kein_Bild)
	enum { IDD = IDD_DIALOG3 };
		// HINWEIS: Der Klassen-Assistent f�gt hier Datenelemente ein
	//}}AFX_DATA


// �berschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktions�berschreibungen
	//{{AFX_VIRTUAL(Kein_Bild)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterst�tzung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(Kein_Bild)
		// HINWEIS: Der Klassen-Assistent f�gt hier Member-Funktionen ein
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f�gt unmittelbar vor der vorhergehenden Zeile zus�tzliche Deklarationen ein.

#endif // AFX_KEIN_BILD_H__14C458BC_D4EB_48B6_BAD1_32C3EE092F18__INCLUDED_
