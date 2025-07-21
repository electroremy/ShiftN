// ShiftNDoc.cpp : Implementierung der Klasse CShiftNDoc
//

#include "stdafx.h"
#include "ShiftN.h"

#include "ShiftNDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShiftNDoc

IMPLEMENT_DYNCREATE(CShiftNDoc, CDocument)

BEGIN_MESSAGE_MAP(CShiftNDoc, CDocument)
	//{{AFX_MSG_MAP(CShiftNDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShiftNDoc Konstruktion/Destruktion

CShiftNDoc::CShiftNDoc()
{
}

CShiftNDoc::~CShiftNDoc()
{
}

BOOL CShiftNDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument()) return FALSE;
	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CShiftNDoc Serialisierung

void CShiftNDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
	}
}

/////////////////////////////////////////////////////////////////////////////
// CShiftNDoc Diagnose

#ifdef _DEBUG
void CShiftNDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CShiftNDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CShiftNDoc Befehle
