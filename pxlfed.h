#include <math.h>

// Stuff
  int          BHandle; // pointer to the device

//Fed Base Address	
unsigned int FEDBASE = 0x3C000000;
unsigned int fedNumber;

//Settable optical input parameters
unsigned int opt_cap[3],opt_inadj[3],opt_ouadj[3];

//input offset dac
unsigned int offs_dac[36];

//clock phases
unsigned int clkphs0_9,clkphs10_18,clkphs19_27,clkphs28_36;

//Blacks 
unsigned int BlackHi[36],BlackLo[36],Ublack[36];

//Channel delays
unsigned int DelayCh[36];

//Signal levels
unsigned int TBM_L0[36],TBM_L1[36],TBM_L2[36],TBM_L3[36],TBM_L4[36];
unsigned int TRL_L0[36],TRL_L1[36],TRL_L2[36],TRL_L3[36],TRL_L4[36];
unsigned int ROC_L0[36][26],ROC_L1[36][26],ROC_L2[36][26],ROC_L3[36][26],ROC_L4[36][26];

//These bits turn off(1) and on(0) channels
unsigned int Ncntrl,NCcntrl,SCcntrl,Scntrl;

//These are delays to the TTCrx
unsigned int CoarseDel,ClkDes2,FineDes2Del;

//Main control reg for determining the DAQ mode
unsigned int Ccntrl;

//Controls Slink
unsigned int Slink;

//Number of ROCS per FED channel
unsigned int NRocs[36];

//ADC gain bits
unsigned int Nadcg,NCadcg,SCadcg,Sadcg;

//Baseline adjustment bits
unsigned int Nbaseln,NCbaseln,SCbaseln,Sbaseln;


// TTCRX_I2C_REG_WRITE( 2, 0xa); //COARSE DELAY REG
unsigned int LAD_N;       
unsigned int LAD_NC;      
unsigned int LAD_SC;     
unsigned int LAD_S;       
unsigned int LAD_C;       
unsigned int READ_GA;     
unsigned int I2C_RES = (FEDBASE+0xa00008);     
unsigned int I2C_LOAD = (FEDBASE+0xa0000c);    
unsigned int I2C_ADDR_RW = (FEDBASE+0xa00010); 
unsigned int I2C_RD_DATA = (FEDBASE+0xa00010); 
unsigned int I2C_RD_STAT = (FEDBASE+0xa00014); 
unsigned int RES_TTCrx = (FEDBASE+0xa00038);   
unsigned int LRES = (FEDBASE+0xa00000);	
unsigned int CLRES =  (FEDBASE+0xa00004);       
unsigned int nCONFIG = (FEDBASE+0xa0003c);     
unsigned int CHIP[4],CH_SubAddr[9];

unsigned int trnsbuff[1024];
unsigned int fif3buff[2048];



int TTCRX_I2C_REG_READ( int Register_Nr);
int TTCRX_I2C_REG_WRITE( int Register_Nr, int Value);
/*void fill_dac_reg();
void decode_spy_data_fifo(int word);
void decode_error_fifo(int word);
int *drain_fifo(unsigned int VmeAddress, int *wdcnt);
void decode_tempdac_fifo(int word);
int config_file();
void set_opto_params();
void set_offset_dacs();
void set_clock_phases();
void set_chnl_delays();
void set_blk_ublk_thold();
void set_data_levels();
void set_chnls_onoff();
int read_vme_event();
void reset_slink();
int grab_transp_ch(int chnl);
void decode_trans();
void set_baseline();
void set_adcgain();
void fill_dac_val(int value);
void fill_dac_vals(int *val1,int *val2,int *val3);
int tdac_header(int event,int *dac,int start,unsigned int V_OFFSET,unsigned int UBLK,unsigned int BLK);
int tdac_trailer(int word,int *dac,int start,unsigned int V_OFFSET,unsigned int UBLK,unsigned int BLK);
int tdac_roc(int lastdac,int *dac,int start,unsigned int V_OFFSET,unsigned int UBLK,unsigned int BLK);
int tdac_pxlhit(int dbc,int pixl,int adc,int *dac,int start,unsigned int V_OFFSET,unsigned int UBLK,unsigned int BLK);
void send_vmetrig();
void fitls(int ndata, double *x,double *y,double *ey,double *a,double *b,double *ea,double *eb,double *cov,double *chi2);
void decode_trans_info(int *nrocs,int *lastdacs,int *nhits,int *dcols,int *pxls,int *adcs,int *hroc);
void dump_BaselineCorr();
int grab_fifo3();
// A routine to analyse error conditions
void analyzeError(CVErrorCodes ret); 
*/////////////////////////////////

void analyzeError(CVErrorCodes ret) {
  switch (ret) {
  case cvGenericError   : cout<<" Generic error !!!"<<endl;
    break ;
  case cvBusError : cout<<" Bus Error !!!"<<endl;
    break ;                                                                    
  case cvCommError : cout<<" Communication Error !!!"<<endl;
    break ;
  case cvInvalidParam : cout<<" Invalid Param Error !!!"<<endl;
    break ;
  default          : cout<<" Unknown Error !!!"<<endl;
    break ;
  }
}



int TTCRX_I2C_REG_READ( int Register_Nr)
 
{
 
        unsigned int d;
                                  int  i2c_addr,i2c_nbytes;
 
  CVErrorCodes ret; // return code
  CVDataWidth dw = cvD32 ; // data width (see CAENVMEtypes.h )
  CVAddressModifier am = cvA32_U_DATA;
 
 
//  RESET I2C STATE MACHINE : Reset procedure :  set 0x2 then set 0x0
   int ds =  0x2;
     ret = CAENVME_WriteCycle(BHandle,I2C_RES,&ds,am,dw);
     if(ret != cvSuccess) {  // Error
     cout<<"Error in write TTCRX_I2C_REG_READ"<<hex<<ret<<" "<<ds<<endl;
     analyzeError(ret); }
    usleep(10);
    ds =  0x0;
     ret = CAENVME_WriteCycle(BHandle,I2C_RES,&ds,am,dw);
     if(ret != cvSuccess) {  // Error
     cout<<"Error in write TTCRX_I2C_REG_READ"<<hex<<ret<<" "<<ds<<dec<<endl;
     analyzeError(ret); }
    usleep(10);//
 
// I2C PAYLOAD
    ds =  Register_Nr;
     ret = CAENVME_WriteCycle(BHandle,I2C_LOAD,&ds,am,dw);
     if(ret != cvSuccess) {  // Error
     cout<<"Error in write TTCRX_I2C_REG_READ"<<hex<<ret<<" "<<ds<<dec<<endl;
 TTCRX_I2C_REG_WRITE( 2, 0xa); //COARSE DELAY REG
     analyzeError(ret); }
    usleep(10);//
 
     i2c_addr=7*2; 
     i2c_nbytes=1;
//VXIout (0x3,I2C_ADDR_RW, 2, (i2c_nbytes<<8)+(i2c_addr<<1)+0/*WRITE*/);
    ds =  (i2c_nbytes<<8)+(i2c_addr<<1)+0;
     ret = CAENVME_WriteCycle(BHandle,I2C_ADDR_RW,&ds,am,dw);
     if(ret != cvSuccess) {  // Error
     cout<<"Error in write TTCRX_I2C_REG_READ"<<hex<<ret<<" "<<ds<<dec<<endl;
     analyzeError(ret); }
   usleep(10);//
 
                   usleep(300);
 
//Check Status
    ret = CAENVME_ReadCycle(BHandle,I2C_RD_STAT,&d,am,dw);
    if(ret != cvSuccess) {
    cout<<"Error in read TTCRX_I2C_REG_READ"<<hex<<ret<<" "<<ds<<dec<<endl;
    analyzeError(ret);   }
   usleep(10);//
   
//Report Status 
 if((d&0xff)==1)printf("ERROR: BUS BUSY !! \n");
 if((d&0xff)==2)printf("ERROR: I2C_ADDR NOT ACKNOWLEDGED !! \n");
 if((d&0xff)==4)printf("ERROR: I2C_WBYTE NOT ACKNOWLEDGED !! \n");
 if((d&0xff)==8)printf("ERROR: I2C_LBYTE NOT ACKNOWLEDGED !! \n");
 
//  RESET I2C STATE MACHINE
   ds =  0x2;
     ret = CAENVME_WriteCycle(BHandle,I2C_RES,&ds,am,dw);
     if(ret != cvSuccess) {  // Error
     cout<<"Error in write TTCRX_I2C_REG_READ"<<hex<<ret<<" "<<ds<<dec<<endl;
     analyzeError(ret); }
   usleep(10);
    ds =  0x0;
     ret = CAENVME_WriteCycle(BHandle,I2C_RES,&ds,am,dw);
     if(ret != cvSuccess) {  // Error
     cout<<"Error in write TTCRX_I2C_REG_READ"<<hex<<ret<<" "<<ds<<dec<<endl;
     analyzeError(ret); }
    usleep(10);
 
 
                    /////////////////////////////  I2C READ  //////////////////////////////////
 
    i2c_addr=7*2 +1 ; i2c_nbytes=1;
//VXIout (0x3,I2C_ADDR_RW, 2, (i2c_nbytes<<8)+(i2c_addr<<1)+0/*WRITE*/);
    ds =  (i2c_nbytes<<8)+(i2c_addr<<1)+1;
     ret = CAENVME_WriteCycle(BHandle,I2C_ADDR_RW,&ds,am,dw);
     if(ret != cvSuccess) {  // Error
     cout<<"Error in write TTCRX_I2C_REG_READ"<<hex<<ret<<" "<<ds<<dec<<endl;
     analyzeError(ret); }
    usleep(10);//
//Check Status
     ret = CAENVME_ReadCycle(BHandle,I2C_RD_STAT,&d,am,dw);
      if(ret != cvSuccess) {
      cout<<"Error in read TTCRX_I2C_REG_READ"<<hex<<ret<<" "<<ds<<dec<<endl;
      analyzeError(ret);   }
    usleep(10);//
//Report Status 
 if((d&0xff)==1)printf("ERROR: BUS BUSY !! \n");
 if((d&0xff)==2)printf("ERROR: I2C_ADDR NOT ACKNOWLEDGED !! \n");
    

             ret = CAENVME_ReadCycle(BHandle,I2C_RD_DATA,&d,am,dw);
            if(ret != cvSuccess) {
            cout<<"Error in read TTCRX_I2C_REG_READ"<<hex<<ret<<" "<<ds<<dec<<endl;
            analyzeError(ret);   }
 
   usleep(10);//
 
 
 
                           return(d&0xff);
}
 
 
 
 
 
 
 
 
 
int TTCRX_I2C_REG_WRITE( int Register_Nr, int Value)
 
{
 
        unsigned int d;
                                  int  i2c_addr,i2c_nbytes;
 
  CVErrorCodes ret; // return code
  CVDataWidth dw = cvD32 ; // data width (see CAENVMEtypes.h )
  CVAddressModifier am = cvA32_U_DATA;
 
 
//- VXIout (0x3,I2C_RES, 2, 0x2);   //  RESET I2C STATE MACHINE
   int ds =  0x2;
     ret = CAENVME_WriteCycle(BHandle,I2C_RES,&ds,am,dw);
     if(ret != cvSuccess) {  // Error
     cout<<"Error in write TTCRX_I2C_REG_WRITE"<<hex<<ret<<" "<<ds<<dec<<endl;
     analyzeError(ret); }
     usleep(1000);
 
// VXIout (0x3,I2C_RES, 2, 0x0);
    ds =  0x0;
     ret = CAENVME_WriteCycle(BHandle,I2C_RES,&ds,am,dw);
     if(ret != cvSuccess) {  // Error
     cout<<"Error in write TTCRX_I2C_REG_WRITE"<<hex<<ret<<" "<<ds<<dec<<endl;
     analyzeError(ret); }
     usleep(1000);
 
// I2C PAYLOAD 
    ds =  Register_Nr;
     ret = CAENVME_WriteCycle(BHandle,I2C_LOAD,&ds,am,dw);
     if(ret != cvSuccess) {  // Error
     cout<<"Error in write TTCRX_I2C_REG_WRITE"<<hex<<ret<<" "<<ds<<dec<<endl;
     analyzeError(ret); }
   usleep(1000);
 
     i2c_addr=7*2; i2c_nbytes=1;
//VXIout (0x3,I2C_ADDR_RW, 2, (i2c_nbytes<<8)+(i2c_addr<<1)+0/*WRITE*/);
    ds =  (i2c_nbytes<<8)+(i2c_addr<<1)+0;
     ret = CAENVME_WriteCycle(BHandle,I2C_ADDR_RW,&ds,am,dw);
     if(ret != cvSuccess) {  // Error
     cout<<"Error in write TTCRX_I2C_REG_WRITE"<<hex<<ret<<" "<<ds<<dec<<endl;
     analyzeError(ret); }
   usleep(1000);
 
                   usleep(300);
 
//                 VXIin (0x3,I2C_RD_STAT,2,&d);
             ret = CAENVME_ReadCycle(BHandle,I2C_RD_STAT,&d,am,dw);
            if(ret != cvSuccess) {
            cout<<"Error in read TTCRX_I2C_REG_WRITE"<<hex<<ret<<" "<<ds<<dec<<endl;
            analyzeError(ret);   }
   usleep(1000);//
 
 
                   if((d&0xff)==1)printf("ERROR: BUS BUSY !! \n");
                   if((d&0xff)==2)printf("ERROR: I2C_ADDR NOT ACKNOWLEDGED !! \n");
                   if((d&0xff)==4)printf("ERROR: I2C_WBYTE NOT ACKNOWLEDGED !! \n");
                   if((d&0xff)==8)printf("ERROR: I2C_LBYTE NOT ACKNOWLEDGED !! \n");
 
//  RESET I2C STATE MACHINE
   ds =  0x2;
     ret = CAENVME_WriteCycle(BHandle,I2C_RES,&ds,am,dw);
     if(ret != cvSuccess) {  // Error
     cout<<"Error in write TTCRX_I2C_REG_WRITE"<<hex<<ret<<" "<<ds<<dec<<endl;
     analyzeError(ret); }
       
   usleep(1000);
//wait10usx(10);

 
// VXIout (0x3,I2C_RES, 2, 0x0);
    ds =  0x0;
     ret = CAENVME_WriteCycle(BHandle,I2C_RES,&ds,am,dw);
     if(ret != cvSuccess) {  // Error
     cout<<"Error in write TTCRX_I2C_REG_WRITE"<<hex<<ret<<" "<<ds<<dec<<endl;
     analyzeError(ret); }
   usleep(1000);
 
 
//                 VXIout (0x3,I2C_LOAD, 2, Value);    // I2C PAYLOAD
    ds =  Value;
     ret = CAENVME_WriteCycle(BHandle,I2C_LOAD,&ds,am,dw);
     if(ret != cvSuccess) {  // Error
     cout<<"Error in write TTCRX_I2C_REG_WRITE"<<hex<<ret<<" "<<ds<<dec<<endl;
     analyzeError(ret); }
   usleep(1000);//
  
 
//                 i2c_addr=(7*2)+1; i2c_nbytes=1;
//                         VXIout (0x3,I2C_ADDR_RW, 2, (i2c_nbytes<<8)+(i2c_addr<<1)+0/*WRITE*/);
//                         wait10usx(30);//
//
//                         VXIin (0x3,I2C_RD_STAT,2,&d);
//                 if((d&0xff)==1)printf("ERROR: BUS BUSY !! \n");
//                 if((d&0xff)==2)printf("ERROR: I2C_ADDR NOT ACKNOWLEDGED !! \n");
//                 if((d&0xff)==4)printf("ERROR: I2C_WBYTE NOT ACKNOWLEDGED !! \n");
//                 if((d&0xff)==8)printf("ERROR: I2C_LBYTE NOT ACKNOWLEDGED !! \n");
    i2c_addr=7*2+1; i2c_nbytes=1;
//VXIout (0x3,I2C_ADDR_RW, 2, (i2c_nbytes<<8)+(i2c_addr<<1)+0/*WRITE*/);
    ds =  (i2c_nbytes<<8)+(i2c_addr<<1)+0;
     ret = CAENVME_WriteCycle(BHandle,I2C_ADDR_RW,&ds,am,dw);
     if(ret != cvSuccess) {  // Error
     cout<<"Error in write TTCRX_I2C_REG_WRITE"<<hex<<ret<<" "<<ds<<dec<<endl;
     analyzeError(ret); }
   usleep(1000);//
 
                           //printf("%x\n",(i2c_nbytes<<8)+(i2c_addr<<1)+0 );
                   usleep(300);
 
//                 VXIin (0x3,I2C_RD_STAT,2,&d);
             ret = CAENVME_ReadCycle(BHandle,I2C_RD_STAT,&d,am,dw);
            if(ret != cvSuccess) {
            cout<<"Error in read TTCRX_I2C_REG_WRITE"<<hex<<ret<<" "<<ds<<dec<<endl;
            analyzeError(ret);   }
   usleep(1000);//
 
 
                   if((d&0xff)==1)printf("ERROR: BUS BUSY !! \n");
                   if((d&0xff)==2)printf("ERROR: I2C_ADDR NOT ACKNOWLEDGED !! \n");
                   if((d&0xff)==4)printf("ERROR: I2C_WBYTE NOT ACKNOWLEDGED !! \n");
                   if((d&0xff)==8)printf("ERROR: I2C_LBYTE NOT ACKNOWLEDGED !! \n");
    
 
                           return(0);
                 }


void decode_spy_data_fifo(int word)
{  unsigned int plsmsk = 0xff;
   unsigned int pxlmsk = 0xff00;
   unsigned int dclmsk = 0x1f0000;
   unsigned int rocmsk = 0x3e00000;
   unsigned int chnlmsk = 0xfc000000;
   if(word&0xfffffff){ 
   cout<<" ADC value in word -- "<<dec<<(word&plsmsk);
   cout<<" Pixel- "<<dec<<((word&pxlmsk)>>8);
   cout<<" DCOL- "<<dec<<((word&dclmsk)>>16);
   cout<<" ROC- "<<dec<<((word&rocmsk)>>21);
   cout<<" Chnl- "<<dec<<((word&chnlmsk)>>26)<<endl;
                    
                        }
//cout<<"N ch0fifo1 "<<hex<<data<<endl;

}
void decode_error_fifo(int word)
{   int timochnlmsk = 0x1f;
    int chnlevnmsk = 0xff;
    int tbmtrlrmsk = 0xff;
    int ovflmsk = 0x100;   
// ng int mtmsk = 0x3c00;
    int ttcevmsk = 0x3fc000;
    int errmsk = 0x3c00000;
    int chnlmsk = 0xfc000000;
    int timeout = 0x1800000;
    int evnrerr = 0x2000000;
    int trailerr = 0x1000000;
   if(word&0xffffffff){
   if((word&errmsk)==timeout)
{cout<<"Channel Time out- TTC Event- "<<dec<<((word&ttcevmsk)>>14)
                       <<" Channel - "<<dec<<(word&timochnlmsk)<<endl;}
   if((word&errmsk)==evnrerr)
{cout<<"Event Number Err- TTC Event- "<<dec<<((word&ttcevmsk)>>14)
                       <<" Channel - "<<dec<<((word&chnlmsk)>>26)
		  <<" Channel Event- "<<dec<<(word&chnlevnmsk)<<endl;}
   if(((word&errmsk)==trailerr)&!(word&ovflmsk))
{cout<<"Channel Trailer Err- TTC Event- "<<dec<<((word&ttcevmsk)>>14)
                          <<" Channel - "<<dec<<((word&chnlmsk)>>26)
		       <<" TMB trailer- "<<dec<<(word&tbmtrlrmsk)<<endl;}
   if(((word&errmsk)==trailerr)&(word&ovflmsk))
{cout<<"Channel Overflow Err- TTC Event- "<<dec<<((word&ttcevmsk)>>14)
                           <<" Channel - "<<dec<<((word&chnlmsk)>>26)
			<<" TMB trailer- "<<dec<<(word&tbmtrlrmsk)<<endl;}
                        }
}

