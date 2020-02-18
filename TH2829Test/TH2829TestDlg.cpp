
// TH2829TestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TH2829Test.h"
#include "TH2829TestDlg.h"
#include "afxdialogex.h"
#include "NI4882.h"
//#include "visa.h"
#include <cmath>
#include <fstream>
#include <regex>
#include <vector>
#include "stdlib.h"
#include "SN.h"


using namespace std;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About
CWinThread* MyThread;
LARGE_INTEGER freq;
LARGE_INTEGER btime;
LARGE_INTEGER etime;
int costtime,ma,snselect,equipmentok;
double costtime2;
HANDLE hCom;
int Char16ToInt(char c);
long long int StrToNumber16(const char *str) ;
void hextochar(char* in,char* out);
void SavedataPass(CString filename);
void SavedataFail(CString filename);
void SavedataAll(CString filename);
HHOOK hkb = NULL;
CString hexstring[1893],productsn,modelname;
BOOL savepass,savefail,saveall;
BOOL testflag;
CStdioFile file,filex2;
char THadd[256],THadd2[256];
struct Date {
    int year;
    int month;
    int day;
};
int Cal_Day();
void RS232Open();
void Diskey();
void Enkey();

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDataPass();
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_DATA_PASS, &CAboutDlg::OnDataPass)
END_MESSAGE_MAP()


// CTH2829TestDlg dialog



CTH2829TestDlg::CTH2829TestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTH2829TestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTH2829TestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listbox1);
	DDX_Control(pDX, IDC_TestResult, m_testresult);
	DDX_Control(pDX, IDC_LISTERR, m_listerr);
}

BEGIN_MESSAGE_MAP(CTH2829TestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CTH2829TestDlg::OnBnClickedOk)
	ON_MESSAGE(WM_MYUPDATEDATA,OnUpdateMyData)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_TimeStart, &CTH2829TestDlg::OnBnClickedTimestart)
	ON_BN_CLICKED(IDC_TimeCancle, &CTH2829TestDlg::OnClickedTimecancle)
	ON_LBN_SELCHANGE(IDC_LIST1, &CTH2829TestDlg::OnLbnSelchangeList1)
	ON_BN_CLICKED(IDC_BUTTON1, &CTH2829TestDlg::OnBnClickedButton1)
	ON_WM_CTLCOLOR()
	ON_COMMAND(ID_DATA_PASS, &CTH2829TestDlg::OnDataPass)
	ON_COMMAND(ID_DATA_FAIL, &CTH2829TestDlg::OnDataFail)
	ON_COMMAND(ID_DATA_ALL, &CTH2829TestDlg::OnDataAll)
	ON_WM_CLOSE()
	ON_COMMAND(ID_MODEL_M0075, &CTH2829TestDlg::OnModelM0075)
	ON_COMMAND(ID_MODEL_M1816, &CTH2829TestDlg::OnModelM1816)
	ON_COMMAND(ID_MODEL_M1817, &CTH2829TestDlg::OnModelM1817)
	ON_COMMAND(ID_MODEL_M1818, &CTH2829TestDlg::OnModelM1818)
	ON_COMMAND(ID_MODEL_M1829, &CTH2829TestDlg::OnModelM1829)
	ON_COMMAND(ID_MODEL_NOSN, &CTH2829TestDlg::OnModelNosn)
	ON_BN_CLICKED(IDC_BUTTON2, &CTH2829TestDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON4, &CTH2829TestDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDCANCEL, &CTH2829TestDlg::OnBnClickedCancel)
	ON_COMMAND(ID_MODEL_M1877, &CTH2829TestDlg::OnModelM1877)
END_MESSAGE_MAP()


// CTH2829TestDlg message handlers

BOOL CTH2829TestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CString  strSerialList[256];      // 临时定义 256 个字符串组，因为系统最多也就 256 个  
HKEY hKey;  
LPCTSTR data_Set=_T("HARDWARE\\DEVICEMAP\\SERIALCOMM\\");  
long ret0 = (::RegOpenKeyEx(HKEY_LOCAL_MACHINE, data_Set, 0, KEY_READ, &hKey));  
if(ret0 != ERROR_SUCCESS)  
{  
return -1;  
}  
int ix = 0;  
//TCHAR *Name=NULL;  
char Name[25];
BYTE szPortName[25];  
LONG Status;  
DWORD dwIndex = 0;  
DWORD dwName;  
DWORD dwSizeofPortName;  
DWORD Type;  
LPWSTR text;
int x22;
dwName = 25;  
dwSizeofPortName = 25;  

do  
{  
Status = RegEnumValueA(hKey, dwIndex++, Name, &dwName, NULL, &Type,  
      szPortName, &dwSizeofPortName);  
if((Status == ERROR_SUCCESS)||(Status == ERROR_MORE_DATA))  
{  
strSerialList[ix] = CString(szPortName);   // 串口字符串保存  
ix++;// 串口计数  
   }  
} while((Status == ERROR_SUCCESS)||(Status == ERROR_MORE_DATA));  
RegCloseKey(hKey);  
	//AfxMessageBox(strSerialList[0]);
for(x22=0;x22<ix;x22++)
{
	//AfxMessageBox(equipmentbuf[x]);
	m_listbox1.AddString(strSerialList[x22]);
	
}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	snselect=1;
	Cal_Day();
	if(!PathIsDirectory(_T("D:\\TH2829RESULTBin")))
	{
		CreateDirectory(_T("D:\\TH2829RESULTBin"),NULL);
	}
	char *equipment;
//equipment=Gpiblist();	
 CString thadd2;

//Gpib_Init();
//AfxMessageBox(CString(equipment));
char *p2;
char *pnext;
char str4[256];
char bufk[256];
int i=0,x;
/*CString equipmentbuf[20];
p2=strtok_s(equipment,",",&pnext);
equipmentbuf[0]=CString(p2);
//MessageBox(p,"Title",MB_OK);
//Curr[0]=atof(p2);
//strpath[1]=CString(pnext);
//AfxMessageBox(equipmentbuf[0]);
strcpy_s(str4,pnext);
i=1;
 while(p2!=NULL)
{
  p2=strtok_s(str4,",",&pnext);
  //AfxMessageBox(CString(p2));
  if(p2!=NULL)
 {
   //MessageBox(p,"Title",MB_OK);
    //strcpy(cfgpath,p);
	//MessageBox(cfgpath,"Title",MB_OK);  
	 equipmentbuf[i]=CString(p2);
	 strcpy_s(str4,pnext);
	
   i++;
  }
}

for(x=0;x<i;x++)
{
	//AfxMessageBox(equipmentbuf[x]);
	m_listbox1.AddString(equipmentbuf[x]);
	
}*/
thadd2=_T("ASRL1::INSTR");

//AfxMessageBox(thadd2);

//_itoa_s(i,bufk,10);
//AfxMessageBox(CString(bufk));
if(!PathIsDirectory(_T("D:\\TH2829Test")))
{
	::CreateDirectory(_T("D:\\TH2829Test"),NULL);
}

CFont *m_font;
m_font=new CFont;	

m_font->CreateFont(25,15,0,0,700,FALSE,FALSE,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_SWISS, _T("Calibri"));  
//m_font.CreatePointFont(800,"Ш砰");
GetDlgItem(IDC_TestResult)->SetFont(m_font);		
//GetDlgItem(IDC_STATUE)->ShowWindow(SW_HIDE);
CFont *m_font2;
m_font2=new CFont;	

m_font2->CreateFont(25,15,0,0,700,FALSE,FALSE,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_SWISS, _T("Calibri"));  
//m_font.CreatePointFont(800,"Ш砰");
GetDlgItem(SNshow)->SetFont(m_font2);	
int eindx;
x=0;
//do
/*while((x<i)&&(equipmentbuf[x].Find(thadd2)==-1))
{
	//equipmentbuf[x].Find(_T("USB"))!=NULL;
	//AfxMessageBox(equipmentbuf[x]);
	x++;
}//while((x<i)&&(equipmentbuf[x].Find(_T("USB"))!=NULL));
//_itoa_s(x,bufk,10);
//AfxMessageBox(CString(bufk));
if(x>=i)
{
GetDlgItem(IDC_TestResult)->SetWindowText(_T("No Instrument"));
GetDlgItem(IDOK)->EnableWindow(FALSE);
}*/
 LPCTSTR p1;
 char * userc;
 CString thadd;
 thadd="com1";
 /* if(x<i)
{
	
	GetDlgItem(IDC_TestResult)->SetWindowText(_T("Instrument Ready"));
	thadd=equipmentbuf[x];
	//AfxMessageBox(thadd);
	p1=thadd.GetBuffer(0);
	thadd.ReleaseBuffer();
	
	userc = new char [thadd.GetLength()+1]; 
	strcpy_s(userc,thadd.GetLength()+1, CT2CA(p1));
	strcpy_s(THadd,userc);

}*/
/*if(x<i)
{
	
	GetDlgItem(IDC_TestResult)->SetWindowText(_T("Instrument Ready"));
//	thadd=equipmentbuf[x];
	//AfxMessageBox(thadd);
	p1=thadd2.GetBuffer(0);
	thadd2.ReleaseBuffer();
	
	userc = new char [thadd2.GetLength()+1]; 
	strcpy_s(userc,thadd2.GetLength()+1, CT2CA(p1));
	strcpy_s(THadd,userc);

//Gpwrite(THadd,"RS232:PRINT ON\n");

}*/
 p1=thadd.GetBuffer(0);
	thadd.ReleaseBuffer();
	
	userc = new char [thadd.GetLength()+1]; 
	strcpy_s(userc,thadd.GetLength()+1, CT2CA(p1));
	strcpy_s(THadd,userc);
 
 /*char buf[256],buf1[256];
 //thadd2=CString(equipmentbuf[x]);
	//AfxMessageBox(thadd);
	p1=thadd2.GetBuffer(0);
	thadd2.ReleaseBuffer();
	
	userc = new char [thadd2.GetLength()+1]; 
	strcpy_s(userc,thadd2.GetLength()+1, CT2CA(p1));
	strcpy_s(THadd2,userc);
 Gpread(THadd2,"*IDN?\n",buf);	
 AfxMessageBox(CString(buf));*/
 savepass=true;
saveall=false;
savefail=false;
CTime Time;
CString filename1,type,non,filename2;	
type=".txt";
non="";
Time = CTime::GetCurrentTime();
	 filename1="D:\\TH2829Test\\";
	 filename1=filename1+Time.Format("%Y")+type;
	 	
	/* if(file.Open(filename1,CFile::modeNoTruncate|CFile::modeWrite)==NULL)
{
file.Open(filename1,CFile::modeCreate|CFile::modeWrite);
}
else
{
	file.SeekToEnd();
file.WriteString(non+(char)0x0d+(char)0x0a+(char)0x0d+(char)0x0a+(char)0x0d+(char)0x0a+(char)0x0d+(char)0x0a);
//file.WriteString(_T("2"));
}
	*/
//AfxMessageBox(Time.Format("%Y-%m-%d"));	 
 filename2="D:\\TH2829Test\\";
	 filename2=filename2+"SNCHECK"+type;
	 	
	 if(filex2.Open(filename2,CFile::modeNoTruncate|CFile::modeWrite)==NULL)
{
filex2.Open(filename2,CFile::modeCreate|CFile::modeWrite);
filex2.WriteString(_T("SN CHECK"));
filex2.WriteString(_T("\r\n"));
filex2.Close();	
	 }
else
{
	filex2.SeekToEnd();
filex2.Close();
	//file.WriteString(non+(char)0x0d+(char)0x0a+(char)0x0d+(char)0x0a+(char)0x0d+(char)0x0a+(char)0x0d+(char)0x0a);
//file.WriteString(_T("2"));
}	
	 
COMSTAT ComStat;
	DWORD dwErrorFlags;
	BOOL bWriteStat;
	DWORD dwBytesWrite;

QueryPerformanceFrequency(&freq);
 RS232Open();	
 if(equipmentok==1)	
 {
	GetDlgItem(IDC_TestResult)->SetWindowText(_T("Instrument Ready"));
 //m_listbox1.AddString(thadd2);
 ClearCommError(hCom,&dwErrorFlags,&ComStat);
	bWriteStat=WriteFile(hCom,"RS232:PRINT ON\n",15,& dwBytesWrite,NULL);
 }
 if(equipmentok==0)
 {
GetDlgItem(IDC_TestResult)->SetWindowText(_T("No Instrument"));
GetDlgItem(IDOK)->EnableWindow(FALSE);
 }
 GetDlgItem(IDC_BUTTON2)->SetFocus();
	return FALSE;  // return TRUE  unless you set the focus to a control
}

