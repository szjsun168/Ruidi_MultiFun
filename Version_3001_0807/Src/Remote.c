/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : Remote.c
* Description        : Main program body.
********************************************************************************/

#include "Remote.h"
#include "tim.h"
#include "eeprom.h"
#include "adc.h"
#include "Wwdg.h"
#include "stm32f0xx_ll_tim.h"

/********************************* ver2 variable begin*************************/

uint16_t ReceiveBuf[REBUF_LEN]; //接收缓存
FlagStatus Flag_EXTIInit = RESET ; 		//外部中断标志位

uint16_t Head = 0 ;    
uint16_t Tail = 0 ;		
uint16_t RBufEnd = 0 ;
uint8_t NumCnt_Preem=0;
int8_t NumCnt_Preem1 = 0;
int8_t CloseAlwaysAction=0;
int8_t OpenAlwaysAction=0;


/********************************ver2 variable end*****************************/

/********************************* ver1 variable begin*************************/
typedef union
{
  uint16_t WORD[18];
  struct
  {
    uint16_t speed_set;  //0
    uint16_t open_time_set; 
    uint16_t error_15V_set;
    uint16_t error_hall_set;
    uint16_t error_el_oc_set;//电磁锁过流
    uint16_t error_el_set; //5 无反馈信号
    uint16_t error_OC_set;
    uint16_t error_OL_set;
    uint16_t error_bus_ov_set;
    uint16_t error_bus_uv_set;
    uint16_t error_eeprom_set;//10
    uint16_t error1_set;
    uint16_t error2_set;
    uint16_t error3_set;
    uint16_t error4_set;
    uint16_t error5_set;//15
    uint16_t run_times;//door_width_set;
    uint16_t check_set;
  }STR;
}Para_Set_TypeDef;


TBFlg_TypeDef  TBFlg;
TBVar_TypeDef  TBVar;


#define MAX_USER        8         // max number of TX that can be learned
#define EL_SIZE    	8         // single record size in bytes

#define BUTTENCODE  (Buffer[3]&0xF0)
uint8_t button_arr[100];
uint8_t idx11;
extern uint16_t Runtimes;
uint32_t ovi4_cnt;
uint32_t interval;
//////////////////////////////////////////////////////////
#define FUNC_NUM_MAX 2
#define EE_ADD_START 0x80
#define PARA_NUM_MAX 18 
const uint8_t para_max[FUNC_NUM_MAX]={3,10};
Para_Set_TypeDef Para;
uint16_t arr17[PARA_NUM_MAX];
extern uint16_t OpenWaitTime;
void TIM4_Cal(void);
const uint16_t data_ini[18]=
{
  1,
  3,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0x5050
};

/////////////////////////////////////////////////////////
//----------------- RF STATE flags  ------------------------------------
bool FHopOK;     // Hopping code verified OK//
bool FSame;      // Same code as previous
bool FLearn;     // LEARN mode active
bool F2Chance;   // Resync required
bool RFFull;     // buffer full
//---------------------OUTPUT flags-----------------------------------------------
uint8_t CLearn, CTLearn;       // learn timers and counter
uint8_t CFlash, CTFlash;       // led flashing timer and counter
//----------------------key check-------------------------
uint8_t FCode;      // function codes and upper nibble of serial number
uint8_t Ind;        // address pointer to record in mem.
uint16_t Dato;      // temp storage for read and write to mem.
uint16_t Hop;       // hopping code sync counter
uint16_t EHop;     // last value of sync counter (from EEPROM)
int16_t ETemp;   // second copy of sync counter
//----------------------descypt-------------------------
uint32_t mpik,mpin;            // decryption key
uint32_t LMNUM,MMNUM;          //factory code L and M 32bit
uint32_t SENUM;                //serial number
uint32_t RFdata_GD,RFdata_TM;  //GD AND TM CODE
uint16_t TMCount;              // 16 bit extended timer 
uint8_t B[9];                 // receive code
uint8_t Buffer[9];            // receive buffer 
uint8_t RFstate;              // receiver state
uint8_t Bptr;                 // receive buffer pointer
uint8_t BitCount;             // received bits counter
uint8_t SEED[4];              // seed value = serial number
uint16_t NextHop;              // resync value for 2 Chance