void decode_tempdac_fifo(int word)
{  unsigned int dacmsk = 0xff;
   unsigned int rocmsk = 0x3e00000;
   unsigned int chnlmsk = 0xfc000000;
   if(word&0xfffffff){ 
   cout<<" Last DAC ADC value in word -- "<<dec<<(word&dacmsk)
   <<" ROC- "<<dec<<((word&rocmsk)>>21)
   <<" Chnl- "<<dec<<((word&chnlmsk)>>26)<<endl;
                    
                        }
//cout<<"N ch0fifo1 "<<hex<<data<<endl;

}
void fill_dac_reg()
{
  //cout<<"Filling the dac register"<<endl;

  CVDataWidth dw = cvD32 ; // data width (see CAENVMEtypes.h )
  CVAddressModifier am = cvA32_U_DATA;

  const unsigned int V_OFFSET = 100; // 100;
  const unsigned int UB = 50;
  //const unsigned int UB = 0;
  const unsigned int B = 300;
  unsigned int dac1[256];
  unsigned int dac2[256];
  unsigned int dac3[256];
  //cout<<"Filling the dac registers"<<endl;
const bool big=false;
  // DAC TEST DATA //////////////////////////////////// 
  for(int i=0;i<256;i++) dac1[i]=V_OFFSET+B; //+B

  int START = 0;  // define the start of the data packet (starts in fifo=10)

  // Limits are     :    270, 400, 500, 600, 700
  // The levels are : 200, 300, 450, 550, 650, 750  

  dac1[START+0] = V_OFFSET+UB;   //UB  TBM-header
  dac1[START+1] = V_OFFSET+UB;   //UB
  dac1[START+2] = V_OFFSET+UB;   //UB
  dac1[START+3] = V_OFFSET+B;    //B
  dac1[START+4] = V_OFFSET+200;  // event number
  dac1[START+5] = V_OFFSET+200;
  dac1[START+6] = V_OFFSET+200;
  dac1[START+7] = V_OFFSET+200;

  START += 8;                    // ROC#0

  dac1[START+0] = V_OFFSET+UB;   //UB  ROC#0
  dac1[START+1] = V_OFFSET+B;    //B
  dac1[START+2] = V_OFFSET+0;  //  LastDac

  dac1[START+3] = V_OFFSET+650;  // dcol MSB PIXEL 1
  dac1[START+4] = V_OFFSET+300;  // dcol LSB     
  dac1[START+5] = V_OFFSET+300;  // pix 
  dac1[START+6] = V_OFFSET+200;  // pix 
  dac1[START+7] = V_OFFSET+200;  // pix   
  dac1[START+8] = V_OFFSET+0;  // PH

  START += 9;                    // ROC#1
if(big){
  dac1[START+0] = V_OFFSET+UB;   //UB  ROC#1
  dac1[START+1] = V_OFFSET+B;    //B
  dac1[START+2] = V_OFFSET+300;  //  LastDac

  dac1[START+3] = V_OFFSET+650;  // dcol MSB PIXEL 1
  dac1[START+4] = V_OFFSET+300;  // dcol LSB     
  dac1[START+5] = V_OFFSET+450;  // pix 
  dac1[START+6] = V_OFFSET+200;  // pix 
  dac1[START+7] = V_OFFSET+200;  // pix   
  dac1[START+8] = V_OFFSET+300;  // PH

  START += 9;                    // ROC#2

  dac1[START+0] = V_OFFSET+UB;   //UB  ROC#2
  dac1[START+1] = V_OFFSET+B;    //B
  dac1[START+2] = V_OFFSET+450;  //  LastDac

  dac1[START+3]= V_OFFSET+650;  // dcol MSB PIXEL 1
  dac1[START+4]= V_OFFSET+300;  // dcol LSB     
  dac1[START+5]= V_OFFSET+550;  // pix 
  dac1[START+6]= V_OFFSET+200;  // pix 
  dac1[START+7]= V_OFFSET+200;  // pix   
  dac1[START+8]= V_OFFSET+450;  // PH

  START += 9;                    // ROC#3

  dac1[START+0] = V_OFFSET+UB;   //UB  ROC#3
  dac1[START+1] = V_OFFSET+B;    //B
  dac1[START+2]= V_OFFSET+550;  //  LastDac

  dac1[START+3]= V_OFFSET+650;  // dcol MSB PIXEL 1
  dac1[START+4]= V_OFFSET+300;  // dcol LSB     
  dac1[START+5]= V_OFFSET+650;  // pix 
  dac1[START+6]= V_OFFSET+200;  // pix 
  dac1[START+7]= V_OFFSET+200;  // pix   
  dac1[START+8]= V_OFFSET+550;  // PH

  START += 9;                    // ROC#4

  dac1[START+0] = V_OFFSET+UB;   //UB  ROC#4
  dac1[START+1] = V_OFFSET+B;    //B
  dac1[START+2]= V_OFFSET+650;  //  LastDac

  dac1[START+3]= V_OFFSET+650;  // dcol MSB PIXEL 1
  dac1[START+4]= V_OFFSET+300;  // dcol LSB     
  dac1[START+5]= V_OFFSET+750;  // pix 
  dac1[START+6]= V_OFFSET+200;  // pix 
  dac1[START+7]= V_OFFSET+200;  // pix   
  dac1[START+8]= V_OFFSET+650;  // PH

  START += 9;                    // ROC#5

  dac1[START+0] = V_OFFSET+UB;   //UB  ROC#5
  dac1[START+1] = V_OFFSET+B;    //B
  dac1[START+2]= V_OFFSET+750;  //  LastDac

  dac1[START+3]= V_OFFSET+650;  // dcol MSB PIXEL 1
  dac1[START+4]= V_OFFSET+300;  // dcol LSB     
  dac1[START+5]= V_OFFSET+200;  // pix 
  dac1[START+6]= V_OFFSET+300;  // pix 
  dac1[START+7]= V_OFFSET+200;  // pix   
  dac1[START+8]= V_OFFSET+750;  // PH

  START += 9;                    // ROC#6

  dac1[START+0] = V_OFFSET+UB;   //UB  ROC#6
  dac1[START+1] = V_OFFSET+B;    //B
  dac1[START+2]= V_OFFSET+200;  //  LastDac

  dac1[START+3]= V_OFFSET+650;  // dcol MSB PIXEL 1
  dac1[START+4]= V_OFFSET+300;  // dcol LSB     
  dac1[START+5]= V_OFFSET+300;  // pix 
  dac1[START+6]= V_OFFSET+300;  // pix 
  dac1[START+7]= V_OFFSET+200;  // pix   
  dac1[START+8]= V_OFFSET+200;  // PH

  START += 9;                    // ROC#7

  dac1[START+0] = V_OFFSET+UB;   //UB  ROC#7
  dac1[START+1] = V_OFFSET+B;    //B
  dac1[START+2] = V_OFFSET+300;  //  LastDac
  
  dac1[START+3]= V_OFFSET+650;  // dcol MSB PIXEL 1
  dac1[START+4]= V_OFFSET+300;  // dcol LSB     
  dac1[START+5]= V_OFFSET+450;  // pix 
  dac1[START+6]= V_OFFSET+300;  // pix 
  dac1[START+7]= V_OFFSET+200;  // pix   
  dac1[START+8]= V_OFFSET+300;  // PH

  START += 9;  // next pixel

  int adc = 0;
  for(int n=0;n<20;n++) {
    dac1[START+0]= V_OFFSET+650;  // dcol MSB PIXEL 1
    dac1[START+1]= V_OFFSET+300;  // dcol LSB     
    dac1[START+2]= V_OFFSET+450;  // pix 
    dac1[START+3]= V_OFFSET+300;  // pix 
    dac1[START+4]= V_OFFSET+200;  // pix   
    dac1[START+5]= V_OFFSET+adc ;  // PH
    adc += 100;
    if(adc>800) adc=0;
    
    START += 6;                    // update
  }
}//end big
int adc=0;
  //START += 9;                    // TBM Trailer

  //cout<<" START = "<<START<<endl;

  dac1[START+0]= V_OFFSET+UB;   // UB  TBM trailer
  dac1[START+1]= V_OFFSET+UB;   // UB
  dac1[START+2]= V_OFFSET+B;    // B
  dac1[START+3]= V_OFFSET+B;    // B
  dac1[START+4]= V_OFFSET+200;  // Status
  dac1[START+5]= V_OFFSET+200;
  dac1[START+6]= V_OFFSET+200;
  dac1[START+7]= V_OFFSET+200;
  
  START+=8;
  if(START>255) cout<<" Error: last DAC address too large "<<START<<endl;


  // DAC TEST DATA //////////////////////////////////// 
  for(int i=0;i<256;i++) dac3[i]=V_OFFSET+B; //+B

  START=0;  // define the start of the data packet (starts in fifo=10)

  // Limits are     :    270, 400, 500, 600, 700
  // The levels are : 200, 300, 450, 550, 650, 750  

  dac3[START+0] = V_OFFSET+UB;   //UB  TBM-header
  dac3[START+1] = V_OFFSET+UB;   //UB
  dac3[START+2] = V_OFFSET+UB;   //UB
  dac3[START+3] = V_OFFSET+B;    //B
  dac3[START+4] = dac1[START+4];  // event number
  dac3[START+5] = dac1[START+5];  // event number
  dac3[START+6] = dac1[START+6];  // event number
  dac3[START+7] = dac1[START+7];  // event number

  START += 8;                    // ROC#0

  dac3[START+0] = V_OFFSET+UB;   //UB  ROC#0
  dac3[START+1] = V_OFFSET+B;    //B
  dac3[START+2]= V_OFFSET+200;  //  LastDac

  dac3[START+3]= V_OFFSET+650;  // dcol MSB PIXEL 1
  dac3[START+4]= V_OFFSET+300;  // dcol LSB     
  dac3[START+5]= V_OFFSET+300;  // pix 
  dac3[START+6]= V_OFFSET+200;  // pix 
  dac3[START+7]= V_OFFSET+200;  // pix   
  dac3[START+8]= V_OFFSET+200;  // PH

  START += 9;                    // ROC#1

  dac3[START+0] = V_OFFSET+UB;   //UB  ROC#1
  dac3[START+1] = V_OFFSET+B;    //B
  dac3[START+2]= V_OFFSET+300;  //  LastDac

  dac3[START+3]= V_OFFSET+650;  // dcol MSB PIXEL 1
  dac3[START+4]= V_OFFSET+300;  // dcol LSB     
  dac3[START+5]= V_OFFSET+450;  // pix 
  dac3[START+6]= V_OFFSET+200;  // pix 
  dac3[START+7]= V_OFFSET+200;  // pix   
  dac3[START+8]= V_OFFSET+300;  // PH

  START += 9;                    // ROC#2

  dac3[START+0] = V_OFFSET+UB;   //UB  ROC#2
  dac3[START+1] = V_OFFSET+B;    //B
  dac3[START+2]= V_OFFSET+450;  //  LastDac

  dac3[START+3]= V_OFFSET+650;  // dcol MSB PIXEL 1
  dac3[START+4]= V_OFFSET+300;  // dcol LSB     
  dac3[START+5]= V_OFFSET+550;  // pix 
  dac3[START+6]= V_OFFSET+200;  // pix 
  dac3[START+7]= V_OFFSET+200;  // pix   
  dac3[START+8]= V_OFFSET+450;  // PH

  START += 9;                    // ROC#3

  dac3[START+0] = V_OFFSET+UB;   //UB  ROC#3
  dac3[START+1] = V_OFFSET+B;    //B
  dac3[START+2]= V_OFFSET+550;  //  LastDac

  dac3[START+3]= V_OFFSET+650;  // dcol MSB PIXEL 1
  dac3[START+4]= V_OFFSET+300;  // dcol LSB     
  dac3[START+5]= V_OFFSET+650;  // pix 
  dac3[START+6]= V_OFFSET+200;  // pix 
  dac3[START+7]= V_OFFSET+200;  // pix   
  dac3[START+8]= V_OFFSET+550;  // PH

  START += 9;                    // ROC#4

  dac3[START+0] = V_OFFSET+UB;   //UB  ROC#4
  dac3[START+1] = V_OFFSET+B;    //B
  dac3[START+2]= V_OFFSET+650;  //  LastDac

  dac3[START+3]= V_OFFSET+650;  // dcol MSB PIXEL 1
  dac3[START+4]= V_OFFSET+300;  // dcol LSB     
  dac3[START+5]= V_OFFSET+750;  // pix 
  dac3[START+6]= V_OFFSET+200;  // pix 
  dac3[START+7]= V_OFFSET+200;  // pix   
  dac3[START+8]= V_OFFSET+650;  // PH

  START += 9;                    // ROC#5

  dac3[START+0] = V_OFFSET+UB;   //UB  ROC#5
  dac3[START+1] = V_OFFSET+B;    //B
  dac3[START+2]= V_OFFSET+750;  //  LastDac

  dac3[START+3]= V_OFFSET+650;  // dcol MSB PIXEL 1
  dac3[START+4]= V_OFFSET+300;  // dcol LSB     
  dac3[START+5]= V_OFFSET+200;  // pix 
  dac3[START+6]= V_OFFSET+300;  // pix 
  dac3[START+7]= V_OFFSET+200;  // pix   
  dac3[START+8]= V_OFFSET+750;  // PH

  START += 9;                    // ROC#6

  dac3[START+0] = V_OFFSET+UB;   //UB  ROC#6
  dac3[START+1] = V_OFFSET+B;    //B
  dac3[START+2]= V_OFFSET+200;  //  LastDac

  dac3[START+3]= V_OFFSET+650;  // dcol MSB PIXEL 1
  dac3[START+4]= V_OFFSET+300;  // dcol LSB     
  dac3[START+5]= V_OFFSET+300;  // pix 
  dac3[START+6]= V_OFFSET+300;  // pix 
  dac3[START+7]= V_OFFSET+200;  // pix   
  dac3[START+8]= V_OFFSET+200;  // PH

  START += 9;                    // ROC#7

  dac3[START+0] = V_OFFSET+UB;   //UB  ROC#7
  dac3[START+1] = V_OFFSET+B;    //B
  dac3[START+2]= V_OFFSET+300;  //  LastDac

  dac3[START+3]= V_OFFSET+650;  // dcol MSB PIXEL 1
  dac3[START+4]= V_OFFSET+300;  // dcol LSB     
  dac3[START+5]= V_OFFSET+450;  // pix 
  dac3[START+6]= V_OFFSET+300;  // pix 
  dac3[START+7]= V_OFFSET+200;  // pix   
  dac3[START+8]= V_OFFSET+300;  // PH

  START += 9;                    // ROC#8

  dac3[START+0] = V_OFFSET+UB;   //UB  ROC#8
  dac3[START+1] = V_OFFSET+B;    //B
  dac3[START+2]= V_OFFSET+450;  //  LastDac

  dac3[START+3]= V_OFFSET+650;  // dcol MSB PIXEL 1
  dac3[START+4]= V_OFFSET+300;  // dcol LSB     
  dac3[START+5]= V_OFFSET+550;  // pix 
  dac3[START+6]= V_OFFSET+300;  // pix 
  dac3[START+7]= V_OFFSET+200;  // pix   
  dac3[START+8]= V_OFFSET+450;  // PH

  START += 9;                    // ROC#9

  dac3[START+0] = V_OFFSET+UB;   //UB  ROC#9
  dac3[START+1] = V_OFFSET+B;    //B
  dac3[START+2]= V_OFFSET+550;  //  LastDac

  dac3[START+3]= V_OFFSET+650;  // dcol MSB PIXEL 1
  dac3[START+4]= V_OFFSET+300;  // dcol LSB     
  dac3[START+5]= V_OFFSET+650;  // pix 
  dac3[START+6]= V_OFFSET+300;  // pix 
  dac3[START+7]= V_OFFSET+200;  // pix   
  dac3[START+8]= V_OFFSET+550;  // PH

  START += 9;                    // ROC#10

  dac3[START+0] = V_OFFSET+UB;   //UB  ROC#10
  dac3[START+1] = V_OFFSET+B;    //B
  dac3[START+2]= V_OFFSET+650;  //  LastDac

  dac3[START+3]= V_OFFSET+650;  // dcol MSB PIXEL 1
  dac3[START+4]= V_OFFSET+300;  // dcol LSB     
  dac3[START+5]= V_OFFSET+750;  // pix 
  dac3[START+6]= V_OFFSET+300;  // pix 
  dac3[START+7]= V_OFFSET+200;  // pix   
  dac3[START+8]= V_OFFSET+650;  // PH

  START += 9;                    // ROC#11

  dac3[START+0] = V_OFFSET+UB;   //UB  ROC#11
  dac3[START+1] = V_OFFSET+B;    //B
  dac3[START+2]= V_OFFSET+750;  //  LastDac

  dac3[START+3]= V_OFFSET+650;  // dcol MSB PIXEL 1
  dac3[START+4]= V_OFFSET+300;  // dcol LSB     
  dac3[START+5]= V_OFFSET+200;  // pix 
  dac3[START+6]= V_OFFSET+450;  // pix 
  dac3[START+7]= V_OFFSET+200;  // pix   
  dac3[START+8]= V_OFFSET+750;  // PH

  START += 9;                    // ROC#12

  dac3[START+0] = V_OFFSET+UB;   //UB  ROC#12
  dac3[START+1] = V_OFFSET+B;    //B
  dac3[START+2]= V_OFFSET+200;  //  LastDac

  dac3[START+3]= V_OFFSET+650;  // dcol MSB PIXEL 1
  dac3[START+4]= V_OFFSET+300;  // dcol LSB     
  dac3[START+5]= V_OFFSET+300;  // pix 
  dac3[START+6]= V_OFFSET+450;  // pix 
  dac3[START+7]= V_OFFSET+200;  // pix   
  dac3[START+8]= V_OFFSET+200;  // PH

  START += 9;                    // ROC#13

  dac3[START+0] = V_OFFSET+UB;   //UB  ROC#13
  dac3[START+1] = V_OFFSET+B;    //B
  dac3[START+2]= V_OFFSET+300;  //  LastDac

  dac3[START+3]= V_OFFSET+650;  // dcol MSB PIXEL 1
  dac3[START+4]= V_OFFSET+300;  // dcol LSB     
  dac3[START+5]= V_OFFSET+450;  // pix 
  dac3[START+6]= V_OFFSET+450;  // pix 
  dac3[START+7]= V_OFFSET+200;  // pix   
  dac3[START+8]= V_OFFSET+300;  // PH

  START += 9;                    // ROC#14

  dac3[START+0] = V_OFFSET+UB;   //UB  ROC#14
  dac3[START+1] = V_OFFSET+B;    //B
  dac3[START+2]= V_OFFSET+450;  //  LastDac

  dac3[START+3]= V_OFFSET+650;  // dcol MSB PIXEL 1
  dac3[START+4]= V_OFFSET+300;  // dcol LSB     
  dac3[START+5]= V_OFFSET+550;  // pix 
  dac3[START+6]= V_OFFSET+450;  // pix 
  dac3[START+7]= V_OFFSET+200;  // pix   
  dac3[START+8]= V_OFFSET+450;  // PH

  START += 9;                    // ROC#15

  dac3[START+0] = V_OFFSET+UB;   //UB  ROC#15
  dac3[START+1] = V_OFFSET+B;    //B
  dac3[START+2]= V_OFFSET+550;  //  LastDac

  dac3[START+3]= V_OFFSET+650;  // dcol MSB PIXEL 1
  dac3[START+4]= V_OFFSET+300;  // dcol LSB     
  dac3[START+5]= V_OFFSET+650;  // pix 
  dac3[START+6]= V_OFFSET+450;  // pix 
  dac3[START+7]= V_OFFSET+200;  // pix   
  dac3[START+8]= V_OFFSET+550;  // PH

  START += 9;  // next pixel

  adc = 0;
  for(int n=0;n<8;n++) {
    dac3[START+0]= V_OFFSET+650;  // dcol MSB PIXEL 1
    dac3[START+1]= V_OFFSET+300;  // dcol LSB     
    dac3[START+2]= V_OFFSET+450;  // pix 
    dac3[START+3]= V_OFFSET+300;  // pix 
    dac3[START+4]= V_OFFSET+200;  // pix   
    dac3[START+5]= V_OFFSET+adc ;  // PH
    adc += 100;
    if(adc>800) adc=0;
    
    START += 6;                    // update
  }

  //START += 9;                    // TBM Trailer

  dac3[START+0]= V_OFFSET+UB;   // UB  TBM trailer
  dac3[START+1]= V_OFFSET+UB;   // UB
  dac3[START+2]= V_OFFSET+B;    // B
  dac3[START+3]= V_OFFSET+B;    // B
  dac3[START+4]= V_OFFSET+200;  // Status
  dac3[START+5]= V_OFFSET+200;
  dac3[START+6]= V_OFFSET+200;
  dac3[START+7]= V_OFFSET+200;
  
  START+=8;
  if(START>255) cout<<" Error: last DAC address too large "<<START<<endl;

//make sure they're the same for now

  for(int i=0;i<256;i++) dac2[i]=dac1[i];//V_OFFSET+B; //+B
  for(int i=0;i<256;i++) dac3[i]=dac1[i];//V_OFFSET+B; //+B



        for(int i=0;i<256;i++){
//VXIout(0x3,LAD_N+0x180000+i,4,TestData[i/4]+((TestData[i/4])<<10) + ((TestData[i/4])<<20) );  //WRITE DAC

    unsigned int tmp1 = (dac1[i] & 0x3ff);
    unsigned int tmp2 = (dac2[i] & 0x3ff);
    unsigned int tmp3 = (dac3[i] & 0x3ff);


  unsigned int data =  tmp1 + (tmp2<<10) + (tmp3<<20);
//int data = 0x350+(0x350<<10)+(0x350<<20);
  CVErrorCodes ret = CAENVME_WriteCycle(BHandle,LAD_N+0x180000+i*4,&data,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write to DAC LAD_N+0x180000+i"<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret);
                       } 
                                }

  //cout<<"Set the start pulse delay to 0? "<<endl;
  unsigned int data = 0x0;
  unsigned int vmeAddress = LAD_C+0x1a8000;
  CVErrorCodes ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); 
                       }  
                                                                                
           usleep(100); // Time for DAC
                                                                                

 
}



