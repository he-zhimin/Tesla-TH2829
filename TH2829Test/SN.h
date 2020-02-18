#pragma once
#include "afxwin.h"


// SN dialog

class SN : public CDialog
{
	DECLARE_DYNAMIC(SN)

public:
	SN(CWnd* pParent = NULL);   // standard constructor
	virtual ~SN();

// Dialog Data
	enum { IDD = IDD_SN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_sninput;
	virtual BOOL OnInitDialog();
	CEdit m_input2;
	afx_msg void OnBnClickedOk();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedCancel();
};