//---------------exti it   ----------- ---------------------
uint16_t Tim_Count_Num = 0;
uint16_t Tim_Count_Num1 = 0;
uint16_t Tim_Count_Num2 = 0;
uint16_t Hlevalue = 0;
uint16_t Llevalue = 0;
uint8_t  HlevalueCount = 0;
uint8_t  LlevalueCount = 0;
uint8_t  PreemCount = 0;
int8_t  PreemCount1 = 0;
//--------------------Learn State Flags------------------------------------
uint16_t FLearnCount = 0;
uint8_t  LEARNMode = 1;
uint8_t  LEARNMode_State = 0;
uint16_t COut;
uint8_t RF_Out1;
uint8_t RF_Out2;
uint8_t RF_Out3;
uint8_t RF_Out4;
uint8_t RF_Out5;
uint8_t RF_Out6;
uint8_t Vlow;
uint8_t func_num=1;
uint8_t para_num=1;
//--------------------------------------------------------
uint8_t I2C1_Buffer_Tx[16];
uint8_t I2C1_Buffer_Rx[16];
uint8_t Er_ROM=0;
uint8_t RF_Cmd_Open=0;
uint16_t TB_RF_Quit=0;
uint16_t TB_RF_Set=0;
uint8_t flgTB_Quit_En=0;
uint8_t RF_Auto;
uint8_t RF_OpenAlways=0;//S0
uint8_t RF_CloseAlways=0;//S4
uint16_t Tim1_ov_cnt=0;

typedef union
{ 
  uint8_t Byte;
  struct 
  {
    uint8_t unused:4;

    uint8_t _S3:1;
    uint8_t _S0:1;
    uint8_t _S1:1;
    uint8_t _S2:1;   
    
  }Bit;
}btn_TypeDef;
btn_TypeDef button;

uint8_t Cnt_500ms,Cnt_200ms;
bool flgBeep_En=FALSE;
bool BeepOn;
uint16_t CNT_Beep_On; 
uint16_t CNT_Beep_Off;


void Beep_On(void)
{
   HAL_GPIO_WritePin(oPWM_BUZ_GPIO_Port,oPWM_BUZ_Pin,GPIO_PIN_SET);
}
void Beep_Off(void)
{
   HAL_GPIO_WritePin(oPWM_BUZ_GPIO_Port,oPWM_BUZ_Pin,GPIO_PIN_RESET);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
       if( Tim1_ov_cnt != 0xffff){
   	    Tim1_ov_cnt++;
       }
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{ 
    if(Flag_EXTIInit == SET) //外部中断初始化后配置上升下降沿双向中断
    {
      Flag_EXTIInit = RESET ;
      LL_TIM_IC_SetPolarity(TIM1,LL_TIM_CHANNEL_CH1,LL_TIM_IC_POLARITY_FALLING);/*允许下降沿触发中断 */
    }
    else
    {
      Flag_EXTIInit = SET ;
      LL_TIM_IC_SetPolarity(TIM1,LL_TIM_CHANNEL_CH1,LL_TIM_IC_POLARITY_RISING);  /* 允许上升沿触发中断*/
    }
    if(Tim1_ov_cnt==0){
              ReceiveBuf[Tail]  = LL_TIM_IC_GetCaptureCH1(TIM1)	;
    	}else{
        	 ReceiveBuf[Tail] =0xffff;
		 Tim1_ov_cnt=0;	 
    }
		
    Tail = (Tail+1) % REBUF_LEN ;
}


void Beep_Set(uint8_t Cnt_500,uint8_t Cnt_200 )
{
  Cnt_500ms=Cnt_500;
  Cnt_200ms=Cnt_200;
  CNT_Beep_On=0;
  
}
void Beep_Cmd(FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    flgBeep_En=TRUE;
    BeepOn=TRUE;
  }
}
    
