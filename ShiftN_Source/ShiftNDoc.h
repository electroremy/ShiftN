// ShiftNDoc.h : Schnittstelle der Klasse CShiftNDoc
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHIFTNDOC_H__AA6ADC9C_1AA5_476B_B4B0_F6FCFF714F4C__INCLUDED_)
#define AFX_SHIFTNDOC_H__AA6ADC9C_1AA5_476B_B4B0_F6FCFF714F4C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CShiftNDoc : public CDocument
{
protected: // Nur aus Serialisierung erzeugen
	CShiftNDoc();
	DECLARE_DYNCREATE(CShiftNDoc)

// Attribute
public:

// Operationen
public:

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CShiftNDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CShiftNDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generierte Message-Map-Funktionen
protected:
	//{{AFX_MSG(CShiftNDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_SHIFTNDOC_H__AA6ADC9C_1AA5_476B_B4B0_F6FCFF714F4C__INCLUDED_)