int drain_fifo(unsigned int VmeAddress,unsigned int *buffer, int max)
{
//drains a fifo slowly...
//returns 0 (empty) or # of words and a pointer to 1st word
  CVDataWidth dw = cvD32 ; // data width (see CAENVMEtypes.h )
  CVAddressModifier am = cvA32_U_DATA;

 unsigned int data=0;
 int wdcnt=0;
  while((wdcnt<max)){
  CVErrorCodes ret = CAENVME_ReadCycle(BHandle, VmeAddress,&data,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in Draining the Fifo "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret);
  }
  {buffer[wdcnt]=data;wdcnt++;}
}  
 return(wdcnt);
}

int config_file()
{int i,DEBUG=0;
int ijx,ijy;
FILE *infile;
infile = fopen("params_opto2.dat","r");
if (infile == NULL) {cout<<"No parameter file!"<<endl; exit(1);}

//Fed Base Address
//int FEDBASE;
fscanf(infile,"FED Base address                         :%x\n",&FEDBASE);
fscanf(infile,"FEDID Number                             :%x\n",&fedNumber);
  if(DEBUG>0)printf("FED Base address     :%08x\n",FEDBASE);
  if(DEBUG>0)printf("FED ID number        :%08d\n",fedNumber);

//int NRocs;
  for(i=0;i<36;i++){
    fscanf(infile,"Number of ROCs Chnl %d:%d \n",&ijx,&NRocs[i]);
    if(DEBUG==1)printf("Number of ROCs per Chnl %d:%d \n",ijx,NRocs[i]);
  }
//Settable optical input parameters
//int opt_cap[3],opt_inadj[3],opt_ouadj[3];
fscanf(infile,"Optical reciever 1  Capacitor Adjust(0-3):%d\n",&opt_cap[0]);
fscanf(infile,"Optical reciever 2  Capacitor Adjust(0-3):%d\n",&opt_cap[1]);
fscanf(infile,"Optical reciever 3  Capacitor Adjust(0-3):%d\n",&opt_cap[2]);
fscanf(infile,"Optical reciever 1  Input Offset (0-15)	:%d\n",&opt_inadj[0]);
fscanf(infile,"Optical reciever 2  Input Offset (0-15)	:%d\n",&opt_inadj[1]);
fscanf(infile,"Optical reciever 3  Input Offset (0-15)	:%d\n",&opt_inadj[2]);
fscanf(infile,"Optical reciever 1 Output Offset (0-3)   :%d\n",&opt_ouadj[0]);
fscanf(infile,"Optical reciever 2 Output Offset (0-3)   :%d\n",&opt_ouadj[1]);
fscanf(infile,"Optical reciever 3 Output Offset (0-3)   :%d\n",&opt_ouadj[2]);

if(DEBUG==1)printf("Optical reciever 1  Capacitor Adjust(0-3):%d\n",opt_cap[0]);
if(DEBUG==1)printf("Optical reciever 2  Capacitor Adjust(0-3):%d\n",opt_cap[1]);
if(DEBUG==1)printf("Optical reciever 3  Capacitor Adjust(0-3):%d\n",opt_cap[2]);
if(DEBUG==1)printf("Optical reciever 1  Input Offset (0-15)  :%d\n",opt_inadj[0]);
if(DEBUG==1)printf("Optical reciever 2  Input Offset (0-15)  :%d\n",opt_inadj[1]);
if(DEBUG==1)printf("Optical reciever 3  Input Offset (0-15)  :%d\n",opt_inadj[2]);
if(DEBUG==1)printf("Optical reciever 1 Output Offset (0-3)   :%d\n",opt_ouadj[0]);
if(DEBUG==1)printf("Optical reciever 2 Output Offset (0-3)   :%d\n",opt_ouadj[1]);
if(DEBUG==1)printf("Optical reciever 3 Output Offset (0-3)   :%d\n",opt_ouadj[2]);

//input offset dac
//int offs_dac[36];
for(int i=0;i<36;i++)fscanf(infile,"Offset DAC channel %d:%d\n",&ijx,&offs_dac[i]);
if(DEBUG==1){for(int i=0;i<36;i++)printf("Offset DAC channel %d:%d\n",i+1,offs_dac[i]);}

//clock phases
//int clkphs0_9,clkphs10_18,clkphs19_27,clkphs28_36;
fscanf(infile,"Clock Phase Bits ch   1-9:%x\n",& clkphs0_9 );
fscanf(infile,"Clock Phase Bits ch 10-18:%x\n",&clkphs10_18);
fscanf(infile,"Clock Phase Bits ch 19-27:%x\n",&clkphs19_27);
fscanf(infile,"Clock Phase Bits ch 28-36:%x\n",&clkphs28_36);
if(DEBUG==1)printf("Clock Phase Bits ch    1-9:%x\n",clkphs0_9 );
if(DEBUG==1)printf("Clock Phase Bits ch  10-18:%x\n",clkphs10_18 );
if(DEBUG==1)printf("Clock Phase Bits ch  19-27:%x\n",clkphs19_27 );
if(DEBUG==1)printf("Clock Phase Bits ch  28-36:%x\n",clkphs28_36 );

//Blacks 
//int BlackHi[36],BlackLo[36],Ublack[36];
for(i=0;i<36;i++){
fscanf(infile,"Black HiThold ch %d:%d \n",&ijx,&BlackHi[i]);
fscanf(infile,"Black LoThold ch %d:%d \n",&ijx,&BlackLo[i]);
fscanf(infile,"ULblack Thold ch %d:%d \n",&ijx, &Ublack[i]);
if(DEBUG==1)printf("Black HiThold ch %d:%d\n",ijx,BlackHi[i]);
if(DEBUG==1)printf("Black LoThold ch %d:%d\n",ijx,BlackLo[i]);
if(DEBUG==1)printf("ULblack Thold ch %d:%d\n",ijx, Ublack[i]);}

//Channel delays
//int DelayCh[36];
for(i=0;i<36;i++){fscanf(infile,"Delay channel %d(0-15):%d\n",&ijx,&DelayCh[i]);}
if(DEBUG==1){for(i=0;i<36;i++){printf("Delay channel %d(0-15):%d\n",i+1,DelayCh[i]);}}

//Signal levels
//int TBM_L0[36],TBM_L1[36],TBM_L2[36],TBM_L3[36],TBM_L4[36];
//int TRL_L0[36],TRL_L1[36],TRL_L2[36],TRL_L3[36],TRL_L4[36];
//int ROC_L0[36][26],ROC_L1[36][26],ROC_L2[36][26],ROC_L3[36][26],ROC_L4[36][26];
for(i=0;i<36;i++)
{
fscanf(infile,"TBM level 0 Channel  %d:%d\n",&ijx,&TBM_L0[i]);
fscanf(infile,"TBM level 1 Channel  %d:%d\n",&ijx,&TBM_L1[i]);
fscanf(infile,"TBM level 2 Channel  %d:%d\n",&ijx,&TBM_L2[i]);
fscanf(infile,"TBM level 3 Channel  %d:%d\n",&ijx,&TBM_L3[i]);
fscanf(infile,"TBM level 4 Channel  %d:%d\n",&ijx,&TBM_L4[i]);
if(DEBUG==1)printf("TBM level 0 Channel  %d:%d\n",ijx,TBM_L0[i]);
if(DEBUG==1)printf("TBM level 1 Channel  %d:%d\n",ijx,TBM_L1[i]);
if(DEBUG==1)printf("TBM level 2 Channel  %d:%d\n",ijx,TBM_L2[i]);
if(DEBUG==1)printf("TBM level 3 Channel  %d:%d\n",ijx,TBM_L3[i]);
if(DEBUG==1)printf("TBM level 4 Channel  %d:%d\n",ijx,TBM_L4[i]);




for(int j=0;j<(int)NRocs[i];j++){
fscanf(infile,"ROC%d level 0 Channel  %d :%d\n",&ijy,&ijx,&ROC_L0[i][j]);
fscanf(infile,"ROC%d level 1 Channel  %d :%d\n",&ijy,&ijx,&ROC_L1[i][j]);
fscanf(infile,"ROC%d level 2 Channel  %d :%d\n",&ijy,&ijx,&ROC_L2[i][j]);
fscanf(infile,"ROC%d level 3 Channel  %d :%d\n",&ijy,&ijx,&ROC_L3[i][j]);
fscanf(infile,"ROC%d level 4 Channel  %d :%d\n",&ijy,&ijx,&ROC_L4[i][j]);
if(DEBUG==1)printf("ROC%d level 0 Channel  %d :%d\n",ijy,ijx,ROC_L0[i][j]);
if(DEBUG==1)printf("ROC%d level 1 Channel  %d :%d\n",ijy,ijx,ROC_L1[i][j]);
if(DEBUG==1)printf("ROC%d level 2 Channel  %d :%d\n",ijy,ijx,ROC_L2[i][j]);
if(DEBUG==1)printf("ROC%d level 3 Channel  %d :%d\n",ijy,ijx,ROC_L3[i][j]);
if(DEBUG==1)printf("ROC%d level 4 Channel  %d :%d\n",ijy,ijx,ROC_L4[i][j]);
                     }

fscanf(infile,"TRLR level 0 Channel %d:%d\n",&ijx,&TRL_L0[i]);
fscanf(infile,"TRLR level 1 Channel %d:%d\n",&ijx,&TRL_L1[i]);
fscanf(infile,"TRLR level 2 Channel %d:%d\n",&ijx,&TRL_L2[i]);
fscanf(infile,"TRLR level 3 Channel %d:%d\n",&ijx,&TRL_L3[i]);
fscanf(infile,"TRLR level 4 Channel %d:%d\n",&ijx,&TRL_L4[i]);
if(DEBUG==1)printf("TRLR level 0 Channel %d:%d\n",ijx,TRL_L0[i]);
if(DEBUG==1)printf("TRLR level 1 Channel %d:%d\n",ijx,TRL_L1[i]);
if(DEBUG==1)printf("TRLR level 2 Channel %d:%d\n",ijx,TRL_L2[i]);
if(DEBUG==1)printf("TRLR level 3 Channel %d:%d\n",ijx,TRL_L3[i]);
if(DEBUG==1)printf("TRLR level 4 Channel %d:%d\n",ijx,TRL_L4[i]);
}


//These bits turn off(1) and on(0) channels
//int Ncntrl,NCcntrl,SCcntrl,Scntrl;

fscanf(infile,"Channel Enbable bits chnls 1-9  (on = 0):%x\n",&Ncntrl);
fscanf(infile,"Channel Enbable bits chnls 10-18(on = 0):%x\n",&NCcntrl);
fscanf(infile,"Channel Enbable bits chnls 19-27(on = 0):%x\n",&SCcntrl);
fscanf(infile,"Channel Enbable bits chnls 28-36(on = 0):%x\n",&Scntrl);
if(DEBUG==1)printf("Channel Enbable bits chnls 1-9  (on = 0):%x\n",Ncntrl);
if(DEBUG==1)printf("Channel Enbable bits chnls 10-18(on = 0):%x\n",NCcntrl);
if(DEBUG==1)printf("Channel Enbable bits chnls 19-27(on = 0):%x\n",SCcntrl);
if(DEBUG==1)printf("Channel Enbable bits chnls 28-36(on = 0):%x\n",Scntrl);

//These are delays to the TTCrx
//int CoarseDel,ClkDes2,FineDes2Del;
fscanf(infile,"TTCrx Coarse Delay Register 2:%d\n",&CoarseDel);
fscanf(infile,"TTCrc      ClkDes2 Register 3:%x\n",&ClkDes2);
fscanf(infile,"TTCrc Fine Dlay ClkDes2 Reg 1:%d\n",&FineDes2Del);
if(DEBUG==1)printf("TTCrx Coarse Delay Register 2:%d\n",CoarseDel);
if(DEBUG==1)printf("TTCrc	   ClkDes2 Register 3:%x\n",ClkDes2);
if(DEBUG==1)printf("TTCrc Fine Dlay ClkDes2 Reg 1:%d\n",FineDes2Del);

//int Ccntrl;
fscanf(infile,"Center Chip Control Reg:%x\n",&Ccntrl);
if(DEBUG==1)printf("Center Chip (DAQ) Control Reg:0x%x\n",Ccntrl);
//int Slink;
fscanf(infile,"Initial Slink DAQ mode:%d\n",&Slink);
if(DEBUG==1)printf("Initial Slink DAQ mode:%d\n",Slink);
   //These bits set ADC Gain/Range 1Vpp(0) and 2Vpp(1) for channels
  fscanf(infile,"Channel ADC Gain bits chnls  1-12(1Vpp = 0):%x\n",&Nadcg);
  fscanf(infile,"Channel ADC Gain bits chnls 13-20(1Vpp = 0):%x\n",&NCadcg);
  fscanf(infile,"Channel ADC Gain bits chnls 21-28(1Vpp = 0):%x\n",&SCadcg);
  fscanf(infile,"Channel ADC Gain bits chnls 29-36(1Vpp = 0):%x\n",&Sadcg);
  if(DEBUG)
    printf("Channel ADC Gain bits chnls  1-12(1Vpp = 0):%x\n",Nadcg);
  if(DEBUG)
    printf("Channel ADC Gain bits chnls 13-20(1Vpp = 0):%x\n",NCadcg);
  if(DEBUG)
    printf("Channel ADC Gain bits chnls 21-28(1Vpp = 0):%x\n",SCadcg);
  if(DEBUG)
    printf("Channel ADC Gain bits chnls 29-36(1Vpp = 0):%x\n",Sadcg);
    
//These bits set Baseline adjustment value (common by FPGA)//can turn on by channel
  fscanf(infile,"Channel Baseline Enbable chnls 1-9  (on = (0x1ff<<16)+):%x\n",&Nbaseln);
  fscanf(infile,"Channel Baseline Enbable chnls 10-18(on = (0x1ff<<16)+):%x\n",&NCbaseln);
  fscanf(infile,"Channel Baseline Enbable chnls 19-27(on = (0x1ff<<16)+):%x\n",&SCbaseln);
  fscanf(infile,"Channel Baseline Enbable chnls 28-36(on = (0x1ff<<16)+):%x\n",&Sbaseln);
  if(DEBUG)
    printf("Channel Baseline Enbable chnls 1-9  (on = (0x1ff<<16)+):%x\n",Nbaseln);
  if(DEBUG)
    printf("Channel Baseline Enbable chnls 10-18(on = (0x1ff<<16)+):%x\n",NCbaseln);
  if(DEBUG)
    printf("Channel Baseline Enbable chnls 19-27(on = (0x1ff<<16)+):%x\n",SCbaseln);
  if(DEBUG)
    printf("Channel Baseline Enbable chnls 28-36(on = (0x1ff<<16)+):%x\n",Sbaseln);
  
	CHIP[0]=FEDBASE;          //ch 1-9   
	CHIP[1]=FEDBASE+0x200000; //ch 10-18 	
	CHIP[2]=FEDBASE+0x400000; //ch 19-27	
	CHIP[3]=FEDBASE+0x600000; //ch 28-36

	CH_SubAddr[0]=0x38000;   //FifoI ch1
	CH_SubAddr[1]= 0x58000;	 //FifoI ch2
	CH_SubAddr[2]= 0x78000;  //FifoI ch3
	CH_SubAddr[3]=0x98000;   //FifoI ch4
	CH_SubAddr[4]= 0xb8000;	 //FifoI ch5 
	CH_SubAddr[5]= 0xd8000;  //FifoI ch6
	CH_SubAddr[6]=0xf8000;   //FifoI ch7 
	CH_SubAddr[7]=0x118000;	 //FifoI ch8 
	CH_SubAddr[8]=0x138000;  //FifoI ch9

fclose(infile);
return(1);
}
int config_from_file(string fileName)
{int i,DEBUG=0;
int ijx,ijy;
FILE *infile;
infile = fopen((fileName.c_str()),"r");
if (infile == NULL) {cout<<"No parameter file!"<<endl; exit(1);}

//Fed Base Address
//int FEDBASE;
fscanf(infile,"FED Base address                         :%x\n",&FEDBASE);
fscanf(infile,"FEDID Number                             :%x\n",&fedNumber);
  if(DEBUG>0)printf("FED Base address     :%08x\n",FEDBASE);
  if(DEBUG>0)printf("FED ID number        :%08d\n",fedNumber);

//int NRocs;
  for(i=0;i<36;i++){
    fscanf(infile,"Number of ROCs Chnl %d:%d \n",&ijx,&NRocs[i]);
    if(DEBUG==1)printf("Number of ROCs per Chnl %d:%d \n",ijx,NRocs[i]);
  }
//Settable optical input parameters
//int opt_cap[3],opt_inadj[3],opt_ouadj[3];
fscanf(infile,"Optical reciever 1  Capacitor Adjust(0-3):%d\n",&opt_cap[0]);
fscanf(infile,"Optical reciever 2  Capacitor Adjust(0-3):%d\n",&opt_cap[1]);
fscanf(infile,"Optical reciever 3  Capacitor Adjust(0-3):%d\n",&opt_cap[2]);
fscanf(infile,"Optical reciever 1  Input Offset (0-15)	:%d\n",&opt_inadj[0]);
fscanf(infile,"Optical reciever 2  Input Offset (0-15)	:%d\n",&opt_inadj[1]);
fscanf(infile,"Optical reciever 3  Input Offset (0-15)	:%d\n",&opt_inadj[2]);
fscanf(infile,"Optical reciever 1 Output Offset (0-3)   :%d\n",&opt_ouadj[0]);
fscanf(infile,"Optical reciever 2 Output Offset (0-3)   :%d\n",&opt_ouadj[1]);
fscanf(infile,"Optical reciever 3 Output Offset (0-3)   :%d\n",&opt_ouadj[2]);

if(DEBUG==1)printf("Optical reciever 1  Capacitor Adjust(0-3):%d\n",opt_cap[0]);
if(DEBUG==1)printf("Optical reciever 2  Capacitor Adjust(0-3):%d\n",opt_cap[1]);
if(DEBUG==1)printf("Optical reciever 3  Capacitor Adjust(0-3):%d\n",opt_cap[2]);
if(DEBUG==1)printf("Optical reciever 1  Input Offset (0-15)  :%d\n",opt_inadj[0]);
if(DEBUG==1)printf("Optical reciever 2  Input Offset (0-15)  :%d\n",opt_inadj[1]);
if(DEBUG==1)printf("Optical reciever 3  Input Offset (0-15)  :%d\n",opt_inadj[2]);
if(DEBUG==1)printf("Optical reciever 1 Output Offset (0-3)   :%d\n",opt_ouadj[0]);
if(DEBUG==1)printf("Optical reciever 2 Output Offset (0-3)   :%d\n",opt_ouadj[1]);
if(DEBUG==1)printf("Optical reciever 3 Output Offset (0-3)   :%d\n",opt_ouadj[2]);

//input offset dac
//int offs_dac[36];
for(int i=0;i<36;i++)fscanf(infile,"Offset DAC channel %d:%d\n",&ijx,&offs_dac[i]);
if(DEBUG==1){for(int i=0;i<36;i++)printf("Offset DAC channel %d:%d\n",i+1,offs_dac[i]);}

//clock phases
//int clkphs0_9,clkphs10_18,clkphs19_27,clkphs28_36;
fscanf(infile,"Clock Phase Bits ch   1-9:%x\n",& clkphs0_9 );
fscanf(infile,"Clock Phase Bits ch 10-18:%x\n",&clkphs10_18);
fscanf(infile,"Clock Phase Bits ch 19-27:%x\n",&clkphs19_27);
fscanf(infile,"Clock Phase Bits ch 28-36:%x\n",&clkphs28_36);
if(DEBUG==1)printf("Clock Phase Bits ch    1-9:%x\n",clkphs0_9 );
if(DEBUG==1)printf("Clock Phase Bits ch  10-18:%x\n",clkphs10_18 );
if(DEBUG==1)printf("Clock Phase Bits ch  19-27:%x\n",clkphs19_27 );
if(DEBUG==1)printf("Clock Phase Bits ch  28-36:%x\n",clkphs28_36 );

//Blacks 
//int BlackHi[36],BlackLo[36],Ublack[36];
for(i=0;i<36;i++){
fscanf(infile,"Black HiThold ch %d:%d \n",&ijx,&BlackHi[i]);
fscanf(infile,"Black LoThold ch %d:%d \n",&ijx,&BlackLo[i]);
fscanf(infile,"ULblack Thold ch %d:%d \n",&ijx, &Ublack[i]);
if(DEBUG==1)printf("Black HiThold ch %d:%d\n",ijx,BlackHi[i]);
if(DEBUG==1)printf("Black LoThold ch %d:%d\n",ijx,BlackLo[i]);
if(DEBUG==1)printf("ULblack Thold ch %d:%d\n",ijx, Ublack[i]);}

//Channel delays
//int DelayCh[36];
for(i=0;i<36;i++){fscanf(infile,"Delay channel %d(0-15):%d\n",&ijx,&DelayCh[i]);}
if(DEBUG==1){for(i=0;i<36;i++){printf("Delay channel %d(0-15):%d\n",i+1,DelayCh[i]);}}

//Signal levels
//int TBM_L0[36],TBM_L1[36],TBM_L2[36],TBM_L3[36],TBM_L4[36];
//int TRL_L0[36],TRL_L1[36],TRL_L2[36],TRL_L3[36],TRL_L4[36];
//int ROC_L0[36][26],ROC_L1[36][26],ROC_L2[36][26],ROC_L3[36][26],ROC_L4[36][26];
for(i=0;i<36;i++)
{
fscanf(infile,"TBM level 0 Channel  %d:%d\n",&ijx,&TBM_L0[i]);
fscanf(infile,"TBM level 1 Channel  %d:%d\n",&ijx,&TBM_L1[i]);
fscanf(infile,"TBM level 2 Channel  %d:%d\n",&ijx,&TBM_L2[i]);
fscanf(infile,"TBM level 3 Channel  %d:%d\n",&ijx,&TBM_L3[i]);
fscanf(infile,"TBM level 4 Channel  %d:%d\n",&ijx,&TBM_L4[i]);
if(DEBUG==1)printf("TBM level 0 Channel  %d:%d\n",ijx,TBM_L0[i]);
if(DEBUG==1)printf("TBM level 1 Channel  %d:%d\n",ijx,TBM_L1[i]);
if(DEBUG==1)printf("TBM level 2 Channel  %d:%d\n",ijx,TBM_L2[i]);
if(DEBUG==1)printf("TBM level 3 Channel  %d:%d\n",ijx,TBM_L3[i]);
if(DEBUG==1)printf("TBM level 4 Channel  %d:%d\n",ijx,TBM_L4[i]);




for(int j=0;j<(int)NRocs[i];j++){
fscanf(infile,"ROC%d level 0 Channel  %d :%d\n",&ijy,&ijx,&ROC_L0[i][j]);
fscanf(infile,"ROC%d level 1 Channel  %d :%d\n",&ijy,&ijx,&ROC_L1[i][j]);
fscanf(infile,"ROC%d level 2 Channel  %d :%d\n",&ijy,&ijx,&ROC_L2[i][j]);
fscanf(infile,"ROC%d level 3 Channel  %d :%d\n",&ijy,&ijx,&ROC_L3[i][j]);
fscanf(infile,"ROC%d level 4 Channel  %d :%d\n",&ijy,&ijx,&ROC_L4[i][j]);
if(DEBUG==1)printf("ROC%d level 0 Channel  %d :%d\n",ijy,ijx,ROC_L0[i][j]);
if(DEBUG==1)printf("ROC%d level 1 Channel  %d :%d\n",ijy,ijx,ROC_L1[i][j]);
if(DEBUG==1)printf("ROC%d level 2 Channel  %d :%d\n",ijy,ijx,ROC_L2[i][j]);
if(DEBUG==1)printf("ROC%d level 3 Channel  %d :%d\n",ijy,ijx,ROC_L3[i][j]);
if(DEBUG==1)printf("ROC%d level 4 Channel  %d :%d\n",ijy,ijx,ROC_L4[i][j]);
                     }

fscanf(infile,"TRLR level 0 Channel %d:%d\n",&ijx,&TRL_L0[i]);
fscanf(infile,"TRLR level 1 Channel %d:%d\n",&ijx,&TRL_L1[i]);
fscanf(infile,"TRLR level 2 Channel %d:%d\n",&ijx,&TRL_L2[i]);
fscanf(infile,"TRLR level 3 Channel %d:%d\n",&ijx,&TRL_L3[i]);
fscanf(infile,"TRLR level 4 Channel %d:%d\n",&ijx,&TRL_L4[i]);
if(DEBUG==1)printf("TRLR level 0 Channel %d:%d\n",ijx,TRL_L0[i]);
if(DEBUG==1)printf("TRLR level 1 Channel %d:%d\n",ijx,TRL_L1[i]);
if(DEBUG==1)printf("TRLR level 2 Channel %d:%d\n",ijx,TRL_L2[i]);
if(DEBUG==1)printf("TRLR level 3 Channel %d:%d\n",ijx,TRL_L3[i]);
if(DEBUG==1)printf("TRLR level 4 Channel %d:%d\n",ijx,TRL_L4[i]);
}


//These bits turn off(1) and on(0) channels
//int Ncntrl,NCcntrl,SCcntrl,Scntrl;

fscanf(infile,"Channel Enbable bits chnls 1-9  (on = 0):%x\n",&Ncntrl);
fscanf(infile,"Channel Enbable bits chnls 10-18(on = 0):%x\n",&NCcntrl);
fscanf(infile,"Channel Enbable bits chnls 19-27(on = 0):%x\n",&SCcntrl);
fscanf(infile,"Channel Enbable bits chnls 28-36(on = 0):%x\n",&Scntrl);
if(DEBUG==1)printf("Channel Enbable bits chnls 1-9  (on = 0):%x\n",Ncntrl);
if(DEBUG==1)printf("Channel Enbable bits chnls 10-18(on = 0):%x\n",NCcntrl);
if(DEBUG==1)printf("Channel Enbable bits chnls 19-27(on = 0):%x\n",SCcntrl);
if(DEBUG==1)printf("Channel Enbable bits chnls 28-36(on = 0):%x\n",Scntrl);

//These are delays to the TTCrx
//int CoarseDel,ClkDes2,FineDes2Del;
fscanf(infile,"TTCrx Coarse Delay Register 2:%d\n",&CoarseDel);
fscanf(infile,"TTCrc      ClkDes2 Register 3:%x\n",&ClkDes2);
fscanf(infile,"TTCrc Fine Dlay ClkDes2 Reg 1:%d\n",&FineDes2Del);
if(DEBUG==1)printf("TTCrx Coarse Delay Register 2:%d\n",CoarseDel);
if(DEBUG==1)printf("TTCrc	   ClkDes2 Register 3:%x\n",ClkDes2);
if(DEBUG==1)printf("TTCrc Fine Dlay ClkDes2 Reg 1:%d\n",FineDes2Del);

//int Ccntrl;
fscanf(infile,"Center Chip Control Reg:%x\n",&Ccntrl);
if(DEBUG==1)printf("Center Chip (DAQ) Control Reg:0x%x\n",Ccntrl);
//int Slink;
fscanf(infile,"Initial Slink DAQ mode:%d\n",&Slink);
if(DEBUG==1)printf("Initial Slink DAQ mode:%d\n",Slink);
   //These bits set ADC Gain/Range 1Vpp(0) and 2Vpp(1) for channels
  fscanf(infile,"Channel ADC Gain bits chnls  1-12(1Vpp = 0):%x\n",&Nadcg);
  fscanf(infile,"Channel ADC Gain bits chnls 13-20(1Vpp = 0):%x\n",&NCadcg);
  fscanf(infile,"Channel ADC Gain bits chnls 21-28(1Vpp = 0):%x\n",&SCadcg);
  fscanf(infile,"Channel ADC Gain bits chnls 29-36(1Vpp = 0):%x\n",&Sadcg);
  if(DEBUG)
    printf("Channel ADC Gain bits chnls  1-12(1Vpp = 0):%x\n",Nadcg);
  if(DEBUG)
    printf("Channel ADC Gain bits chnls 13-20(1Vpp = 0):%x\n",NCadcg);
  if(DEBUG)
    printf("Channel ADC Gain bits chnls 21-28(1Vpp = 0):%x\n",SCadcg);
  if(DEBUG)
    printf("Channel ADC Gain bits chnls 29-36(1Vpp = 0):%x\n",Sadcg);
    
//These bits set Baseline adjustment value (common by FPGA)//can turn on by channel
  fscanf(infile,"Channel Baseline Enbable chnls 1-9  (on = (0x1ff<<16)+):%x\n",&Nbaseln);
  fscanf(infile,"Channel Baseline Enbable chnls 10-18(on = (0x1ff<<16)+):%x\n",&NCbaseln);
  fscanf(infile,"Channel Baseline Enbable chnls 19-27(on = (0x1ff<<16)+):%x\n",&SCbaseln);
  fscanf(infile,"Channel Baseline Enbable chnls 28-36(on = (0x1ff<<16)+):%x\n",&Sbaseln);
  if(DEBUG)
    printf("Channel Baseline Enbable chnls 1-9  (on = (0x1ff<<16)+):%x\n",Nbaseln);
  if(DEBUG)
    printf("Channel Baseline Enbable chnls 10-18(on = (0x1ff<<16)+):%x\n",NCbaseln);
  if(DEBUG)
    printf("Channel Baseline Enbable chnls 19-27(on = (0x1ff<<16)+):%x\n",SCbaseln);
  if(DEBUG)
    printf("Channel Baseline Enbable chnls 28-36(on = (0x1ff<<16)+):%x\n",Sbaseln);
  
	CHIP[0]=FEDBASE;          //ch 1-9   
	CHIP[1]=FEDBASE+0x200000; //ch 10-18 	
	CHIP[2]=FEDBASE+0x400000; //ch 19-27	
	CHIP[3]=FEDBASE+0x600000; //ch 28-36

	CH_SubAddr[0]=0x38000;   //FifoI ch1
	CH_SubAddr[1]= 0x58000;	 //FifoI ch2
	CH_SubAddr[2]= 0x78000;  //FifoI ch3
	CH_SubAddr[3]=0x98000;   //FifoI ch4
	CH_SubAddr[4]= 0xb8000;	 //FifoI ch5 
	CH_SubAddr[5]= 0xd8000;  //FifoI ch6
	CH_SubAddr[6]=0xf8000;   //FifoI ch7 
	CH_SubAddr[7]=0x118000;	 //FifoI ch8 
	CH_SubAddr[8]=0x138000;  //FifoI ch9

fclose(infile);
return(1);
}
void set_opto_params()
{
  CVDataWidth dw = cvD32; // data width (see CAENVMEtypes.h )
  CVAddressModifier am = cvA32_U_DATA;
//form opto word from params
  cout<<"opto params"<<endl;
  unsigned int data = (opt_cap[0]<<6)+opt_inadj[0]+(opt_ouadj[0]<<4);
  cout<<hex<<" 0x"<<opt_cap[0]<<" 0x"<<opt_inadj[0]<<" 0x"<<opt_ouadj[0]<<endl;
  cout<<hex<<data<<endl;
  unsigned int vmeAddress = LAD_S+0x180000;
  CVErrorCodes ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); } 

  data = (opt_cap[1]<<6)+opt_inadj[1]+(opt_ouadj[1]<<4); 
  vmeAddress = LAD_S+0x188000;
  ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); } 

  data =(opt_cap[2]<<6)+opt_inadj[2]+(opt_ouadj[2]<<4);  
  vmeAddress = LAD_S+0x190000;
  ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); }
}
void set_offset_dacs()
{
  CVDataWidth dw = cvD32 ; // data width (see CAENVMEtypes.h )
  CVAddressModifier am = cvA32_U_DATA;

//Offset DAC V2: Need this because of the LRES
		    
		    for(int ix=0;ix<12;ix++){
  unsigned int data = 0x1000+(ix<<8)+offs_dac[ix]; // data for Offset
//  ch 1-12^  Ch#^  Offs^
  unsigned int vmeAddress = LAD_N+0x190000;
  CVErrorCodes ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write Offset DAC ch 1-12"<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); }		    
    
  data = 0x2000+(ix<<8)+offs_dac[ix+12]; // data for Offset
//  ch 13-24^  Ch#^  Offs^
  vmeAddress = LAD_N+0x190000;
  ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write Offset DAC ch 13-24"<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); }		    

  data = 0x4000+(ix<<8)+offs_dac[ix+24]; // data for Offset
//  ch 25-36^  Ch#^  Offs^
  vmeAddress = LAD_N+0x190000;
  ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write Offset DAC ch 25-36"<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); }		    
		                            } 
		    
           usleep(10000); // Time for offset DAC to settle
}
void set_clock_phases()
{
/* a 9 bit number, 1 bit for each of 9 channels in each of 4 chips
1=on   means use negative clock edge    
0=off        use positive clock edge
for now all set to negative clock edge
*/
  CVDataWidth dw = cvD32 ; // data width (see CAENVMEtypes.h )
  CVAddressModifier am = cvA32_U_DATA;
		   
   unsigned int data = clkphs0_9; // data all negative clock phase
  unsigned int vmeAddress = CHIP[0]+0x1b0000;
  CVErrorCodes ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); }		    

   data = clkphs10_18; // data all negative clock phase
  vmeAddress = CHIP[1]+0x1b0000;
  ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); }		    

   data = clkphs19_27; // data all negative clock phase
  vmeAddress = CHIP[2]+0x1b0000;
  ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); }		    

   data = clkphs28_36; // data all negative clock phase
  vmeAddress = CHIP[3]+0x1b0000;
  ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); }		    
}
void set_blk_ublk_thold()		    
{
 CVDataWidth dw = cvD32 ; // data width (see CAENVMEtypes.h )
  CVAddressModifier am = cvA32_U_DATA;
            for(int chip_nr=0;chip_nr<4;chip_nr++) {
              for(int channel=1;channel<10;channel++) {

     unsigned int UB_B_ThreshAddr=CHIP[chip_nr]+(channel<<17)+(2<<14);

/*UB_B_ThreshAddr is the chip +bit[20-17]=channel + bit[16-14] set to = 2	
black and Ultra-balck are assumed to be the same for all ROC's
*/	
  unsigned int data = Ublack[chip_nr*9+channel-1]+ 
  ((BlackLo[chip_nr*9+channel-1])<<10) + 
  ((BlackHi[chip_nr*9+channel-1])<<20); // data UB BL BH
//      UBLvl  BlackLowLvl   BlackHighLvl 
  
 CVErrorCodes ret = CAENVME_WriteCycle(BHandle,UB_B_ThreshAddr,&data,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); }		    
                                                  }
                                               }

}
void set_chnl_delays()
{
 CVDataWidth dw = cvD32 ; // data width (see CAENVMEtypes.h )
  CVAddressModifier am = cvA32_U_DATA;
/* 
This one is a little different as the clock distribution is
controlled on the center chip, and we are setting delays
for each input channel 1-36
*/ 		    
              for(int channel=1;channel<37;channel++) {

  unsigned int CLOCK_DelAddr=LAD_C+0x198000; 
  unsigned int data = 0x800+((channel)<<4) + DelayCh[channel-1]; // data for delay
//      enable channel(6 bits) delay(4 bits)
  CVErrorCodes ret = CAENVME_WriteCycle(BHandle,CLOCK_DelAddr,&data,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); }		

                                                  }

}
void set_data_levels()
{

 CVDataWidth dw = cvD32 ; // data width (see CAENVMEtypes.h )
 CVAddressModifier am = cvA32_U_DATA;

            for(int chip_nr=0;chip_nr<4;chip_nr++) {
              for(int channel=1;channel<10;channel++) {

     unsigned int L012_ThreshAddr=CHIP[chip_nr]+(channel<<17)+(4<<14);
     unsigned int L34_ThreshAddr=CHIP[chip_nr]+(channel<<17)+(5<<14);
/*
This is a little complicated. We actually have 2 "extra" ROC's in each
input channel. 
TBM has the levels for the input header from the TBM acts like 1st ROC really
Roc 0-(#of readout chips-1) has the levels for the actual pixel readout chips
TRL has the levels for the TBM trailer acts like last ROC really 
*/

     unsigned int data = TBM_L0[chip_nr*9+channel-1]+
          ((TBM_L1[chip_nr*9+channel-1])<<10)+
	  ((TBM_L2[chip_nr*9+channel-1])<<20);
//        Lvl 0     Lvl 1         Lvl 2     
  unsigned int vmeAddress = L012_ThreshAddr+((0)<<2);
  CVErrorCodes ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); }		
     
     data =TBM_L3[chip_nr*9+channel-1]+ 
         ((TBM_L4[chip_nr*9+channel-1])<<10);
//        Lvl 3    Lvl 4        
  vmeAddress = L34_ThreshAddr+((0)<<2);
  ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<hex<<endl;
    analyzeError(ret); }		




        for(unsigned int roc=0;roc<NRocs[chip_nr*9+channel-1];roc++) {
     data = ROC_L0[chip_nr*9+channel-1][roc]+
          ((ROC_L1[chip_nr*9+channel-1][roc])<<10)+
	  ((ROC_L2[chip_nr*9+channel-1][roc])<<20);
//        Lvl 0     Lvl 1         Lvl 2     
  vmeAddress = L012_ThreshAddr+((roc+1)<<2);
  ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); }		
     
     data =ROC_L3[chip_nr*9+channel-1][roc]+ 
         ((ROC_L4[chip_nr*9+channel-1][roc])<<10);
