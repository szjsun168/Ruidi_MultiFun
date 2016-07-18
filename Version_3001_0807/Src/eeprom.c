/********************* (C) COPYRIGHT 2010 Panasonic Electric Works Co.,Ltd. *******************
* File Name          : eeprom.c
* Description        : basic Code.
***********************************************************************************************/



#include "eeprom.h" 

/****************************************************************************
*****************************************************************************/
//��ʼ��IIC�ӿ�

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

//��ʼ��IIC
void IIC_Init(void)
{		
  GPIO_InitTypeDef GPIO_InitStruct;

	__HAL_RCC_GPIOB_CLK_ENABLE();/*��ʹ������IO PORTBʱ�� */
	
	 /*Configure GPIO pins : I2C1_SDA_Pin |I2C1_SCL_Pin  */
      GPIO_InitStruct.Pin = I2C1_SDA_Pin |I2C1_SCL_Pin;
      GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	  
       IIC_SDA_1   
       IIC_SCL_1 
}
//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();     //sda�����
       IIC_SDA_1   
       IIC_SCL_1 
       delay_us(2);
       IIC_SDA_0         
	delay_us(2);
       IIC_SCL_0         
}	  
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
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
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
uint8_t IIC_Wait_Ack(void)
{
	SDA_IN();      //SDA����Ϊ���� 
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
//����ACKӦ��
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
//������ACKӦ��		    
void IIC_NAck(void)
{
	SDA_OUT();
       IIC_SDA_1   
	
	delay_us(2);
       IIC_SCL_1 
	
	delay_us(2);
       IIC_SCL_0         
	

}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
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
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
uint8_t IIC_Read_Byte(void)
{

	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
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

		IIC_Send_Byte(0xA0);	   //����д����
		IIC_Wait_Ack();

		IIC_Send_Byte(2*addr);   //���͵͵�ַ
		IIC_Wait_Ack();
		IIC_Start(); 	 
		IIC_Send_Byte(0xA1);	   //����du����	 
		   
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
	IIC_Stop();//����һ��ֹͣ����
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
		/*--- ����ͨ���쳣ʱ���� ---*/
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
			IIC_Send_Byte(0xA0);	    //����д����
			IIC_Wait_Ack();   
			IIC_Send_Byte(2*addr);   //���͵͵�ַ
			IIC_Wait_Ack();
			for(i=0;i<2;i++)
			{ 		
				IIC_Send_Byte(buffer[i]);     //�����ֽ�							   
				IIC_Wait_Ack();
			}  		    	   
			IIC_Stop();//����һ��ֹͣ���� 
			delay_ms(5);	
		
			r_data = read_eeprom(addr);
			/*--- ���д�����ݺͶ���������һ�¾ͻ�����ѭ����---*/
			if(data == r_data){
				error_cnt = 0;
				break;
				}
#if 0
		printf("\n\r  %6d  %6d %6d",error_cnt,data, r_data);
#endif                	 
	}
	if(error_cnt != (uint8_t)0){
		/*--- ����ͨ���쳣ʱ���� ---*/
		action_error_EEPROM();
	}
	
}

/******************* (C) COPYRIGHT 2010 Panasonic Electric Works *****END OF FILE****/