void CTH2829TestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTH2829TestDlg::OnPaint()
{
	
	
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTH2829TestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{

	PKBDLLHOOKSTRUCT pkh = (PKBDLLHOOKSTRUCT)lParam;
	//KBDLLHOOKSTRUCT p; 
	   //::MessageBox(0, (char*)lParam, TEXT("Title"), MB_OK|MB_ICONASTERISK );
   //LPKBDLLHOOKSTRUCT lpKBHookStruct;

	if (HC_ACTION == nCode)

	{
		// return 1;


			/*if((pkh->vkCode==65)||(pkh->vkCode==VK_NUMPAD1))
			{


			}*/
		switch (wParam)
		{
		
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
			switch (pkh->vkCode)
			{
			case VK_LWIN:
			case VK_RWIN:
			
				return 1;  // 吃掉消息
				break;
			}
			return 1;
			break;
		}



	}


	LRESULT RetVal = CallNextHookEx(hkb, nCode, wParam, lParam);
	return RetVal;
	//return 0;
}

void CTH2829TestDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	fstream binfile;
	char charlen[256],bufv[1892],bufhex[1893],bufxxx[1892],buf[1893],bufe[256],common[256];
CString thadd,xx,/*productsn,*/filename1,filename2,type,type2,hexname,neirong,fuhao;
char * userc;
int passj,failj,allj,errcount;
 int dx,i;
 char *cl;
 LPCTSTR p1;
unsigned char* bufx;
unsigned char tmp;
unsigned char bufx2[1893];
//BOOL testflag;
union
{
	float f;
	char buff[4];
}fdata;
SYSTEMTIME DateTime;
	CTime Time;
CByteArray hexdata;
//ViSession xi;
/*buf=(LPSTR)new char[1892];
 ZeroMemory(buf,1892);*/
// bufx=0;
passj=0;
failj=0;
allj=0;
fuhao=" "; 
type=".txt";
 type2="_";
 hexname="hex";

	 fstream fs;	
fs.open("D:\\TH2829RESULTBin\\TH2829RESULT.bin",ios::in|ios::binary);
if(!fs)
{
//AfxMessageBox(_T("fail"));
CStdioFile fileX(_T("D:\\TH2829RESULTBin\\TH2829RESULT.bin"),CFile::modeCreate);
}



if(m_listbox1.GetCount()>0)
{
SN sninput;	

 if(snselect==0)
 {
	 if(m_listerr.GetCount()>0)
 {
	 m_listerr.ResetContent();
 }
		
		Time = CTime::GetCurrentTime();
	 filename1="D:\\TH2829Test\\";
	// filename1=filename1+productsn+type2+Time.Format("%Y%m%d%H%M")+type2+hexname+type;
	// CStdioFile file2(filename1,CFile::modeCreate|CFile::modeWrite);	

	GetDlgItem(IDC_TestResult)->SetWindowText(_T("Testing"));

	COMSTAT ComStat;
	DWORD dwErrorFlags;
	BOOL bWriteStat;
	DWORD dwBytesWrite;
	unsigned char str_result[1893];
	DWORD wCount = 1893;//读取的字节数
	BOOL bReadStat;
	ClearCommError(hCom, &dwErrorFlags, &ComStat);
	bWriteStat = WriteFile(hCom, "*IDN?\n", 6, &dwBytesWrite, NULL);

	memset(bufe, '\0', 256);
	delay(100);
	bReadStat = ReadFile(hCom, bufe, wCount, &wCount, NULL);
	//	AfxMessageBox(CString(bufe));
	if (!bReadStat)
	{
		AfxMessageBox(_T("Read Com Fail!"));
	}
	PurgeComm(hCom, PURGE_TXABORT |
		PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	CString ee;
	ee = "";
	ee = CString(bufe);
	if (ee.Find(_T("T")) == -1)
	{
		failj = 1;
		m_listerr.AddString(_T("GPIB Fail"));
		GetDlgItem(IDC_TestResult)->SetWindowText(_T("FAIL"));
		//AfxMessageBox(_T("why1?"));
	}


	if ((testflag == true) && (ee.Find(_T("T")) != -1))
	{

		GetDlgItem(SNshow)->SetWindowText(productsn);

		if (m_listerr.GetCount() > 0)
		{
			m_listerr.ResetContent();
		}

		Time = CTime::GetCurrentTime();
		filename1 = "D:\\TH2829Test\\debug.txt";
		//	filename1 = filename1 + productsn + type2 + Time.Format("%Y%m%d%H%M") + type2 + hexname + type;
		//CStdioFile file2(filename1, CFile::modeCreate | CFile::modeWrite);

		GetDlgItem(IDC_TestResult)->SetWindowText(_T("Testing"));

		wCount = 1893;
		ClearCommError(hCom, &dwErrorFlags, &ComStat);
		bWriteStat = WriteFile(hCom, "TRIG\n", 5, &dwBytesWrite, NULL);
		delay(2800);

		bReadStat = ReadFile(hCom, str_result, wCount, &wCount, NULL);
		if (!bReadStat)
			AfxMessageBox(_T("Read Com Fail!"));
		PurgeComm(hCom, PURGE_TXABORT |
			PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

		for (i = 0; i < 1891; i++)
		{
			sprintf_s(buf, "%02X", str_result[i]);
			hexstring[i] = CString(buf);
			if ((i + 1) % 16 != 0)
			{
				neirong = neirong + hexstring[i] + fuhao;


			}
			else
			{
				neirong = neirong + hexstring[i] + (char)0x0d + (char)0x0a;


			}
		}
		//file2.WriteString(neirong);
		//file2.Close();

		if (hexstring[1730] == "01")
		{
			passj = 1;
			GetDlgItem(IDC_TestResult)->SetWindowText(_T("PASS"));
		}
		int i6, item;
		if (hexstring[1730] != "01")
		{
			i6 = 0;//turn
			while (((hexstring[1330 + i6] == "00") || (hexstring[1330 + i6] == "01")) && (i6 < 40))
			{
				i6 = i6 + 1;
			}
			if (i6 < 40)
			{
				failj = 1;
				m_listerr.AddString(_T("Turn Fail"));
				GetDlgItem(IDC_TestResult)->SetWindowText(_T("FAIL"));
			}

			i6 = 0;//lx
			while (((hexstring[1370 + i6] == "00") || (hexstring[1370 + i6] == "01")) && (i6 < 40))
			{
				i6 = i6 + 1;
			}
			if (i6 < 40)
			{
				failj = 1;
				m_listerr.AddString(_T("Lx Fail"));
				GetDlgItem(IDC_TestResult)->SetWindowText(_T("FAIL"));
			}

			i6 = 0;//lk
			while (((hexstring[1410 + i6] == "00") || (hexstring[1410 + i6] == "01")) && (i6 < 40))
			{
				i6 = i6 + 1;
			}
			if (i6 < 40)
			{
				failj = 1;
				m_listerr.AddString(_T("Lk Fail"));
				GetDlgItem(IDC_TestResult)->SetWindowText(_T("FAIL"));
			}

			i6 = 0;//cx
			while (((hexstring[1450 + i6] == "00") || (hexstring[1450 + i6] == "01")) && (i6 < 40))
			{
				i6 = i6 + 1;
			}
			if (i6 < 40)
			{
				failj = 1;
				m_listerr.AddString(_T("Cx Fail"));
				GetDlgItem(IDC_TestResult)->SetWindowText(_T("FAIL"));
			}

			i6 = 0;//dcr
			while (((hexstring[1490 + i6] == "00") || (hexstring[1490 + i6] == "01")) && (i6 < 40))
			{
				i6 = i6 + 1;
			}
			if (i6 < 40)
			{
				failj = 1;
				m_listerr.AddString(_T("DCR Fail"));
				GetDlgItem(IDC_TestResult)->SetWindowText(_T("FAIL"));
			}

			i6 = 0;//q
			while (((hexstring[1530 + i6] == "00") || (hexstring[1530 + i6] == "01")) && (i6 < 40))
			{
				i6 = i6 + 1;
			}
			if (i6 < 40)
			{
				failj = 1;
				m_listerr.AddString(_T("Q Fail"));
				GetDlgItem(IDC_TestResult)->SetWindowText(_T("FAIL"));
			}

			i6 = 0;//acr
			while (((hexstring[1570 + i6] == "00") || (hexstring[1570 + i6] == "01")) && (i6 < 40))
			{
				i6 = i6 + 1;
			}
			if (i6 < 40)
			{
				failj = 1;
				m_listerr.AddString(_T("ACR Fail"));
				GetDlgItem(IDC_TestResult)->SetWindowText(_T("FAIL"));
			}

			i6 = 0;//zx
			while (((hexstring[1610 + i6] == "00") || (hexstring[1610 + i6] == "01")) && (i6 < 40))
			{
				i6 = i6 + 1;
			}
			if (i6 < 40)
			{
				failj = 1;
				m_listerr.AddString(_T("Zx Fail"));
				GetDlgItem(IDC_TestResult)->SetWindowText(_T("FAIL"));
			}

			i6 = 0;//bl
			while (((hexstring[1650 + i6] == "00") || (hexstring[1650 + i6] == "01")) && (i6 < 40))
			{
				i6 = i6 + 1;
			}
			if (i6 < 40)
			{
				failj = 1;
				m_listerr.AddString(_T("BL Fail"));
				GetDlgItem(IDC_TestResult)->SetWindowText(_T("FAIL"));
			}

			i6 = 0;//ps
			while (((hexstring[1690 + i6] == "00") || (hexstring[1690 + i6] == "01")) && (i6 < 40))
			{
				i6 = i6 + 1;
			}
			if (i6 < 40)
			{
				failj = 1;
				m_listerr.AddString(_T("PS Fail"));
				GetDlgItem(IDC_TestResult)->SetWindowText(_T("FAIL"));
			}
		}
		//	Gpread4(THadd,"TRIG\n","D:\\TH2829RESULTBin\\TH2829RESULT.bin");

	}

 }
 if(snselect==1)	 
 {
	COMSTAT ComStat;
	DWORD dwErrorFlags;
	BOOL bWriteStat;
	DWORD dwBytesWrite;
	unsigned char str_result[1893];
	DWORD wCount=1893;//读取的字节数
	BOOL bReadStat; 

	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
	//Diskey();
	 //do
//{
	UnhookWindowsHookEx(hkb);
	hkb = NULL;
	if(sninput.DoModal()==IDOK)
		{
		productsn=sninput.m_sninput;
		m_listerr.AddString(productsn);
		testflag=true;
		
	}
		 else
		 {
testflag=false;

		 }
	//Gpread(THadd,"*IDN?\n",bufe);	 
//	if (!hkb)

	//{

		hkb = SetWindowsHookExW(WH_KEYBOARD_LL, (HOOKPROC)&KeyboardProc, theApp.m_hInstance, 0);
	//}
		if (testflag == true)
		{
			try
			{
				ClearCommError(hCom, &dwErrorFlags, &ComStat);
				bWriteStat = WriteFile(hCom, "*IDN?\n", 6, &dwBytesWrite, NULL);

				memset(bufe, '\0', 256);
				delay(100);
				bReadStat = ReadFile(hCom, bufe, wCount, &wCount, NULL);
				//	AfxMessageBox(CString(bufe));
				if (!bReadStat)
				{
					AfxMessageBox(_T("Read Com Fail!"));
				}
				PurgeComm(hCom, PURGE_TXABORT |
					PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
				//AfxMessageBox(_T("Purge"));
			}
			catch (exception& e)
			{
				AfxMessageBox(_T("Fail!"));

			}

		}


			CString ee;
			ee = "";
			ee = CString(bufe);
			if (ee.Find(_T("T")) == -1)
			{
				failj = 1;
				m_listerr.AddString(_T("GPIB Fail"));
				GetDlgItem(IDC_TestResult)->SetWindowText(_T("FAIL"));
				//AfxMessageBox(_T("why1?"));
			}

		
	if ((testflag == true) && (ee.Find(_T("T")) != -1))
	{

		GetDlgItem(SNshow)->SetWindowText(productsn);

		if (m_listerr.GetCount() > 0)
		{
			m_listerr.ResetContent();
		}

		Time = CTime::GetCurrentTime();
		filename1 = "D:\\TH2829Test\\debug.txt";
	//	filename1 = filename1 + productsn + type2 + Time.Format("%Y%m%d%H%M") + type2 + hexname + type;
		CStdioFile file2(filename1, CFile::modeCreate | CFile::modeWrite);
		// AfxMessageBox(filename1);
		/*m_listbox1.GetText(0,thadd);
	//AfxMessageBox(thadd);
	p1=thadd.GetBuffer(0);
		thadd.ReleaseBuffer();

		userc = new char [thadd.GetLength()+1];
		strcpy_s(userc,thadd.GetLength()+1, CT2CA(p1));
		strcpy_s(THadd,userc);*/
		//	Gpwrite(THadd,"RS232:PRINT ON");
		GetDlgItem(IDC_TestResult)->SetWindowText(_T("Testing"));
		/*if(modelname=="M1818")
		{AfxMessageBox(_T("Wait Test"),MB_OK|MB_ICONEXCLAMATION);}*/
		//	Gpwrite(THadd,"TRIG\n");
	//viScanf(xi,"%X",bufhex);
	//Gpread2(THadd,"TRS:DATA?",hexdata.GetData());
	//sprintf_s(bufhex, "%X",bufv);
		//Gpread4(THadd,"TRS:DATA?\n","D:\\TH2829RESULTBin\\TH2829RESULT.bin");
	//Gpread4(THadd,"TRIG\n","D:\\TH2829RESULTBin\\TH2829RESULT.bin");
		wCount = 1893;
		ClearCommError(hCom, &dwErrorFlags, &ComStat);
		bWriteStat = WriteFile(hCom, "TRIG\n", 5, &dwBytesWrite, NULL);
		delay(2800);
		/*ClearCommError(hCom,&dwErrorFlags,&ComStat);
		bWriteStat=WriteFile(hCom,"TRS:DATA?\n",10,& dwBytesWrite,NULL);

		memset(str_result,'\0',1893);
		delay(1990);*/
		bReadStat = ReadFile(hCom, str_result, wCount, &wCount, NULL);
		if (!bReadStat)
			AfxMessageBox(_T("Read Com Fail!"));
		PurgeComm(hCom, PURGE_TXABORT |
			PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
		//AfxMessageBox(CString(str_result));
			//delay(300);
		for (i = 0; i < 1891; i++)
		{
			sprintf_s(buf, "%02X", str_result[i]);
			hexstring[i] = CString(buf);
			if ((i + 1) % 16 != 0)
			{
				neirong = neirong + hexstring[i] + fuhao;


			}
			else
			{
				neirong = neirong + hexstring[i] + (char)0x0d + (char)0x0a;


			}
		}

		//binfile.open("D:\\TH2829RESULTBin\\TH2829RESULT.bin",ios::in|ios::binary);
	//binfile.read(bufhex,1893*sizeof(char));
	/*for( i=0;i<1891;i++)
	{


		//tmp=(unsigned char)bufhex[i];
		tmp=bufx2[i];
		sprintf_s(buf, "%02X",tmp);
		hexstring[i]=CString(buf);

		//AfxMessageBox(hexstring[i]);
		if((i+1)%16!=0)
		{
			neirong=neirong+hexstring[i]+fuhao;
			//AfxMessageBox(neirong);
			//filen.WriteString(hexstring[i]);

		}
		else
		{
			neirong=neirong+hexstring[i]+(char)0x0d+(char)0x0a;
			//AfxMessageBox(neirong2);
			//filen.WriteString(hexstring[i]);

		}
	//AfxMessageBox(neirong);
	}*/
		file2.WriteString(neirong);
		file2.Close();
		//binfile.close();
		//AfxMessageBox(hexstring[1730]);
		//CloseHandle(hCom);
		if (hexstring[1730] == "01")
		{
			passj = 1;
			//GetDlgItem(IDC_TestResult)->SetWindowText(_T("PASS"));
		}
		int i6, item;
		if (hexstring[1730] != "01")
		{
			i6 = 0;//turn
			while (((hexstring[1330 + i6] == "00") || (hexstring[1330 + i6] == "01")) && (i6 < 40))
			{
				i6 = i6 + 1;
			}
			if (i6 < 40)
			{
				failj = 1;
				m_listerr.AddString(_T("Turn Fail"));
				GetDlgItem(IDC_TestResult)->SetWindowText(_T("FAIL"));
			}

			i6 = 0;//lx
			while (((hexstring[1370 + i6] == "00") || (hexstring[1370 + i6] == "01")) && (i6 < 40))
			{
				i6 = i6 + 1;
			}
			if (i6 < 40)
			{
				failj = 1;
				m_listerr.AddString(_T("Lx Fail"));
				GetDlgItem(IDC_TestResult)->SetWindowText(_T("FAIL"));
			}

			i6 = 0;//lk
			while (((hexstring[1410 + i6] == "00") || (hexstring[1410 + i6] == "01")) && (i6 < 40))
			{
				i6 = i6 + 1;
			}
			if (i6 < 40)
			{
				failj = 1;
				m_listerr.AddString(_T("Lk Fail"));
				GetDlgItem(IDC_TestResult)->SetWindowText(_T("FAIL"));
			}

			i6 = 0;//cx
			while (((hexstring[1450 + i6] == "00") || (hexstring[1450 + i6] == "01")) && (i6 < 40))
			{
				i6 = i6 + 1;
			}
			if (i6 < 40)
			{
				failj = 1;
				m_listerr.AddString(_T("Cx Fail"));
				GetDlgItem(IDC_TestResult)->SetWindowText(_T("FAIL"));
			}

			i6 = 0;//dcr
			while (((hexstring[1490 + i6] == "00") || (hexstring[1490 + i6] == "01")) && (i6 < 40))
			{
				i6 = i6 + 1;
			}
			if (i6 < 40)
			{
				failj = 1;
				m_listerr.AddString(_T("DCR Fail"));
				GetDlgItem(IDC_TestResult)->SetWindowText(_T("FAIL"));
			}

			i6 = 0;//q
			while (((hexstring[1530 + i6] == "00") || (hexstring[1530 + i6] == "01")) && (i6 < 40))
			{
				i6 = i6 + 1;
			}
			if (i6 < 40)
			{
				failj = 1;
				m_listerr.AddString(_T("Q Fail"));
				GetDlgItem(IDC_TestResult)->SetWindowText(_T("FAIL"));
			}

			i6 = 0;//acr
			while (((hexstring[1570 + i6] == "00") || (hexstring[1570 + i6] == "01")) && (i6 < 40))
			{
				i6 = i6 + 1;
			}
			if (i6 < 40)
			{
				failj = 1;
				m_listerr.AddString(_T("ACR Fail"));
				GetDlgItem(IDC_TestResult)->SetWindowText(_T("FAIL"));
			}

			i6 = 0;//zx
			while (((hexstring[1610 + i6] == "00") || (hexstring[1610 + i6] == "01")) && (i6 < 40))
			{
				i6 = i6 + 1;
			}
			if (i6 < 40)
			{
				failj = 1;
				m_listerr.AddString(_T("Zx Fail"));
				GetDlgItem(IDC_TestResult)->SetWindowText(_T("FAIL"));
			}

			i6 = 0;//bl
			while (((hexstring[1650 + i6] == "00") || (hexstring[1650 + i6] == "01")) && (i6 < 40))
			{
				i6 = i6 + 1;
			}
			if (i6 < 40)
			{
				failj = 1;
				m_listerr.AddString(_T("BL Fail"));
				GetDlgItem(IDC_TestResult)->SetWindowText(_T("FAIL"));
			}

			i6 = 0;//ps
			while (((hexstring[1690 + i6] == "00") || (hexstring[1690 + i6] == "01")) && (i6 < 40))
			{
				i6 = i6 + 1;
			}
			if (i6 < 40)
			{
				failj = 1;
				m_listerr.AddString(_T("PS Fail"));
				GetDlgItem(IDC_TestResult)->SetWindowText(_T("FAIL"));
			}
		}

		CString passfilename, passstring, failfilename, failstring, allfilename, allstring;
		passstring = "PASS";
		failstring = "FAIL";
		allstring = "ALL";
		Time = CTime::GetCurrentTime();
		filename1 = "D:\\TH2829Test\\";
		passfilename = filename1 + productsn + type2 + Time.Format("%Y%m%d%H%M") + type2 + passstring + type;
		failfilename = filename1 + productsn + type2 + Time.Format("%Y%m%d%H%M") + type2 + failstring + type;
		allfilename = filename1 + productsn + type2 + Time.Format("%Y%m%d%H%M") + type2 + allstring + type;
		//AfxMessageBox(passfilename);
		if ((savepass == true) && (passj == 1) && (ee.Find(_T("T")) != -1))
		{
		//	this->SetFocus();
			SavedataPass(passfilename);
			GetDlgItem(IDC_TestResult)->SetWindowText(_T("PASS"));
			GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
			UnhookWindowsHookEx(hkb);
			hkb = NULL;
		
		}
		if (failj == 1){ UnhookWindowsHookEx(hkb); }
		if ((savefail == true) && (failj == 1))
		{
			SavedataFail(failfilename);
		}

		if (saveall == true)
		{
			SavedataAll(allfilename);
			GetDlgItem(IDC_TestResult)->SetWindowText(_T("PASS"));
			GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
			UnhookWindowsHookEx(hkb);
			hkb = NULL;
		}
		
}
Sleep(100);

//}while(testflag==true);

}

}

	
	//	CDialogEx::OnOK();
}

UINT MyThreadFunction( LPVOID pParam )
{
	int ms,s,min,hour,costtime3;
	CString msbuf,sbuf,minbuf,hourbuf,fuhao;
	char * userc;
	char buf[256];
	CString timebuf;
LPCTSTR p1;

	HWND hwnd; //希望找到资源管理器

	ms=0;
	s=0;
	min=0;
	hour=0;
	fuhao=":";
	//costtime2=0;
	hwnd=::FindWindow(NULL,_T("TH2829Test"));
	QueryPerformanceCounter(&btime);
	do{
//	QueryPerformanceCounter(&btime);
	do{
		QueryPerformanceCounter(&etime);
	//costtime=int((double)(etime.QuadPart-btime.QuadPart)/(double)freq.QuadPart);
	costtime2=(double)(etime.QuadPart-btime.QuadPart)/(double)freq.QuadPart;
	//costtime3=(int)costtime2*1000;
	if(costtime2*1000>=1000)
	{
	s=s+1;
	costtime2=0;
	QueryPerformanceCounter(&btime);
	}
if(s>=60)	
	{
	min=min+1;
	s=0;
	}
if(min>=60)	
	{
	hour=hour+1;
	min=0;
	}	
msbuf.Format(_T("%.0f"),costtime2*1000);
	sbuf.Format(_T("%d"),s);
	minbuf.Format(_T("%d"),min);
	hourbuf.Format(_T("%d"),hour);
	//AfxMessageBox(msbuf);
	
	
	timebuf=hourbuf+fuhao+minbuf+fuhao+sbuf+fuhao+msbuf;
	//AfxMessageBox(timebuf);
	p1=timebuf.GetBuffer(0);
	timebuf.ReleaseBuffer();
	
	userc = new char [timebuf.GetLength()+1]; 
	strcpy_s(userc,timebuf.GetLength()+1, CT2CA(p1));
//	AfxMessageBox(CString(userc));
	strcpy_s(buf,userc);
	SendMessage(hwnd, // 获得主窗口在句柄   
           WM_MYUPDATEDATA,  // 发送自己的窗口消息   
		   (WPARAM)&buf,     // 设置发送的内容   
             NULL);
	//x++;

	
	}while((costtime2*1000<1000)&&(ma==1));
	/*if(costtime2*1000>=1000)
	{
	s=s+1;
	}
	if(s>=60)	
	{
	min=min+1;
	s=0;
	}*/
		/*costtime2=0;
	QueryPerformanceCounter(&btime);*/
	//AfxMessageBox(_T("1000"));
	/*if(costtime2==1000)
	{
AfxMessageBox(_T("1000"));
	}*/
	
	/*SendMessage(hwnd, // 获得主窗口在句柄   
           WM_MYUPDATEDATA,  // 发送自己的窗口消息   
		   NULL,     // 设置发送的内容   
             NULL);*/
	Sleep(10);
	}while(ma==1);


	return 0;
}


LRESULT CTH2829TestDlg::OnUpdateMyData(WPARAM wParam, LPARAM lParam)
{
	//timebuf.Format(_T("%d"),costtime);
	char *str22= (char*)wParam;;
	CStatic* pst=(CStatic*)GetDlgItem(stopwatch);
	/*ms=0;
	s=0;
	min=0;
	hour=0;
	fuhao=":";*/
	//timebuf.Format(_T("%.0f"),costtime2*1000);
	//ms=_ttoi(timebuf);
	
		GetDlgItem(stopwatch)->SetWindowText(CString(str22));
	
		
	//::SendMessage(GetDlgItem(stopwatch)->GetSafeHwnd(),WM_SETTEXT,0,timebuf);
	
	RECT rect;
	pst->GetWindowRect(&rect);
	ScreenToClient(&rect);
	//InvalidateRect(&rect,FALSE);
	InvalidateRect(&rect,TRUE);
	//	Invalidate(FALSE);
return 0;
}

BOOL CAboutDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	
	
	
	//return TRUE;
	return CDialogEx::OnEraseBkgnd(pDC);
}


BOOL CTH2829TestDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
//	return TRUE;
	return CDialogEx::OnEraseBkgnd(pDC);
}


void CTH2829TestDlg::OnBnClickedTimestart()
{
	// TODO: Add your control notification handler code here
		ma=1;
MyThread=AfxBeginThread(MyThreadFunction,NULL,THREAD_PRIORITY_NORMAL,0,0,NULL);


}


void CTH2829TestDlg::OnClickedTimecancle()
{
	// TODO: Add your control notification handler code here
ma=0;

}

void CTH2829TestDlg::OnLbnSelchangeList1()
{
	// TODO: Add your control notification handler code here
}


void CTH2829TestDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
fstream binfile;	
char bufhex[1893],buf[1893];
char* buf2;
int readint,readint2,i;
unsigned char tmp;
long long  shu;
CString neirong,fuhao1,fuhao2,neirong2,hexstring2[1893],neirong3,databuf;
float actdata;
	char * userc;
char *userc1; 
LPCTSTR p1;
LPCTSTR pp1;
union
{
	float f;
	char buff[4];
}fdata;
neirong="";
neirong2="";
fuhao1=" ";
fuhao2="";
binfile.open("D:\\TH2829RESULT.bin",ios::in|ios::binary);
binfile.read(bufhex,1893*sizeof(char));
//_itoa_s(readint,bufhex,16);
//strcpy_s(fdata.buff,"1e6ada3a");
//hextochar("af53803f",fdata.buff);
/*fdata.buff[0]='c8';
fdata.buff[1]='4b';
fdata.buff[2]='80';
fdata.buff[3]='3f';*/
databuf="41b6851e";
pp1=databuf.GetBuffer(0);
	databuf.ReleaseBuffer();	
	userc1 = new char [databuf.GetLength()+1]; 
	strcpy_s(userc1,databuf.GetLength()+1, CT2CA(pp1));
hextochar(userc1,fdata.buff);

AfxMessageBox(CString(fdata.buff));
/*shu=StrToNumber16("fb0862bf");
actdata=hextof(shu);*/
sprintf_s(buf, "%f",fdata.f);
AfxMessageBox(CString(buf));
CStdioFile filen(_T("D:\\bufresult.txt"),CFile::modeCreate|CFile::modeWrite);

for( i=0;i<1891;i++)
{
	

	tmp=(unsigned char)bufhex[i];
	sprintf_s(buf, "%02X",tmp);
	hexstring[i]=CString(buf);
	hexstring2[i]=hexstring[i];

	if((i+1)%16!=0)
	{
		neirong=neirong+hexstring[i]+fuhao1;
	
	
	}
	else
	{
		neirong=neirong+hexstring[i]+(char)0x0d+(char)0x0a;

	
	}
	

}

filen.WriteString(neirong);
filen.Close();
CString fname;
fname="D:\\actresult.txt";
//CStdioFile filen2(_T("d:\\actresult.txt"),CFile::modeCreate|CFile::modeWrite);
SavedataPass(fname);









}

int Char16ToInt(char c)  
{  
    int num;  
    num = 0;//  
    switch (c)  
    {  
    case '0':  
            num = 0;  
            break;  
    case '1':  
            num = 1;  
            break;  
    case '2':  
            num = 2;  
            break;  
    case '3':  
            num = 3;  
            break;  
    case '4':  
            num = 4;  
            break;  
    case '5':  
            num = 5;  
            break;  
    case '6':  
            num = 6;  
            break;  
    case '7':  
            num = 7;  
            break;  
    case '8':  
            num = 8;  
            break;  
    case '9':  
            num = 9;  
            break;  
    case 'a':  
    case 'A':  
            num = 10;  
            break;  
    case 'b':  
    case 'B':  
            num = 11;  
            break;  
    case 'c':  
    case 'C':  
            num = 12;  
            break;  
    case 'd':  
    case 'D':  
            num = 13;  
            break;  
    case 'e':  
    case 'E':  
            num = 14;  
            break;  
    case 'f':  
    case 'F':  
            num = 15;  
            break;  
    default:  
        break;  
    }  
    return num;  
}
long long int StrToNumber16(const char *str)  
{  
      int len,i;  
   long long int num;
	  CString ss2;
	num = 0;//使用数据必须初始化否则产生不确定值  
    len = strlen(str);  
    for (i = 0; i < len; i++)  
    {  
  //   AfxMessageBox(CString(str[i]));
		//num = num*16 + Char16ToInt(str[i]);/*十六进制字符串与10进制的对应数据*/   
        num=num+pow(16.0,len-1-i)*Char16ToInt(str[i]);
	//ss2.Format(_T("%d"),num);
//AfxMessageBox(ss2);
	}  
  


	return num;  
}

void hextochar(char* in,char* out)
{
	unsigned int ch;
	while(sscanf_s(in,"%2x",&ch)!=EOF)
	{
*out++=ch;
in+=2;


	}
	*out='\0';



}

HBRUSH CTH2829TestDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Return a different brush if the default is not desired
CString tr;
GetDlgItemText(IDC_TestResult,tr);
if(tr=="FAIL")
{
	pDC->SetTextColor(RGB(255,0,0));	
}
 if(tr=="PASS")
		{
			pDC->SetTextColor(RGB(0,255,0));
		}
if(tr=="No Instrument")
{
	pDC->SetTextColor(RGB(255,0,0));	
}
if(tr=="Instrument Ready")
{
	pDC->SetTextColor(RGB(0,0,255));	
}
if(tr=="Testing")
{
	pDC->SetTextColor(RGB(0,0,255));	
}
	




return hbr;
}

