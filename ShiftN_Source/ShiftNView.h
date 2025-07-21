// ShiftNView.h : Schnittstelle der Klasse CShiftNView
//
/////////////////////////////////////////////////////////////////////////////

#undef FAR

#if !defined(AFX_SHIFTNVIEW_H__81C9C6D6_853F_4B8C_A993_E824F347600A__INCLUDED_)
#define AFX_SHIFTNVIEW_H__81C9C6D6_853F_4B8C_A993_E824F347600A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CShiftNView : public CView
{
protected: // Nur aus Serialisierung erzeugen
	CShiftNView();
	DECLARE_DYNCREATE(CShiftNView)

// Attribute
public:
	CShiftNDoc* GetDocument();

// Operationen
public:

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CShiftNView)
	public:
	virtual void OnDraw(CDC* pDC);  // überladen zum Zeichnen dieser Ansicht
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnFinalRelease();
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave();
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	protected:
	virtual BOOL OnScrollBy(CSize sizeScroll, BOOL bDoScroll = TRUE);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CShiftNView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generierte Message-Map-Funktionen
protected:
	//{{AFX_MSG(CShiftNView)
	afx_msg void OnSave();
	afx_msg void OnSaveAs();
	afx_msg void OnSaveExit();
	afx_msg void OnLoad();
	afx_msg void OnParam();
	afx_msg void OnCtrlC();
	afx_msg void OnCtrlV();
	afx_msg void OnGo();
	afx_msg void OnGitter();
	afx_msg void Reoptimize();
	afx_msg void OnBatch();
	afx_msg void OnDarst();
	afx_msg void OnLang();
	afx_msg void OnDownMH();
	afx_msg void OnUpMH();
	afx_msg void OnSchaltfl();
	afx_msg void OnAnwendenKorrekturanpassen();
	afx_msg void OnGoB();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDelMH();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnUpdateGitter(CCmdUI* pCmdUI);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // Testversion in ShiftNView.cpp
inline CShiftNDoc* CShiftNView::GetDocument()
   { return (CShiftNDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_SHIFTNVIEW_H__81C9C6D6_853F_4B8C_A993_E824F347600A__INCLUDED_)