// the value as below is based on the timebase of 5ms
void Beep(void)
{
  if(BeepOn)  
  {
    if(Cnt_500ms!=0)
    {
      if(CNT_Beep_On++==0)
        Beep_On();
      else if(CNT_Beep_On==100)
      {
        Beep_Off();
        BeepOn=FALSE; 
        CNT_Beep_Off=0;
        Cnt_500ms--;
      }
    }
    else if(Cnt_200ms!=0)
    {
      if(CNT_Beep_On++==0)
        Beep_On();                                                                                                                                                                                                                                         
      else if(CNT_Beep_On==40)
      {
        Beep_Off();
        BeepOn=FALSE; 
        CNT_Beep_Off=0;
        Cnt_200ms--;
      }
    }
    else
      flgBeep_En=FALSE;
  }
  else 
  {
    if(CNT_Beep_Off++==40) 
    {
      CNT_Beep_On=0;
      BeepOn=TRUE;
    }
  }
}

void RF_ClearnOutPut(void)
{
  if ( COut > 0)      // outputs timing //keep output state
  {
    COut--;
    if ( COut == 0)        
    {				
      RF_Out1 = 0;		
      RF_Out2 = 0;		
      RF_Out3 = 0;	
      RF_Out4 = 0;
      RF_Out5 = 0;

      RF_Out6 = 0;
      RF_Cmd_Open=0;
      Vlow = 0;		  
    }
  } 
}

