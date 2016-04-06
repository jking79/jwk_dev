/* Standalone routine: FED for data taking
*/

#include <iostream>
using namespace std;
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h> 
#include<fstream>

#include "CAENVMElib.h"  // CAEN library prototypes
#include "pxlfed.h"
#include "OPTOInterface.h"
#define REPETITION 100000.0

unsigned int revfifo3[2048];
unsigned int lastErrorValue[8];
    int i,j,s;
//set this to 0 for VMEFED trigger, 1 for TTC trigger    

///////////////////////////////////////////////////////////////////////////

void VMEin(int AddressType, unsigned long int Address, int DataSize, unsigned long int *Data);
void VMEout(int AddressType, unsigned long int Address, int DataSize, unsigned long int Data);


void BinDisplay(unsigned long int In_Word, int Bits,int Header,int del)
{
  int i1;
  unsigned long mask;

  mask=1<<(Bits-1);

  if (Header) {
    //printf("\n\233\67\155\n");
    if(del) printf("|");
    for (i1=Bits;i1>0;i1--) {
      printf("%1d",(i1-1)%10);
    if(del) {if ( !((i1-1)%4) ) printf("|");}
    }
    printf("\n");
    //printf("\233\60\155 \n");
  }
  if (del) printf("|");
  for (i1=0;i1<Bits;i1++) {
    if ((In_Word<<i1) & mask) printf("1");
    else printf("0");
    if(del) {if ( !((i1+1)%4) ) printf("|");  }
  }
  //printf("\n");

  return;

}
///////////////////////////////////////////////////////////////////////

  CVErrorCodes    ret; // return code
    
// MAIN

int main(int argc, char *argv[]) {
  CVBoardTypes  VMEBoard = cvV2718;  // define interface type
  short         Link = 0;  // define device & link
  short         Device = 0;
  double	runtime = 0;
  string 	dummy;

  if( argc > 1 ){
	 runtime = (double)atoi(argv[1]);
	 cout << "set runtime to : " << runtime << endl;
//	 cin >> dummy;
  }
  else runtime = 60; 

  // Open interface
  if( CAENVME_Init(VMEBoard, Device, Link, &BHandle) != cvSuccess ) {
    cout<<" Error opening the device"<<endl;
    exit(1);
  }
	char FWrRel[3];
  ret = CAENVME_BoardFWRelease(BHandle,FWrRel);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in accessing VME "<<hex<<ret<<" CAEN firmware check failed "<<endl;
    analyzeError(ret); }
  else		    
    cout<<"Board Firmware release "<<FWrRel<<endl;
 
//FEDBASE=0x3C000000;
LAD_N	     =(FEDBASE);
LAD_NC       =(FEDBASE+0x200000);
LAD_SC       =(FEDBASE+0x400000);
LAD_S	     =(FEDBASE+0x600000);
LAD_C	     =(FEDBASE+0x800000);

 for(int i=0;i<8;++i) {
    lastErrorValue[i]=0x0;
  }
	unsigned long du, dd;
	VMEin(0x3,LAD_S+0x158000,4,&du);
	VMEin(0x3,LAD_S+0x178000,4,&dd);


//	    vmeDevicePtr->read("LAD_S", &du, 0x158000);
//	vmeDevicePtr->read("LAD_S", &dd, 0x178000);
	cout<<" S Piggy version: ";
	printf("%1x%1x%1x%1x%1x%1x\n", (dd>>20)&0xf, (dd>>12)&0xf, (dd>>4)&0xf, (du>>20)&0xf, (du>>12)&0xf, (du>>4)&0xf);

  // Define VME parameters 
  CVDataWidth dw = cvD32 ; // data width (see CAENVMEtypes.h )
  CVAddressModifier am = cvA32_U_DATA;

       // Only South Piggy ch 33/34 enabled
       VMEout( 0x3,LAD_N+0x1a0000,4,0x1ff);
       VMEout(0x3,LAD_NC+0x1a0000,4,0x1ff);
       VMEout(0x3,LAD_SC+0x1a0000,4,0x1ff);
       VMEout( 0x3,LAD_S+0x1a0000,4,0x19f); //0x19f



        //Set transparent FIFO to fiber 11 on S piggy

        VMEout(0x3, LAD_S + 0x1a8000, 4, 0x80002304); // ch selection for transparent mode:
                                                                                 // 5-fiber#12, 4-fiber#11, 3-fiber#10,           
                                                                                 // 2-fiber#9, 1-fiber# 8, 0-fiber#7

	 TTCRX_I2C_REG_WRITE( 2, 0xa); //COARSE DELAY REG

  
