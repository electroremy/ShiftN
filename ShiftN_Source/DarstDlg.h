#if !defined(AFX_DARSTDLG_H__B448491F_7304_47BD_BB94_A11C88DB1637__INCLUDED_)
#define AFX_DARSTDLG_H__B448491F_7304_47BD_BB94_A11C88DB1637__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DarstDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld DarstDlg 

class DarstDlg : public CDialog
{
// Konstruktion
public:
	int darstellungstyp;
	int speichertyp;
	int besch;
	int kub;
	int sharp;
	int usebackrgb;
	int useexifdate;
	DarstDlg(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(DarstDlg)
	enum { IDD = IDD_DIALOG4 };
	BOOL	m_check_jpg;
	BOOL	m_check_bmp;
	BOOL	m_check_txt;
	BOOL	m_beschneiden;
	BOOL	m_schaerfen;
	BOOL	m_usebackrgb;
	BOOL	m_use_exif_date;
	double	m_scharfpar;
	int		m_jpegqualitaet;
	long	m_backrgb;
	BOOL	m_kubik;
	CString	m_dsuffix;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(DarstDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(DarstDlg)
	afx_msg void OnCheck4();
	afx_msg void OnCheck5();
	afx_msg void OnCheck6();
	afx_msg void OnCheck7();
	afx_msg void OnCheck8();
	afx_msg void OnCheck9();
	virtual void OnOK();
	afx_msg void OnKillfocusEdit3();
	afx_msg void OnKillfocusEdit4();
	afx_msg void OnKillfocusEdit6();
	afx_msg void OnKillfocusEdit9();
	afx_msg void OnCheck11();
	afx_msg void OnCheck10();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_DARSTDLG_H__B448491F_7304_47BD_BB94_A11C88DB1637__INCLUDED_
