#if !defined(AFX_HINWEISE_H__8B2FE2A3_EC57_4D2D_A35B_DD6F74529310__INCLUDED_)
#define AFX_HINWEISE_H__8B2FE2A3_EC57_4D2D_A35B_DD6F74529310__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Hinweise.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld Hinweise 

class Hinweise : public CDialog
{
// Konstruktion
public:
	Hinweise(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(Hinweise)
	enum { IDD = IDD_DIALOG12 };
		// HINWEIS: Der Klassen-Assistent f�gt hier Datenelemente ein
	//}}AFX_DATA


// �berschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktions�berschreibungen
	//{{AFX_VIRTUAL(Hinweise)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterst�tzung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(Hinweise)
		// HINWEIS: Der Klassen-Assistent f�gt hier Member-Funktionen ein
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f�gt unmittelbar vor der vorhergehenden Zeile zus�tzliche Deklarationen ein.

#endif // AFX_HINWEISE_H__8B2FE2A3_EC57_4D2D_A35B_DD6F74529310__INCLUDED_