void SavedataPass(CString filename)
{
//AfxMessageBox(_T("why11"));
	int testi,i2,i3,i4,lxitem,i5,i,days;
char * userc;
char *userc1; 
char buf[1893],buferr[1893],bufday[256];
CString hexs[1893],snin,snin1;
union
{
	float f;
	char buff[4];
}fdata;
LPCTSTR p1;
LPCTSTR pp1;
CString databuf,turnbuf[40],lxbuf[40];
//CStdioFile filen2(filename,CFile::modeCreate|CFile::modeWrite);
snin="SN:";
snin1="SN:";
days=Cal_Day();
_itoa_s(days,bufday,10);
//AfxMessageBox(hexstring[1730]);
/*for(i=0;i<1893;i++)
{
	hexs[i]=hexstring[i];
}
*/
if(hexstring[1730]=="01")
{
databuf="";
snin=snin+productsn+"---"+CString(bufday);
snin1=snin1+productsn;
file.WriteString(snin+(char)0x20+(char)0x20);
if(hexstring[1330]!="")//turn
{
//AfxMessageBox(_T("lx"));
//filen2.WriteString(_T("TurnResult:"));
file.WriteString(_T("TurnResult:"));
	i2=0;	
	i4=0;
	for(testi=0;testi<40;testi++)
{

if(hexstring[1330+i4]!="00")
{
	databuf="";
	for(i3=0;i3<4;i3++)	
{databuf=databuf+hexstring[i2+10+i3];}

pp1=databuf.GetBuffer(0);
	databuf.ReleaseBuffer();	
	userc1 = new char [databuf.GetLength()+1]; 
	strcpy_s(userc1,databuf.GetLength()+1, CT2CA(pp1));
hextochar(userc1,fdata.buff);
sprintf_s(buf, "%f",fdata.f);	
lxbuf[testi]=CString(buf);



file.WriteString(lxbuf[testi]+(char)0x20);
}
sprintf_s(buferr,"%d",i2);
i2=i2+4;
i4=i4+1;
	}
CString non;
non="";
file.WriteString(non+(char)0x20+(char)0x20);

}



if(hexstring[1370]!="")//lx
{
//AfxMessageBox(_T("lx"));
file.WriteString(_T("LxResult:"));

	i2=0;	
	i4=0;
	for(testi=0;testi<40;testi++)
{

if(hexstring[1370+i4]!="00")
{
	databuf="";
	for(i3=0;i3<4;i3++)	
{
databuf=databuf+hexstring[i2+170+i3];
	}
	
pp1=databuf.GetBuffer(0);
	databuf.ReleaseBuffer();	
	userc1 = new char [databuf.GetLength()+1]; 
	strcpy_s(userc1,databuf.GetLength()+1, CT2CA(pp1));
hextochar(userc1,fdata.buff);
sprintf_s(buf, "%.2f",fdata.f*1000000);	
lxbuf[testi]=CString(buf);
//AfxMessageBox(lxbuf[testi]);


file.WriteString(lxbuf[testi]+(char)0x20);
}
sprintf_s(buferr,"%d",i2);
i2=i2+4;
i4=i4+1;
	}
CString non;
non="";
file.WriteString(non+(char)0x20+(char)0x20);

}


if(hexstring[1410]!="")//lk
{
file.WriteString(_T("LkResult:"));

	i2=0;	
	i4=0;
	for(testi=0;testi<40;testi++)
{

if(hexstring[1410+i4]!="00")
{
	databuf="";
	for(i3=0;i3<4;i3++)	
{databuf=databuf+hexstring[i2+330+i3];}
//AfxMessageBox(databuf);
pp1=databuf.GetBuffer(0);
	databuf.ReleaseBuffer();	
	userc1 = new char [databuf.GetLength()+1]; 
	strcpy_s(userc1,databuf.GetLength()+1, CT2CA(pp1));
hextochar(userc1,fdata.buff);
sprintf_s(buf, "%.2f",fdata.f*1000000);	
lxbuf[testi]=CString(buf);
//AfxMessageBox(lxbuf[testi]);


file.WriteString(lxbuf[testi]+(char)0x20);
}
sprintf_s(buferr,"%d",i2);
i2=i2+4;
i4=i4+1;
	}
CString non;
non="";
file.WriteString(non+(char)0x20+(char)0x20);


}


if(hexstring[1450]!="")//CX
{
//AfxMessageBox(_T("lx"));
file.WriteString(_T("CxResult:"));

	i2=0;	
	i4=0;
	for(testi=0;testi<40;testi++)
{

if(hexstring[1450+i4]!="00")
{
	databuf="";
	for(i3=0;i3<4;i3++)	
{
databuf=databuf+hexstring[i2+490+i3];
	}
//AfxMessageBox(databuf);	
pp1=databuf.GetBuffer(0);
	databuf.ReleaseBuffer();	
	userc1 = new char [databuf.GetLength()+1]; 
	strcpy_s(userc1,databuf.GetLength()+1, CT2CA(pp1));
hextochar(userc1,fdata.buff);
sprintf_s(buf, "%.2f",fdata.f*1000000000000);	
lxbuf[testi]=CString(buf);
//AfxMessageBox(lxbuf[testi]);


file.WriteString(lxbuf[testi]+(char)0x20);
}
sprintf_s(buferr,"%d",i2);
i2=i2+4;
i4=i4+1;
	}
CString non;
non="";
file.WriteString(non+(char)0x20+(char)0x20);

}

if(hexstring[1530]!="")//Q
{
//AfxMessageBox(_T("lx"));
file.WriteString(_T("QResult:"));

	i2=0;	
	i4=0;
	for(testi=0;testi<40;testi++)
{

if(hexstring[1530+i4]!="00")
{
	databuf="";
	for(i3=0;i3<4;i3++)	
{
databuf=databuf+hexstring[i2+810+i3];
	}
	
pp1=databuf.GetBuffer(0);
	databuf.ReleaseBuffer();	
	userc1 = new char [databuf.GetLength()+1]; 
	strcpy_s(userc1,databuf.GetLength()+1, CT2CA(pp1));
hextochar(userc1,fdata.buff);
sprintf_s(buf, "%.2f",fdata.f);	
lxbuf[testi]=CString(buf);
//AfxMessageBox(lxbuf[testi]);


file.WriteString(lxbuf[testi]+(char)0x20);
}
sprintf_s(buferr,"%d",i2);
i2=i2+4;
i4=i4+1;
	}
CString non;
non="";
file.WriteString(non+(char)0x20+(char)0x20);

}

if(hexstring[1490]!="")//DCR
{
//AfxMessageBox(_T("lx"));
file.WriteString(_T("DCRResult:"));

	i2=0;	
	i4=0;
	for(testi=0;testi<40;testi++)
{

if(hexstring[1490+i4]!="00")
{
	databuf="";
	for(i3=0;i3<4;i3++)	
{
databuf=databuf+hexstring[i2+650+i3];
	}
	
pp1=databuf.GetBuffer(0);
	databuf.ReleaseBuffer();	
	userc1 = new char [databuf.GetLength()+1]; 
	strcpy_s(userc1,databuf.GetLength()+1, CT2CA(pp1));
hextochar(userc1,fdata.buff);
sprintf_s(buf, "%.2f",fdata.f*1000);	
lxbuf[testi]=CString(buf);
//AfxMessageBox(lxbuf[testi]);


file.WriteString(lxbuf[testi]+(char)0x20);
}
sprintf_s(buferr,"%d",i2);
i2=i2+4;
i4=i4+1;
	}
CString non;
non="";
file.WriteString(non+(char)0x20+(char)0x20);

}


if(hexstring[1570]!="")//ACR
{
//AfxMessageBox(_T("lx"));
file.WriteString(_T("ACRResult:"));

	i2=0;	
	i4=0;
	for(testi=0;testi<40;testi++)
{

if(hexstring[1570+i4]!="00")
{
	databuf="";
	for(i3=0;i3<4;i3++)	
{
databuf=databuf+hexstring[i2+970+i3];
	}
	
pp1=databuf.GetBuffer(0);
	databuf.ReleaseBuffer();	
	userc1 = new char [databuf.GetLength()+1]; 
	strcpy_s(userc1,databuf.GetLength()+1, CT2CA(pp1));
hextochar(userc1,fdata.buff);
sprintf_s(buf, "%.2f",fdata.f);	
lxbuf[testi]=CString(buf);
//AfxMessageBox(lxbuf[testi]);
file.WriteString(lxbuf[testi]+(char)0x20);
}
sprintf_s(buferr,"%d",i2);
i2=i2+4;
i4=i4+1;
	}
CString non;
non="";
file.WriteString(non+(char)0x20+(char)0x20);

}

if(hexstring[1610]!="")//Zx
{
//AfxMessageBox(_T("lx"));
file.WriteString(_T("ZxResult:"));

	i2=0;	
	i4=0;
	for(testi=0;testi<40;testi++)
{

if(hexstring[1610+i4]!="00")
{
	databuf="";
	for(i3=0;i3<4;i3++)	
{
databuf=databuf+hexstring[i2+1130+i3];
	}
	
pp1=databuf.GetBuffer(0);
	databuf.ReleaseBuffer();	
	userc1 = new char [databuf.GetLength()+1]; 
	strcpy_s(userc1,databuf.GetLength()+1, CT2CA(pp1));
hextochar(userc1,fdata.buff);
sprintf_s(buf, "%.2f",fdata.f);	
lxbuf[testi]=CString(buf);
//AfxMessageBox(lxbuf[testi]);


file.WriteString(lxbuf[testi]+(char)0x20);
}
sprintf_s(buferr,"%d",i2);
i2=i2+4;
i4=i4+1;
	}
CString non;
non="";
file.WriteString(non+(char)0x20+(char)0x20);

}


file.WriteString(_T("PolResult:"));//pol
i4=0;
for(testi=0;testi<40;testi++)
{
	//AfxMessageBox(hexstring[1290+i4]);
	if(hexstring[1290+i4]!="20")
{
	//AfxMessageBox(_T("why"));
	file.WriteString(hexstring[1290+i4]+(char)0x20);

	}
i4=i4+1;
}
CString non;
non="";
//file.WriteString(non+(char)0x0d+(char)0x0a+(char)0x0d+(char)0x0a+(char)0x0d+(char)0x0a+(char)0x0d+(char)0x0a);
file.WriteString(non + (char)0x0d + (char)0x0a);
//AfxMessageBox(_T("why"));
CString filename2,type;
type=".txt"; 
filename2="D:\\TH2829Test\\";
	 filename2=filename2+"SNCHECK"+type;
 CStdioFile filexx(filename2,CFile::modeNoTruncate|CFile::modeWrite);
	// filex2.Open(filename2,CFile::modeNoTruncate|CFile::modeWrite);
filexx.SeekToEnd();
filexx.WriteString(snin1);
filexx.WriteString(_T("\r\n"));
filexx.Close();





//filen2.Close();
}



}

