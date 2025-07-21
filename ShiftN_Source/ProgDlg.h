// ProgDlg.h : Header-Datei
// CG: Diese Datei wurde von der Komponente "Dialogfeld Fortschritt" hinzugefügt

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg Dialogfeld

#ifndef __PROGDLG_H__
#define __PROGDLG_H__

class CProgressDlg : public CDialog
{
// Konstruktion / Destruktion
public:
    CProgressDlg(UINT nCaptionID = 0);   // Standard-Konstruktor
    ~CProgressDlg();

    BOOL Create(CWnd *pParent=NULL);

    // Anpassen des Dialogfelds Fortschritt
    void SetRange(int nLower,int nUpper);
    int  SetStep(int nStep);
    int  SetPos(int nPos);
    int  OffsetPos(int nPos);
    int  StepIt();
	void SetCaptionText(CString strCapt);
        
// Dialogfelddaten
    //{{AFX_DATA(CProgressDlg)
    enum { IDD = CG_IDD_PROGRESS };
    CProgressCtrl	m_Progress;
    //}}AFX_DATA

// Überschreibungen
    // Vom Klassen-Assistenten generierte Überschreibungen virtueller Funktionen
    //{{AFX_VIRTUAL(CProgressDlg)
    public:
    virtual BOOL DestroyWindow();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
    //}}AFX_VIRTUAL

// Implementierung
protected:
	UINT m_nCaptionID;
    int m_nLower;
    int m_nUpper;
    int m_nStep;
    BOOL m_bParentDisabled;

    void ReEnableParent();

    virtual void OnCancel();
    virtual void OnOK() {}; 
    void PumpMessages();

    // Generierte Nachrichtenzuordnungsfunktionen
    //{{AFX_MSG(CProgressDlg)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif // __PROGDLG_H__
