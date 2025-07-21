// MainFrm.h : Schnittstelle der Klasse CMainFrame
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__07844C91_0738_48D0_A00E_E330425FBF25__INCLUDED_)
#define AFX_MAINFRM_H__07844C91_0738_48D0_A00E_E330425FBF25__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMainFrame : public CFrameWnd
{
	
protected: // Nur aus Serialisierung erzeugen
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attribute
public:

// Operationen
public:

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // Eingebundene Elemente der Steuerleiste
	CStatusBar  m_wndStatusBar;

// Generierte Message-Map-Funktionen
protected:
	CDialogBar m_wndMyDialogBar;
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnHilfeMH();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_MAINFRM_H__07844C91_0738_48D0_A00E_E330425FBF25__INCLUDED_)