void SavedataFail(CString filename)
{
	


	int testi,i2,i3,i4,lxitem,i5,i;
char * userc;
char *userc1; 
char buf[1893],buferr[1893];
CString hexs[1893],snin,snin1;
union
{
	float f;
	char buff[4];
}fdata;
LPCTSTR p1;
LPCTSTR pp1;
CString databuf,turnbuf[40],lxbuf[40];
snin="SN:";
snin1="SN:";
//CStdioFile filen2(filename,CFile::modeCreate|CFile::modeWrite);
//AfxMessageBox(hexstring[1730]);
/*for(i=0;i<1893;i++)
{
	hexs[i]=hexstring[i];
}
*/
if(hexstring[1730]=="02")
{
databuf="";
snin=snin+productsn;
snin1=snin1+productsn;
file.WriteString(snin+(char)0x0d+(char)0x0a);
if(hexstring[1330]!="")//turn
{
//AfxMessageBox(_T("lx"));
file.WriteString(_T("TurnResult:"));

	i2=0;	
	i4=0;
	for(testi=0;testi<40;testi++)
{

if(hexstring[1330+i4]!="00")
{
	databuf="";
	for(i3=0;i3<4;i3++)	
{databuf=databuf+hexstring[i2+10+i3];}

pp1=databuf.GetBuffer(0);
	databuf.ReleaseBuffer();	
	userc1 = new char [databuf.GetLength()+1]; 
	strcpy_s(userc1,databuf.GetLength()+1, CT2CA(pp1));
hextochar(userc1,fdata.buff);
sprintf_s(buf, "%f",fdata.f);	
lxbuf[testi]=CString(buf);



file.WriteString(lxbuf[testi]+(char)0x20);
}
sprintf_s(buferr,"%d",i2);
i2=i2+4;
i4=i4+1;
	}
CString non;
non="";
file.WriteString(non+(char)0x0d+(char)0x0a);

}



if(hexstring[1370]!="")//lx
{
//AfxMessageBox(_T("lx"));
file.WriteString(_T("LxResult:"));

	i2=0;	
	i4=0;
	for(testi=0;testi<40;testi++)
{

if(hexstring[1370+i4]!="00")
{
	databuf="";
	for(i3=0;i3<4;i3++)	
{
databuf=databuf+hexstring[i2+170+i3];
	}
	
pp1=databuf.GetBuffer(0);
	databuf.ReleaseBuffer();	
	userc1 = new char [databuf.GetLength()+1]; 
	strcpy_s(userc1,databuf.GetLength()+1, CT2CA(pp1));
hextochar(userc1,fdata.buff);
sprintf_s(buf, "%.2f",fdata.f*1000000);	
lxbuf[testi]=CString(buf);
//AfxMessageBox(lxbuf[testi]);


file.WriteString(lxbuf[testi]+(char)0x20);
}
sprintf_s(buferr,"%d",i2);
i2=i2+4;
i4=i4+1;
	}
CString non;
non="";
file.WriteString(non+(char)0x0d+(char)0x0a);

}


if(hexstring[1410]!="")//lk
{
file.WriteString(_T("LkResult:"));

	i2=0;	
	i4=0;
	for(testi=0;testi<40;testi++)
{

if(hexstring[1410+i4]!="00")
{
	databuf="";
	for(i3=0;i3<4;i3++)	
{databuf=databuf+hexstring[i2+330+i3];}
//AfxMessageBox(databuf);
pp1=databuf.GetBuffer(0);
	databuf.ReleaseBuffer();	
	userc1 = new char [databuf.GetLength()+1]; 
	strcpy_s(userc1,databuf.GetLength()+1, CT2CA(pp1));
hextochar(userc1,fdata.buff);
sprintf_s(buf, "%.2f",fdata.f*1000000);	
lxbuf[testi]=CString(buf);
//AfxMessageBox(lxbuf[testi]);


file.WriteString(lxbuf[testi]+(char)0x20);
}
sprintf_s(buferr,"%d",i2);
i2=i2+4;
i4=i4+1;
	}
CString non;
non="";
file.WriteString(non+(char)0x0d+(char)0x0a);


}


if(hexstring[1450]!="")//CX
{
//AfxMessageBox(_T("lx"));
file.WriteString(_T("CxResult:"));

	i2=0;	
	i4=0;
	for(testi=0;testi<40;testi++)
{

if(hexstring[1450+i4]!="00")
{
	databuf="";
	for(i3=0;i3<4;i3++)	
{
databuf=databuf+hexstring[i2+490+i3];
	}
	
pp1=databuf.GetBuffer(0);
	databuf.ReleaseBuffer();	
	userc1 = new char [databuf.GetLength()+1]; 
	strcpy_s(userc1,databuf.GetLength()+1, CT2CA(pp1));
hextochar(userc1,fdata.buff);
sprintf_s(buf, "%.2f",fdata.f*1000000000000);	
lxbuf[testi]=CString(buf);
//AfxMessageBox(lxbuf[testi]);


file.WriteString(lxbuf[testi]+(char)0x20);
}
sprintf_s(buferr,"%d",i2);
i2=i2+4;
i4=i4+1;
	}
CString non;
non="";
file.WriteString(non+(char)0x0d+(char)0x0a);

}

if(hexstring[1530]!="")//Q
{
//AfxMessageBox(_T("lx"));
file.WriteString(_T("QResult:"));

	i2=0;	
	i4=0;
	for(testi=0;testi<40;testi++)
{

if(hexstring[1530+i4]!="00")
{
	databuf="";
	for(i3=0;i3<4;i3++)	
{
databuf=databuf+hexstring[i2+810+i3];
	}
	
pp1=databuf.GetBuffer(0);
	databuf.ReleaseBuffer();	
	userc1 = new char [databuf.GetLength()+1]; 
	strcpy_s(userc1,databuf.GetLength()+1, CT2CA(pp1));
hextochar(userc1,fdata.buff);
sprintf_s(buf, "%.2f",fdata.f);	
lxbuf[testi]=CString(buf);
//AfxMessageBox(lxbuf[testi]);


file.WriteString(lxbuf[testi]+(char)0x20);
}
sprintf_s(buferr,"%d",i2);
i2=i2+4;
i4=i4+1;
	}
CString non;
non="";
file.WriteString(non+(char)0x0d+(char)0x0a);

}

if(hexstring[1490]!="")//DCR
{
//AfxMessageBox(_T("lx"));
file.WriteString(_T("DCRResult:"));

	i2=0;	
	i4=0;
	for(testi=0;testi<40;testi++)
{

if(hexstring[1490+i4]!="00")
{
	databuf="";
	for(i3=0;i3<4;i3++)	
{
databuf=databuf+hexstring[i2+650+i3];
	}
	
pp1=databuf.GetBuffer(0);
	databuf.ReleaseBuffer();	
	userc1 = new char [databuf.GetLength()+1]; 
	strcpy_s(userc1,databuf.GetLength()+1, CT2CA(pp1));
hextochar(userc1,fdata.buff);
sprintf_s(buf, "%.2f",fdata.f*1000);	
lxbuf[testi]=CString(buf);
//AfxMessageBox(lxbuf[testi]);


file.WriteString(lxbuf[testi]+(char)0x20);
}
sprintf_s(buferr,"%d",i2);
i2=i2+4;
i4=i4+1;
	}
CString non;
non="";
file.WriteString(non+(char)0x0d+(char)0x0a);

}


if(hexstring[1570]!="")//ACR
{
//AfxMessageBox(_T("lx"));
file.WriteString(_T("ACRResult:"));

	i2=0;	
	i4=0;
	for(testi=0;testi<40;testi++)
{

if(hexstring[1570+i4]!="00")
{
	databuf="";
	for(i3=0;i3<4;i3++)	
{
databuf=databuf+hexstring[i2+970+i3];
	}
	
pp1=databuf.GetBuffer(0);
	databuf.ReleaseBuffer();	
	userc1 = new char [databuf.GetLength()+1]; 
	strcpy_s(userc1,databuf.GetLength()+1, CT2CA(pp1));
hextochar(userc1,fdata.buff);
sprintf_s(buf, "%.2f",fdata.f);	
lxbuf[testi]=CString(buf);
//AfxMessageBox(lxbuf[testi]);
file.WriteString(lxbuf[testi]+(char)0x20);
}
sprintf_s(buferr,"%d",i2);
i2=i2+4;
i4=i4+1;
	}
CString non;
non="";
file.WriteString(non+(char)0x0d+(char)0x0a);

}

if(hexstring[1610]!="")//Zx
{
//AfxMessageBox(_T("lx"));
file.WriteString(_T("ZxResult:"));

	i2=0;	
	i4=0;
	for(testi=0;testi<40;testi++)
{

if(hexstring[1610+i4]!="00")
{
	databuf="";
	for(i3=0;i3<4;i3++)	
{
databuf=databuf+hexstring[i2+1130+i3];
	}
	
pp1=databuf.GetBuffer(0);
	databuf.ReleaseBuffer();	
	userc1 = new char [databuf.GetLength()+1]; 
	strcpy_s(userc1,databuf.GetLength()+1, CT2CA(pp1));
hextochar(userc1,fdata.buff);
sprintf_s(buf, "%.2f",fdata.f);	
lxbuf[testi]=CString(buf);
//AfxMessageBox(lxbuf[testi]);


file.WriteString(lxbuf[testi]+(char)0x20);
}
sprintf_s(buferr,"%d",i2);
i2=i2+4;
i4=i4+1;
	}
CString non;
non="";
file.WriteString(non+(char)0x0d+(char)0x0a);

}


file.WriteString(_T("PolResult:"));//pol
i4=0;
for(testi=0;testi<40;testi++)
{
	//AfxMessageBox(hexstring[1290+i4]);
	if(hexstring[1290+i4]!="20")
{
	//AfxMessageBox(_T("why"));
	file.WriteString(hexstring[1290+i4]+(char)0x20);

	}
i4=i4+1;
}
CString non;
non="";
file.WriteString(non+(char)0x0d+(char)0x0a+(char)0x0d+(char)0x0a+(char)0x0d+(char)0x0a+(char)0x0d+(char)0x0a);












//filen2.Close();
}



}


