
// TH2829TestDlg.h : header file
//

#pragma once
#include "afxwin.h"
UINT MyThreadFunction(LPVOID pParam);
#define WM_MYUPDATEDATA WM_USER+100
// CTH2829TestDlg dialog
class CTH2829TestDlg : public CDialogEx
{
// Construction
public:
	CTH2829TestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TH2829TEST_DIALOG };
	           
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg  LRESULT OnUpdateMyData(WPARAM wParam, LPARAM lParam);
	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedTimestart();
	afx_msg void OnClickedTimecancle();
	CListBox m_listbox1;
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnBnClickedButton1();
	CStatic m_testresult;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDataPass();
	afx_msg void OnDataFail();
	afx_msg void OnDataAll();
	CListBox m_listerr;
	afx_msg void OnClose();
	afx_msg void OnModelM0075();
	afx_msg void OnModelM1816();
	afx_msg void OnModelM1817();
	afx_msg void OnModelM1818();
	afx_msg void OnModelM1829();
	afx_msg void OnModelNosn();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnModelM1877();
};
