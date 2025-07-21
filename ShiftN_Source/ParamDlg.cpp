// ParamDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "ShiftN.h"
#include "ParamDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld ParamDlg 


ParamDlg::ParamDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ParamDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ParamDlg)
	m_minlen = 0;
	m_mincon = 0;
	m_maxdisp = 0;
	m_shiftamount = 0;
	m_respara=0;
	m_dorot = FALSE;
	m_ori = FALSE;
	m_parlot = FALSE;
	m_distortion = 0.0;
	m_flen = 0;
	m_horifac = 0.0;
	//}}AFX_DATA_INIT
}


void ParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ParamDlg)
	DDX_Text(pDX, IDC_EDIT15, m_respara);
	DDV_MinMaxInt(pDX, m_respara, 100, 32767);
	DDX_Text(pDX, IDC_EDIT6, m_minlen);
	DDV_MinMaxInt(pDX, m_minlen, 0, 32767);
	DDX_Text(pDX, IDC_EDIT2, m_mincon);
	DDV_MinMaxInt(pDX, m_mincon, 1, 255);
	DDX_Text(pDX, IDC_EDIT9, m_maxdisp);
	DDV_MinMaxInt(pDX, m_maxdisp, 5, 45);
	DDX_Text(pDX, IDC_EDIT11, m_shiftamount);
	DDV_MinMaxInt(pDX, m_shiftamount, 0, 200);
	DDX_Check(pDX, IDC_CHECK1, m_dorot);
	DDX_Check(pDX, IDC_CHECK2, m_ori);
	DDX_Check(pDX, IDC_CHECK3, m_parlot);
	DDX_Text(pDX, IDC_EDIT12, m_distortion);
	DDV_MinMaxDouble(pDX, m_distortion, -1.0, 1.0);
	DDX_Text(pDX, IDC_EDIT13, m_flen);
	DDV_MinMaxInt(pDX, m_flen, 8, 200);
	DDX_Text(pDX, IDC_EDIT14, m_horifac);
	DDV_MinMaxDouble(pDX, m_horifac, -5.0, 5.0);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ParamDlg, CDialog)
	//{{AFX_MSG_MAP(ParamDlg)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHECK2, &ParamDlg::OnBnClickedCheck2)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten ParamDlg 


void ParamDlg::OnButton1() 
{
	m_minlen=0;
	m_mincon=3;
	m_maxdisp=30;
	m_shiftamount=100;
	m_dorot=1;
	m_respara=1200;
	m_ori=0;
	m_parlot=0;
	m_distortion=0.0;
	m_horifac=0.0;
	m_flen=28;
	UpdateData(FALSE);
}

void ParamDlg::OnBnClickedCheck2()
{
	// TODO: Add your control notification handler code here
}