cout<<endl;
cout<<"A test of the fifo-3 operation at high rates"<<endl;
cout<<"We run triggers as in the tif, and check for wordcount and unpacking errors"<<endl;
cout<<endl;


std::ofstream fout;
fout.open("spySlink.dat", std::ios::binary|std::ios::app);
unsigned long long TopMark[1];
TopMark[0]=0xf0f0f0f0;
fout.write((char*)TopMark,8);

std::ofstream errout;
errout.open("Error.dat", std::ios::binary|std::ios::app);



unsigned int evcnt=0;
unsigned int newevent=0;
cout<<"TTC set for tif events, 5000 events being processed"<<endl;

unsigned int vmeAddress = 0;
unsigned int data = 0;

time_t start, stop;
time(&start);
time(&stop);
//---------------------------------------------------------loop start -----------------------------------------------------------
//for(int ierrs=0;ierrs<50;ierrs++){
while( difftime(stop, start) <= runtime ){
//cout << "Top loop: " << (int)difftime(stop, start ) << " : " << (int)runtime << endl;

while(evcnt==newevent)
{
vmeAddress = LAD_C+0x190000;
//  
ret = CAENVME_ReadCycle(BHandle,vmeAddress,&newevent,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); } 
}		    
evcnt=newevent;
//-have a new event

////////////////////////////////////////  
/////               piggydown
/////////////////////////////////////

       unsigned long d;
       printf("\nPIGGYdown\n");
       for(int i2=0;i2<256;i2++) {
             VMEin(0x3,LAD_S+0x20000,4,&d);
             if (i2<42) {
                   printf("%04x %04x              ",(d>>16)&0xffff, d&0xffff);
                   BinDisplay((d>>16)&0xffff, 16,0,0);
                   printf("    ");
                   BinDisplay(d&0xffff, 16,0,0);
                   printf("\n");

             }


       }

///////////////////////////////////////

        //Scope FIFO
                unsigned long scopeFIFOreadaddr, scopeMODEREGaddr, scopeMODEREGdata;
                unsigned long int destaddr[1024];

                 scopeFIFOreadaddr=LAD_S+0x150000;    scopeMODEREGaddr=LAD_S+0x1c0000;   scopeMODEREGdata=0x5<<8; //CH#33
                 //scopeFIFOreadaddr=LAD_S+0x150000;     scopeMODEREGaddr=LAD_S+0x1c0000;   scopeMODEREGdata=0x3<<8; //CH#31
                 //scopeFIFOreadaddr=LAD_SC+0x150000;     scopeMODEREGaddr=LAD_SC+0x1c0000;  scopeMODEREGdata=0x8<<8; //CH#27
                 //scopeFIFOreadaddr=LAD_N+0x150000; scopeMODEREGaddr=LAD_N+0x1c0000;   scopeMODEREGdata=0x2<<8; //CH#3

                 printf("\n\nSCOPE FIFO:\n");
                 VMEout (0x3,scopeMODEREGaddr, 4, scopeMODEREGdata);
                 printf("%x\n", scopeMODEREGaddr);  printf("%x\n", scopeMODEREGdata); printf("%x\n", scopeFIFOreadaddr);

                 s=1024;
                 for(i=0;i<1024;i++) VMEin (0x3,scopeFIFOreadaddr, 4,  &destaddr[i]);
                 //VXImove(0x1003,scopeFIFOreadaddr,0,(UINT32)destaddr,s,4);
                  for(i=0;i<1024;i++) {
                   //printf("%3x",(destaddr[i]&0xff));
                   if ((destaddr[i]&0xff)) printf("%3x",(destaddr[i]&0xff));
                   if (((destaddr[i]&0xff)>>4)==0xb) printf("          %4d\n",(destaddr[i]&0x3ff00)>>8);
                   if (((destaddr[i]&0xff)>>4)==0x7) printf("                   %4d\n",(destaddr[i]&0x3ff00)>>8);
                   if (((destaddr[i]&0xff)>>4)==0x6) printf("    %4d\n",(destaddr[i]&0x3ff00)>>8);
                   if (((destaddr[i]&0xff)>>4)==0xf) printf("          %4d\n",(destaddr[i]&0x3ff00)>>8);
                  }
                  printf("\n\n");
                 /////////////////////