void SavedataAll(CString filename)
{
int testi,i2,i3,i4,lxitem,i5,i;
char * userc;
char *userc1; 
char buf[1893],buferr[1893];
CString hexs[1893],snin;
union
{
	float f;
	char buff[4];
}fdata;
LPCTSTR p1;
LPCTSTR pp1;
CString databuf,turnbuf[40],lxbuf[40];
snin="SN:";
//CStdioFile filen2(filename,CFile::modeCreate|CFile::modeWrite);
//AfxMessageBox(hexstring[1730]);
/*for(i=0;i<1893;i++)
{
	hexs[i]=hexstring[i];
}
*/
if((hexstring[1730]=="02")||(hexstring[1730]=="01"))
{
databuf="";
snin=snin+productsn;
file.WriteString(snin+(char)0x0d+(char)0x0a);
if(hexstring[1330]!="")//turn
{
//AfxMessageBox(_T("lx"));
file.WriteString(_T("TurnResult:"));

	i2=0;	
	i4=0;
	for(testi=0;testi<40;testi++)
{

if(hexstring[1330+i4]!="00")
{
	databuf="";
	for(i3=0;i3<4;i3++)	
{databuf=databuf+hexstring[i2+10+i3];}

pp1=databuf.GetBuffer(0);
	databuf.ReleaseBuffer();	
	userc1 = new char [databuf.GetLength()+1]; 
	strcpy_s(userc1,databuf.GetLength()+1, CT2CA(pp1));
hextochar(userc1,fdata.buff);
sprintf_s(buf, "%f",fdata.f);	
lxbuf[testi]=CString(buf);



file.WriteString(lxbuf[testi]+(char)0x20);
}
sprintf_s(buferr,"%d",i2);
i2=i2+4;
i4=i4+1;
	}
CString non;
non="";
file.WriteString(non+(char)0x0d+(char)0x0a);

}



if(hexstring[1370]!="")//lx
{
//AfxMessageBox(_T("lx"));
file.WriteString(_T("LxResult:"));

	i2=0;	
	i4=0;
	for(testi=0;testi<40;testi++)
{

if(hexstring[1370+i4]!="00")
{
	databuf="";
	for(i3=0;i3<4;i3++)	
{
databuf=databuf+hexstring[i2+170+i3];
	}
	
pp1=databuf.GetBuffer(0);
	databuf.ReleaseBuffer();	
	userc1 = new char [databuf.GetLength()+1]; 
	strcpy_s(userc1,databuf.GetLength()+1, CT2CA(pp1));
hextochar(userc1,fdata.buff);
sprintf_s(buf, "%.2f",fdata.f*1000000);	
lxbuf[testi]=CString(buf);
//AfxMessageBox(lxbuf[testi]);


file.WriteString(lxbuf[testi]+(char)0x20);
}
sprintf_s(buferr,"%d",i2);
i2=i2+4;
i4=i4+1;
	}
CString non;
non="";
file.WriteString(non+(char)0x0d+(char)0x0a);

}


if(hexstring[1410]!="")//lk
{
file.WriteString(_T("LkResult:"));

	i2=0;	
	i4=0;
	for(testi=0;testi<40;testi++)
{

if(hexstring[1410+i4]!="00")
{
	databuf="";
	for(i3=0;i3<4;i3++)	
{databuf=databuf+hexstring[i2+330+i3];}
//AfxMessageBox(databuf);
pp1=databuf.GetBuffer(0);
	databuf.ReleaseBuffer();	
	userc1 = new char [databuf.GetLength()+1]; 
	strcpy_s(userc1,databuf.GetLength()+1, CT2CA(pp1));
hextochar(userc1,fdata.buff);
sprintf_s(buf, "%.2f",fdata.f*1000000);	
lxbuf[testi]=CString(buf);
//AfxMessageBox(lxbuf[testi]);


file.WriteString(lxbuf[testi]+(char)0x20);
}
sprintf_s(buferr,"%d",i2);
i2=i2+4;
i4=i4+1;
	}
CString non;
non="";
file.WriteString(non+(char)0x0d+(char)0x0a);


}


if(hexstring[1450]!="")//CX
{
//AfxMessageBox(_T("lx"));
file.WriteString(_T("CxResult:"));

	i2=0;	
	i4=0;
	for(testi=0;testi<40;testi++)
{

if(hexstring[1450+i4]!="00")
{
	databuf="";
	for(i3=0;i3<4;i3++)	
{
databuf=databuf+hexstring[i2+490+i3];
	}
	
pp1=databuf.GetBuffer(0);
	databuf.ReleaseBuffer();	
	userc1 = new char [databuf.GetLength()+1]; 
	strcpy_s(userc1,databuf.GetLength()+1, CT2CA(pp1));
hextochar(userc1,fdata.buff);
sprintf_s(buf, "%.2f",fdata.f*1000000000000);	
lxbuf[testi]=CString(buf);
//AfxMessageBox(lxbuf[testi]);


file.WriteString(lxbuf[testi]+(char)0x20);
}
sprintf_s(buferr,"%d",i2);
i2=i2+4;
i4=i4+1;
	}
CString non;
non="";
file.WriteString(non+(char)0x0d+(char)0x0a);

}

if(hexstring[1530]!="")//Q
{
//AfxMessageBox(_T("lx"));
file.WriteString(_T("QResult:"));

	i2=0;	
	i4=0;
	for(testi=0;testi<40;testi++)
{

if(hexstring[1530+i4]!="00")
{
	databuf="";
	for(i3=0;i3<4;i3++)	
{
databuf=databuf+hexstring[i2+810+i3];
	}
	
pp1=databuf.GetBuffer(0);
	databuf.ReleaseBuffer();	
	userc1 = new char [databuf.GetLength()+1]; 
	strcpy_s(userc1,databuf.GetLength()+1, CT2CA(pp1));
hextochar(userc1,fdata.buff);
sprintf_s(buf, "%.2f",fdata.f);	
lxbuf[testi]=CString(buf);
//AfxMessageBox(lxbuf[testi]);


file.WriteString(lxbuf[testi]+(char)0x20);
}
sprintf_s(buferr,"%d",i2);
i2=i2+4;
i4=i4+1;
	}
CString non;
non="";
file.WriteString(non+(char)0x0d+(char)0x0a);

}

if(hexstring[1490]!="")//DCR
{
//AfxMessageBox(_T("lx"));
file.WriteString(_T("DCRResult:"));

	i2=0;	
	i4=0;
	for(testi=0;testi<40;testi++)
{

if(hexstring[1490+i4]!="00")
{
	databuf="";
	for(i3=0;i3<4;i3++)	
{
databuf=databuf+hexstring[i2+650+i3];
	}
	
pp1=databuf.GetBuffer(0);
	databuf.ReleaseBuffer();	
	userc1 = new char [databuf.GetLength()+1]; 
	strcpy_s(userc1,databuf.GetLength()+1, CT2CA(pp1));
hextochar(userc1,fdata.buff);
sprintf_s(buf, "%.2f",fdata.f*1000);	
lxbuf[testi]=CString(buf);
//AfxMessageBox(lxbuf[testi]);


file.WriteString(lxbuf[testi]+(char)0x20);
}
sprintf_s(buferr,"%d",i2);
i2=i2+4;
i4=i4+1;
	}
CString non;
non="";
file.WriteString(non+(char)0x0d+(char)0x0a);

}


if(hexstring[1570]!="")//ACR
{
//AfxMessageBox(_T("lx"));
file.WriteString(_T("ACRResult:"));

	i2=0;	
	i4=0;
	for(testi=0;testi<40;testi++)
{

if(hexstring[1570+i4]!="00")
{
	databuf="";
	for(i3=0;i3<4;i3++)	
{
databuf=databuf+hexstring[i2+970+i3];
	}
	
pp1=databuf.GetBuffer(0);
	databuf.ReleaseBuffer();	
	userc1 = new char [databuf.GetLength()+1]; 
	strcpy_s(userc1,databuf.GetLength()+1, CT2CA(pp1));
hextochar(userc1,fdata.buff);
sprintf_s(buf, "%.2f",fdata.f);	
lxbuf[testi]=CString(buf);
//AfxMessageBox(lxbuf[testi]);
file.WriteString(lxbuf[testi]+(char)0x20);
}
sprintf_s(buferr,"%d",i2);
i2=i2+4;
i4=i4+1;
	}
CString non;
non="";
file.WriteString(non+(char)0x0d+(char)0x0a);

}

if(hexstring[1610]!="")//Zx
{
//AfxMessageBox(_T("lx"));
file.WriteString(_T("ZxResult:"));

	i2=0;	
	i4=0;
	for(testi=0;testi<40;testi++)
{

if(hexstring[1610+i4]!="00")
{
	databuf="";
	for(i3=0;i3<4;i3++)	
{
databuf=databuf+hexstring[i2+1130+i3];
	}
	
pp1=databuf.GetBuffer(0);
	databuf.ReleaseBuffer();	
	userc1 = new char [databuf.GetLength()+1]; 
	strcpy_s(userc1,databuf.GetLength()+1, CT2CA(pp1));
hextochar(userc1,fdata.buff);
sprintf_s(buf, "%.2f",fdata.f);	
lxbuf[testi]=CString(buf);
//AfxMessageBox(lxbuf[testi]);


file.WriteString(lxbuf[testi]+(char)0x20);
}
sprintf_s(buferr,"%d",i2);
i2=i2+4;
i4=i4+1;
	}
CString non;
non="";
file.WriteString(non+(char)0x0d+(char)0x0a);

}


file.WriteString(_T("PolResult:"));//pol
i4=0;
for(testi=0;testi<40;testi++)
{
	//AfxMessageBox(hexstring[1290+i4]);
	if(hexstring[1290+i4]!="20")
{
	//AfxMessageBox(_T("why"));
	file.WriteString(hexstring[1290+i4]+(char)0x20);

	}
i4=i4+1;
}
CString non;
non="";
file.WriteString(non+(char)0x0d+(char)0x0a+(char)0x0d+(char)0x0a+(char)0x0d+(char)0x0a+(char)0x0d+(char)0x0a);











//filen2.Close();
}



}