//        Lvl 3    Lvl 4        
  vmeAddress = L34_ThreshAddr+((roc+1)<<2);
  ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<hex<<endl;
    analyzeError(ret); }		
     
                		 }

     data = TRL_L0[chip_nr*9+channel-1]+
          ((TRL_L1[chip_nr*9+channel-1])<<10)+
	  ((TRL_L2[chip_nr*9+channel-1])<<20);
//        Lvl 0     Lvl 1         Lvl 2     
  vmeAddress = L012_ThreshAddr+((NRocs[chip_nr*9+channel-1]+1)<<2);
  ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); }		
     
     data =TRL_L3[chip_nr*9+channel-1]+ 
         ((TRL_L4[chip_nr*9+channel-1])<<10);
//        Lvl 3    Lvl 4        
  vmeAddress = L34_ThreshAddr+((NRocs[chip_nr*9+channel-1]+1)<<2);
  ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<hex<<endl;
    analyzeError(ret); }		




                                                }
                                             }

}

void set_chnls_onoff()
{
 CVDataWidth dw = cvD32 ; // data width (see CAENVMEtypes.h )
 CVAddressModifier am = cvA32_U_DATA;
// bits 0-8 on = channels 1-9 off
   unsigned int data = Ncntrl; 
  unsigned int vmeAddress = LAD_N+0x1a0000;
  CVErrorCodes ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); }		    
  data = NCcntrl; // bits 0-8 on = channels 10-18 on
  vmeAddress = LAD_NC+0x1a0000;
  ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); }		    
  data = SCcntrl; // bits 0-8 on = channels 19-27 on
  vmeAddress = LAD_SC+0x1a0000;
  ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); }		    
  data = Scntrl; // bits 0-8 on = channels 28-36 on
  vmeAddress = LAD_S+0x1a0000;
  ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); }		    
}
int read_vme_event()
{
//Check VME event counter
 CVDataWidth dw = cvD32 ; // data width (see CAENVMEtypes.h )
 CVAddressModifier am = cvA32_U_DATA;
// bits 0-8 on = channels 1-9 off
  unsigned int data; 
  CVErrorCodes ret = CAENVME_ReadCycle(BHandle, LAD_C+0x190000,&data,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret);
                       }
