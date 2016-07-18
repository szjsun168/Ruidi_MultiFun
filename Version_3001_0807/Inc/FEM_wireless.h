
#ifndef FEM__wireless_H__
#define FEM__wireless_H__

#include "stm32f10x_lib.h"

#define REBUF_LEN 1024

extern u16 ReceiveBuf[REBUF_LEN];
extern u16 Head  ;
extern u16 Tail  ;

extern FlagStatus Flag_EXTIInit  ; //�ⲿ�жϳ�ʼ����־λ

void RBufProcess(void);
void RDataInit(void);
void RBufInit(void);


#endif