unsigned long fif3done=0x0;


vmeAddress = LAD_C+0x190000;
//  
ret = CAENVME_ReadCycle(BHandle,vmeAddress,&fif3done,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); } 
//cout<<"before= 0x"<<hex<<fif3done<<endl;

//-first check
int iwait=0;

while(((fif3done&0x80000000)==0x80000000)&&(iwait<1000))
{
vmeAddress = LAD_C+0x190000;
//  
ret = CAENVME_ReadCycle(BHandle,vmeAddress,&fif3done,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); } 

//cout<<"pre-check 0x"<<hex<<fif3done<<endl;
    usleep(1000);
iwait++;    
}		    

vmeAddress = LAD_C+0x190000;
//  
ret = CAENVME_ReadCycle(BHandle,vmeAddress,&fif3done,am,dw);	
  if(ret != cvSuccess) {  // Error100000
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); } 
//cout<<"After 1st wait= 0x"<<hex<<fif3done<<endl;


  data = 0x2; 
  vmeAddress = LAD_C+0x1c0000;
  ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); } usleep(100);		    

fif3done=0x0;
iwait=0;
while(((fif3done&0x80000000)!=0x80000000)&&(iwait<1000))
{
vmeAddress = LAD_C+0x190000;
//  
ret = CAENVME_ReadCycle(BHandle,vmeAddress,&fif3done,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); } 
    //cout<<"in disabled checking state. 0x"<<hex<<fif3done<<endl;
    usleep(1000);
iwait++;    
}		    
cout<<"iwait = "<<dec<<iwait<<endl;

int wdcnt=0;
if(iwait<1000){ wdcnt=spySlink64();
//reverse words for binary dump file
for(int io=0;io<2*(wdcnt+1);io+=2){
revfifo3[io]=fif3buff[io+1];
revfifo3[io+1]=fif3buff[io];
}
cout<<"0x"<<hex<<fif3buff[0]<<endl;
cout<<"0x"<<hex<<fif3buff[1]<<endl;
fout.write((char*)revfifo3,(2*(wdcnt+1))*sizeof(unsigned int));
cout<<"0x"<<hex<<fif3buff[2*wdcnt]<<endl;
cout<<"0x"<<hex<<fif3buff[2*(wdcnt+1)]<<endl;
} else {cout<<"fifo-3 timeout"<<endl;}
cout<<"wordcount "<<dec<<wdcnt<<endl;
vmeAddress = LAD_C+0x190000;
//  
ret = CAENVME_ReadCycle(BHandle,vmeAddress,&fif3done,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); } 
//cout<<"After 1st Spy fifo read= 0x"<<hex<<fif3done<<endl;


  data = 0x0; 
  vmeAddress = LAD_C+0x1c0000;
  ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);	
  if(ret != cvSuccess) {  // Err100000or
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); } usleep(10);

	time(&stop);
	cout << "time: " << (double)difftime(stop, start)<< endl;
//	cin >> dummy;	

}//ierr loop

fout.close();
cout<<"done"<<endl;
 CAENVME_End(BHandle);
}

void VMEin(int AddressType, unsigned long int Address, int DataSize, unsigned long int *Data) {

  CVAddressModifier AM;
  CVDataWidth DW;

  AM=cvA32_U_DATA;

  switch (DataSize)
    {
    case 4:  DW=cvD32; break;
    case 2:  DW=cvD16; break;
    default: DW=cvD32; break;
    }

  ret = CAENVME_ReadCycle(BHandle,Address,Data,AM,DW);
  //printf("%x\n",(unsigned long int)*Data);
  if(ret != cvSuccess) analyzeError(ret);

}
void VMEout(int AddressType, unsigned long int Address, int DataSize, unsigned long int Data) {

  CVAddressModifier AM;
  CVDataWidth DW;

  AM=cvA32_U_DATA;

  switch (DataSize)
    {
    case 4:  DW=cvD32; break;
    case 2:  DW=cvD16; break;
    default: DW=cvD32; break;
    }

  ret = CAENVME_WriteCycle(BHandle,Address,&Data,AM,DW);
  if(ret != cvSuccess) analyzeError(ret);
}

