#if !defined(AFX_TOOLBAR_H__2DCE7A6A_4E18_493F_865A_427ACDB4BA69__INCLUDED_)
#define AFX_TOOLBAR_H__2DCE7A6A_4E18_493F_865A_427ACDB4BA69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Toolbar.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld Toolbar 

class Toolbar : public CDialog
{
// Konstruktion
public:
	Toolbar(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(Toolbar)
	enum { IDD = CG_IDD_MYDIALOGBAR };
		// HINWEIS: Der Klassen-Assistent f�gt hier Datenelemente ein
	//}}AFX_DATA


// �berschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktions�berschreibungen
	//{{AFX_VIRTUAL(Toolbar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterst�tzung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(Toolbar)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f�gt unmittelbar vor der vorhergehenden Zeile zus�tzliche Deklarationen ein.

#endif // AFX_TOOLBAR_H__2DCE7A6A_4E18_493F_865A_427ACDB4BA69__INCLUDED_