void Remote_Init(void)
{  
      MX_TIM1_Init();
      AT24C01B_Init();
      InitReceiver();
}
//-----------------------------------------------------------
// NAME: Remote
// Function: main descrypt function
// INPUT:none
// OUTPUT:none
//------------------------------------------------------------
void Remote()
{ 
  static uint8_t i;
  for(i=0;i<9;i++)
  {
    Buffer[i]=B[i];
    B[i]=0;
  }

  RFFull = FALSE;  // ready to receive a new frame	
  RFdata_TM=0;
  RFdata_GD=0;	
  for(i=4;i>0;i--)
  {
    RFdata_TM|=Buffer[i-1]<<(8*(i-1));	
  }
  for(i=8;i>4;i--)
  {
    RFdata_GD|=Buffer[i-1]<<(8*(i-5));	
  }
  SENUM=RFdata_GD&0x0fffffff;	    //get ID
  
  NormalKeyGen();                     // compute the decryption key 
  Decrypt();                          // decrypt the hopping code portion

  if ( DecCHK() == FALSE)             // decription failed 
      return;
     
  button.Byte=Buffer[3];//debug
//   if(idx11>100)
//     idx11=0;
//   button_arr[idx11++]=BUTTENCODE;

  if (LEARNMode)
  {
      if(LEARNMode_State<4)
      {
        COut = 50;
        switch(LEARNMode_State)
        {
        case 0:                          //2103
          if(BUTTENCODE==0x20)
          {
            Beep_Set(0,1);
            Beep_Cmd(ENABLE);
            LEARNMode_State = 1;
          }
          else 
          {
            LEARNMode_State = 0;
          }
          break;
        case 1:
          if(BUTTENCODE==0x40)
          {
            Beep_Set(0,1);
            Beep_Cmd(ENABLE);
            LEARNMode_State = 2;
          }
          else if(BUTTENCODE!=0x20) 
          {
            LEARNMode_State = 0;
          }
          break;
        case 2:
          if(BUTTENCODE==0x80)
          {
            Beep_Set(0,1);
            Beep_Cmd(ENABLE);
            LEARNMode_State = 3;
          }
          else if(BUTTENCODE!=0x40)
          {
            Beep_Set(0,1);
            Beep_Cmd(ENABLE);
            LEARNMode_State = 0;
          }
          break;					
        case 3:
          if(BUTTENCODE==0x10)
          {  

            if ( Find()== FALSE) // could not find the Serial Number in memory
            {
              if ( !Insert())             // look for new space
                return;                 // fail if no memory available
            }// ASSERT Ind is pointing to a valid memory location

            IDWrite();                  // write Serial Number in memory
            FHopOK = TRUE;              // enable updating Hopping Code
            HopUpdate();                // Write Hoping code in memory  
      
            Beep_Set(1,0);
            Beep_Cmd(ENABLE);
            LEARNMode = 0; 
            LEARNMode_State = 4;
          }
          else if(BUTTENCODE!=0x80)
          {
            LEARNMode_State = 0;
          }
          break;
          
        default:
          LEARNMode = 0; 
          break;
        } 
     } // LEARN OVER
  }
  
    else // Normal Mode of operation
    {
      if ( Find()== FALSE )
        return; 
      if ( !HopCHK())                 // check Hopping code integrity
        return;
      
      if ( FSame )                     // identified same code as last memorized
      {
        if ( COut >0)               // if output is still active
        {
          COut = 50;            // reload timer to keep active

          if(BUTTENCODE== 0xA0)    //0110  （S0+S2）
           {  
           	;
           }
           else
             TB_RF_Set=0;
        }
        else
          return;  
                         // else discard
      }
      else                            // hopping code incrementing properly
      {
        switch(BUTTENCODE)
        {
        case 0x20://study key 0010  S0
        //Mode/Selct//open/close the door automatically
              RF_OpenAlways=0;
              RF_CloseAlways=0;
              CloseAlwaysAction=0;
              OpenAlwaysAction=0;
              RF_Auto=1;
              Beep_Set(0,1);
              Beep_Cmd(ENABLE);
              ELOCK_ON();			  
              break;
			  
	 case 0x40://0100  S1
         //open the door
#ifndef VER_RELEASE
              RF_Cmd_Open=1;
              Beep_Set(0,1);
              Beep_Cmd(ENABLE);
#endif
              ELOCK_OFF();
              break;

        case 0x80://1000  S2  up
              Beep_Set(0,1);
              Beep_Cmd(ENABLE);
              break;
			  
	 case 0x10://0001  S3  down
              Beep_Set(0,1);
              Beep_Cmd(ENABLE);
              break;
			  
        case 0x30://1001 key5 （S2+S3）
          //open always
              RF_Auto=0;
              RF_CloseAlways=0;
              CloseAlwaysAction=0;
              OpenAlwaysAction=0;
              RF_OpenAlways=1;
              Beep_Set(0,1);
              Beep_Cmd(ENABLE);
              break;
			  
	 case 0xc0://0110 key6 （S0+S1）
//         close always
              RF_Auto=0;
              OpenAlwaysAction=0;
              CloseAlwaysAction=0;
//         RF_OpenAlways=0;
              RF_CloseAlways=1;
              Beep_Set(0,1);
              Beep_Cmd(ENABLE);
              break;
			  
	 default:
	 	break;
        }
		
    //set low battery flag if necessary
      if ( BIT_TEST(Buffer[8],VFlag))
      {
        Vlow = 1;
      }
  // check against learned function code (ce shi xue xi de an jian)
      if ( (( Buffer[7] ^ FCode) & 0xf0) == 0)
      {
      }                
  // init output timer
      COut = 50;
      HopUpdate();
      }// recognized
    } // normal mode
}// remote


