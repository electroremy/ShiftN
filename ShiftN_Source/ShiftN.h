// ShiftN.h : Haupt-Header-Datei f�r die Anwendung SHIFTN
//

#if !defined(AFX_SHIFTN_H__7B6802F7_3227_4975_8723_7C39D7D56895__INCLUDED_)
#define AFX_SHIFTN_H__7B6802F7_3227_4975_8723_7C39D7D56895__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // Hauptsymbole

/////////////////////////////////////////////////////////////////////////////
// CShiftNApp:
// Siehe ShiftN.cpp f�r die Implementierung dieser Klasse
//

class CShiftNApp : public CWinApp
{
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CShiftNApp();

// �berladungen
	// Vom Klassenassistenten generierte �berladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CShiftNApp)
	public:
	virtual BOOL InitInstance();
	virtual void OnFinalRelease();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementierung
	//{{AFX_MSG(CShiftNApp)
	afx_msg void OnAppAbout();
	afx_msg void OnHinweise();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f�gt unmittelbar vor der vorhergehenden Zeile zus�tzliche Deklarationen ein.

#endif // !defined(AFX_SHIFTN_H__7B6802F7_3227_4975_8723_7C39D7D56895__INCLUDED_)
