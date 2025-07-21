#if !defined(AFX_LADEFEHLER_DIALOG_H__291C1C35_E120_4CB3_B009_F0AE0FFB1A56__INCLUDED_)
#define AFX_LADEFEHLER_DIALOG_H__291C1C35_E120_4CB3_B009_F0AE0FFB1A56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Ladefehler_Dialog.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld Ladefehler_Dialog 

class Ladefehler_Dialog : public CDialog
{
// Konstruktion
public:
	Ladefehler_Dialog(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(Ladefehler_Dialog)
	enum { IDD = IDD_DIALOG1 };
		// HINWEIS: Der Klassen-Assistent fügt hier Datenelemente ein
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(Ladefehler_Dialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(Ladefehler_Dialog)
		// HINWEIS: Der Klassen-Assistent fügt hier Member-Funktionen ein
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_LADEFEHLER_DIALOG_H__291C1C35_E120_4CB3_B009_F0AE0FFB1A56__INCLUDED_