void InitReceiver()
{
  RFstate = TRFreset;         // reset state machine in all other cases
  RFFull = FALSE;                 // start with buffer empty
  COut=50;                  
}    
//-----------------------------------------------------------
// NAME: Des_Dkey_Code
// Function: key code descypt
// INPUT:DataInM  DataInL  DataIAO
// OUTPUT:DataIAO
//------------------------------------------------------------
uint32_t Des_Dkey_Code( uint32_t DataInM, uint32_t DataInL, uint32_t DataIAO)
{
  uint32_t lut[32] =
  { 0,1,1,1, 0,1,0,0, 0,0,1,0, 1,1,1,0, 0,0,1,1, 1,0,1,0, 0,1,0,1, 1,1,0,0 };
  /*   E   ,    2   ,    4   ,    7   ,    C   ,    5   ,    A   ,    3   */
  static uint32_t pik,pin,bitin,keybit,keybit2;
  static uint16_t bitlu,ix;
  pik = DataInM;//M
  pin = DataInL;//L
  for(ix=0; ix < 528; ix++)
  {
    bitin = getbit(DataIAO,31);
    DataIAO<<=1;
    keybit2=getbit(pin,15);
    keybit=getbit(pik,31);
    pik=((uint32_t)(pik<<1))|getbit(pin,31);
    pin=(pin<<1)|keybit; /* 64-bit left rotate */
    bitlu = 0;
    ifbit (DataIAO, 1) setbit(bitlu,0);
    ifbit (DataIAO, 9) setbit(bitlu,1);
    ifbit (DataIAO,20) setbit(bitlu,2);
    ifbit (DataIAO,26) setbit(bitlu,3);
    ifbit (DataIAO,31) setbit(bitlu,4);
    DataIAO = DataIAO ^ bitin ^ ((DataIAO&0x10000L)>>16) ^ lut[bitlu]^ keybit2;
  }
  return DataIAO;
}
//-----------------------------------------------------------
// NAME: Decrypt
// Function: RFcode Decrypt
// INPUT:none
// OUTPUT:none
//------------------------------------------------------------
void Decrypt()
{
  static uint32_t Dcode_data;  
  static uint8_t i;	
  Dcode_data=Des_Dkey_Code(mpik,mpin,RFdata_TM);
  for(i=0;i<4;i++)
  {     
    Buffer[i]=(Dcode_data>>(8*i));	
  }
}

void LoadManufCode()
{
  LMNUM	= 0x954C2F50;
  MMNUM	= 0x04B7679A;	
}
//-----------------------------------------------------------
// NAME: NormalKeyGen
// Function: key code descypt
// INPUT:none
// OUTPUT:none
//------------------------------------------------------------
void NormalKeyGen()
{
  //check if same SerialNumbe as last time while output active
  //it is stored in seed
  if (( SEED[0] != Buffer[4]) || ( SEED[1] != Buffer[5]) || ( SEED[2] != Buffer[6]) || \
      ( SEED[3] != (Buffer[7] & 0x0f)) )//|| (COut == 0))
  {
    SEED[0] = Buffer[4];
    SEED[1] = Buffer[5];
    SEED[2] = Buffer[6];
    SEED[3] = Buffer[7]& 0x0f;			
    LoadManufCode();
    mpik=Des_Dkey_Code(MMNUM,LMNUM,(SENUM|0x60000000));//密钥的高32位
    mpin=Des_Dkey_Code(MMNUM,LMNUM,(SENUM|0x20000000));//密钥的低32位
  }
}
//----------------------------------------------------------------------
// NAME:   DecCHK
// INPUT:  Serial Number (Buffer[4..7])
//         Hopping Code  (Buffer[0..3])
// OUTPUT: TRUE if discrimination bits == lsb Serial Number
//         and decrypted function code == plain text function code 
//---------------------------------------------------------
bool DecCHK()    // verify discrimination bits
{
  if ( DisLo != IDLo)     // compare low 8bit of Serial Number
    return FALSE;
  if ( ( (Buffer[3] ^ IDMi) & 0x3)!= 0) // compare 9th and 10th bit of SN 
    return FALSE;
  
  // verify function code
  if ( ((Buffer[3] ^ Buffer[7]) & 0xf0)!= 0)
  {
    return FALSE;
  }
  return TRUE;
} 
//----------------------------------------------------------------------
// NAME : HopCHK
// FUNCTION:Hopping Code Verification
// INPUT:  Hopping Code  (Buffer[0..3])
//         and previous value stored in EEPROM EHop
// OUTPUT: TRUE if hopping code is incrementing and inside a safe window (16)
//----------------------------------------------------------------------
bool HopCHK()
{
  FHopOK  = FALSE;             // Hopping Code is not verified yet
  FSame = FALSE;              // Hopping Code is not the same as previous    
  Hop = ((uint16_t)HopHi << 8) + HopLo;   // make it a 16 bit signed integer //但是这里Hop是u16
  if ( F2Chance )
    if ( NextHop == Hop)
    {
      F2Chance = FALSE;       // resync success
      FHopOK = TRUE;       
      return TRUE;
    }
  
  if ( EHop != ETemp)// verify EEPROM integrity
    return ReqResync();         // memory corrupted need a resync   
  
  // main comparison
  ETemp = Hop - EHop;             // subtract last value from new one
  
  if ( ETemp < 0)                 // locked region
    return FALSE;               // fail
  else if ( ETemp > 16)           // resync region
    return ReqResync();
  else                            // 0>= ETemp >16 ; open window
  {
    if ( ETemp == 0)            // same code (ETemp == 0)
      FSame = TRUE;           // rise a flag
    FHopOK = TRUE;           
    return TRUE;
  }
} 