return(data);
}

void reset_slink()
{
 CVDataWidth dw = cvD32 ; // data width (see CAENVMEtypes.h )
 CVAddressModifier am = cvA32_U_DATA;
  unsigned int data = 0x4+Slink; // reset S-Link
  unsigned int vmeAddress = LAD_C+0x1c0000;;
  CVErrorCodes ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); }usleep(10);		    

   data = Slink; // back to usual
  vmeAddress = LAD_C+0x1c0000;;
  ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); }usleep(10);		    
}
int grab_transp_ch(int chnl)
{
  unsigned int vmeAddress;
  CVErrorCodes ret;

if(chnl<1) return(-1);
if(chnl>36) return(-1);	
if(chnl<10)vmeAddress=CHIP[0]+CH_SubAddr[chnl-1];
if((chnl>9)&(chnl<19))vmeAddress=CHIP[1]+CH_SubAddr[chnl-10];
if((chnl>18)&(chnl<28))vmeAddress=CHIP[2]+CH_SubAddr[chnl-19];
if((chnl>27)&(chnl<37))vmeAddress=CHIP[3]+CH_SubAddr[chnl-28];

 CVDataWidth dw[1024];   // data width (see CAENVMEtypes.h )
 CVAddressModifier am[1024]; 
 CVErrorCodes errs[1024];
 int cycles=1024;
 unsigned int addrses[1024];

for(int i=0;i<1024;i++){
dw[i]=cvD32;
am[i]=cvA32_U_DATA;
addrses[i]=vmeAddress;
}

  ret = CAENVME_MultiRead(BHandle,addrses,trnsbuff,cycles,am,dw,errs);
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<trnsbuff[0]<<dec<<endl;
    analyzeError(ret);
                       }
//for(int icx=0;icx<512;icx++)cout<<icx<<" "<<((trnsbuff[icx]&0xffc00000)>>22)<<endl;
		       
return(0);
}

void decode_trans()
{
cout<<" It takes 4 clock cycles to decode a channel "<<endl;
cout<<" This means there's a lag between Raw and Decoded Word "<<endl;
 

for(int icx=0;icx<256;icx++)
{
cout<<dec<<icx<<" raw fifo 1 "<<hex<<trnsbuff[icx];
cout<<" raw pulse height "<<dec<<((trnsbuff[icx]&0xffc00000)>>22)<<endl;
if(trnsbuff[icx]&0x3fffff){
cout<<"ROC Toggle "<<dec<<((trnsbuff[icx]&0x200000)>>21)<<endl;
cout<<"DCOL "<<dec<<((trnsbuff[icx]&0x1f0000)>>16)<<endl;
cout<<"PxL "<<dec<<((trnsbuff[icx]&0xff00)>>8)<<endl;
cout<<"PlsHgt "<<dec<<((trnsbuff[icx]&0xff))<<endl;}
}

cout<<" There's never Transparent Header Decoded Info "<<endl;
cout<<" There's never Transparent Trailer Decoded Info "<<endl;
	  
}
void set_baseline(){

 CVDataWidth dw = cvD32 ; // data width (see CAENVMEtypes.h )
 CVAddressModifier am = cvA32_U_DATA;
  unsigned int data = Nbaseln; 
  unsigned int vmeAddress = LAD_N+0x1d0000;;
  CVErrorCodes ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); }usleep(10);		    

  data = NCbaseln; 
  vmeAddress = LAD_NC+0x1d0000;;
  ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); }usleep(10);		    

  data = SCbaseln; 
  vmeAddress = LAD_SC+0x1d0000;;
  ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);	   
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); }usleep(10);		    

  data = Sbaseln; 
  vmeAddress = LAD_S+0x1d0000;;
  ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);	   
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); }usleep(10);		    

}
void set_adcgain(){

 CVDataWidth dw = cvD32 ; // data width (see CAENVMEtypes.h )
 CVAddressModifier am = cvA32_U_DATA;
  unsigned int data = Nadcg; 
  unsigned int vmeAddress = LAD_N+0x1b8000;;
  CVErrorCodes ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); }usleep(10);		    

  data = NCadcg; 
  vmeAddress = LAD_NC+0x1b8000;;
  ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); }usleep(10);		    

  data = SCadcg; 
  vmeAddress = LAD_SC+0x1b8000;;
  ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);	   
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); }usleep(10);		    

  data = Sadcg; 
  vmeAddress = LAD_S+0x1b8000;;
  ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);	   
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); }usleep(10);		    

}
void fill_dac_vals(int *val1,int *val2,int *val3)
{
  //cout<<"Filling the dac register"<<endl;

  CVDataWidth dw[256];
  CVAddressModifier am[256]; 
  unsigned int data[256];
  unsigned int vmeaddress[256];
  CVErrorCodes errs[256];
  int cycles=256;

        for(int i=0;i<256;i++){

  dw[i] = cvD32;
  am[i] = cvA32_U_DATA;


    unsigned int tmp1 = (val1[i] & 0x3ff);
    unsigned int tmp2 = (val2[i] & 0x3ff);
    unsigned int tmp3 = (val3[i] & 0x3ff);
    data[i] =  tmp1 + (tmp2<<10) + (tmp3<<20);
  
   vmeaddress[i]=LAD_N+0x180000+i*4;
                               }

  CVErrorCodes ret = CAENVME_MultiWrite(BHandle,vmeaddress,data,cycles,am,dw,errs);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write to DAC LAD_N+0x180000+i"<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret);
                       } 
                                

 // cout<<"Set the start pulse delay to 0? "<<endl;
  unsigned int datar = 0x0;
  unsigned int vmeAddressr = LAD_C+0x1a8000;
  ret = CAENVME_WriteCycle(BHandle,vmeAddressr,&datar,am[0],dw[0]);
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); 
                       }  
                                                                                
           usleep(100); // Time for DAC
                                                                                

 
}
void fill_dac_valn(int *val1,int *val2,int *val3,int max)
{
  //cout<<"Filling the dac register"<<endl;

  CVDataWidth dw[256];
  CVAddressModifier am[256]; 
  unsigned int data[256];
  unsigned int vmeaddress[256];
  CVErrorCodes errs[256];
  int cycles=256;

        for(int i=0;i<256;i++){

  dw[i] = cvD32;
  am[i] = cvA32_U_DATA;


    unsigned int tmp1 = (val1[i] & 0x3ff);
    unsigned int tmp2 = (val2[i] & 0x3ff);
    unsigned int tmp3 = (val3[i] & 0x3ff);
    data[i] =  tmp1 + (tmp2<<10) + (tmp3<<20);
  if(i==max){data[i]=data[i]+0x80000000;cout<<"dacmax="<<dec<<i<<endl;}  
   vmeaddress[i]=LAD_N+0x180000+i*4;
                               }

  CVErrorCodes ret = CAENVME_MultiWrite(BHandle,vmeaddress,data,cycles,am,dw,errs);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write to DAC LAD_N+0x180000+i"<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret);
                       } 
                                

 // cout<<"Set the start pulse delay to 0? "<<endl;
  unsigned int datar = 0x0;
  unsigned int vmeAddressr = LAD_C+0x1a8000;
  ret = CAENVME_WriteCycle(BHandle,vmeAddressr,&datar,am[0],dw[0]);
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); 
                       }  
                                                                                
           usleep(100); // Time for DAC
                                                                                

 
}
void fill_dac_val(int value)
{
  //cout<<"Filling the dac register"<<endl;

  CVDataWidth dw[256];
  CVAddressModifier am[256]; 
  unsigned int data[256];
  unsigned int vmeaddress[256];
  CVErrorCodes errs[256];
  int cycles=256;

        for(int i=0;i<256;i++){

  dw[i] = cvD32;
  am[i] = cvA32_U_DATA;


    unsigned int tmp1 = (value & 0x3ff);
    unsigned int tmp2 = (value & 0x3ff);
    unsigned int tmp3 = (value & 0x3ff);
    data[i] =  tmp1 + (tmp2<<10) + (tmp3<<20);
  
   vmeaddress[i]=LAD_N+0x180000+i*4;
                               }

  CVErrorCodes ret = CAENVME_MultiWrite(BHandle,vmeaddress,data,cycles,am,dw,errs);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write to DAC LAD_N+0x180000+i"<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret);
                       } 
                                

 // cout<<"Set the start pulse delay to 0? "<<endl;
  unsigned int datar = 0x0;
  unsigned int vmeAddressr = LAD_C+0x1a8000;
  ret = CAENVME_WriteCycle(BHandle,vmeAddressr,&datar,am[0],dw[0]);
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); 
                       }  
                                                                                
           usleep(100); // Time for DAC
                                                                                

 
}
int tdac_header(int event,int *dac,int start,unsigned int V_OFFSET,unsigned int UBLK,unsigned int BLK){

//  const unsigned int V_OFFSET=100;
//  const unsigned int UB =50;
//  const unsigned int B = 300;

  const int levels[6] = {200,300,450,550,650,750};  

  int n1, n2, n3, n4;  // event 

  // Limits are     :    270, 400, 500, 600, 700
  // The levels are : 200, 300, 450, 550, 650, 750  

  // Change the header label
  dac[start+0] = V_OFFSET+UBLK;	   //UB  TBM-header
  dac[start+1] = V_OFFSET+UBLK;	   //UB
  dac[start+2] = V_OFFSET+UBLK;	   //UB
  dac[start+3] = V_OFFSET+BLK;	   //B

n1=(event&0x3);
n2=(event&0xc)>>2;
n3=(event&0x30)>>4;
n4=(event&0xc0)>>6;
 
  // Change the event numebr
  dac[start+4] = V_OFFSET+levels[n4];  // event number
  dac[start+5] = V_OFFSET+levels[n3];
  dac[start+6] = V_OFFSET+levels[n2];
  dac[start+7] = V_OFFSET+levels[n1];

//  cout<<" DACs "<<dac1[START+4]<<" "<<dac1[START+5]<<" "<<dac1[START+6]<<" "<<dac1[START+7]<<endl;
return start+8;
 
}
int tdac_trailer(int word,int *dac,int start,unsigned int V_OFFSET,unsigned int UBLK,unsigned int BLK){

//  const unsigned int V_OFFSET=100;
//  const unsigned int UB =50;
//  const unsigned int B = 300;

  const int levels[6] = {200,300,450,550,650,750};  

int n1,n2,n3,n4;

  
  dac[start+0] = V_OFFSET+UBLK;	   //UB  TBM-header
  dac[start+1] = V_OFFSET+UBLK;	   //UB
  dac[start+2] = V_OFFSET+BLK;	   //UB
  dac[start+3] = V_OFFSET+BLK;	   //B
  
n1=(word&0x3);
n2=(word&0xc)>>2;
n3=(word&0x30)>>4;
n4=(word&0xc0)>>6;
 
  // Change the event numebr
  dac[start+4] = V_OFFSET+levels[n4];  // event number
  dac[start+5] = V_OFFSET+levels[n3];
  dac[start+6] = V_OFFSET+levels[n2];
  dac[start+7] = V_OFFSET+levels[n1];

//cout<<"trailer"<<endl;
//for(int ix=0;ix<8;ix++)cout<<dec<<dac[start+ix]<<" "<<dec<<ix<<endl;
  
 return start+8; 
  
}
int tdac_roc(int lastdac,int *dac,int start,unsigned int V_OFFSET,unsigned int UBLK,unsigned int BLK){

//  const unsigned int V_OFFSET=100;
//  const unsigned int UB =50;
//  const unsigned int B = 300;


  dac[start+0] = V_OFFSET+UBLK;    //UB  ROC#3
  dac[start+1] = V_OFFSET+BLK;     //B
  dac[start+2] = V_OFFSET+lastdac;  //  LastDac
if((V_OFFSET+lastdac)<1023){
  dac[start+2]= V_OFFSET+lastdac;  // PH
} else {dac[start+2]=1023;}   
  
   return start+3;
}
  
  
int tdac_pxlhit(int dbc,int pixl,int adc,int *dac,int start,unsigned int V_OFFSET,unsigned int UBLK,unsigned int BLK){

//  const unsigned int V_OFFSET=100;
//  const unsigned int UBLK =50;
//  const unsigned int B = 300;
  const int levels[6] = {200,300,450,550,640,750};  
unsigned d1,d2;
unsigned int n1,n2,n3;

//Form pixel code
int pxl=pixl;
if(pxl<0) pxl=0;
if(pxl>215) pxl=215;

if(pxl<36){
n3=0;

 if(pxl<6){
 n2=0;
 } else if ((pxl>5)&(pxl<12)) {
 n2=1;
 pxl=pxl-6;
 } else if ((pxl>11)&(pxl<18)) {
 n2=2;
 pxl=pxl-12;
 } else if ((pxl>17)&(pxl<24)) {
 n2=3;
 pxl=pxl-18;
 } else if ((pxl>23)&(pxl<30)) {
 n2=4;
 pxl=pxl-24;
 } else if ((pxl>29)&(pxl<36)) {
 n2=5;
 pxl=pxl-30;
 }

n1=pxl;


} else if ((pxl>35)&(pxl<72)) {
n3=1;

pxl=pxl-36;

 if(pxl<6){
 n2=0;
 } else if ((pxl>5)&(pxl<12)) {
 n2=1;
 pxl=pxl-6;
 } else if ((pxl>11)&(pxl<18)) {
 n2=2;
 pxl=pxl-12;
 } else if ((pxl>17)&(pxl<24)) {
 n2=3;
 pxl=pxl-18;
 } else if ((pxl>23)&(pxl<30)) {
 n2=4;
 pxl=pxl-24;
 } else if ((pxl>29)&(pxl<36)) {
 n2=5;
 pxl=pxl-30;
 }

n1=pxl;


} else if ((pxl>71)&(pxl<108)) {
n3=2;
pxl=pxl-72;

 if(pxl<6){
 n2=0;
 } else if ((pxl>5)&(pxl<12)) {
 n2=1;
 pxl=pxl-6;
 } else if ((pxl>11)&(pxl<18)) {
 n2=2;
 pxl=pxl-12;
 } else if ((pxl>17)&(pxl<24)) {
 n2=3;
 pxl=pxl-18;
 } else if ((pxl>23)&(pxl<30)) {
 n2=4;
 pxl=pxl-24;
 } else if ((pxl>29)&(pxl<36)) {
 n2=5;
 pxl=pxl-30;
 }

n1=pxl;

} else if ((pxl>107)&(pxl<144)) {
n3=3;
pxl=pxl-108;

 if(pxl<6){
 n2=0;
 } else if ((pxl>5)&(pxl<12)) {
 n2=1;
 pxl=pxl-6;
 } else if ((pxl>11)&(pxl<18)) {
 n2=2;
 pxl=pxl-12;
 } else if ((pxl>17)&(pxl<24)) {
 n2=3;
 pxl=pxl-18;
 } else if ((pxl>23)&(pxl<30)) {
 n2=4;
 pxl=pxl-24;
 } else if ((pxl>29)&(pxl<36)) {
 n2=5;
 pxl=pxl-30;
 }

n1=pxl;

} else if ((pxl>143)&(pxl<180)) {

n3=4;
pxl=pxl-144;

 if(pxl<6){
 n2=0;
 } else if ((pxl>5)&(pxl<12)) {
 n2=1;
 pxl=pxl-6;
 } else if ((pxl>11)&(pxl<18)) {
 n2=2;
 pxl=pxl-12;
 } else if ((pxl>17)&(pxl<24)) {
 n2=3;
 pxl=pxl-18;
 } else if ((pxl>23)&(pxl<30)) {
 n2=4;
 pxl=pxl-24;
 } else if ((pxl>29)&(pxl<36)) {
 n2=5;
 pxl=pxl-30;
 }

n1=pxl;

} else if ((pxl>179)&(pxl<216)) {

n3=5;
pxl=pxl-180;

 if(pxl<6){
 n2=0;
 } else if ((pxl>5)&(pxl<12)) {
 n2=1;
 pxl=pxl-6;
 } else if ((pxl>11)&(pxl<18)) {
 n2=2;
 pxl=pxl-12;
 } else if ((pxl>17)&(pxl<24)) {
 n2=3;
 pxl=pxl-18;
 } else if ((pxl>23)&(pxl<30)) {
 n2=4;
 pxl=pxl-24;
 } else if ((pxl>29)&(pxl<36)) {
 n2=5;
 pxl=pxl-30;
 }

n1=pxl;

}

int dc=dbc;

if(dbc<0) dc=0;
if(dbc>35)dc=35;
 if(dc<6){
 d2=0;
 } else if ((dc>5)&(dc<12)) {
 d2=1;
 dc=dc-6;
 } else if ((dc>11)&(dc<18)) {
 d2=2;
 dc=dc-12;
 } else if ((dc>17)&(dc<24)) {
 d2=3;
 dc=dc-18;
 } else if ((dc>23)&(dc<30)) {
 d2=4;
 dc=dc-24;
 } else if ((dc>29)&(dc<36)) {
 d2=5;
 dc=dc-30;
 }

d1=dc;

  dac[start+0]= V_OFFSET+levels[d2];  // dcol MSB PIXEL 1
  dac[start+1]= V_OFFSET+levels[d1];  // dcol LSB     
  dac[start+2]= V_OFFSET+levels[n3];  // pix 
  dac[start+3]= V_OFFSET+levels[n2];  // pix 
  dac[start+4]= V_OFFSET+levels[n1];  // pix  
if((V_OFFSET+adc)<1023){
  dac[start+5]= V_OFFSET+adc;  // PH
} else {dac[start+5]=1023;}   


   return start+6;

}

void send_vmetrig(){
////sent an event via vme

CVDataWidth dw = cvD32 ; // data width (see CAENVMEtypes.h )
CVAddressModifier  am = cvA32_U_DATA;
  unsigned int data = 0x200; // Event 1  
  unsigned int vmeAddress = LAD_C+0x180000;
   CVErrorCodes ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); }
              usleep(10);
}
void fitls(int ndata, double *x,double *y,double *ey,double *a,double *b,double *ea,double *eb,double *cov,double *chi2)
{//fit a straight line y=a+bx with weighted y's

double S=0.0;
double Sx=0.0;
double Sy=0.0;
double Sxx=0.0;
double Sxy=0.0;
double DET=0.0;

a[0]=9999.0;
b[0]=9999.0;
ea[0]=9999.0;
eb[0]=9999.0;
cov[0]=9999.0;
chi2[0]=9999.0;

for(int ij=0;ij<ndata;ij++){
if(ey[ij]<=0.0) return;
//cout<<x[ij]<<" "<<y[ij]<<" "<<ey[ij]<<" "<<ij<<endl;
S=S+ 1.0/(ey[ij]*ey[ij]);
Sx=Sx+ x[ij]/(ey[ij]*ey[ij]);
Sxx=Sxx+ x[ij]*x[ij]/(ey[ij]*ey[ij]);
Sy=Sy+y[ij]/(ey[ij]*ey[ij]);
Sxy=Sxy+ x[ij]*y[ij]/(ey[ij]*ey[ij]);
}
DET=S*Sxx-Sx*Sx;
if(DET<=double(0)) return;
a[0]=(Sxx*Sy-Sx*Sxy)/DET;
b[0]=(-Sx*Sy+S*Sxy)/DET;
ea[0]=Sxx/DET;
eb[0]=S/DET;
cov[0]=-Sx/DET;
chi2[0]=0.0;

for(int ij=0;ij<ndata;ij++){
chi2[0]=chi2[0]+(y[ij]-a[0]-b[0]*x[ij])*(y[ij]-a[0]-b[0]*x[ij])/(ey[ij]*ey[ij]);}
//cout<<b[0]<<" slope +/- "<<eb[0]<<" "<<endl;
//cout<<a[0]<<" intcp +/- "<<ea[0]<<" "<<endl;
//cout<<chi2[0]<<" Chi2, DOF "<<ndata-2<<" "<<endl;

}