void CAboutDlg::OnDataPass()
{
	// TODO: Add your command handler code here
}


void CTH2829TestDlg::OnDataPass()
{
	// TODO: Add your command handler code here
UINT flag;	
CMenu* mmenu = GetMenu();
CMenu* submenu = mmenu->GetSubMenu(0);	
flag=GetMenu()->GetSubMenu(0)->GetMenuState(0,MF_CHECKED);
if(flag&MF_CHECKED) 
{
GetMenu()->GetSubMenu(0)->CheckMenuItem(0,MF_CHECKED|MF_BYPOSITION); 	
GetMenu()->GetSubMenu(0)->CheckMenuItem(1,MF_UNCHECKED|MF_BYPOSITION);
GetMenu()->GetSubMenu(0)->CheckMenuItem(2,MF_UNCHECKED|MF_BYPOSITION);
savepass=true;
saveall=false;
savefail=false;
}

}


void CTH2829TestDlg::OnDataFail()
{
	// TODO: Add your command handler code here
UINT flag;	
CMenu* mmenu = GetMenu();
CMenu* submenu = mmenu->GetSubMenu(0);	
flag=GetMenu()->GetSubMenu(0)->GetMenuState(1,MF_CHECKED);
if(flag&MF_CHECKED) 
{
GetMenu()->GetSubMenu(0)->CheckMenuItem(1,MF_CHECKED|MF_BYPOSITION); 	
GetMenu()->GetSubMenu(0)->CheckMenuItem(0,MF_UNCHECKED|MF_BYPOSITION);
GetMenu()->GetSubMenu(0)->CheckMenuItem(2,MF_UNCHECKED|MF_BYPOSITION);
savepass=false;
saveall=false;
savefail=true;
}



}


void CTH2829TestDlg::OnDataAll()
{
	// TODO: Add your command handler code here
UINT flag;	
CMenu* mmenu = GetMenu();
CMenu* submenu = mmenu->GetSubMenu(0);	
flag=GetMenu()->GetSubMenu(0)->GetMenuState(2,MF_CHECKED);
if(flag&MF_CHECKED) 
{
GetMenu()->GetSubMenu(0)->CheckMenuItem(2,MF_CHECKED|MF_BYPOSITION); 	
GetMenu()->GetSubMenu(0)->CheckMenuItem(0,MF_UNCHECKED|MF_BYPOSITION);
GetMenu()->GetSubMenu(0)->CheckMenuItem(1,MF_UNCHECKED|MF_BYPOSITION);
savepass=false;
saveall=true;
savefail=false;
}


}


