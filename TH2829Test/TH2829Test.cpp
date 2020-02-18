
// TH2829Test.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "TH2829Test.h"
#include "TH2829TestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTH2829TestApp

BEGIN_MESSAGE_MAP(CTH2829TestApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CTH2829TestApp construction

CTH2829TestApp::CTH2829TestApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CTH2829TestApp object

CTH2829TestApp theApp;


// CTH2829TestApp initialization

BOOL CTH2829TestApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	HANDLE hMap=CreateFileMapping((HANDLE)0xFFFFFFFF,NULL,
PAGE_READWRITE,0,128,/*_T("RadomTest")*/_T("TH2829Test"));
if(hMap==NULL)
{
MessageBox(NULL,_T("FAIL"),_T("ERR"),MB_OK);
return FALSE;
}
else if(GetLastError()==ERROR_ALREADY_EXISTS)
{
LPVOID lpMem=MapViewOfFile(hMap,FILE_MAP_WRITE,0,0,0);
UnmapViewOfFile(lpMem);
CloseHandle(hMap);
MessageBox(NULL,_T("Programe Running"),_T("Waring"),MB_OK);
char lpszStr[64]="TEST07";
char m_strWinText[254];
CWnd *pPrevHwnd=NULL;
LPTSTR ff;
ff=(LPTSTR)m_strWinText;
pPrevHwnd=CWnd::GetDesktopWindow ()->GetWindow (GW_CHILD);
while(*pPrevHwnd)
{
pPrevHwnd->GetWindowText (ff,254);
//pPrevHwnd->GetWindowText(
if(strstr(m_strWinText,lpszStr)!=NULL)
{
if(pPrevHwnd->IsIconic())
pPrevHwnd->ShowWindow (SW_RESTORE);
pPrevHwnd->SetForegroundWindow ();

return FALSE;
}
pPrevHwnd=pPrevHwnd->GetWindow(GW_HWNDNEXT);
}
return FALSE;
}
else
{
LPVOID lpMem=MapViewOfFile(hMap, FILE_MAP_WRITE, 0,0,0);
UnmapViewOfFile(lpMem);
} 	
	
	
	
	
	
	
	
	
	
	
	
	
	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CTH2829TestDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	// Delete the shell manager created above.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

