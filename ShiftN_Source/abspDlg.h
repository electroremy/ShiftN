#if !defined(AFX_ABSPDLG_H__12DEF299_F674_458F_9059_3F4483053475__INCLUDED_)
#define AFX_ABSPDLG_H__12DEF299_F674_458F_9059_3F4483053475__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// abspDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld abspDlg 

class abspDlg : public CDialog
{
// Konstruktion
public:
	abspDlg(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(abspDlg)
	enum { IDD = IDD_DIALOG8 };
		// HINWEIS: Der Klassen-Assistent f�gt hier Datenelemente ein
	//}}AFX_DATA


// �berschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktions�berschreibungen
	//{{AFX_VIRTUAL(abspDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterst�tzung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(abspDlg)
		// HINWEIS: Der Klassen-Assistent f�gt hier Member-Funktionen ein
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f�gt unmittelbar vor der vorhergehenden Zeile zus�tzliche Deklarationen ein.

#endif // AFX_ABSPDLG_H__12DEF299_F674_458F_9059_3F4483053475__INCLUDED_
