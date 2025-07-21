#if !defined(AFX_PARAMDLG_H__4268A146_CCA4_4970_8D94_585D172117B4__INCLUDED_)
#define AFX_PARAMDLG_H__4268A146_CCA4_4970_8D94_585D172117B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ParamDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld ParamDlg 

class ParamDlg : public CDialog
{
// Konstruktion
public:
	ParamDlg(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(ParamDlg)
	enum { IDD = IDD_DIALOG6 };
	int		m_minlen;
	int		m_mincon;
	int		m_maxdisp;
	int		m_shiftamount;
	int		m_respara;
	BOOL	m_dorot;
	BOOL	m_ori;
	BOOL    m_parlot;
	double	m_distortion;
	int		m_flen;
	double	m_horifac;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(ParamDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(ParamDlg)
	afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCheck2();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_PARAMDLG_H__4268A146_CCA4_4970_8D94_585D172117B4__INCLUDED_
