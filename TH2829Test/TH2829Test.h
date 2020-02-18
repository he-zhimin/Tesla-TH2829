
// TH2829Test.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CTH2829TestApp:
// See TH2829Test.cpp for the implementation of this class
//

class CTH2829TestApp : public CWinApp
{
public:
	CTH2829TestApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CTH2829TestApp theApp;