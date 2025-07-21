// DarstDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "ShiftN.h"
#include "DarstDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld DarstDlg 


DarstDlg::DarstDlg(CWnd* pParent /*=NULL*/)
	: CDialog(DarstDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(DarstDlg)
	m_check_jpg = FALSE;
	m_check_bmp = FALSE;
	m_check_txt = FALSE;
	m_beschneiden = FALSE;
	m_schaerfen = FALSE;
	m_usebackrgb = FALSE;
	m_backrgb = 0;
	m_scharfpar = 0.0;
	m_jpegqualitaet = 0;
	m_use_exif_date = FALSE;
	m_kubik = FALSE;
	m_dsuffix = _T("");
	//}}AFX_DATA_INIT
}

void DarstDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DarstDlg)
	DDX_Check(pDX, IDC_CHECK4, m_check_jpg);
	DDX_Check(pDX, IDC_CHECK5, m_check_bmp);
	DDX_Check(pDX, IDC_CHECK6, m_check_txt);
	DDX_Check(pDX, IDC_CHECK7, m_beschneiden);
	DDX_Check(pDX, IDC_CHECK8, m_schaerfen);
	DDX_Check(pDX, IDC_CHECK10, m_use_exif_date);
	DDX_Check(pDX, IDC_CHECK11, m_usebackrgb);
	DDX_Text(pDX, IDC_EDIT3, m_scharfpar);
	DDV_MinMaxDouble(pDX, m_scharfpar, 0., 5.);
	DDX_Text(pDX, IDC_EDIT9, m_backrgb);
	DDV_MinMaxInt(pDX, m_backrgb, 0, 16777215);
	DDX_Text(pDX, IDC_EDIT4, m_jpegqualitaet);
	DDV_MinMaxInt(pDX, m_jpegqualitaet, 2, 100);
	DDX_Check(pDX, IDC_CHECK9, m_kubik);
	DDX_Check(pDX, IDC_CHECK11, m_usebackrgb);
	DDX_Text(pDX, IDC_EDIT6, m_dsuffix);
	DDV_MaxChars(pDX, m_dsuffix, 16);
	//}}AFX_DATA_MAP
	CEdit* Edit3Visible=(CEdit*) GetDlgItem(IDC_EDIT3);
	if (m_schaerfen) {
		Edit3Visible->EnableWindow(TRUE);
	}
	else {
		Edit3Visible->EnableWindow(FALSE);
	}
	CEdit* Edit4Visible=(CEdit*) GetDlgItem(IDC_EDIT4);
	if (m_check_jpg) {
		Edit4Visible->EnableWindow(TRUE);
	}
	else {
		Edit4Visible->EnableWindow(FALSE);
	}
	CEdit* Edit9Visible=(CEdit*) GetDlgItem(IDC_EDIT9);
	if (m_usebackrgb) {
		Edit9Visible->EnableWindow(TRUE);
	}
	else {
		Edit9Visible->EnableWindow(FALSE);
	}
}

BEGIN_MESSAGE_MAP(DarstDlg, CDialog)
	//{{AFX_MSG_MAP(DarstDlg)
	ON_BN_CLICKED(IDC_CHECK4, OnCheck4)
	ON_BN_CLICKED(IDC_CHECK5, OnCheck5)
	ON_BN_CLICKED(IDC_CHECK6, OnCheck6)
	ON_BN_CLICKED(IDC_CHECK7, OnCheck7)
	ON_BN_CLICKED(IDC_CHECK8, OnCheck8)
	ON_BN_CLICKED(IDC_CHECK9, OnCheck9)
	ON_EN_KILLFOCUS(IDC_EDIT3, OnKillfocusEdit3)
	ON_EN_KILLFOCUS(IDC_EDIT4, OnKillfocusEdit4)
	ON_EN_KILLFOCUS(IDC_EDIT6, OnKillfocusEdit6)
	ON_EN_KILLFOCUS(IDC_EDIT9, OnKillfocusEdit9)
	ON_BN_CLICKED(IDC_CHECK10, OnCheck10)
	ON_BN_CLICKED(IDC_CHECK11, OnCheck11)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten DarstDlg 

void DarstDlg::OnCheck4() {
	CEdit* Edit4Visible=(CEdit*) GetDlgItem(IDC_EDIT4);
	m_check_jpg=!m_check_jpg;
	UpdateData(FALSE);
	if (m_check_jpg) {
		Edit4Visible->EnableWindow(TRUE);
	}
	else {
		Edit4Visible->EnableWindow(FALSE);
	}
}

void DarstDlg::OnCheck5() {
	m_check_bmp=!m_check_bmp;
	UpdateData(FALSE);
}

void DarstDlg::OnCheck6() {
	m_check_txt=!m_check_txt;
	UpdateData(FALSE);
}

void DarstDlg::OnCheck7() {
	m_beschneiden=!m_beschneiden;
	UpdateData(FALSE);
}

void DarstDlg::OnCheck9() {
	m_kubik=!m_kubik;
	UpdateData(FALSE);
}

void DarstDlg::OnCheck8() {
	CEdit* Edit3Visible=(CEdit*) GetDlgItem(IDC_EDIT3);
	m_schaerfen=!m_schaerfen;
	UpdateData(FALSE);
	if (m_schaerfen) {
		Edit3Visible->EnableWindow(TRUE);
	}
	else {
		Edit3Visible->EnableWindow(FALSE);
	}
}

void DarstDlg::OnCheck11() {
	CEdit* Edit9Visible=(CEdit*) GetDlgItem(IDC_EDIT9);
	m_usebackrgb=!m_usebackrgb;
	UpdateData(FALSE);
	if (m_usebackrgb) {
		Edit9Visible->EnableWindow(TRUE);
	}
	else {
		Edit9Visible->EnableWindow(FALSE);
	}
}

void DarstDlg::OnOK() 
{
	speichertyp=0;
	besch=0;
	sharp=0;
	usebackrgb=0;
	kub=0;
	useexifdate=0;
	if (m_check_jpg==TRUE) speichertyp+=1;
	if (m_check_bmp==TRUE) speichertyp+=2;
	if (m_check_txt==TRUE) speichertyp+=4;
	if (m_beschneiden==TRUE) besch=1;
	if (m_schaerfen==TRUE) sharp=1;
	if (m_usebackrgb==TRUE) usebackrgb=1;
	if (m_kubik==TRUE) kub=1;
	if (m_use_exif_date==TRUE) useexifdate=1;
	CDialog::OnOK();
}

void DarstDlg::OnKillfocusEdit3() 
{
	UpdateData(TRUE);	
}

void DarstDlg::OnKillfocusEdit4() 
{
	UpdateData(TRUE);
}

void DarstDlg::OnKillfocusEdit6() 
{
	UpdateData(TRUE);
}

void DarstDlg::OnKillfocusEdit9() 
{
	UpdateData(TRUE);
}

void DarstDlg::OnCheck10() 
{
	m_use_exif_date=!m_use_exif_date;
	UpdateData(FALSE);
}