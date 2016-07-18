

/********************* (C) COPYRIGHT 2010 Panasonic Electric Works Co.,Ltd. *******************
* File Name          : eeprom.h
***************************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EEPROM_H
#define __EEPROM_H
#include "stm32f0xx_ll_gpio.h"



//IO方向设置
#define SDA_IN()    {LL_GPIO_SetPinMode(I2C1_SDA_GPIO_Port,I2C1_SDA_Pin,LL_GPIO_MODE_INPUT);}
#define SDA_OUT() {LL_GPIO_SetPinMode(I2C1_SDA_GPIO_Port,I2C1_SDA_Pin,LL_GPIO_MODE_OUTPUT);}

//IO操作函数	 
#define IIC_SDA_1      {LL_GPIO_SetOutputPin(I2C1_SDA_GPIO_Port,I2C1_SDA_Pin);} 
#define IIC_SCL_1       {LL_GPIO_SetOutputPin(I2C1_SCL_GPIO_Port,I2C1_SCL_Pin);}
#define IIC_SDA_0      {LL_GPIO_ResetOutputPin(I2C1_SDA_GPIO_Port,I2C1_SDA_Pin);} 
#define IIC_SCL_0       {LL_GPIO_ResetOutputPin(I2C1_SCL_GPIO_Port,I2C1_SCL_Pin);} 



//IIC所有操作函数
extern void IIC_Init(void);                //初始化IIC的IO口				 
extern void IIC_Start(void);				//发送IIC开始信号
extern void IIC_Stop(void);	  			//发送IIC停止信号
extern void IIC_Send_Byte(uint8_t txd);			//IIC发送一个字节
extern uint8_t   IIC_Read_Byte(void);//IIC读取一个字节
extern uint8_t   IIC_Wait_Ack(void); 				//IIC等待ACK信号
extern void IIC_Ack(void);					//IIC发送ACK信号
extern void IIC_NAck(void);				//IIC不发送ACK信号

extern void  IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
extern uint8_t    IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);	  

extern void AT24C01B_Init(void);


/****************************************************************************/
extern void delay_us(unsigned char i);
extern void delay_ms(uint16_t i);
extern uint16_t read_eeprom(uint16_t addr);
extern void write_eeprom(uint16_t addr,uint16_t data);
#endif
