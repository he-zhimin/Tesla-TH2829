// SN.cpp : implementation file
//

#include "stdafx.h"
#include "TH2829Test.h"
#include "SN.h"
#include "afxdialogex.h"
//#include "stdlib.h"

// SN dialog

IMPLEMENT_DYNAMIC(SN, CDialog)

SN::SN(CWnd* pParent /*=NULL*/)
	: CDialog(SN::IDD, pParent)
	, m_sninput(_T(""))
{

}

SN::~SN()
{
}

void SN::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, SnEdit, m_sninput);
	DDX_Control(pDX, SnEdit, m_input2);
}


BEGIN_MESSAGE_MAP(SN, CDialog)
	ON_BN_CLICKED(IDOK, &SN::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &SN::OnBnClickedCancel)
END_MESSAGE_MAP()


// SN message handlers


BOOL SN::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_input2.SetFocus();
	m_input2.SetWindowTextW(_T(""));


	return FALSE;  // return TRUE unless you set the focus to a control

	// EXCEPTION: OCX Property Pages should return FALSE
}


void SN::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
CStdioFile file;
CString filename1,type,non;	
CTime Time;
CString neirong,psn,psntcp,psntcp2;
char bufk[256];
int panduan;
panduan=0;
type=".txt";
non="";
GetDlgItem(SnEdit)->GetWindowText(psn);
Time = CTime::GetCurrentTime();
	 filename1="D:\\TH2829Test\\";
		 filename1=filename1+"SNCHECK"+type;
	
	 file.Open(filename1,CFile::shareDenyNone|CFile::modeRead);
	 file.SeekToBegin();
	 while((file.ReadString(neirong))&&(panduan==0))
	 {
		if( neirong.Find(_T("SN:"))!=-1)
		{
		//AfxMessageBox(neirong,MB_OK|MB_ICONEXCLAMATION);
			int pos=neirong.Find(_T(":"));
//int pos2=psn.Find(_T("TCP"));
//_itoa_s(pos,bufk,10);
//AfxMessageBox(CString(bufk));
psntcp=neirong.Mid(pos+1,psn.GetLength());

//psntcp2=psn.Mid(pos2,15);		
//AfxMessageBox(psntcp,MB_OK|MB_ICONEXCLAMATION);
//AfxMessageBox(psn,MB_OK|MB_ICONEXCLAMATION);
if(psntcp==psn)
{
	m_input2.SetWindowTextW(_T(""));
	m_input2.SetFocus();
	panduan=1;
}
		
		}
	 
	 }

	 
	 if(panduan==1)	 
{
AfxMessageBox(_T("SN Duplicate"),MB_OK|MB_ICONEXCLAMATION);
file.Close();
	 }
if(panduan==0)
{
	//AfxMessageBox(_T("Wait Test"),MB_OK|MB_ICONEXCLAMATION);

	file.SeekToEnd();
	 file.Close();
	 CDialog::OnOK();
}
}


BOOL SN::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message==WM_KEYDOWN)
	{
	  switch (pMsg->wParam)
	  {
case VK_RETURN:
	//return true;
break;

	  }
	
	
	
	
	
	}
	
	
	
	
	
	
	return CDialog::PreTranslateMessage(pMsg);
}


void SN::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}