void decode_trans_info(int *nrocs,int *lastdacs,int *nhits,int *dcols,int *pxls,int *adcs,int *hroc)
{
//cout<<" It takes 4 clock cycles to decode a channel "<<endl;
//cout<<" This means there's a lag between Raw and Decoded Word "<<endl;
nrocs[0]=-1; 
nhits[0]=-1;
unsigned int roctog=2;
for(int icx=11;icx<256;icx++)
{
//cout<<dec<<icx<<" raw fifo 1 "<<hex<<trnsbuff[icx];
//cout<<" raw pulse height "<<dec<<((trnsbuff[icx]&0xffc00000)>>22)<<endl;
if(trnsbuff[icx]&0x3fffff){
//cout<<"ROC Toggle "<<dec<<((trnsbuff[icx]&0x200000)>>21)<<endl;
if(((trnsbuff[icx]&0x200000)>>21)!=roctog)
{nrocs[0]++;roctog=((trnsbuff[icx]&0x200000)>>21);
 if(icx>=20){
lastdacs[nrocs[0]]=((trnsbuff[icx-10]&0xffc00000)>>22);
 } else {
cout<<"!!Error: Last dac too soon!"<<endl;
lastdacs[nrocs[0]]=-999;
 }}
nhits[0]++;
//cout<<"DCOL "<<dec<<((trnsbuff[icx]&0x1f0000)>>16)<<endl;
//cout<<"PxL "<<dec<<((trnsbuff[icx]&0xff00)>>8)<<endl;
//cout<<"PlsHgt "<<dec<<((trnsbuff[icx]&0xff))<<endl;}
dcols[nhits[0]]=((trnsbuff[icx]&0x1f0000)>>16);
pxls[nhits[0]]=((trnsbuff[icx]&0xff00)>>8);
adcs[nhits[0]]=((trnsbuff[icx]&0xff));
hroc[nhits[0]]=nrocs[0];
}

//cout<<" There's never Transparent Header Decoded Info "<<endl;
//cout<<" There's never Transparent Trailer Decoded Info "<<endl;
	  
}

}
void dump_BaselineCorr()
{
 CVDataWidth dw = cvD32 ; // data width (see CAENVMEtypes.h )
  CVAddressModifier am = cvA32_U_DATA;

unsigned int iwrdat;
int blstat[36]; 

unsigned int VmeAddress=LAD_N+0x1d0000;
  CVErrorCodes ret = CAENVME_ReadCycle(BHandle, VmeAddress,&iwrdat,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in Draining the Fifo "<<hex<<ret<<" "<<iwrdat<<dec<<endl;
    analyzeError(ret);
  }
//vmeDevicePtr->read("NRdBaseL321",&iwrdat);
blstat[0]=(iwrdat&0x3ff);
blstat[1]=(iwrdat&0xffc00)>>10;
blstat[2]=(iwrdat&0x3ff00000)>>20;

VmeAddress=LAD_N+0x1d8000;
 ret = CAENVME_ReadCycle(BHandle, VmeAddress,&iwrdat,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in Draining the Fifo "<<hex<<ret<<" "<<iwrdat<<dec<<endl;
    analyzeError(ret);
  }
//vmeDevicePtr->read("NRdBaseL654",&iwrdat);
blstat[3]=(iwrdat&0x3ff);
blstat[4]=(iwrdat&0xffc00)>>10;
blstat[5]=(iwrdat&0x3ff00000)>>20;

VmeAddress=LAD_N+0x1e0000;
 ret = CAENVME_ReadCycle(BHandle, VmeAddress,&iwrdat,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in Draining the Fifo "<<hex<<ret<<" "<<iwrdat<<dec<<endl;
    analyzeError(ret);
  }
//vmeDevicePtr->read("NRdBaseL987",&iwrdat);
blstat[6]=(iwrdat&0x3ff);
blstat[7]=(iwrdat&0xffc00)>>10;
blstat[8]=(iwrdat&0x3ff00000)>>20;

VmeAddress=LAD_NC+0x1d0000;
 ret = CAENVME_ReadCycle(BHandle, VmeAddress,&iwrdat,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in Draining the Fifo "<<hex<<ret<<" "<<iwrdat<<dec<<endl;
    analyzeError(ret);
  }
//vmeDevicePtr->read("NCRdBaseL321",&iwrdat);
blstat[9]=(iwrdat&0x3ff);
blstat[10]=(iwrdat&0xffc00)>>10;
blstat[11]=(iwrdat&0x3ff00000)>>20;


VmeAddress=LAD_NC+0x1d8000;
 ret = CAENVME_ReadCycle(BHandle, VmeAddress,&iwrdat,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in Draining the Fifo "<<hex<<ret<<" "<<iwrdat<<dec<<endl;
    analyzeError(ret);
  }
//vmeDevicePtr->read("NCRdBaseL654",&iwrdat);
blstat[12]=(iwrdat&0x3ff);
blstat[13]=(iwrdat&0xffc00)>>10;
blstat[14]=(iwrdat&0x3ff00000)>>20;

VmeAddress=LAD_N+0x1e0000;
   ret = CAENVME_ReadCycle(BHandle, VmeAddress,&iwrdat,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in Draining the Fifo "<<hex<<ret<<" "<<iwrdat<<dec<<endl;
    analyzeError(ret);
  }
//vmeDevicePtr->read("NCRdBaseL987",&iwrdat);
blstat[15]=(iwrdat&0x3ff);
blstat[16]=(iwrdat&0xffc00)>>10;
blstat[17]=(iwrdat&0x3ff00000)>>20;


VmeAddress=LAD_SC+0x1d0000;
  ret = CAENVME_ReadCycle(BHandle, VmeAddress,&iwrdat,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in Draining the Fifo "<<hex<<ret<<" "<<iwrdat<<dec<<endl;
    analyzeError(ret);
  }
//vmeDevicePtr->read("SCRdBaseL321",&iwrdat);
blstat[18]=(iwrdat&0x3ff);
blstat[19]=(iwrdat&0xffc00)>>10;
blstat[20]=(iwrdat&0x3ff00000)>>20;

VmeAddress=LAD_SC+0x1d8000;
   ret = CAENVME_ReadCycle(BHandle, VmeAddress,&iwrdat,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in Draining the Fifo "<<hex<<ret<<" "<<iwrdat<<dec<<endl;
    analyzeError(ret);
  }
//vmeDevicePtr->read("SCRdBaseL654",&iwrdat);
blstat[21]=(iwrdat&0x3ff);
blstat[22]=(iwrdat&0xffc00)>>10;
blstat[23]=(iwrdat&0x3ff00000)>>20;


VmeAddress=LAD_N+0x1e0000;
  ret = CAENVME_ReadCycle(BHandle, VmeAddress,&iwrdat,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in Draining the Fifo "<<hex<<ret<<" "<<iwrdat<<dec<<endl;
    analyzeError(ret);
  }
//vmeDevicePtr->read("SCRdBaseL987",&iwrdat);
blstat[24]=(iwrdat&0x3ff);
blstat[25]=(iwrdat&0xffc00)>>10;
blstat[26]=(iwrdat&0x3ff00000)>>20;


VmeAddress=LAD_S+0x1d0000;
  ret = CAENVME_ReadCycle(BHandle, VmeAddress,&iwrdat,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in Draining the Fifo "<<hex<<ret<<" "<<iwrdat<<dec<<endl;
    analyzeError(ret);
  }

//vmeDevicePtr->read("SRdBaseL321",&iwrdat);
blstat[27]=(iwrdat&0x3ff);
blstat[28]=(iwrdat&0xffc00)>>10;
blstat[29]=(iwrdat&0x3ff00000)>>20;

VmeAddress=LAD_S+0x1d8000;
   ret = CAENVME_ReadCycle(BHandle, VmeAddress,&iwrdat,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in Draining the Fifo "<<hex<<ret<<" "<<iwrdat<<dec<<endl;
    analyzeError(ret);
  }
//vmeDevicePtr->read("SRdBaseL654",&iwrdat);
blstat[30]=(iwrdat&0x3ff);
blstat[31]=(iwrdat&0xffc00)>>10;
blstat[32]=(iwrdat&0x3ff00000)>>20;

VmeAddress=LAD_N+0x1e0000;
  ret = CAENVME_ReadCycle(BHandle, VmeAddress,&iwrdat,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in Draining the Fifo "<<hex<<ret<<" "<<iwrdat<<dec<<endl;
    analyzeError(ret);
  }
//vmeDevicePtr->read("SRdBaseL987",&iwrdat);
blstat[33]=(iwrdat&0x3ff);
blstat[34]=(iwrdat&0xffc00)>>10;
blstat[35]=(iwrdat&0x3ff00000)>>20;

for(int i=0;i<36;i++){
if(i==0){cout<<"Baseline Value for Channels 1-9 is:"<<Nbaseln<<endl;}
if(i==9){cout<<"Baseline Value for Channels 10-18 is:"<<NCbaseln<<endl;}
if(i==18){cout<<"Baseline Value for Channels 19-27 is:"<<SCbaseln<<endl;}
if(i==27){cout<<"Baseline Value for Channels 28-36 is:"<<Sbaseln<<endl;}
if(blstat[i]&0x200){
cout<<"Baseline Adjust for Channel "<<dec<<i+1<<" is -"<<((~blstat[i])&0x1ff)+1<<endl;
} else {
cout<<"Baseline Adjust for Channel "<<dec<<i+1<<" is "<<blstat[i]<<endl;
}


}

}//end dump
void read_BaselineCorr(int *blstat)
{
 CVDataWidth dw = cvD32 ; // data width (see CAENVMEtypes.h )
  CVAddressModifier am = cvA32_U_DATA;

unsigned int iwrdat;
//int blstat[36]; 

unsigned int VmeAddress=LAD_N+0x1d0000;
  CVErrorCodes ret = CAENVME_ReadCycle(BHandle, VmeAddress,&iwrdat,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in Draining the Fifo "<<hex<<ret<<" "<<iwrdat<<dec<<endl;
    analyzeError(ret);
  }
//vmeDevicePtr->read("NRdBaseL321",&iwrdat);
blstat[0]=(iwrdat&0x3ff);
blstat[1]=(iwrdat&0xffc00)>>10;
blstat[2]=(iwrdat&0x3ff00000)>>20;

VmeAddress=LAD_N+0x1d8000;
 ret = CAENVME_ReadCycle(BHandle, VmeAddress,&iwrdat,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in Draining the Fifo "<<hex<<ret<<" "<<iwrdat<<dec<<endl;
    analyzeError(ret);
  }
//vmeDevicePtr->read("NRdBaseL654",&iwrdat);
blstat[3]=(iwrdat&0x3ff);
blstat[4]=(iwrdat&0xffc00)>>10;
blstat[5]=(iwrdat&0x3ff00000)>>20;

VmeAddress=LAD_N+0x1e0000;
 ret = CAENVME_ReadCycle(BHandle, VmeAddress,&iwrdat,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in Draining the Fifo "<<hex<<ret<<" "<<iwrdat<<dec<<endl;
    analyzeError(ret);
  }
//vmeDevicePtr->read("NRdBaseL987",&iwrdat);
blstat[6]=(iwrdat&0x3ff);
blstat[7]=(iwrdat&0xffc00)>>10;
blstat[8]=(iwrdat&0x3ff00000)>>20;

VmeAddress=LAD_NC+0x1d0000;
 ret = CAENVME_ReadCycle(BHandle, VmeAddress,&iwrdat,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in Draining the Fifo "<<hex<<ret<<" "<<iwrdat<<dec<<endl;
    analyzeError(ret);
  }
//vmeDevicePtr->read("NCRdBaseL321",&iwrdat);
blstat[9]=(iwrdat&0x3ff);
blstat[10]=(iwrdat&0xffc00)>>10;
blstat[11]=(iwrdat&0x3ff00000)>>20;


VmeAddress=LAD_NC+0x1d8000;
 ret = CAENVME_ReadCycle(BHandle, VmeAddress,&iwrdat,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in Draining the Fifo "<<hex<<ret<<" "<<iwrdat<<dec<<endl;
    analyzeError(ret);
  }
//vmeDevicePtr->read("NCRdBaseL654",&iwrdat);
blstat[12]=(iwrdat&0x3ff);
blstat[13]=(iwrdat&0xffc00)>>10;
blstat[14]=(iwrdat&0x3ff00000)>>20;

VmeAddress=LAD_N+0x1e0000;
   ret = CAENVME_ReadCycle(BHandle, VmeAddress,&iwrdat,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in Draining the Fifo "<<hex<<ret<<" "<<iwrdat<<dec<<endl;
    analyzeError(ret);
  }
//vmeDevicePtr->read("NCRdBaseL987",&iwrdat);
blstat[15]=(iwrdat&0x3ff);
blstat[16]=(iwrdat&0xffc00)>>10;
blstat[17]=(iwrdat&0x3ff00000)>>20;


VmeAddress=LAD_SC+0x1d0000;
  ret = CAENVME_ReadCycle(BHandle, VmeAddress,&iwrdat,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in Draining the Fifo "<<hex<<ret<<" "<<iwrdat<<dec<<endl;
    analyzeError(ret);
  }
//vmeDevicePtr->read("SCRdBaseL321",&iwrdat);
blstat[18]=(iwrdat&0x3ff);
blstat[19]=(iwrdat&0xffc00)>>10;
blstat[20]=(iwrdat&0x3ff00000)>>20;

VmeAddress=LAD_SC+0x1d8000;
   ret = CAENVME_ReadCycle(BHandle, VmeAddress,&iwrdat,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in Draining the Fifo "<<hex<<ret<<" "<<iwrdat<<dec<<endl;
    analyzeError(ret);
  }
//vmeDevicePtr->read("SCRdBaseL654",&iwrdat);
blstat[21]=(iwrdat&0x3ff);
blstat[22]=(iwrdat&0xffc00)>>10;
blstat[23]=(iwrdat&0x3ff00000)>>20;


VmeAddress=LAD_N+0x1e0000;
  ret = CAENVME_ReadCycle(BHandle, VmeAddress,&iwrdat,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in Draining the Fifo "<<hex<<ret<<" "<<iwrdat<<dec<<endl;
    analyzeError(ret);
  }
//vmeDevicePtr->read("SCRdBaseL987",&iwrdat);
blstat[24]=(iwrdat&0x3ff);
blstat[25]=(iwrdat&0xffc00)>>10;
blstat[26]=(iwrdat&0x3ff00000)>>20;


VmeAddress=LAD_S+0x1d0000;
  ret = CAENVME_ReadCycle(BHandle, VmeAddress,&iwrdat,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in Draining the Fifo "<<hex<<ret<<" "<<iwrdat<<dec<<endl;
    analyzeError(ret);
  }

//vmeDevicePtr->read("SRdBaseL321",&iwrdat);
blstat[27]=(iwrdat&0x3ff);
blstat[28]=(iwrdat&0xffc00)>>10;
blstat[29]=(iwrdat&0x3ff00000)>>20;

VmeAddress=LAD_S+0x1d8000;
   ret = CAENVME_ReadCycle(BHandle, VmeAddress,&iwrdat,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in Draining the Fifo "<<hex<<ret<<" "<<iwrdat<<dec<<endl;
    analyzeError(ret);
  }
//vmeDevicePtr->read("SRdBaseL654",&iwrdat);
blstat[30]=(iwrdat&0x3ff);
blstat[31]=(iwrdat&0xffc00)>>10;
blstat[32]=(iwrdat&0x3ff00000)>>20;

VmeAddress=LAD_N+0x1e0000;
  ret = CAENVME_ReadCycle(BHandle, VmeAddress,&iwrdat,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in Draining the Fifo "<<hex<<ret<<" "<<iwrdat<<dec<<endl;
    analyzeError(ret);
  }
//vmeDevicePtr->read("SRdBaseL987",&iwrdat);
blstat[33]=(iwrdat&0x3ff);
blstat[34]=(iwrdat&0xffc00)>>10;
blstat[35]=(iwrdat&0x3ff00000)>>20;

/*for(int i=0;i<36;i++){
if(i==0){cout<<"Baseline Value for Channels 1-9 is:"<<Nbaseln<<endl;}
if(i==9){cout<<"Baseline Value for Channels 10-18 is:"<<NCbaseln<<endl;}
if(i==18){cout<<"Baseline Value for Channels 19-27 is:"<<SCbaseln<<endl;}
if(i==27){cout<<"Baseline Value for Channels 28-36 is:"<<Sbaseln<<endl;}
if(blstat[i]&0x200){
cout<<"Baseline Adjust for Channel "<<dec<<i+1<<" is -"<<((~blstat[i])&0x1ff)+1<<endl;
} else {
cout<<"Baseline Adjust for Channel "<<dec<<i+1<<" is "<<blstat[i]<<endl;
}


}
*/
}//end dump
////////////////////////////////////////////////////////////////////////////
void set_nrocs(void)
{
 CVDataWidth dw = cvD32; // data width (see CAENVMEtypes.h )
  CVAddressModifier am = cvA32_U_DATA;
for(unsigned int is=1;is<10;is++){
  unsigned int data = NRocs[is-1];
  unsigned int vmeAddress = LAD_N+(is<<17);
  CVErrorCodes ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); } 
}
for(unsigned int is=1;is<10;is++){
  unsigned int data = NRocs[is+8];
  unsigned int vmeAddress = LAD_NC+(is<<17);
  CVErrorCodes ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); } 
}
for(unsigned int is=1;is<10;is++){
  unsigned int data = NRocs[is+17];
  unsigned int vmeAddress = LAD_SC+(is<<17);
  CVErrorCodes ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); } 
}
for(unsigned int is=1;is<10;is++){
  unsigned int data = NRocs[is+26];
  unsigned int vmeAddress = LAD_S+(is<<17);
  CVErrorCodes ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret); } 
}

}
//////////////////////////////////////////////////////////////////////////////////////////
void set_standard(unsigned int *iV_OFFSET,unsigned int *iUBLK,unsigned int *iBLK)
{
for(int ij=0;ij<3;ij++)opt_inadj[ij]=10;
for(int ij=0;ij<3;ij++)opt_ouadj[ij]=0;
for(int ij=0;ij<36;ij++)offs_dac[ij]=0;
set_opto_params();
set_offset_dacs();

Nadcg=0;
NCadcg=0;
SCadcg=0;
Sadcg=0;
set_adcgain();

unsigned int V_OFFSET=100;
unsigned int UBLK=50;
unsigned int BLK=300;

int ibaseln=300;

//Nbaseln=(0x1ff<<16)+ibaseln;
//NCbaseln=(0x1ff<<16)+ibaseln;
//SCbaseln=(0x1ff<<16)+ibaseln;
//Sbaseln=(0x1ff<<16)+ibaseln;
Nbaseln= ibaseln;
NCbaseln=ibaseln;
SCbaseln=ibaseln;
Sbaseln= ibaseln;

set_baseline();

for(int jj=0;jj<36;jj++){int xdata=grab_transp_ch(jj+1);if(xdata!=0)cout<<"grab_transp_ch error"<<endl;}//clear
//try to get all channels >= ibaseln

int iminm=0;
fill_dac_val(V_OFFSET+BLK);
 send_vmetrig();
for(int jj=0;jj<36;jj++){int xdata=grab_transp_ch(jj+1);if(xdata!=0)cout<<"grab_transp_ch error"<<endl;
double xtot=0; 
double xtot2=0; 
for(int ik=141;ik<241;ik++)
{xtot+=double((trnsbuff[ik]&0xffc00000)>>22);
xtot2+=double((trnsbuff[ik]&0xffc00000)>>22)*double((trnsbuff[ik]&0xffc00000)>>22);}
xtot=xtot/100.0;
xtot2=sqrt(xtot2-100.0*xtot*xtot);
int itot=int(xtot);
if(itot<ibaseln){
if((iminm>0)&(itot<iminm))iminm=itot;
if(iminm==0)iminm=itot;}

//cout<<dec<<((trnsbuff[25]&0xffc00000)>>22)<<" "<<jj+1<<endl;
}

if(iminm>0)V_OFFSET=V_OFFSET+ibaseln-iminm;
cout<<dec<<iminm<<" iminimum value for setting offsets and testDAC"<<endl;
if(iminm<200)cout<<"Minimum less that 200, did board pass tests?"<<endl;
fill_dac_val(V_OFFSET+BLK);
 send_vmetrig();
for(int jj=0;jj<36;jj++){int xdata=grab_transp_ch(jj+1);if(xdata!=0)cout<<"grab_transp_ch error"<<endl;
double xtot=0; 
for(int ik=141;ik<241;ik++)
{xtot+=double((trnsbuff[ik]&0xffc00000)>>22);
}
xtot=xtot/100.0;
int itot=int(xtot);
if(itot>ibaseln)offs_dac[jj]=int(float(itot-ibaseln)/2.01);
//cout<<"raw tot = "<<xtot<<" int avg = "<<dec<<int(xtot/20.0)<<" offst = "<<dec<<offs_dac[jj]<<" ch = "<<dec<<jj+1<<endl;

//cout<<dec<<((trnsbuff[25]&0xffc00000)>>22)<<" set = "<<ibaseln<<" offset ="<<dec<<offs_dac[jj]<<" ch= "<<dec<<jj+1<<endl;
}

set_offset_dacs();
for(int jj=0;jj<36;jj++){int xdata=grab_transp_ch(jj+1);if(xdata!=0)cout<<"grab_transp_ch error"<<endl;}
send_vmetrig();
for(int jj=0;jj<36;jj++){int xdata=grab_transp_ch(jj+1);if(xdata!=0)cout<<"grab_transp_ch error"<<endl;
double xtot=0.0;
for(int ik=141;ik<241;ik++)
{xtot+=float((trnsbuff[ik]&0xffc00000)>>22);}
xtot=xtot/100.0;
}

int val1[256],val2[256],val3[256];
int ievent=1;

for(int kl=0;kl<256;kl++){val1[kl]=BLK+V_OFFSET;val2[kl]=val1[kl];val3[kl]=val1[kl];}

int start=0;
int next=0;
next = tdac_header(ievent,val1,start,V_OFFSET,UBLK,BLK);

for(int ir=0;ir<16;ir++){
start=next;
int lastdac=773;
next = tdac_roc(lastdac,val1,start,V_OFFSET,UBLK,BLK);

int dbc = rand()%25;
int pixl = rand()%159;
int adc = (rand()%255)<<2;
start=next;
next = tdac_pxlhit(dbc,pixl,adc,val1,start,V_OFFSET,UBLK,BLK);
}

int word=0;
start=next;
next = tdac_trailer(word,val1,start,V_OFFSET,UBLK,BLK);
  *iV_OFFSET=V_OFFSET;
  *iUBLK=UBLK;
  *iBLK=BLK;

for(int kl=0;kl<256;kl++){val2[kl]=val1[kl];val3[kl]=val1[kl];}




fill_dac_vals(val1,val2,val3);
 CVDataWidth dw = cvD32 ; // data width (see CAENVMEtypes.h )
  CVAddressModifier am = cvA32_U_DATA;

  //cout<<"Setting LRES"<<endl;
  
  unsigned int data = 0x80000000; // data for reset bit    
  CVErrorCodes ret = CAENVME_WriteCycle(BHandle,LRES,&data,am,dw);	
//LRES resets pxl state machine, 
//clears the pipeline, 
//flushes FIFoI,II,Err,
//ChOfsett DACS and Temperature fifos
 //CLRES flushes the header and fifoIII  
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write for LRES"<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret);
  } 

  // reset CLRES
  //CLRES flushes the header and fifoIII
  
  //cout<<"Setting CLRES"<<endl;
  
  data = 0x80000000; 
  ret = CAENVME_WriteCycle(BHandle,CLRES,&data,am,dw);
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write for CLRES "<<hex<<CLRES<<" "<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret);
  }

