/**
  ******************************************************************************
  * File Name          : mxconstants.h
  * Description        : This file contains the common defines of the application
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MXCONSTANT_H
#define __MXCONSTANT_H
  /* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define nLBV_Pin GPIO_PIN_13
#define nLBV_GPIO_Port GPIOC
#define iBat_Volt_Sense_Pin GPIO_PIN_3
#define iBat_Volt_Sense_GPIO_Port GPIOA
#define iLck_Cur_Sense_Pin GPIO_PIN_4
#define iLck_Cur_Sense_GPIO_Port GPIOA
#define iLock_FB_Pin GPIO_PIN_5
#define iLock_FB_GPIO_Port GPIOA
#define oPWM_Lock_Pin GPIO_PIN_6
#define oPWM_Lock_GPIO_Port GPIOA
#define iEmer_Pin GPIO_PIN_7
#define iEmer_GPIO_Port GPIOA
#define iILock_Pin GPIO_PIN_0
#define iILock_GPIO_Port GPIOB
#define oILock_Pin GPIO_PIN_1
#define oILock_GPIO_Port GPIOB
#define iSW_LOCK_Pin GPIO_PIN_2
#define iSW_LOCK_GPIO_Port GPIOB
#define iSW_COPEN_Pin GPIO_PIN_10
#define iSW_COPEN_GPIO_Port GPIOB
#define iSW_I2O_Pin GPIO_PIN_11
#define iSW_I2O_GPIO_Port GPIOB
#define iSW_HALF_Pin GPIO_PIN_12
#define iSW_HALF_GPIO_Port GPIOB
#define iSW_AUTO_Pin GPIO_PIN_13
#define iSW_AUTO_GPIO_Port GPIOB
#define oHalf_Pin GPIO_PIN_14
#define oHalf_GPIO_Port GPIOB
#define oPowerOff_Pin GPIO_PIN_15
#define oPowerOff_GPIO_Port GPIOB
#define RF_Data_Pin GPIO_PIN_8
#define RF_Data_GPIO_Port GPIOA
#define iClose_Wait_Pin GPIO_PIN_9
#define iClose_Wait_GPIO_Port GPIOA
#define iInfra_Pin GPIO_PIN_10
#define iInfra_GPIO_Port GPIOA
#define iOpen_Cmd_Pin GPIO_PIN_11
#define iOpen_Cmd_GPIO_Port GPIOA
#define oOpen_Pin GPIO_PIN_12
#define oOpen_GPIO_Port GPIOA
#define iEM_Pin GPIO_PIN_6
#define iEM_GPIO_Port GPIOF
#define TEST1_Pin GPIO_PIN_7
#define TEST1_GPIO_Port GPIOF
#define iI2OOpen_Pin GPIO_PIN_3
#define iI2OOpen_GPIO_Port GPIOB
#define iO2IOpen_Pin GPIO_PIN_4
#define iO2IOpen_GPIO_Port GPIOB
#define oPWM_BUZ_Pin GPIO_PIN_5
#define oPWM_BUZ_GPIO_Port GPIOB
#define TEST2_Pin GPIO_PIN_6
#define TEST2_GPIO_Port GPIOB
#define TEST3_Pin GPIO_PIN_7
#define TEST3_GPIO_Port GPIOB
#define I2C1_SDA_Pin GPIO_PIN_8
#define I2C1_SDA_GPIO_Port GPIOB
#define I2C1_SCL_Pin GPIO_PIN_9
#define I2C1_SCL_GPIO_Port GPIOB

#define APP_TIMEOUT_VALUE   500         /*5000 ms*/
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __MXCONSTANT_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