bool ReqResync()
{
  F2Chance= TRUE;         // flag that a second (sequential) transmission 
  NextHop = Hop+1;        // is needed to resynchronize receiver
  return FALSE;           // cannot accept for now
}
//----------------------------------------------------------------------
// NAME : RDword / WRword
// FUNCTION:READ AND WRITE WORD 
// INPUT:  Ind 
// OUTPUT: NONE
//----------------------------------------------------------------------



void RDword(uint16_t Ind)//200KHz 约250us
{ 
  Dato =  read_eeprom(Ind);
}

void WRword(uint16_t Ind)
{
 write_eeprom(Ind,Dato);
}
//------------------------------------------------------------
// Table structure definition:
//   DatoHi  DatoLo  offset
//  +-------+-------+
//  | FCode | IDLo  |  0    XF contains the function codes (buttons) used during learning
//  +-------+-------+       and the top 4 bit of Serial Number
//  | IDHi  | IDMi  |  +2   IDHi IDMi IDLo contain the 24 lsb of the Serial Number 
//  +-------+-------+
//  | HopHi | HopLo |  +4   sync counter 
//  +-------+-------+
//  | HopHi2| HopLo2|  +6   second copy of sync counter for integrity checking
//  +-------+-------+
//----------------------------------------------------------------------
// NAME : Find
// FUNCTION:search through the whole table the given a record whose ID match
// INPUT:  IDHi, IDMi, IDLo,   serial number to search
// OUTPUT: Ind                 address of record (if found)
//         EHop                sync counter value
//         ETemp               second copy of sync counter
// RETURN:  TRUE if matching record  found     
//----------------------------------------------------------------------
bool Find()
{
  bool Found;
  Found = FALSE;      // init to not found
  
  for (Ind=0; Ind < (EL_SIZE * MAX_USER); Ind+=EL_SIZE)
  {
    RDword( Ind );       // read first Word
    FCode = (Dato>>8);
    if ( (FCode & 0xf0) == 0xf0)        // check if 1111xxxx
      continue;    
    
    if (IDLo != (Dato & 0xff))
      continue;   // fails match
    
    RDword( Ind+2);
    if ( ( (Dato & 0xff) == IDMi) && ( (Dato>>8) == IDHi))
    {
      Found = TRUE;     // match
      break;
    }
  } 
  if (Found == TRUE)
  { 
    RDword( Ind+4);
    EHop = Dato;
    RDword( Ind+6);       
    ETemp= Dato;
  }
  return Found;
}
//----------------------------------------------------------------------
// NAME : Insert
// FUNCTION:search through the whole table for an empty space
// INPUT:  NONE 
// OUTPUT: Ind                 address of empty record
// RETURN:               FALSE if no empty space found         
//----------------------------------------------------------------------
bool Insert()
{
  for (Ind=0; Ind < (EL_SIZE * MAX_USER); Ind+=EL_SIZE)
  {        
    RDword(Ind);        // read first Word
    FCode = (Dato>>8);
    // check if 1111xxxx
    if ( (FCode & 0xf0) == 0xf0)
      return TRUE;    // insert point found
  } // for        

  return  FALSE;          // could not find any empty slot
} 
//----------------------------------------------------------------------
// NAME : IDWrite
// FUNCTION:store IDHi,Mi,Lo + XF at current address Ind
// INPUT:  NONE 
// OUTPUT: TRUE OR FALSE
//----------------------------------------------------------------------
bool IDWrite()
{
  if (!LEARNMode) 
    return FALSE;           // Guard statement: check if LEARN ON

  Dato = Buffer[7];
  Dato = (Dato<<8) + IDLo;      //Buffer[4] Serial Number
  WRword(Ind);                  // write first word

  Dato = IDHi;		   //Buffer[6] Serial Number
  Dato = (Dato<<8) + IDMi;      //Buffer[5] Serial Number
  WRword(Ind+2);                // write second word   
  return TRUE;
} 
//----------------------------------------------------------------------
// NAME : HopUpdate
// FUNCTION:update sync counter of user record at current location
// INPUT:  NONE 
// OUTPUT: TRUE OR FALSE
//----------------------------------------------------------------------
bool HopUpdate()
{
  if (!FHopOK) 
    return FALSE;           // Guard statement: check if Hop update 

  Hop = ((uint16_t)HopHi<<8) + HopLo;  //current sync counter
  Dato = Hop;
  WRword(Ind+4);              // write at offset +4
  Dato = Hop;
  WRword(Ind+6);              // back up copy at offset +6
  FHopOK = FALSE;             // for safety disable updating hopping code    
  return TRUE;
} 
//----------------------------------------------------------------------
// NAME : ClearMem
// FUNCTION:mark all records free 
// INPUT:  NONE 
// OUTPUT: NONE
//----------------------------------------------------------------------
bool ClearMem()
{
  for (Ind=0; Ind < (EL_SIZE * MAX_USER); Ind+=EL_SIZE)
  {
    Dato = 0xffff;        
    WRword(Ind);
  }
  return TRUE;
} 

