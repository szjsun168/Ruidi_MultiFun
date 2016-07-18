/********************* (C) COPYRIGHT 2010 Panasonic Electric Works Co.,Ltd. *******************
* File Name          : eeprom.c
* Description        : basic Code.
***********************************************************************************************/



#include "eeprom.h" 

/****************************************************************************
*****************************************************************************/
//初始化IIC接口

  /*sysclock=48MHz*/
void delay_ms(uint16_t i)
{
	uint16_t k,j;
	for(k=0;k<i;k++)
	{
		for(j=0;j<4000;j++)        
		{
		}
	}
}
  /*sysclock=48MHz*/
void delay_us(unsigned char i)
{
	uint8_t k,j;
	for(k=0;k<i;k++)
	{
		for(j=0;j<5;j++)               
		{
		}
	}
} 

//初始化IIC
void IIC_Init(void)
{		
  GPIO_InitTypeDef GPIO_InitStruct;

	__HAL_RCC_GPIOB_CLK_ENABLE();/*先使能外设IO PORTB时钟 */
	
	 /*Configure GPIO pins : I2C1_SDA_Pin |I2C1_SCL_Pin  */
      GPIO_InitStruct.Pin = I2C1_SDA_Pin |I2C1_SCL_Pin;
      GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	  
       IIC_SDA_1   
       IIC_SCL_1 
}
//产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
       IIC_SDA_1   
       IIC_SCL_1 
       delay_us(2);
       IIC_SDA_0         
	delay_us(2);
       IIC_SCL_0         
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
        IIC_SCL_0
	 delay_us(2);
       IIC_SDA_0         
	delay_us(2);
    	IIC_SCL_1
	delay_us(2); 
       IIC_SDA_1   
	delay_us(2);
       IIC_SCL_0         
	delay_us(5);
									
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t IIC_Wait_Ack(void)
{
	SDA_IN();      //SDA设置为输入 
       IIC_SCL_0         
	delay_us(2);	   
       IIC_SCL_1 
	delay_us(2);	 
       if(HAL_GPIO_ReadPin(GPIOB,I2C1_SDA_Pin))
	{
              IIC_SCL_0         
		delay_us(2);
		return 1;	
	}
       IIC_SCL_0         
	return 0;  
} 
//产生ACK应答
void IIC_Ack(void)
{
	SDA_OUT();
       IIC_SDA_0         
	
	delay_us(2);
       IIC_SCL_1 
	delay_us(2);
       IIC_SCL_0         
	delay_us(2);
       IIC_SDA_1   
	
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	SDA_OUT();
       IIC_SDA_1   
	
	delay_us(2);
       IIC_SCL_1 
	
	delay_us(2);
       IIC_SCL_0         
	

}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(uint8_t txd)
{                        
	uint8_t t;   
	SDA_OUT(); 	    
	
	for(t=8;t>0;t--)
  	{   
              IIC_SCL_0         
		
		delay_us(2);         
		if(0x80==(txd&0x80))
		{
                     IIC_SDA_1   
		}
		else
		{
                   IIC_SDA_0         
		}
		txd=txd<<1;
              IIC_SCL_1 
		delay_us(2);
	}
	IIC_SCL_0;	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t IIC_Read_Byte(void)
{

	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
	for(i=8;i>0;i-- )
	{
              IIC_SCL_0         
		delay_us(2);
		receive=receive<<1;
		if(HAL_GPIO_ReadPin(GPIOB,I2C1_SDA_Pin))
		{
		receive++; 
		}
              IIC_SCL_1 
		delay_us(2);
	 
			
	
	}					 
       IIC_SCL_0         
	return receive;
}

void AT24C01B_Init(void)
{
	IIC_Init();
}

uint16_t read_word_eeprom(uint16_t addr)
{
	uint8_t i;
	uint8_t buffer[2];
	uint16_t data;
		IIC_Start();  

		IIC_Send_Byte(0xA0);	   //发送写命令
		IIC_Wait_Ack();

		IIC_Send_Byte(2*addr);   //发送低地址
		IIC_Wait_Ack();
		IIC_Start(); 	 
		IIC_Send_Byte(0xA1);	   //发送du命令	 
		   
		IIC_Wait_Ack();	 

	for(i=0;i<2;i++)
	{
		buffer[i]=IIC_Read_Byte();
		if(i==0)
		{
			IIC_Ack();
		}
		else
		{
			IIC_NAck();	
		}
	}   
	IIC_Stop();//产生一个停止条件
	data=buffer[0];
	data=(data<<8)|buffer[1];	    
	return data ;
}

void action_error_EEPROM(void)
{
}

uint16_t read_eeprom(uint16_t addr)
{
uint16_t i,j,error_cnt;

	for(error_cnt = 0; error_cnt < (uint8_t)10; error_cnt++){
		i=read_word_eeprom(addr);
		j=read_word_eeprom(addr);
		if(i == j){
			error_cnt = 0;
			break;
			}
	}
	if(error_cnt != (uint8_t)0){
		/*--- 引起通信异常时处理 ---*/
		action_error_EEPROM();
	}
   return i;
}



void write_eeprom(uint16_t addr,uint16_t data)
{
	uint8_t i;
	uint8_t buffer[2];
	uint8_t error_cnt;
	uint16_t r_data;
	
	buffer[1]=data & 0xff;
	buffer[0]=data>>8;
	
	for(error_cnt = 0; error_cnt < (uint8_t)3; error_cnt++){
			IIC_Start();  
			IIC_Send_Byte(0xA0);	    //发送写命令
			IIC_Wait_Ack();   
			IIC_Send_Byte(2*addr);   //发送低地址
			IIC_Wait_Ack();
			for(i=0;i<2;i++)
			{ 		
				IIC_Send_Byte(buffer[i]);     //发送字节							   
				IIC_Wait_Ack();
			}  		    	   
			IIC_Stop();//产生一个停止条件 
			delay_ms(5);	
		
			r_data = read_eeprom(addr);
			/*--- 如果写入数据和读入数据是一致就会跳出循环体---*/
			if(data == r_data){
				error_cnt = 0;
				break;
				}
#if 0
		printf("\n\r  %6d  %6d %6d",error_cnt,data, r_data);
#endif                	 
	}
	if(error_cnt != (uint8_t)0){
		/*--- 引起通信异常时处理 ---*/
		action_error_EEPROM();
	}
	
}

/******************* (C) COPYRIGHT 2010 Panasonic Electric Works *****END OF FILE****/
