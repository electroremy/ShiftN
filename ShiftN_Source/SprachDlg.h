#if !defined(AFX_SPRACHDLG_H__541434CC_A41C_4623_BF00_CB6BC30234F2__INCLUDED_)
#define AFX_SPRACHDLG_H__541434CC_A41C_4623_BF00_CB6BC30234F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SprachDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld SprachDlg 

class SprachDlg : public CDialog
{
// Konstruktion
public:
	SprachDlg(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(SprachDlg)
	enum { IDD = IDD_DIALOG10 };
	int		m_sprach;
	//}}AFX_DATA


// �berschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktions�berschreibungen
	//{{AFX_VIRTUAL(SprachDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterst�tzung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(SprachDlg)
		// HINWEIS: Der Klassen-Assistent f�gt hier Member-Funktionen ein
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f�gt unmittelbar vor der vorhergehenden Zeile zus�tzliche Deklarationen ein.

#endif // AFX_SPRACHDLG_H__541434CC_A41C_4623_BF00_CB6BC30234F2__INCLUDED_
