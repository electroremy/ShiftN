#if !defined(AFX_ADJUSTDLG_H__7991888B_06AE_4678_915E_1EC0FF27D2A1__INCLUDED_)
#define AFX_ADJUSTDLG_H__7991888B_06AE_4678_915E_1EC0FF27D2A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdjustDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld AdjustDlg 

class AdjustDlg : public CDialog
{
// Konstruktion
public:
	AdjustDlg(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(AdjustDlg)
	enum { IDD = IDD_DIALOG11 };
	int		m_shiftreg;
	int		m_drehreg;
	int		m_distoreg;
	int     m_horizoreg;
	double	m_shiftd;
	double	m_drehd;
	double	m_distd;
	double  m_horizd;
	double	m_reset_param;
	double	m_reset_winkel;
	double	m_reset_distortion;
	double  m_reset_horizontal;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(AdjustDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(AdjustDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnReleasedcaptureSlider1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReleasedcaptureSlider2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReleasedcaptureSlider3(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReleasedcaptureSlider4(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdrawSlider1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdrawSlider2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdrawSlider3(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdrawSlider4(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusEdit1();
	afx_msg void OnKillfocusEdit2();
	afx_msg void OnKillfocusEdit3();
	afx_msg void OnKillfocusEdit4();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_ADJUSTDLG_H__7991888B_06AE_4678_915E_1EC0FF27D2A1__INCLUDED_