cout<<"Set now to a baseline of 300 and a 16 ROC event, 1hit/Roc"<<endl;
//cout<<"baseline adjustment is on"<<endl;


}

////////////////////////////////////////////////////////////////////////////////////
int spySlink64() 
{//cout<<item<<" "<<hex<<offset<<dec<<" "<<length<<endl;

//drain the spy fifo 3up
//look through the words
//find header and trailer
//check data length
//drain spy fifo 3dn the correct number of words
//form data words

 CVDataWidth dw[1024];   // data width (see CAENVMEtypes.h )
 CVAddressModifier am[1024]; 
 CVErrorCodes errs[1024];
 int cycles=1024;
 unsigned int addrses[1024];
 unsigned int data[1024];
 cout << "FEDBASE: " << FEDBASE << endl;

for(int i=0;i<1024;i++){
dw[i]=cvD32;
am[i]=cvA32_U_DATA;
addrses[i]=LAD_C+0x140000;
}

  CVErrorCodes ret = CAENVME_MultiRead(BHandle,addrses,data,cycles,am,dw,errs);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data[0]<<dec<<endl;
    analyzeError(ret);
                       }

int mwdcnt=-1;

if(((data[0]&0xf0000000)>>28)!=0x5){
  cout<<"Slink Header Messed up!"<<endl;
  cout<<"Will dump the first 50 words in the buffer:"<<endl;
  for(unsigned int i=0;i<50;i++){
    cout<<" data data["<<i<<"]="<<hex<<data[i]<<dec<<endl;
  }
  return mwdcnt;
}


int pwdcnt=0;

//get word count

while((mwdcnt<0)&(pwdcnt<1024)){
if(((data[pwdcnt]&0xf0000000)>>28)!=0xa) {pwdcnt++;} else {mwdcnt=pwdcnt;}
}

if(mwdcnt<0)
{cout<<"No trailer in 1024 words dumping up and down buffers"<<endl;
unsigned int data2[1024];
cycles=1024;
for(int i=0;i<1024;i++){
addrses[i]=LAD_C+0x160000;
}

  ret = CAENVME_MultiRead(BHandle,addrses,data2,cycles,am,dw,errs);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data2[0]<<dec<<endl;
    analyzeError(ret);
                       }
for(int ig=0;ig<1024;ig++)cout<<"0x"<<hex<<data[ig]<<" "<<dec<<ig<<" 0x"<<hex<<data2[ig]<<endl;
return mwdcnt;}

if(((data[mwdcnt]&0x00ffffff)-1)!=(unsigned int) mwdcnt)
{cout<<"Unpacked word count does not match actual";
 cout<<".. data[mwdcnt]="<<dec<<(data[mwdcnt]&0x00ffffff)<<" and mwdcnt="<<(unsigned int)mwdcnt<<endl;
  cout<<"Will dump the first 50 words in the buffer:"<<endl;
  for(unsigned int i=0;i<50;i++){
    cout<<"data["<<i<<"]="<<hex<<data[i]<<dec<<endl;
  }
return -2;}

for(int ij=0;ij<(mwdcnt+1)*2;ij+=2)fif3buff[ij]=data[ij/2];
cycles=mwdcnt+1;
//unpack just as many words as we need
for(int i=0;i<1024;i++){
addrses[i]=LAD_C+0x160000;
}

  ret = CAENVME_MultiRead(BHandle,addrses,data,cycles,am,dw,errs);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data[0]<<dec<<endl;
    analyzeError(ret);
                       }

for(int ij=1;ij<(mwdcnt+1)*2+1;ij+=2)fif3buff[ij]=data[(ij-1)/2];

  return(mwdcnt);
 } //end

