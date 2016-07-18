/******************* (C) COPYRIGHT 2012  ********************
* File Name          :Remote.c
* Description        : Remote program body.
*******************************************************************************
* History:
* 2012/04/29 v1.0

*******************************************************************************
main functino description

*****************************************************************************/
#ifndef __Remote_h
#define __Remote_h

#include "stm32f0xx_hal.h"

typedef enum {FALSE = 0, TRUE = !FALSE} bool;

#define HopLo  Buffer[0] 	//sync counter
#define HopHi  Buffer[1]	 //
#define DisLo  Buffer[2] 	//discrimination bits LSB
#define DOK    Buffer[3]	 //Disc. MSB + Ovf + Key
#define IDLo   Buffer[4]	 //S/N LSB
#define IDMi   Buffer[5]	 //S/N 
#define IDHi   Buffer[6]	 //S/N MSB

#define	S0  	5  	 //  Buffer[3] function codes
#define S1  	6  	 //  Buffer[3] function codes
#define	S2  	7   	//  Buffer[3] function codes
#define	S3  	4  	 //  Buffer[3] function codes
#define VFlag 	7		//  Buffer[8] low battery flag

//---------------------------- timings----------------------------------------
#define TOUT   	3             //   5 * 71ms = 350ms output delay
#define TFLASH 	2           //   4 * 71ms = 280ms half period
#define TLEARN  255       // 255 * 71ms =  18s  learn timeout

#define setbit( b, n)  	((b) |= ((uint32_t) 1 << (n)))
#define getbit( b, n)   (((b) & ((uint32_t)1<<n)) ? 1 : 0)
#define ifbit(x,y)    	if (getbit(x,y))
#define BIT_TEST( x, y)	 (( (x) & ((uint32_t)1<<(y))) != 0)

#define TRFreset    	0
#define TRFSYNC    	1
#define RFstate_P      	2
#define RFstate_H     	3
#define RFstate_H_END   4
#define NBIT            65      // number of bit to receive -1

extern void Remote_DeInit(void);
extern void Remote_Init(void);
extern void Remote(void);
extern void InitReceiver(void);
extern uint32_t Des_Dkey_Code( uint32_t DataInM, uint32_t DataInL, uint32_t DataIAO);
extern void Decrypt(void);
extern void LoadManufCode(void);
extern void NormalKeyGen(void);
extern bool DecCHK(void);    // verify discrimination bits
extern bool HopCHK(void);
extern bool ReqResync(void);
extern void RDword(uint16_t Ind);
extern void WRword(uint16_t Ind);
extern bool Find(void);
extern bool Insert(void);
extern bool IDWrite(void);
extern bool HopUpdate(void);
extern bool ClearMem(void);
extern void RFstate_DeInit(void);
extern void RF_ClearnOutPut(void);
extern void TBVar_Flg_Init(void);

//
//typedef struct
//{
//  uint16_t Mode:1; //0: Mode ;  1: ParaSet
//  uint16_t Open:1; //1: Open the door
//  uint16_t Auto:2; //1: control door automately
//  uint16_t Up:1;
//  uint16_t Down:1;
//  uint16_t Set:1; //Store the Para or quit 
//  uint8_t unused:6;
//}RF_TypeDef;*/

typedef union 
{
  uint16_t Value;
  struct
  {
    uint16_t _2ms :1; 
    uint16_t _5ms :1; 
    uint16_t _10ms :1; 
    uint16_t _50ms :1; 
    uint16_t _100ms :1; 
    uint16_t Beep200msOn: 1;
    uint16_t Beep200msOff: 1;
    uint16_t Beep500msOn: 1;
    uint16_t unused:8;
  }Bit;
}TBFlg_TypeDef;

typedef struct
{
  uint16_t _2ms ; 
  uint16_t _5ms ; 
  uint16_t _10ms ; 
  uint16_t _50ms ; 
  uint16_t _100ms ; 
  uint16_t Beep200ms;
  uint16_t Beep500ms;
  uint16_t Beep3s; 
  uint16_t Beep15s; 
}TBVar_TypeDef;


extern bool eeprom_check(void);

/*******************************************ver2 begin************************/
#define REBUF_LEN 256

extern uint16_t ReceiveBuf[REBUF_LEN];
extern uint16_t Head  ;
extern uint16_t Tail  ;

extern FlagStatus Flag_EXTIInit  ; //外部中断初始化标志位
extern TBFlg_TypeDef  TBFlg;
extern TBVar_TypeDef  TBVar;
extern bool flgBeep_En;
extern bool BeepOn;
extern uint16_t CNT_Beep_On; 
extern uint16_t CNT_Beep_Off;
extern bool RFFull;     // buffer full
extern uint16_t TB_RF_Quit;
extern uint16_t TB_RF_Set;
extern uint8_t flgTB_Quit_En;

extern void RBufProcess(void);
extern void RDataInit(void);
extern void RBufInit(void);
extern void Beep_Set(uint8_t ,uint8_t);
extern void Beep_Cmd(FunctionalState);
extern void Beep(void);




/*******************************ver2 end***************************************/


#endif
