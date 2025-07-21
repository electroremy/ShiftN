// AdjustDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "ShiftN.h"
#include "AdjustDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld AdjustDlg 


AdjustDlg::AdjustDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AdjustDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AdjustDlg)
	m_shiftreg = 0;
	m_drehreg = 0;
	m_distoreg = 0;
	m_horizoreg = 0;
	m_shiftd = 0.0;
	m_drehd = 0.0;
	m_distd = 0.0;
	m_horizd = 0.0;
	m_reset_param = 0.0;
	m_reset_winkel = 0.0;
	m_reset_distortion = 0.0;
	m_reset_horizontal = 0.0;
	//}}AFX_DATA_INIT
}


void AdjustDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AdjustDlg)
	DDX_Slider(pDX, IDC_SLIDER1, m_shiftreg);
	DDX_Slider(pDX, IDC_SLIDER2, m_drehreg);
	DDX_Slider(pDX, IDC_SLIDER3, m_distoreg);
	DDX_Slider(pDX, IDC_SLIDER4, m_horizoreg);
	DDX_Text(pDX, IDC_EDIT1, m_shiftd);
	DDV_MinMaxDouble(pDX, m_shiftd, -1.6, 1.6);
	DDX_Text(pDX, IDC_EDIT2, m_drehd);
	DDV_MinMaxDouble(pDX, m_drehd, -8.0, 8.0);
	DDX_Text(pDX, IDC_EDIT3, m_distd);
	DDV_MinMaxDouble(pDX, m_distd, -1.6, 1.6);
	DDX_Text(pDX, IDC_EDIT4, m_horizd);
	DDV_MinMaxDouble(pDX, m_distd, -5.0, 5.0);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AdjustDlg, CDialog)
	//{{AFX_MSG_MAP(AdjustDlg)
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER1, OnReleasedcaptureSlider1)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER2, OnReleasedcaptureSlider2)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER3, OnReleasedcaptureSlider3)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER4, OnReleasedcaptureSlider4)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, OnCustomdrawSlider1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER2, OnCustomdrawSlider2)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER3, OnCustomdrawSlider3)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER4, OnCustomdrawSlider4)
	ON_EN_KILLFOCUS(IDC_EDIT1, OnKillfocusEdit1)
	ON_EN_KILLFOCUS(IDC_EDIT2, OnKillfocusEdit2)
	ON_EN_KILLFOCUS(IDC_EDIT3, OnKillfocusEdit3)
	ON_EN_KILLFOCUS(IDC_EDIT4, OnKillfocusEdit4)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &AdjustDlg::OnBnClickedButton1)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten AdjustDlg 

int AdjustDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

void AdjustDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CSliderCtrl* pRegler1 = (CSliderCtrl*) GetDlgItem(IDC_SLIDER1);
	CSliderCtrl* pRegler2 = (CSliderCtrl*) GetDlgItem(IDC_SLIDER2);
	CSliderCtrl* pRegler3 = (CSliderCtrl*) GetDlgItem(IDC_SLIDER3);
	CSliderCtrl* pRegler4 = (CSliderCtrl*) GetDlgItem(IDC_SLIDER4);
	CDialog::OnShowWindow(bShow, nStatus);
	pRegler1->EnableWindow(TRUE);
	pRegler1->SetTicFreq(250);
	pRegler1->SetRange(-2000,2000,TRUE);
	pRegler1->SetPos(m_shiftreg);
	pRegler2->EnableWindow(TRUE);
	pRegler2->SetTicFreq(250);
	pRegler2->SetRange(-2000,2000,TRUE);
	pRegler2->SetPos(m_drehreg);
	pRegler3->EnableWindow(TRUE);
	pRegler3->SetTicFreq(250);
	pRegler3->SetRange(-2000,2000,TRUE);
	pRegler3->SetPos(m_distoreg);
	pRegler4->EnableWindow(TRUE);
	pRegler4->SetTicFreq(400);
	pRegler4->SetRange(-2000,2000,TRUE);
	pRegler4->SetPos(m_distoreg);
	m_shiftd=m_shiftreg/1250.0;
	m_drehd=m_drehreg/250.0;
	m_distd=m_distoreg/1250.0;
	m_horizd=m_horizoreg/400.0;
	UpdateData(FALSE);
}

void AdjustDlg::OnReleasedcaptureSlider1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData(TRUE);
	m_shiftd=m_shiftreg/1250.0;
	UpdateData(FALSE);
	*pResult = 0;
}

void AdjustDlg::OnReleasedcaptureSlider2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData(TRUE);
	m_drehd=m_drehreg/250.0;
	UpdateData(FALSE);
	*pResult = 0;
}

void AdjustDlg::OnReleasedcaptureSlider3(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData(TRUE);
	m_distd=m_distoreg/1250.0;
	UpdateData(FALSE);
	*pResult = 0;
}

void AdjustDlg::OnReleasedcaptureSlider4(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData(TRUE);
	m_horizd=m_horizoreg/400.0;
	UpdateData(FALSE);
	*pResult = 0;
}

void AdjustDlg::OnCustomdrawSlider1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData(TRUE);
	m_shiftd=m_shiftreg/1250.0;
	UpdateData(FALSE);
	*pResult = 0;
}

void AdjustDlg::OnCustomdrawSlider2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData(TRUE);
	m_drehd=m_drehreg/250.0;
	UpdateData(FALSE);
	*pResult = 0;
}

void AdjustDlg::OnCustomdrawSlider3(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData(TRUE);
	m_distd=m_distoreg/1250.0;
	UpdateData(FALSE);
	*pResult = 0;
}

void AdjustDlg::OnCustomdrawSlider4(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData(TRUE);
	m_horizd=m_horizoreg/400.0;
	UpdateData(FALSE);
	*pResult = 0;
}

void AdjustDlg::OnKillfocusEdit1() 
{
	UpdateData(TRUE);
	m_shiftreg=(int) (m_shiftd*1250.0);
	UpdateData(FALSE);	
}

void AdjustDlg::OnKillfocusEdit2() 
{
	UpdateData(TRUE);
	m_drehreg=(int) (m_drehd*250.0);
	UpdateData(FALSE);	
}

void AdjustDlg::OnKillfocusEdit3() 
{
	UpdateData(TRUE);
	m_distoreg=(int) (m_distd*1250.0);
	UpdateData(FALSE);
}

void AdjustDlg::OnKillfocusEdit4() 
{
	UpdateData(TRUE);
	m_horizoreg=(int) (m_horizd*400.0);
	UpdateData(FALSE);
}

void AdjustDlg::OnBnClickedButton1()
{
	m_distd=m_reset_distortion;
	m_shiftd=m_reset_param;
	m_drehd=m_reset_winkel*180/3.1415926535897932;
	m_horizd=m_reset_horizontal;
	m_shiftreg=(int) (m_shiftd*1250.0);
	m_drehreg=(int) (m_drehd*250.0);
	m_distoreg=(int) (m_distd*1250.0);
	m_horizoreg=(int) (m_horizd*400.0);
	m_distd=((double) ((int) (1000*m_distd+0.5)))/1000.0;
	m_drehd=((double) ((int) (1000*m_drehd+0.5)))/1000.0;
	m_shiftd=((double) ((int) (1000*m_shiftd+0.5)))/1000.0;
	m_horizd=((double) ((int) (1000*m_horizd+0.5)))/1000.0;
	UpdateData(FALSE);
}
