#pragma once


// AddSet dialog

class AddSet : public CDialog
{
	DECLARE_DYNAMIC(AddSet)

public:
	AddSet(CWnd* pParent = NULL);   // standard constructor
	virtual ~AddSet();

// Dialog Data
	enum { IDD = IDD_COMSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
