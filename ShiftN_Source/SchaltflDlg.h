#if !defined(AFX_SCHALTFLDLG_H__7D4D46A7_EF0A_4FF1_BAEC_677E0F0BCBC7__INCLUDED_)
#define AFX_SCHALTFLDLG_H__7D4D46A7_EF0A_4FF1_BAEC_677E0F0BCBC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SchaltflDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld SchaltflDlg 

class SchaltflDlg : public CDialog
{
// Konstruktion
public:
	SchaltflDlg(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(SchaltflDlg)
	enum { IDD = IDD_DIALOG13 };
	int		m_saveexit;
	BOOL	m_saveexit_check;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(SchaltflDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(SchaltflDlg)
	afx_msg void OnCheck1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_SCHALTFLDLG_H__7D4D46A7_EF0A_4FF1_BAEC_677E0F0BCBC7__INCLUDED_