void RFstate_DeInit(void)
{
  RFstate = TRFSYNC;
  RFFull = FALSE;
  Bptr = 0;
  BitCount=0;
  Hlevalue=0;
  Llevalue=0;
  HlevalueCount = 0;
  LlevalueCount = 0;
  PreemCount = 0;
  Tim_Count_Num = 0;
  Tim_Count_Num1 = 0;
  Tim_Count_Num2 = 0;
  EXTI->FTSR &= 0xFFBF;	
}

bool eeprom_check(void)
{
  RDword(EE_ADD_START+(PARA_NUM_MAX-1)*2);
  if(Dato==0x5050)
    return TRUE;
  else
    return FALSE;
}




void RBufProcess()
{
	/*接收缓存数据终点处理 和超限处理*/
	RBufEnd = Tail ;
	if(RBufEnd >= 3)	
	{													
		if(RBufEnd%2 == 0)
		{
			RBufEnd = RBufEnd - 2 ;
		}
		else
		{
			RBufEnd = RBufEnd - 1 ;
		}
	}
	else
	{
		return;
	}
	/*接收编码处理函数*/
	while(Head != RBufEnd)
	{		
		Hlevalue = ReceiveBuf[Head];
		Llevalue = ReceiveBuf[Head+1] ;
		switch( RFstate )
		{
			case TRFSYNC:
				if((Hlevalue>=200)&&(Hlevalue<600)&&(Llevalue>=200)&&(Llevalue<600))
				{  
					if((Hlevalue	-	Llevalue)	>(-300) &&( (Hlevalue	-Llevalue	)	<	300))
					{       
						NumCnt_Preem++;
			  			if(NumCnt_Preem  > 4)                         
						{
							RFstate = RFstate_P;
							NumCnt_Preem1 = 11 - NumCnt_Preem;
						}                     
					}              
				}
				else
				{
					RDataInit();
				}
				break;
				
			case RFstate_P:
				if((NumCnt_Preem1--) >= 0 )
				{
					if((Llevalue - Hlevalue)>2000 && (Llevalue > 2800)&&(Llevalue < 6000))
					{
						RFstate = RFstate_H;
						BitCount = 0;
						Bptr = 0;
					}                  
				}
				else
				{
					RDataInit();
				}
				break;
			case RFstate_H:		
				if((Hlevalue>250)&&(Hlevalue<1250)&&(Llevalue>250)&&(Llevalue<1250))
				{
					if(Hlevalue > Llevalue)
					{
						BitCount++;		
						B[Bptr] = B[Bptr]>>1;
						B[Bptr]&=0x7f ;                                              
					}
					else
					{
						BitCount++;
						B[Bptr] = B[Bptr]>>1;
						B[Bptr]|=0x80 ;
					}  
					if ( BitCount%8 == 0)
					{
						Bptr++;
					}									 
					if (BitCount == NBIT)
					{
						RFFull = TRUE;
						RFstate = TRFSYNC;
						Bptr = 0;
						BitCount = 0;
						NumCnt_Preem = 0;
					}						
				}                    
				else
				{
					RDataInit();			
				}
				break;
			case TRFreset:
			default:
				RDataInit();				
				break;
		}
		Head = (Head+2)%REBUF_LEN ;
	}
}
			