void CTH2829TestDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
/*fstream binfile;
binfile.open("d:\\TH2829RESULT.bin",ios::in|ios::binary);*/
	//AfxMessageBox(_T("why"));
	DeleteFile(_T("D:\\TH2829RESULTBin\\TH2829RESULT.bin"));
	file.Close();

	CDialogEx::OnClose();
}
int Cal_Day() 
{
/*Date date;
    smatch m;
    string date_str;
    regex reg("([[:d:]]{4})([-./])?([01]?[[:d:]])([-./])?([0123]?[[:d:]])");*/
	CTime Time;
	/*auto sum = 0;
   // vector<int> temp={31,28,31,30,31,30,31,31,30,31,30,31 };
	vector<int> temp2;
	date_str=CT2A(Time.Format("%Y-%m-%d").GetBuffer());
	temp2={31,28,31,30,31,30,31,31,30,31,30,31};*/
int mon,day,year,y,x;
LPCTSTR p1;
 char * userc;
 char buff[256];
 x=0;
 y=0;
 Time = CTime::GetCurrentTime();
 p1=Time.Format("%Y").GetBuffer(0);
	Time.Format("%Y").ReleaseBuffer();
	
	userc = new char [Time.Format("%Y").GetLength()+1]; 
	strcpy_s(userc,Time.Format("%Y").GetLength()+1, CT2CA(p1));
	//strcpy_s(THadd,userc);

	year=atoi(userc);
	/*_itoa_s(year,buff,10);
AfxMessageBox(Time.Format("%d"));*/

	if((year%4==0&&year%100!=0)||(year%400==0))
	{
y=1;
	}
else 
{
y=0;
	}
p1=Time.Format("%m").GetBuffer(0);
	Time.Format("%m").ReleaseBuffer();
	
	userc = new char [Time.Format("%m").GetLength()+1]; 
	strcpy_s(userc,Time.Format("%m").GetLength()+1, CT2CA(p1));
	//strcpy_s(THadd,userc);
	mon=atoi(userc);

	/*_itoa_s(userc,10);
	_itoa_s(i,bufk,10);
	sprintf_s(*/
switch(mon)

 {

case 12:x=30+x;

case 11:x=31+x; 

case 10:x=30+x;

case 9:x=31+x;

case 8:x=31+x;

case 7:x=30+x;

case 6:x=31+x;

case 5:x=30+x;

case 4:x=31+x;

case 3:x=x+28+y;

case 2:x=x+31;

case 1:x=x;break;

default:printf("input error\n");

}
p1=Time.Format("%d").GetBuffer(0);
	Time.Format("%d").ReleaseBuffer();
	
	userc = new char [Time.Format("%d").GetLength()+1]; 
	strcpy_s(userc,Time.Format("%d").GetLength()+1, CT2CA(p1));
	//strcpy_s(THadd,userc);
	day=atoi(userc);
x=x+day;
/*_itoa_s(x,buff,10);
AfxMessageBox(CString(buff));*/
return x;



}

void CTH2829TestDlg::OnModelM0075()
{
	// TODO: Add your command handler code here
UINT flag;	
CMenu* mmenu = GetMenu();
CMenu* submenu = mmenu->GetSubMenu(1);	
flag=GetMenu()->GetSubMenu(1)->GetMenuState(4,MF_CHECKED);
if(flag&MF_CHECKED) 
{
GetMenu()->GetSubMenu(1)->CheckMenuItem(4,MF_CHECKED|MF_BYPOSITION); 	
GetMenu()->GetSubMenu(1)->EnableMenuItem(0,MF_UNCHECKED|MF_BYPOSITION|MF_GRAYED);
GetMenu()->GetSubMenu(1)->EnableMenuItem(1,MF_UNCHECKED|MF_BYPOSITION|MF_GRAYED);
GetMenu()->GetSubMenu(1)->EnableMenuItem(2,MF_UNCHECKED|MF_BYPOSITION|MF_GRAYED);
GetMenu()->GetSubMenu(1)->EnableMenuItem(3,MF_UNCHECKED|MF_BYPOSITION|MF_GRAYED);
GetMenu()->GetSubMenu(1)->EnableMenuItem(5,MF_UNCHECKED|MF_BYPOSITION|MF_GRAYED);
GetMenu()->GetSubMenu(1)->EnableMenuItem(6,MF_UNCHECKED|MF_BYPOSITION|MF_GRAYED);
}
if(!PathIsDirectory(_T("D:\\TH2829Test\\M0075")))
{
	::CreateDirectory(_T("D:\\TH2829Test\\M0075"),NULL);
}

modelname="M0075";
CTime Time;
CString filename1,type,non,filename2;	
type=".txt";
non="";
Time = CTime::GetCurrentTime();
	 filename1="D:\\TH2829Test\\M0075\\";
	 filename1=filename1+Time.Format("%Y")+type;
	 	
	 if(file.Open(filename1,CFile::modeNoTruncate|CFile::modeWrite)==NULL)
{
file.Open(filename1,CFile::modeCreate|CFile::modeWrite);
}
else
{
	file.SeekToEnd();
file.WriteString(non+(char)0x0d+(char)0x0a+(char)0x0d+(char)0x0a+(char)0x0d+(char)0x0a+(char)0x0d+(char)0x0a);
//file.WriteString(_T("2"));
}



}


void CTH2829TestDlg::OnModelM1816()
{
	// TODO: Add your command handler code here
UINT flag;	
CMenu* mmenu = GetMenu();
CMenu* submenu = mmenu->GetSubMenu(1);	
flag=GetMenu()->GetSubMenu(1)->GetMenuState(3,MF_CHECKED);
if(flag&MF_CHECKED) 
{
GetMenu()->GetSubMenu(1)->CheckMenuItem(3,MF_CHECKED|MF_BYPOSITION); 	
GetMenu()->GetSubMenu(1)->EnableMenuItem(0,MF_UNCHECKED|MF_BYPOSITION|MF_GRAYED);
GetMenu()->GetSubMenu(1)->EnableMenuItem(1,MF_UNCHECKED|MF_BYPOSITION|MF_GRAYED);
GetMenu()->GetSubMenu(1)->EnableMenuItem(2,MF_UNCHECKED|MF_BYPOSITION|MF_GRAYED);
GetMenu()->GetSubMenu(1)->EnableMenuItem(4,MF_UNCHECKED|MF_BYPOSITION|MF_GRAYED);
GetMenu()->GetSubMenu(1)->EnableMenuItem(5,MF_UNCHECKED|MF_BYPOSITION|MF_GRAYED);
GetMenu()->GetSubMenu(1)->EnableMenuItem(6, MF_UNCHECKED | MF_BYPOSITION | MF_GRAYED);
}
if(!PathIsDirectory(_T("D:\\TH2829Test\\M1816")))
{
	::CreateDirectory(_T("D:\\TH2829Test\\M1816"),NULL);
}
modelname="M1816";
CTime Time;
CString filename1,type,non,filename2;	
type=".txt";
non="";
Time = CTime::GetCurrentTime();
	 filename1="D:\\TH2829Test\\M1816\\";
	 filename1=filename1+Time.Format("%Y")+type;
	 	
	 if(file.Open(filename1,CFile::modeNoTruncate|CFile::modeWrite)==NULL)
{
file.Open(filename1,CFile::modeCreate|CFile::modeWrite);
}
else
{
	file.SeekToEnd();
file.WriteString(non+(char)0x0d+(char)0x0a+(char)0x0d+(char)0x0a+(char)0x0d+(char)0x0a+(char)0x0d+(char)0x0a);
//file.WriteString(_T("2"));
}


}


void CTH2829TestDlg::OnModelM1817()
{
	// TODO: Add your command handler code here
UINT flag;	
CMenu* mmenu = GetMenu();
CMenu* submenu = mmenu->GetSubMenu(1);	
flag=GetMenu()->GetSubMenu(1)->GetMenuState(0,MF_CHECKED);
if(flag&MF_CHECKED) 
{
GetMenu()->GetSubMenu(1)->CheckMenuItem(0,MF_CHECKED|MF_BYPOSITION); 	
GetMenu()->GetSubMenu(1)->EnableMenuItem(3,MF_UNCHECKED|MF_BYPOSITION|MF_GRAYED);
GetMenu()->GetSubMenu(1)->EnableMenuItem(1,MF_UNCHECKED|MF_BYPOSITION|MF_GRAYED);
GetMenu()->GetSubMenu(1)->EnableMenuItem(2,MF_UNCHECKED|MF_BYPOSITION|MF_GRAYED);
GetMenu()->GetSubMenu(1)->EnableMenuItem(4,MF_UNCHECKED|MF_BYPOSITION|MF_GRAYED);
GetMenu()->GetSubMenu(1)->EnableMenuItem(5,MF_UNCHECKED|MF_BYPOSITION|MF_GRAYED);
GetMenu()->GetSubMenu(1)->EnableMenuItem(6, MF_UNCHECKED | MF_BYPOSITION | MF_GRAYED);
}
if(!PathIsDirectory(_T("D:\\TH2829Test\\M1817")))
{
	::CreateDirectory(_T("D:\\TH2829Test\\M1817"),NULL);
}
modelname="M1817";
CTime Time;
CString filename1,type,non,filename2;	
type=".txt";
non="";
Time = CTime::GetCurrentTime();
	 filename1="D:\\TH2829Test\\M1817\\";
	 filename1=filename1+Time.Format("%Y")+type;
	 	
	 if(file.Open(filename1,CFile::modeNoTruncate|CFile::modeWrite)==NULL)
{
file.Open(filename1,CFile::modeCreate|CFile::modeWrite);
}
else
{
	file.SeekToEnd();
file.WriteString(non+(char)0x0d+(char)0x0a+(char)0x0d+(char)0x0a+(char)0x0d+(char)0x0a+(char)0x0d+(char)0x0a);
//file.WriteString(_T("2"));
}




}


void CTH2829TestDlg::OnModelM1818()
{
	// TODO: Add your command handler code here
UINT flag;	
CMenu* mmenu = GetMenu();
CMenu* submenu = mmenu->GetSubMenu(1);	
flag=GetMenu()->GetSubMenu(1)->GetMenuState(2,MF_CHECKED);
if(flag&MF_CHECKED) 
{
GetMenu()->GetSubMenu(1)->CheckMenuItem(2,MF_CHECKED|MF_BYPOSITION); 	
GetMenu()->GetSubMenu(1)->EnableMenuItem(0,MF_UNCHECKED|MF_BYPOSITION|MF_GRAYED);
GetMenu()->GetSubMenu(1)->EnableMenuItem(1,MF_UNCHECKED|MF_BYPOSITION|MF_GRAYED);
GetMenu()->GetSubMenu(1)->EnableMenuItem(3,MF_UNCHECKED|MF_BYPOSITION|MF_GRAYED);
GetMenu()->GetSubMenu(1)->EnableMenuItem(4,MF_UNCHECKED|MF_BYPOSITION|MF_GRAYED);
GetMenu()->GetSubMenu(1)->EnableMenuItem(5,MF_UNCHECKED|MF_BYPOSITION|MF_GRAYED);
GetMenu()->GetSubMenu(1)->EnableMenuItem(6, MF_UNCHECKED | MF_BYPOSITION | MF_GRAYED);
}
if(!PathIsDirectory(_T("D:\\TH2829Test\\M1818")))
{
	::CreateDirectory(_T("D:\\TH2829Test\\M1818"),NULL);
}
modelname="M1818";
CTime Time;
CString filename1,type,non,filename2;	
type=".txt";
non="";
Time = CTime::GetCurrentTime();
	 filename1="D:\\TH2829Test\\M1818\\";
	 filename1=filename1+Time.Format("%Y")+type;
	 	
	 if(file.Open(filename1,CFile::modeNoTruncate|CFile::modeWrite)==NULL)
{
file.Open(filename1,CFile::modeCreate|CFile::modeWrite);
}
else
{
	file.SeekToEnd();
file.WriteString(non+(char)0x0d+(char)0x0a+(char)0x0d+(char)0x0a+(char)0x0d+(char)0x0a+(char)0x0d+(char)0x0a);
//file.WriteString(_T("2"));
}







}


