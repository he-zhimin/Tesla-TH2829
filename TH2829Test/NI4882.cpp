#include "stdafx.h"
#include "NI4882.h"
//#include "TH2829TestDlg.h"
long s1;

void delay(int Time)
{long t1,t2;
 t1=GetTickCount();
 do{
 t2=GetTickCount();
 }while(t2-t1<Time);
 }

void Gpwrite(char* Address,char* cmd )
   { //long s; //defaulrm
     //char Address[256];//address
    // long Accessmodel;
    // unsigned long P;
     long h;
     long t;//ststus
     unsigned long r;//transfer len
     t=viOpen(s1,Address,0,0,&h);
     if(t!=0)
	 {/* AfxMessageBox(_T("GPIB ERR"));*/ }

     //t=viSetAttribute(h,1073676314,1000);
     t=viWrite(h,cmd,strlen(cmd),&r);
     t=viClose(h);
       // delay(50);
         }
  void Gpread(char Address[256],char cmd[256],char buf[256])
        {long h;
         long t;//ststus
         unsigned long r;//transfer len
         t=viOpen(s1,Address,0,0,&h);
           if(t!=0)
     { /*AfxMessageBox(_T("GPIB ERR"));*/}
        // t=viSetAttribute(h,1073676314,5000);
         // delay(50);
         t=viWrite(h,cmd,strlen(cmd),&r);
           //delay(50);
         t=viRead(h,buf,256,&r);
         t= viClose(h);
       //  delay(50);
          }

 void Gpread2(char Address[256],char cmd[256],unsigned char buf[1893])
        {long h;
         long t;//ststus
         unsigned long r;//transfer len
        char buf2[1893];
		int i;
		t=viOpen(s1,Address,0,0,&h);
           if(t!=0)
     {/* AfxMessageBox(_T("GPIB ERR")); */}
        // t=viSetAttribute(h,1073676314,5000);
         // delay(50);
         t=viWrite(h,cmd,strlen(cmd),&r);
          // delay(500);
         t=viRead(h,buf2,1893,&r);
       for( i=0;i<1891;i++)
{
	

	buf[i]=(unsigned char)buf2[i];
	//sprintf_s(buf, "%02X",tmp);
		
	   }	 
		 
		 
		 
		 
		 t= viClose(h);
         //delay(50);
          }

void Gpread4(char Address[256],char cmd[256],char* buf)
        {long h;
         long t;//ststus
         unsigned long r;//transfer len
         t=viOpen(s1,Address,0,0,&h);
           if(t!=0)
     {/* AfxMessageBox(_T("GPIB ERR")); */}
        // t=viSetAttribute(h,1073676314,5000);
         // delay(50);
         t=viWrite(h,cmd,strlen(cmd),&r);
          // delay(500);
         t=viReadToFile(h,buf,18812,&r);
         t= viClose(h);
         //delay(50);
          }

 
 void Gpread3(char Address[256],char cmd[256],char buf[43])
        {long h;
         long t;//ststus
         unsigned long r;//transfer len
         t=viOpen(s1,Address,0,0,&h);
           if(t!=0)
     { /*AfxMessageBox(_T("GPIB ERR"));*/ }
        // t=viSetAttribute(h,1073676314,5000);
         // delay(50);
         t=viWrite(h,cmd,strlen(cmd),&r);
          // delay(500);
         t=viRead(h,buf,43,&r);
         t= viClose(h);
         //delay(50);
          }
void Gpib_Init()
{
 long x;
  long t; //ststus
   //long s; //defaulrm
  long v; //count
  long u;//nlist
  char str[256],str2[256];
  t=viOpenDefaultRM(&s1);
   t=viStatusDesc(s1,t,str);
   t=viFindRsrc(s1,"?*",&u,&v,str2);
 
  // AfxMessageBox(CString(str2));
   //Instrument->Items->Strings[0]=str2;
//m_Str=str2;
//m_lisAuto.AddString(str2);
 x=0;
   do{
   t=viFindNext(u,str2);
   t=viStatusDesc(s1,t,str);
    x=x+1;
 // AfxMessageBox(CString(str2));
	// m_lisAuto.AddString(str2); 
  }while(x<v);		



}

char * Gpiblist()
{
  char *result;
 long x;
  long t; //ststus
   //long s; //defaulrm
  long v; //count
  long u;//nlist
  char str[256],str2[256],str3[256];
 t=viOpenDefaultRM(&s1);
   t=viStatusDesc(s1,t,str);
   t=viFindRsrc(s1,"?*",&u,&v,str2);
   //Instrument->Items->Strings[0]=str2;
//m_Str=str2;
//m_lisAuto.AddString(str2);
   strcpy_s(str3,256,str2);
   x=0;
 //  AfxMessageBox(CString(str3));
   //  do
  while(x<v)
   {
   t=viFindNext(u,str2);
   t=viStatusDesc(s1,t,str);
    x=x+1;
  strcat_s(str3,",");
	strcat_s(str3,str2);
  
	// m_lisAuto.AddString(str2); 
  }/*while(x<v);	*/	

 result=str3;
	
	
	
	return result;
}