void RDataInit()
{
	RFstate = TRFSYNC;  
	Bptr = 0;
	BitCount	=	0;
	NumCnt_Preem = 0;
}
/*  无线接收初始化程序*/
void RBufInit()
{
	RDataInit() ;
       LL_TIM_IC_SetPolarity(TIM1,LL_TIM_CHANNEL_CH1,LL_TIM_IC_POLARITY_RISING);  /* 允许上升沿触发中断*/
	Flag_EXTIInit = SET ;
}


void TBVar_Flg_Init(void)
{
  TBVar._2ms = 1 ; 
  TBVar._5ms = 4 ; 
  TBVar.Beep500ms = 99;//
  TBVar.Beep15s=29;    
  TBFlg.Value=0;
}
void TB_2msLoop(void)
{
#ifdef HAL_ADC_MODULE_ENABLED  
  hadc.Instance = ADC1;
  HAL_ADC_Start(&hadc);
#endif
#ifdef HAL_WWDG_MODULE_ENABLED 
  hwwdg.Instance = WWDG;
  HAL_WWDG_Refresh(&hwwdg,127);
#endif 
   LL_GPIO_SetOutputPin(TEST1_GPIO_Port,TEST1_Pin);
  RBufProcess();//RF ver2
   LL_GPIO_ResetOutputPin(TEST1_GPIO_Port,TEST1_Pin);
//  -------------------------
  TBFlg.Bit._2ms = 0;    
  TBVar._2ms = 1;
}

uint32_t  iBat_Volt_Sense,iLck_Cur_Sense;

void TB_5msLoop(void)
{ 
    HAL_ADC_Start(&hadc);
     iBat_Volt_Sense=HAL_ADC_GetValue(&hadc);
     iLck_Cur_Sense=HAL_ADC_GetValue(&hadc);

}
/*******************************************************************************
* Function Name  : SysTickHandler
* Description    : This function handles SysTick Handler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void HAL_SYSTICK_Callback(void)
{
  if(TBVar._2ms!=0)
       TBVar._2ms--;
  else
  {
    TBFlg.Bit._2ms = 1;
       TB_2msLoop();
  }
  
  if(TBVar._5ms!=0)
       TBVar._5ms--;
  else
  {
//       HAL_GPIO_TogglePin(TEST1_GPIO_Port,TEST1_Pin);
       TBFlg.Bit._5ms = 1;
       TBVar._5ms=4;
        TB_5msLoop();
        if(TBVar.Beep500ms!=0){
           TBVar.Beep500ms--;
         }else{
    //         HAL_GPIO_TogglePin(TEST2_GPIO_Port,TEST2_Pin);
             TBVar.Beep500ms=99;
             if(TBVar.Beep15s!=0){
                TBVar.Beep15s--;
             }else{
      //            HAL_GPIO_TogglePin(TEST3_GPIO_Port,TEST3_Pin);
                  TBVar.Beep15s=29;
                   LEARNMode=0;
             } 
       }
  }
}