int setVMEevntnmbr(const int value) {
  //cout<<"Set VME event number = "<<dec<<value<<endl;
  unsigned int data = 0x100+(value&0xff); // lower 8 bits(7-0) for event number, next bit(8) to write
  unsigned int vmeAddress = LAD_C+0x180000;
 CVDataWidth dw = cvD32 ; // data width (see CAENVMEtypes.h )
  CVAddressModifier am = cvA32_U_DATA;
 
  CVErrorCodes ret = CAENVME_WriteCycle(BHandle,vmeAddress,&data,am,dw);
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data<<dec<<endl;
    analyzeError(ret);
    return -1;
  }
  return 0;
} // end
///////////////////////////////////////////////////////////////////////////
// Decode error FIFO 
void decodeErrorFifo(unsigned int word) {   
// Works for both, the error FIFO and the SLink error words. d.k. 25/04/07

  const unsigned int errorMask	= 0x3e00000;
  const unsigned int timeOut	= 0x3a00000;
  const unsigned int eventNumError  = 0x3e00000;
  const unsigned int trailError	= 0x3c00000;
  const unsigned int fifoError	= 0x3800000;

//  const unsigned int  timeOutChannelMask = 0x1f;  // channel mask for timeouts
  const unsigned int eventNumMask = 0x1fe000; // event number mask
  const unsigned int channelMask = 0xfc000000; // channel num mask
  const unsigned int tbmEventMask = 0xff;    // tbm event num mask
  const unsigned int overflowMask = 0x100;   // data overflow
  const unsigned int tbmStatusMask = 0xff;   //TBM trailer info
  const unsigned int BlkNumMask = 0x700;   //pointer to error fifo #
  const unsigned int FsmErrMask = 0x600;   //pointer to FSM errors
  const unsigned int RocErrMask = 0x800;   //pointer to #Roc errors 
  const unsigned int ChnFifMask = 0x1f;   //channel mask for fifo error 
  const unsigned int Fif2NFMask = 0x40;   //mask for fifo2 NF 
  const unsigned int TrigNFMask = 0x80;   //mask for trigger fifo NF 

 const int offsets[8] = {0,4,9,13,18,22,27,31};


  if(word&0xffffffff){

    cout<<"error word "<<hex<<word<<endl;
  
if( (word&errorMask)==timeOut ) { // TIMEOUT
// More than 1 channel within a group can have a timeout error
    unsigned int index = (word & 0x1F);  // index within a group of 4/5
    unsigned int chip = (word& BlkNumMask)>>8;
      int offset = offsets[chip];
      cout<<"Timeout Error- channels: ";
      for(int i=0;i<5;i++) {
	if( (index & 0x1) != 0) {
	  int channel = offset + i + 1;
	  cout<<dec<<channel<<" ";
	}
	index = index >> 1;
      }
//end of timeout  chip and channel decoding
      
    } else if( (word&errorMask) == eventNumError ) { // EVENT NUMBER ERROR
      unsigned int channel =  (word & channelMask) >>26;
      unsigned int tbm_event   =  (word & tbmEventMask);

      cout<<"Event Number Error- channel: "<<dec<<channel<<" tbm event nr. "
	  <<dec<<tbm_event;

    } else if( ((word&errorMask) == trailError)) {
      unsigned int channel =  (word & channelMask) >>26;
      unsigned int tbm_status   =  (word & tbmStatusMask);
if(word & RocErrMask)cout<<"Number of Rocs Error- "<<"channel: "<<dec<<channel<<" ";
if(word & FsmErrMask)cout<<"Finite State Machine Error- "<<"channel: "<<dec<<channel<<
" Error status:0x"<<hex<< ((word & FsmErrMask)>>9)<<dec<<" ";;
if(word & overflowMask)cout<<"Overflow Error- "<<"channel: "<<dec<<channel<<" ";
if(!((word & RocErrMask)|(word & FsmErrMask)|(word & overflowMask))) cout<<"Trailer Error- ";
cout<<"channel: "<<dec<<channel<<" TBM status:0x"<<hex<<tbm_status<<dec<<" ";
      
    } else if((word&errorMask)==fifoError) {
if(word & Fif2NFMask)cout<<"A fifo 2 is Nearly full- ";
if(word & TrigNFMask)cout<<"The trigger fifo is nearly Full - ";
if(word & ChnFifMask)cout<<"fifo-1 is nearly full for channel"<<(word & ChnFifMask);

    }
      unsigned int event   =  (word & eventNumMask) >>13;
      //unsigned int tbm_status   =  (word & tbmStatusMask);

       if(event>0)cout<<":event: "<<dec<<event;
       cout<<endl;
    
    
  }
  
}
//////////////////////////////////////////////////////////////////////////////
void decodeSpyDataFifo(unsigned int word) {  
  const bool ignoreInvalidData=false;
  if(word&0xfffffff){ 
    const unsigned int plsmsk = 0xff;
    const unsigned int pxlmsk = 0xff00;
    const unsigned int dclmsk = 0x1f0000;
    const unsigned int rocmsk = 0x3e00000;
    const unsigned int chnlmsk = 0xfc000000;
    unsigned int chan= ((word&chnlmsk)>>26);
    unsigned int roc= ((word&rocmsk)>>21);

// Check for embeded special words
if(roc>25){

if((word&0xffffffff)==0xffffffff) {cout<<" fifo-2 End of Event word"<<endl;
} else if (roc==26) {cout<<"Gap word"<<endl;
} else if (roc==27) {cout<<"Dummy Data Word"<<endl;
} else {decodeErrorFifo(word);} 

} else if(chan>0 && chan<37) {
      //cout<<hex<<word<<dec;
      cout<<" Chnl- "<<dec<<chan;
      cout<<" ROC- "<<dec<<((word&rocmsk)>>21);
      cout<<" DCOL- "<<dec<<((word&dclmsk)>>16);
      cout<<" Pixel- "<<dec<<((word&pxlmsk)>>8);
      cout<<" ADC- "<<dec<<(word&plsmsk)<<endl;
    } else {
     if(!ignoreInvalidData) cout<<" Invalid channel, possible Fifo-2 event count "<<dec<<(word&0xffffff)<<" "<<hex<<(word&0xffffffff)<<endl;
    }
  } else {
    if(!ignoreInvalidData)cout<<" Possible Fifo-2 Begin of Event, data = "<<hex<<word<<endl;       
  }
} // end
/////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void SpyDataFifoInfo(unsigned int word,int *chn,int *rc,int *dc,int *pxl,int *adc) {  
  //const bool ignoreInvalidData=false;
  if(word&0xfffffff){ 
    const unsigned int plsmsk = 0xff;
    const unsigned int pxlmsk = 0xff00;
    const unsigned int dclmsk = 0x1f0000;
    const unsigned int rocmsk = 0x3e00000;
    const unsigned int chnlmsk = 0xfc000000;
    unsigned int chan= ((word&chnlmsk)>>26);
    unsigned int roc= ((word&rocmsk)>>21);

*chn=40;
*rc=roc;

// Check for embeded special words
if(roc>25){
*chn=chan;
*rc=roc;
} else if(chan>0 && chan<37) {
      *chn=chan;
      *rc=roc;
      *dc=((word&dclmsk)>>16);
      *pxl=((word&pxlmsk)>>8);
      *adc=(word&plsmsk);
    } 
  } 
} // end
/////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//Poisson Random Number generator with mean a
//////////////////////////////////////////////////////////////////////////
int mypoiss(const double a)
{

int i;
double u,p,f;
i = 0;
f = p = exp(-a);
u = fabs((double)rand()/(double)(RAND_MAX+1));
//cout<<" "<<dec<<u<<endl;
while(f <= u)
{
p *= (a / (i + 1.0));
f += p;
i++;
}
return i;

}
//////////////////////////////////////////////////////////////////////////
void printfifostatus(unsigned int word)
{
/*
[0] .. AllmostFull FIFO I  North ( all 9 InputChannels ored )
[1] .. NearlyFull FIFO II  North ( or of both FIFO II )
[2] .. AllmostFull FIFO I  NorthCenter
[3] .. NearlyFull FIFO II  NorthCenter
[4] .. AllmostFull FIFO I  SouthCenter
[5] .. NearlyFull FIFO II  SouthCenter
[6] .. AllmostFull FIFO I  South
[7] .. NearlyFull FIFO II  South
[8] .. AllmostFull FIFO III UP
[9] .. ALLmostFUll FIFO III DOWN
*/
unsigned int mark=1;
for(int ij=0;ij<10;ij++)
{if((mark&word)>0){
if(ij==0)cout<<"[0] .. AllmostFull FIFO I  North ( all 9 InputChannels ored )"<<endl;
if(ij==1)cout<<"[1] .. NearlyFull FIFO II  North ( or of both FIFO II )      "<<endl;
if(ij==2)cout<<"[2] .. AllmostFull FIFO I  NorthCenter			     "<<endl;
if(ij==3)cout<<"[3] .. NearlyFull FIFO II  NorthCenter			     "<<endl;
if(ij==4)cout<<"[4] .. AllmostFull FIFO I  SouthCenter			     "<<endl;
if(ij==5)cout<<"[5] .. NearlyFull FIFO II  SouthCenter			     "<<endl;
if(ij==6)cout<<"[6] .. AllmostFull FIFO I  South			     "<<endl;
if(ij==7)cout<<"[7] .. NearlyFull FIFO II  South			     "<<endl;
if(ij==8)cout<<"[8] .. AllmostFull FIFO III UP				     "<<endl;
if(ij==9)cout<<"[9] .. ALLmostFUll FIFO III DOWN			     "<<endl;
}
mark=mark<<1;
}
}
void decodeTTSFifo(unsigned int word) {  

int itts=word&0xf;
cout<<" TTS Status = 0x"<<hex<<itts;
cout<<" TTS Word = 0x"<<hex<<word<<" ";

switch (itts)
{
	  case 0://0000
	cout<<"**TTS**Disconnected    "<<" TTS Event = "<<dec<<((word&0xffffff0)>>4)<<endl;
	break;
	  case 1://0001
	cout<<"**TTS**Overflow Warning"<<" TTS Event = "<<dec<<((word&0xffffff0)>>4)<<endl;
	break;
	  case 2://0010
	cout<<"**TTS**Out of Sync     "<<" TTS Event = "<<dec<<((word&0xffffff0)>>4)<<endl;
	break;
	  case 3://0011
	cout<<"**TTS**Undefined       "<<" TTS Event = "<<dec<<((word&0xffffff0)>>4)<<endl;
	break;
	  case 4://0100
	cout<<"**TTS**Busy            "<<" TTS Event = "<<dec<<((word&0xffffff0)>>4)<<endl;
	break;
	  case 5://0101
	cout<<"**TTS**Undefined       "<<" TTS Event = "<<dec<<((word&0xffffff0)>>4)<<endl;
	break;
	  case 6://0110
	cout<<"**TTS**Undefined       "<<" TTS Event = "<<dec<<((word&0xffffff0)>>4)<<endl;
	break;
	  case 7://0111
	cout<<"**TTS**Undefined       "<<" TTS Event = "<<dec<<((word&0xffffff0)>>4)<<endl;
	break;
	  case 8://1000
	cout<<"**TTS**Ready           "<<" TTS Event = "<<dec<<((word&0xffffff0)>>4)<<endl;
	break;
	  case 9://1001
	cout<<"**TTS**Undefined       "<<" TTS Event = "<<dec<<((word&0xffffff0)>>4)<<endl;
	break;
	  case 10://1010
	cout<<"**TTS**Undefined       "<<" TTS Event = "<<dec<<((word&0xffffff0)>>4)<<endl;
	break;
	  case 11://1011
	cout<<"**TTS**Undefined       "<<" TTS Event = "<<dec<<((word&0xffffff0)>>4)<<endl;
	break;
	  case 12://1100
	cout<<"**TTS**Error           "<<" TTS Event = "<<dec<<((word&0xffffff0)>>4)<<endl;
	break;
	  case 13://1101
	cout<<"**TTS**Undefined       "<<" TTS Event = "<<dec<<((word&0xffffff0)>>4)<<endl;
	break;
	  case 14://1110
	cout<<"**TTS**Undefined       "<<" TTS Event = "<<dec<<((word&0xffffff0)>>4)<<endl;
	break;
	  case 15://1111
	cout<<"**TTS**Disconnected    "<<" TTS Event = "<<dec<<((word&0xffffff0)>>4)<<endl;
	break;
}	  



} // end
///////////////////////////////////////////////////////////////////////////
// Decode the FIFO-1 data in normal mode 
// ADD SIZE
void printNormalFifo1Data(unsigned int * data, const int length) {
  //const bool DECODE = false;
  cout<<" Decode FIFO1 normal data "<<length<<endl;
  unsigned int olddata=0;
  unsigned int newdata=0;
  int count=0;
  bool trailer=true;
  int channel = 0;
  static unsigned int lastValue[37] = {0};

  // Print & analyze the data buffer
  for(int icx=0;icx<length;icx++) {   // add size as an argument    
    newdata=data[icx];
    if(icx>0 && newdata==olddata) {
      count++;
      cout<<icx<<" raw "<<hex<<newdata<<dec<<" same data "<<count<<endl;
      //if(count>10) break;  // why?, I am not sure. There seem to be 5 same lines beween channels.
      lastValue[channel]=newdata;

    } else {  // new, valid data

      count=0;
      olddata = newdata;
      channel = (newdata & 0xfc000000) >> 26;
      cout<<icx<<" raw "<<hex<<newdata<<dec;

      if(trailer && newdata == lastValue[channel]) {
	cout<<" leftover from previous event "<<endl;
	continue;
      }

      if(channel>0 && channel<37) { // valid channel
	  int roc  = (newdata & 0x3e00000)>>21;
      
	if(roc==30) { // this is a tbm trailer, or error trailer 
	  unsigned int status = newdata & 0xff;
	  unsigned int rocerr = newdata & 0x800;
	  unsigned int fsmerr = (newdata & 0x600)>>8;
	  unsigned int ovferr = newdata & 0x100;
	  if(ovferr>0) cout<<" Too many words-  ";
	  if(rocerr>0) cout<<" Wrong number of ROCS-  ";
	  if(fsmerr>0) cout<<" State Machine error # "<<fsmerr<<"- ";
	    cout<<" TBM Trailer (event status) = "<<hex<<status<<" Channel "<<dec<<channel<<endl;
	    trailer=true;
	  } else if(roc==31) {
	   unsigned int status = newdata & 0xff;
	    cout<<" TBM Header (event number) = "<<dec<<status<<" Channel "<<dec<<channel<<endl;
	    trailer=false;
	} else if( roc>0 && ((newdata&0x1fff00) == 0x0) ) { // this is a last dac
	  int roc = (newdata & 0x3e00000)>>21;
	  int dac = (newdata & 0xff);
	  cout<<" Last dac for roc "<<roc<<" = "<<dac<<endl;
	} else {
	  int dcol = (newdata & 0x1f0000)>>16;
	  int pix  = (newdata & 0xff00)>>8;
	  int ana  = (newdata & 0xff);
	  cout<<" Channel "<<dec<<channel<<" roc/dcol/pix "<<roc<<" "<<dcol<<" "<<pix
	      <<" ana "<<ana<<hex<<"/"<<ana<<dec<<endl;
	}
      } else {
	cout <<" error, invalid channel number "<<channel<<endl; 
      } // if valid channel 
    } // if new data 
  } // for loop

  
} // end
////////////////////////////////////////////////////////////////////////////
void SetTdacErr(int ErrCon,int myevn,int trailerwd,unsigned int V_OFFSET,unsigned int UBLK,unsigned int BLK)
{
//puts error conditions in the test dac
//Choose idividual Tdacs channels by setting
//0x1 = dac 1
//0x100 = dac 2 (8 bits over)
//0x10000 = dac 3 (16 bits over)
//ErrCon = 0 Standard event
//ErrCon = 0x1 State Machine error 1 - expect timeout
//ErrCon = 0x2 State Machine error 2 - expect ok event with trailer error
//ErrCon = 0x4 State Machine error 3 - expect ok event with trailer error
// note errors 2 and 3 in same event will result in error 3
//ErrCon = 0x8 missing roc
//ErrCon = 0x10 extra roc
//ErrCon = 0x20 event number +1
//ErrCon = 0x40 non-zero trailer, word in ErrCon (27-31) 0xf0000000
//ErrCon = 0x80 event too long (missing trailer)
//not implemented ErrCon = 0x100 extra pixel word

int val1[256],val2[256],val3[256];


for(int kl=0;kl<256;kl++){val1[kl]=BLK+V_OFFSET;val2[kl]=val1[kl];val3[kl]=val1[kl];}
int start;
int next;

for(int idac=0;idac<3;idac++){

start=2;
next=0;
if(ErrCon&0x20){if(idac==0)next = tdac_header(myevn+1,val1,start,V_OFFSET,UBLK,BLK);
                          }else{if(idac==0)next = tdac_header(myevn,val1,start,V_OFFSET,UBLK,BLK);}
if(((ErrCon&(0x20<<8)))){if(idac==1)next = tdac_header(myevn+1,val2,start,V_OFFSET,UBLK,BLK);
                          }else{if(idac==1)next = tdac_header(myevn,val2,start,V_OFFSET,UBLK,BLK);}
if(((ErrCon&(0x20<<16)))){if(idac==2)next = tdac_header(myevn+1,val3,start,V_OFFSET,UBLK,BLK);
                          }else{if(idac==2)next = tdac_header(myevn,val3,start,V_OFFSET,UBLK,BLK);}


if((idac==0)&&(ErrCon&0x1)){val1[next]=V_OFFSET+BLK;next++;}
if((idac==1)&&(ErrCon&(0x1<<8))){val2[next]=V_OFFSET+BLK;next++;}
if((idac==2)&&(ErrCon&(0x1<<16))){val3[next]=V_OFFSET+BLK;next++;}

int ihit=0;
start=next;
int nrocks=16;
//cout<<dec<<start<<endl;
if((ErrCon&0x8)&&(idac==0))nrocks=nrocks-1;
if((ErrCon&(0x8<<8))&&(idac==1))nrocks=nrocks-1;
if((ErrCon&(0x8<<16))&&(idac==2))nrocks=nrocks-1;
if((ErrCon&(0x10))&&(idac==0))nrocks=nrocks+1;
if((ErrCon&(0x10<<8))&&(idac==1))nrocks=nrocks+1;
if((ErrCon&(0x10<<16))&&(idac==2))nrocks=nrocks+1;
//cout<<"nrocks"<<dec<<nrocks<<endl;
for(int ir=0;ir<nrocks;ir++){
start=next;
int lastdac=250+rand()%773;
if(idac==0)next = tdac_roc(lastdac,val1,start,V_OFFSET,UBLK,BLK);
if(idac==1)next = tdac_roc(lastdac,val2,start,V_OFFSET,UBLK,BLK);
if(idac==2)next = tdac_roc(lastdac,val3,start,V_OFFSET,UBLK,BLK);

int ixtra=mypoiss(0.86);
for (int iht=0;iht<ixtra;iht++) {
int dbc = rand()%25;
int pixl = rand()%159;
int adc=(rand()%255)<<2;
//if(ir==15) adc = 999;//(rand()%255)<<2;
start=next;
if(idac==0)next = tdac_pxlhit(dbc,pixl,adc,val1,start,V_OFFSET,UBLK,BLK);
if(idac==1)next = tdac_pxlhit(dbc,pixl,adc,val2,start,V_OFFSET,UBLK,BLK);
if(idac==2)next = tdac_pxlhit(dbc,pixl,adc,val3,start,V_OFFSET,UBLK,BLK);

ihit++;
}
}
if(ihit>28){start=10;
if((idac==0)&&(ErrCon&0x1))start=11;
if((idac==1)&&(ErrCon&(0x1<<8)))start=11;
if((idac==2)&&(ErrCon&(0x1<<16)))start=11;
cout<<"too many hits...compensating"<<endl;
for(int ir=0;ir<nrocks;ir++){
int lastdac=250+rand()%773;
if(idac==0)next = tdac_roc(lastdac,val1,start,V_OFFSET,UBLK,BLK);
if(idac==1)next = tdac_roc(lastdac,val2,start,V_OFFSET,UBLK,BLK);
if(idac==2)next = tdac_roc(lastdac,val3,start,V_OFFSET,UBLK,BLK);

int ixtra=1;
for (int iht=0;iht<ixtra;iht++) {
int dbc = rand()%25;
int pixl = rand()%159;
int adc=(rand()%255)<<2;
//if(ir==15) adc = 999;//(rand()%255)<<2;
start=next;
if(idac==0)next = tdac_pxlhit(dbc,pixl,adc,val1,start,V_OFFSET,UBLK,BLK);
if(idac==1)next = tdac_pxlhit(dbc,pixl,adc,val2,start,V_OFFSET,UBLK,BLK);
if(idac==2)next = tdac_pxlhit(dbc,pixl,adc,val3,start,V_OFFSET,UBLK,BLK);

ihit++;
}
}
}


int word=trailerwd;
start=next;
if(ErrCon&0x40)if(idac==0)word=(ErrCon&0xff000000)>>24;
if(ErrCon&(0x40<<8))if(idac==1)word=(ErrCon&0xff000000)>>24;
if(ErrCon&(0x40<<16))if(idac==2)word=(ErrCon&0xff000000)>>24;
if(ErrCon&0x80){}else{if(idac==0)next = tdac_trailer(word,val1,start,V_OFFSET,UBLK,BLK);}
if(ErrCon&(0x80<<8)){}else{if(idac==1)next = tdac_trailer(word,val2,start,V_OFFSET,UBLK,BLK);}
if(ErrCon&(0x80<<16)){}else{if(idac==2)next = tdac_trailer(word,val3,start,V_OFFSET,UBLK,BLK);}

if((idac==0)&&(ErrCon&0x4)){val1[next-5]= 1023;}
if((idac==1)&&(ErrCon&(0x4<<8))){val2[next-5]= 1023;}
if((idac==2)&&(ErrCon&(0x4<<16))){val3[next-5]= 1023;}

if((idac==0)&&(ErrCon&0x2)){val1[next-6]= 1023;}
if((idac==1)&&(ErrCon&(0x2<<8))){val2[next-6]= 1023;}
if((idac==2)&&(ErrCon&(0x2<<16))){val3[next-6]= 1023;}





}//idac
//if((ErrCon&(0x10)))for(int iy=0;iy<256;iy++)cout<<dec<<val1[iy]<<endl;
//if((ErrCon&(0x10<<8)))for(int iy=0;iy<256;iy++)cout<<dec<<val2[iy]<<endl;
//if((ErrCon&(0x10<<16)))for(int iy=0;iy<256;iy++)cout<<dec<<val3[iy]<<endl;

fill_dac_vals(val1,val2,val3);
}
//////////////////////////////////////////////////////////////////////////////
void CheckErrors(unsigned int word,int errorbuff[][9])
{
// check fifo3, fifo2, error fifos for effor info, load array with ch,type #
// error type 0 SM 1
// error type 1 SM 2
// error type 2 SM 3
// error type 3 ROC#
// error type 4 EVNum
// error type 5 trailer error
// error type 6 too many hits
// error type 7 timeout
// error type 8 fifo 1 nearly full
// Decode error FIFO 

  const unsigned int errorMask	= 0x3e00000;
  const unsigned int timeOut	= 0x3a00000;
  const unsigned int eventNumError  = 0x3e00000;
  const unsigned int trailError	= 0x3c00000;
  const unsigned int fifoError	= 0x3800000;

//  const unsigned int  timeOutChannelMask = 0x1f;  // channel mask for timeouts
//  const unsigned int eventNumMask = 0x1fe000; // event number mask
  const unsigned int channelMask = 0xfc000000; // channel num mask
//  const unsigned int tbmEventMask = 0xff;    // tbm event num mask
  const unsigned int overflowMask = 0x100;   // data overflow
  const unsigned int tbmStatusMask = 0xff;   //TBM trailer info
  const unsigned int BlkNumMask = 0x700;   //pointer to error fifo #
  const unsigned int FsmErrMask = 0x600;   //pointer to FSM errors
  const unsigned int RocErrMask = 0x800;   //pointer to #Roc errors 
  const unsigned int ChnFifMask = 0x1f;   //channel mask for fifo error 
  const unsigned int Fif2NFMask = 0x40;   //mask for fifo2 NF 
  const unsigned int TrigNFMask = 0x80;   //mask for trigger fifo NF 

 const int offsets[8] = {0,4,9,13,18,22,27,31};



  if(word&0xffffffff){

//    cout<<"error word "<<hex<<word<<endl;
  
if( (word&errorMask)==timeOut ) { // TIMEOUT
// More than 1 channel within a group can have a timeout error
    unsigned int index = (word & 0x1F);  // index within a group of 4/5
    unsigned int chip = (word& BlkNumMask)>>8;
      int offset = offsets[chip];
//      cout<<"Timeout Error- channel: ";
      for(int i=0;i<5;i++) {
	if( (index & 0x1) != 0) {
	  int channel = offset + i + 1;
	  errorbuff[channel-1][7]++;//cout<<dec<<channel<<" ";
	}
	index = index >> 1;
      }
//end of timeout  chip and channel decoding
      
    } else if( (word&errorMask) == eventNumError ) { // EVENT NUMBER ERROR
      unsigned int channel =  (word & channelMask) >>26;
      //unsigned int tbm_event   =  (word & tbmEventMask);

      errorbuff[channel-1][4]++;//cout<<"Event Number Error- channel: "<<dec<<channel<<" tbm event nr. "<<dec<<tbm_event;

    } else if( ((word&errorMask) == trailError)) {
      unsigned int channel =  (word & channelMask) >>26;
      unsigned int tbm_status   =  (word & tbmStatusMask);
if(word & RocErrMask)errorbuff[channel-1][3]++;//cout<<"Number of Rocs Error- "<<"channel: "<<dec<<channel<<" ";
if(word & FsmErrMask)errorbuff[channel-1][((word & FsmErrMask)>>9)-1]++;
//cout<<"Finite State Machine Error- "<<"channel: "<<dec<<channel<<
//" Error status:0x"<<hex<< ((word & FsmErrMask)>>9)<<dec<<" ";;
if(word & overflowMask)errorbuff[channel-1][6]++;//cout<<"Overflow Error- "<<"channel: "<<dec<<channel<<" ";
if(!((word & RocErrMask)|(word & FsmErrMask)|(word & overflowMask))){errorbuff[channel-1][5]++;}
else { if(tbm_status>0)errorbuff[channel-1][5]++;} 
//cout<<"Trailer Error- ";cout<<"channel: "<<dec<<channel<<" TBM status:0x"<<hex<<tbm_status<<dec<<" ";
      
    } else if((word&errorMask)==fifoError) {
if(word & Fif2NFMask)cout<<"A fifo 2 is Nearly full- ";
if(word & TrigNFMask)cout<<"The trigger fifo is nearly Full - ";
if(word & ChnFifMask)errorbuff[(word & ChnFifMask)-1][8]++;
//cout<<"fifo-1 is nearly full for channel"<<(word & ChnFifMask);

    }
      //unsigned int event   =  (word & eventNumMask) >>13;
      //unsigned int tbm_status   =  (word & tbmStatusMask);

//       if(event>0)cout<<":event: "<<dec<<event;
 //      cout<<endl;
    
    
  }
  
}

void getHistoTrip(int trip,int histobuff[][32])
{CVErrorCodes ret;
 CVDataWidth dw[32*32];   // data width (see CAENVMEtypes.h )
 CVAddressModifier am[32*32]; 
 CVErrorCodes errs[32*32];
 int cycles=32*32;
 unsigned int addrses[32*32];
 unsigned int data[32*32];

for(int i=0;i<(32*32);i++){
dw[i]=cvD32;
am[i]=cvA32_U_DATA;}



if(trip<7){ //looking at channels 1-18

//select tripplet
unsigned int VMEAddress=LAD_C+0x30000;
       unsigned int word=trip;
       ret = CAENVME_WriteCycle(BHandle,VMEAddress,&word,am[0],dw[0]);
       if(ret != cvSuccess) {  // Error
	 cout<<"Error in Selecting histogram tripple "<<hex<<ret<<" "<<dec<<word<<endl;
	 analyzeError(ret);
       }


for(int i=0;i<32;i++){//loop over ROCs
for(int j=0;j<32;j++){//loop over DC's

addrses[i*32+j]=LAD_C+0x40000+(i+1)*4;

}}

  ret = CAENVME_MultiRead(BHandle,addrses,data,cycles,am,dw,errs);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in read "<<hex<<ret<<" "<<data[0]<<dec<<endl;
    analyzeError(ret);}
 
for(int i=0;i<32;i++){histobuff[trip-1][i]=data[i];} 

for(int i=0;i<32;i++){//loop over ROCs
for(int j=0;j<32;j++){//loop over DC's

histobuff[i][j]=data[i*32+j];

}}
 
} else {  //looking at channels 19-36
unsigned int VMEAddress=LAD_C+0x38000;
       unsigned int word=trip-6;
       ret = CAENVME_WriteCycle(BHandle,VMEAddress,&word,am[0],dw[0]);
       if(ret != cvSuccess) {  // Error
	 cout<<"Error in Selecting histogram tripple "<<hex<<ret<<" "<<dec<<word<<endl;
	 analyzeError(ret);
       }

for(int i=0;i<32;i++){//loop over ROCs
for(int j=0;j<32;j++){//loop over DC's

addrses[i*32+j]=LAD_C+0x48000+(i+1)*4;

}}

       ret = CAENVME_MultiRead(BHandle,addrses,data,cycles,am,dw,errs);	
       if(ret != cvSuccess) {  // Error
	 cout<<"Error in read "<<hex<<ret<<" "<<dec<<data[0]<<endl;
	 analyzeError(ret);
       }

for(int i=0;i<32;i++){//loop over ROCs
for(int j=0;j<32;j++){//loop over DC's

histobuff[i][j]=data[i*32+j];

}}




}     
     


}
////////////////////////////////////////////////////////////////////////////////////
int dumpSlink64() 
{//cout<<item<<" "<<hex<<offset<<dec<<" "<<length<<endl;

//drain the spy fifo 3up
//look through the words
//find header and trailer
//check data length
//drain spy fifo 3dn the correct number of words
//form data words

 CVDataWidth dw[1024];   // data width (see CAENVMEtypes.h )
 CVAddressModifier am[1024]; 
 CVErrorCodes errs[1024];
 int cycles=1024;
 unsigned int addrses[1024];
 unsigned int data[1024];

for(int i=0;i<1024;i++){
dw[i]=cvD32;
am[i]=cvA32_U_DATA;
addrses[i]=LAD_C+0x140000;
}

  CVErrorCodes ret = CAENVME_MultiRead(BHandle,addrses,data,cycles,am,dw,errs);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data[0]<<dec<<endl;
    analyzeError(ret);
                       }

int mwdcnt=-1;

if(((data[0]&0xf0000000)>>28)!=0x5){
  cout<<"Slink Header Messed up!"<<endl;
  cout<<"Will dump the first 50 words in the buffer:"<<endl;
  for(unsigned int i=0;i<50;i++){
    cout<<" data data["<<i<<"]="<<hex<<data[i]<<dec<<endl;
  }
  return mwdcnt;
}
cout<<"0x"<<hex<<data[0]<<"Header for event = "<<dec<<(data[0]&0xffffff)<<endl;

int pwdcnt=0;

//get word count

while((mwdcnt<0)&(pwdcnt<1024)){
if(((data[pwdcnt]&0xf0000000)>>28)!=0xa) {pwdcnt++;} else {mwdcnt=pwdcnt;}
}

if(mwdcnt<0)
{cout<<"No trailer in 1024 words"<<endl;return mwdcnt;}
cout<<"0x"<<hex<<data[mwdcnt]<<"Trailer for event = "<<dec<<(data[0]&0xffffff)<<endl;

if(((data[mwdcnt]&0x00ffffff)-1)!=(unsigned int) mwdcnt)
{cout<<"Unpacked word count does not match actual";
 cout<<".. data[mwdcnt]="<<dec<<(data[mwdcnt]&0x00ffffff)<<" and mwdcnt="<<(unsigned int)mwdcnt<<endl;
  cout<<"Will dump the first 50 words in the buffer:"<<endl;
  for(unsigned int i=0;i<50;i++){
    cout<<"data["<<i<<"]="<<hex<<data[i]<<dec<<endl;
  }
return -2;}

for(int ij=0;ij<(mwdcnt+1)*2;ij+=2)fif3buff[ij]=data[ij/2];
cycles=mwdcnt+1;
//unpack just as many words as we need
for(int i=0;i<1024;i++){
addrses[i]=LAD_C+0x160000;
}

  ret = CAENVME_MultiRead(BHandle,addrses,data,cycles,am,dw,errs);	
  if(ret != cvSuccess) {  // Error
    cout<<"Error in write "<<hex<<ret<<" "<<data[0]<<dec<<endl;
    analyzeError(ret);
                       }

for(int ij=1;ij<(mwdcnt+1)*2+1;ij+=2)fif3buff[ij]=data[(ij-1)/2];

  return(mwdcnt);
 } //end

///////////////////////////////////////////////////////////////////////////