void CTH2829TestDlg::OnModelM1829()
{
	// TODO: Add your command handler code here
UINT flag;	
CMenu* mmenu = GetMenu();
CMenu* submenu = mmenu->GetSubMenu(1);	
flag=GetMenu()->GetSubMenu(1)->GetMenuState(1,MF_CHECKED);
if(flag&MF_CHECKED) 
{
GetMenu()->GetSubMenu(1)->CheckMenuItem(1,MF_CHECKED|MF_BYPOSITION); 	
GetMenu()->GetSubMenu(1)->EnableMenuItem(0,MF_UNCHECKED|MF_BYPOSITION|MF_GRAYED);
GetMenu()->GetSubMenu(1)->EnableMenuItem(3,MF_UNCHECKED|MF_BYPOSITION|MF_GRAYED);
GetMenu()->GetSubMenu(1)->EnableMenuItem(2,MF_UNCHECKED|MF_BYPOSITION|MF_GRAYED);
GetMenu()->GetSubMenu(1)->EnableMenuItem(4,MF_UNCHECKED|MF_BYPOSITION|MF_GRAYED);
GetMenu()->GetSubMenu(1)->EnableMenuItem(5,MF_UNCHECKED|MF_BYPOSITION|MF_GRAYED);
GetMenu()->GetSubMenu(1)->EnableMenuItem(6, MF_UNCHECKED | MF_BYPOSITION | MF_GRAYED);
}
if(!PathIsDirectory(_T("D:\\TH2829Test\\M1829")))
{
	::CreateDirectory(_T("D:\\TH2829Test\\M1829"),NULL);
}
modelname="M1829";
CTime Time;
CString filename1,type,non,filename2;	
type=".txt";
non="";
Time = CTime::GetCurrentTime();
	 filename1="D:\\TH2829Test\\M1829\\";
	 filename1=filename1+Time.Format("%Y")+type;
	 	
	 if(file.Open(filename1,CFile::modeNoTruncate|CFile::modeWrite)==NULL)
{
file.Open(filename1,CFile::modeCreate|CFile::modeWrite);
}
else
{
	file.SeekToEnd();
file.WriteString(non+(char)0x0d+(char)0x0a+(char)0x0d+(char)0x0a+(char)0x0d+(char)0x0a+(char)0x0d+(char)0x0a);
//file.WriteString(_T("2"));
}










}


void CTH2829TestDlg::OnModelNosn()
{
	// TODO: Add your command handler code here
UINT flag;	
CMenu* mmenu = GetMenu();
CMenu* submenu = mmenu->GetSubMenu(1);	
flag=GetMenu()->GetSubMenu(1)->GetMenuState(5,MF_CHECKED);
snselect=0;
if(flag&MF_CHECKED) 
{
GetMenu()->GetSubMenu(1)->CheckMenuItem(5,MF_CHECKED|MF_BYPOSITION); 	
GetMenu()->GetSubMenu(1)->EnableMenuItem(0,MF_UNCHECKED|MF_BYPOSITION|MF_GRAYED);
GetMenu()->GetSubMenu(1)->EnableMenuItem(3,MF_UNCHECKED|MF_BYPOSITION|MF_GRAYED);
GetMenu()->GetSubMenu(1)->EnableMenuItem(2,MF_UNCHECKED|MF_BYPOSITION|MF_GRAYED);
GetMenu()->GetSubMenu(1)->EnableMenuItem(4,MF_UNCHECKED|MF_BYPOSITION|MF_GRAYED);
GetMenu()->GetSubMenu(1)->EnableMenuItem(1,MF_UNCHECKED|MF_BYPOSITION|MF_GRAYED);
GetMenu()->GetSubMenu(1)->EnableMenuItem(6, MF_UNCHECKED | MF_BYPOSITION | MF_GRAYED);
}
if(!PathIsDirectory(_T("D:\\TH2829Test\\NOSN")))
{
	::CreateDirectory(_T("D:\\TH2829Test\\NOSN"),NULL);
}

CTime Time;
CString filename1,type,non,filename2;	
type=".txt";
non="";
Time = CTime::GetCurrentTime();
	 filename1="D:\\TH2829Test\\NOSN\\";
	 filename1=filename1+Time.Format("%Y")+type;
	 	
	 if(file.Open(filename1,CFile::modeNoTruncate|CFile::modeWrite)==NULL)
{
file.Open(filename1,CFile::modeCreate|CFile::modeWrite);
}
else
{
	file.SeekToEnd();
file.WriteString(non+(char)0x0d+(char)0x0a+(char)0x0d+(char)0x0a+(char)0x0d+(char)0x0a+(char)0x0d+(char)0x0a);
//file.WriteString(_T("2"));
//file.Close();
	 }



}


void CTH2829TestDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	COMSTAT ComStat;
	DWORD dwErrorFlags;
	BOOL bWriteStat;
	DWORD dwBytesWrite;
	unsigned char str_result[1893];
	DWORD wCount=1893;//读取的字节数
	BOOL bReadStat; 


	char bufe[256];
	HWND buttonhWnd,buttonhWnd1,buttonhWnd3,buttonhWnd4;
	buttonhWnd=::GetDlgItem(m_hWnd,IDOK);	
	CString ee;
	testflag=true;
	do
	{
		//AfxMessageBox(_T("why2"));
		hkb = SetWindowsHookExW(WH_KEYBOARD_LL, (HOOKPROC)&KeyboardProc, theApp.m_hInstance, 0);
		ee="";
	strcpy_s(bufe,"kk");	
	//AfxMessageBox(CString(bufe));
	//Gpread(THadd,"*IDN?\n",bufe);	 
	wCount=256;
	ClearCommError(hCom,&dwErrorFlags,&ComStat);
	bWriteStat=WriteFile(hCom,"*IDN?\n",6,& dwBytesWrite,NULL);
	
	memset(bufe,'\0',256);
	delay(100);
	bReadStat=ReadFile(hCom,bufe,wCount,&wCount,NULL);
	//AfxMessageBox(CString(bufe));
	if(!bReadStat)
		AfxMessageBox(_T("Read Com Fail!"));
	/*PurgeComm(hCom, PURGE_TXABORT|
		PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);*/
	ee=CString(bufe);
	//AfxMessageBox(ee);
	if( ee.Find(_T("Tong"))==-1)
{
//failj=1;

	m_listerr.AddString(_T("GPIB Fail"));	
GetDlgItem(IDC_TestResult)->SetWindowText(_T("FAIL"));
testflag=false;
	
	}
		
		if( ee.Find(_T("Tong"))!=-1)
		{	
		::SendMessage(buttonhWnd, BM_CLICK, 0, 0);  
		this->SetFocus();
		}
	
	}while((testflag==true)&&(snselect==1));
}

void RS232Open()
{
	//AfxMessageBox(CString(THadd));
	hCom=CreateFile(CString(THadd),//COM3口
		GENERIC_READ|GENERIC_WRITE, //允许读和写
		0, //独占方式
		NULL,
		OPEN_EXISTING, //打开而不是创建
		0, //同步方式
		NULL);
	if(hCom==INVALID_HANDLE_VALUE)
	{
		equipmentok=0;
		AfxMessageBox(_T("Open COM Fail!"));
		
		//return FALSE;
	}
	else
	{
equipmentok=1;
	}
SetupComm(hCom,1893,1893); //输入缓冲区和输出缓冲区的大小都是100

	COMMTIMEOUTS TimeOuts;
	//设定读超时
	TimeOuts.ReadIntervalTimeout=MAXDWORD;
	TimeOuts.ReadTotalTimeoutMultiplier=0;
	TimeOuts.ReadTotalTimeoutConstant=0;
	//在读一次输入缓冲区的内容后读操作就立即返回，
	//而不管是否读入了要求的字符。


	//设定写超时
	TimeOuts.WriteTotalTimeoutMultiplier=100;
	TimeOuts.WriteTotalTimeoutConstant=500;
	SetCommTimeouts(hCom,&TimeOuts); //设置超时

	DCB dcb;
	GetCommState(hCom,&dcb);
	dcb.BaudRate=9600; //波特率为9600
	dcb.ByteSize=8; //每个字节有8位
	dcb.Parity=NOPARITY; //无奇偶校验位
	dcb.StopBits= ONESTOPBIT; //两个停止位
	SetCommState(hCom,&dcb);

	PurgeComm(hCom,PURGE_TXCLEAR|PURGE_RXCLEAR);	




}

void CTH2829TestDlg::OnBnClickedButton4()
{
	// TODO: Add your control notification handler code here

/*	COMSTAT ComStat;
	DWORD dwErrorFlags;
	BOOL bWriteStat;
	DWORD dwBytesWrite;
//char common[256]="CH01Y!";
	char *common;
	char *common2;
	unsigned char str_result[1893];
	DWORD wCount=1893;//读取的字节数
	BOOL bReadStat;
	char buf[1893];
	char bufe[1893];
	CString Testspecmax,t1,Testspecmax2,hex[1893];
LPCTSTR pp1,pp2;
int i,length;

	





ClearCommError(hCom,&dwErrorFlags,&ComStat);
	common="*IDN?\n";
length=strlen(common);
sprintf_s(buf, "%d",length);
	AfxMessageBox(CString(buf));
	bWriteStat=WriteFile(hCom,common,length,& dwBytesWrite,NULL);
	
	memset(bufe,'\0',1893);
	delay(100);
	bReadStat=ReadFile(hCom,bufe,wCount,&wCount,NULL);
	//AfxMessageBox(CString(bufe));
	if(!bReadStat)
		AfxMessageBox(_T("Read Com Fail!"));
	PurgeComm(hCom, PURGE_TXABORT|
		PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
//CloseHandle(hCom);
//	RS232Open();



	







	wCount=1893;


ClearCommError(hCom,&dwErrorFlags,&ComStat);
	bWriteStat=WriteFile(hCom,"TRIG\n",5,& dwBytesWrite,NULL);
	delay(800);
	ClearCommError(hCom,&dwErrorFlags,&ComStat);
	bWriteStat=WriteFile(hCom,"TRS:DATA?\n",10,& dwBytesWrite,NULL);
	//bWriteStat=WriteFile(hCom,"TRIG\n",5,& dwBytesWrite,NULL);
	memset(str_result,'\0',1893);
	delay(2000);
	bReadStat=ReadFile(hCom,str_result,wCount,&wCount,NULL);
	if(!bReadStat)
		AfxMessageBox(_T("Read Com Fail!"));
	PurgeComm(hCom, PURGE_TXABORT|
		PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
//AfxMessageBox(CString(str_result));
	//delay(300);
	for(i=0;i<1891;i++)
{
sprintf_s(buf, "%02X",str_result[i]);
hex[i]=CString(buf);
}
AfxMessageBox(hex[1730]);
*/


}


void CTH2829TestDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CloseHandle(hCom);
	CDialogEx::OnCancel();
}


void CTH2829TestDlg::OnModelM1877()
{
	// TODO: Add your command handler code here
	UINT flag;
	CMenu* mmenu = GetMenu();
	CMenu* submenu = mmenu->GetSubMenu(1);
	flag = GetMenu()->GetSubMenu(1)->GetMenuState(1, MF_CHECKED);
	if (flag&MF_CHECKED)
	{
		GetMenu()->GetSubMenu(1)->CheckMenuItem(6, MF_CHECKED | MF_BYPOSITION);
		GetMenu()->GetSubMenu(1)->EnableMenuItem(0, MF_UNCHECKED | MF_BYPOSITION | MF_GRAYED);
		GetMenu()->GetSubMenu(1)->EnableMenuItem(3, MF_UNCHECKED | MF_BYPOSITION | MF_GRAYED);
		GetMenu()->GetSubMenu(1)->EnableMenuItem(2, MF_UNCHECKED | MF_BYPOSITION | MF_GRAYED);
		GetMenu()->GetSubMenu(1)->EnableMenuItem(4, MF_UNCHECKED | MF_BYPOSITION | MF_GRAYED);
		GetMenu()->GetSubMenu(1)->EnableMenuItem(5, MF_UNCHECKED | MF_BYPOSITION | MF_GRAYED);
		GetMenu()->GetSubMenu(1)->EnableMenuItem(1, MF_UNCHECKED | MF_BYPOSITION | MF_GRAYED);
	}
	if (!PathIsDirectory(_T("D:\\TH2829Test\\M1877")))
	{
		::CreateDirectory(_T("D:\\TH2829Test\\M1877"), NULL);
	}
	modelname = "M1877";
	CTime Time;
	CString filename1, type, non, filename2;
	type = ".txt";
	non = "";
	Time = CTime::GetCurrentTime();
	filename1 = "D:\\TH2829Test\\M1877\\";
	filename1 = filename1 + Time.Format("%Y") + type;

	if (file.Open(filename1, CFile::modeNoTruncate | CFile::modeWrite) == NULL)
	{
		file.Open(filename1, CFile::modeCreate | CFile::modeWrite);
	}
	else
	{
		file.SeekToEnd();
		file.WriteString(non + (char)0x0d + (char)0x0a + (char)0x0d + (char)0x0a + (char)0x0d + (char)0x0a + (char)0x0d + (char)0x0a);
		//file.WriteString(_T("2"));
	}
}
