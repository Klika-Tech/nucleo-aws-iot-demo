/**
 ******************************************************************************
 * @file    stm32_spwf_wifi.c
 * @author  Central LAB
 * @version V2.0.0
 * @date    10-February-2016
 * @brief   HAL related functionality of X-CUBE-WIFI1
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
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
/* Includes ------------------------------------------------------------------*/
#include "wifi_module.h"
#include "stm32_spwf_wifi.h"
#include "ring_buffer.h"

/** @addtogroup BSP
* @{
*/ 


/** @defgroup  NUCLEO_WIFI_DRIVER
  * @brief Wi-Fi_driver modules
  * @{
  */


/** @defgroup NUCLEO_WIFI_DRIVER_Private_Defines
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Prescaler declaration */
uint32_t uwPrescalerValue = 0;  

void HAL_GPIO_Init1(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);

/* TIM handle declaration */
TIM_HandleTypeDef    TimHandle, PushTimHandle;
extern UART_HandleTypeDef UartMsgHandle;
extern UART_HandleTypeDef UartWiFiHandle;//main USART handle

/**
  * @}
  */


/** @addtogroup NUCLEO_WIFI_DRIVER_Private_Variables
  * @{
  */
/* Private variables ---------------------------------------------------------*/

/**
  * @}
  */

  
/** @defgroup NUCLEO_WIFI_DRIVER_Private_Functions
  * @{
  */

/**
  * @brief UART MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  *           - NVIC configuration for UART interrupt request enable
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{ 
  GPIO_InitTypeDef  GPIO_InitStruct;

  if (huart==&UartWiFiHandle)
  {
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  USARTx_TX_GPIO_CLK_ENABLE();
  USARTx_RX_GPIO_CLK_ENABLE();


  /* Enable USARTx clock */
  USARTx_CLK_ENABLE(); 
    __SYSCFG_CLK_ENABLE();

#ifdef USE_STM32F1xx_NUCLEO      
   __HAL_AFIO_REMAP_USART3_PARTIAL();
#endif
   
  /*##-2- Configure peripheral GPIO ##########################################*/  
  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = WiFi_USART_TX_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
#if defined (USE_STM32L0XX_NUCLEO) || (USE_STM32F4XX_NUCLEO) || (USE_STM32L4XX_NUCLEO) 
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  GPIO_InitStruct.Alternate = WiFi_USARTx_TX_AF;
#endif  
#ifdef USE_STM32F1xx_NUCLEO      
   GPIO_InitStruct.Pull     = GPIO_PULLUP;
#endif  
  GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
  
  HAL_GPIO_Init(WiFi_USART_TX_GPIO_PORT, &GPIO_InitStruct);

  /* UART RX GPIO pin configuration  */
  GPIO_InitStruct.Pin = WiFi_USART_RX_PIN;
#ifdef USE_STM32F1xx_NUCLEO  
  GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
#endif  
#if defined (USE_STM32L0XX_NUCLEO) || (USE_STM32F4XX_NUCLEO) || (USE_STM32L4XX_NUCLEO)
  GPIO_InitStruct.Alternate = WiFi_USARTx_RX_AF;
#endif
  
  HAL_GPIO_Init(WiFi_USART_RX_GPIO_PORT, &GPIO_InitStruct);
  
  
  /* UART RTS GPIO pin configuration  */
  GPIO_InitStruct.Pin = WiFi_USART_RTS_PIN;
#ifdef USE_STM32F1xx_NUCLEO  
  GPIO_InitStruct.Pull     = GPIO_PULLDOWN;
  GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;//GPIO_MODE_AF_PP;
#endif  
#if defined (USE_STM32L0XX_NUCLEO) || (USE_STM32F4XX_NUCLEO) || (USE_STM32L4XX_NUCLEO)
  GPIO_InitStruct.Pull     = GPIO_PULLUP;
  GPIO_InitStruct.Alternate = WiFi_USARTx_RX_AF;
#endif
  
  HAL_GPIO_Init(WiFi_USART_RTS_GPIO_PORT, &GPIO_InitStruct);
  
  /*##-3- Configure the NVIC for UART ########################################*/
  /* NVIC for USART */
	#if defined (USE_STM32L0XX_NUCLEO) || (USE_STM32F4XX_NUCLEO) || (USE_STM32L4XX_NUCLEO)
  HAL_NVIC_SetPriority(USARTx_IRQn, 3, 0);
	#else
	HAL_NVIC_SetPriority(USARTx_IRQn, 1, 0);
	#endif
  HAL_NVIC_EnableIRQ(USARTx_IRQn);
  
  }
  
}


/**
  * @brief UART MSP De-Initialization 
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO and NVIC configuration to their default state
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
  if (huart==&UartWiFiHandle)
  {
  /*##-1- Reset peripherals ##################################################*/
  USARTx_FORCE_RESET();
  USARTx_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks #################################*/
  /* Configure UART Tx as alternate function  */
  HAL_GPIO_DeInit(WiFi_USART_TX_GPIO_PORT, WiFi_USART_TX_PIN);
  /* Configure UART Rx as alternate function  */
  HAL_GPIO_DeInit(WiFi_USART_RX_GPIO_PORT, WiFi_USART_RX_PIN);
  
  /*##-3- Disable the NVIC for UART ##########################################*/
  HAL_NVIC_DisableIRQ(USARTx_IRQn);
  }
}
    




/**
  * @brief TIM MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  * @param htim: TIM handle pointer
  * @retval None
  */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
  if(htim==&PushTimHandle)
  {
    /* TIMx Peripheral clock enable */
    TIMp_CLK_ENABLE();
    
    /*##-2- Configure the NVIC for TIMx ########################################*/
    /* Set the TIMx priority */
    HAL_NVIC_SetPriority(TIMp_IRQn, 3, 0);
    
    /* Enable the TIMx global Interrupt */
    HAL_NVIC_EnableIRQ(TIMp_IRQn);
  }
  else
  {    
    /* TIMx Peripheral clock enable */
    TIMx_CLK_ENABLE();
    
    /*##-2- Configure the NVIC for TIMx ########################################*/
    /* Set the TIMx priority */
    HAL_NVIC_SetPriority(TIMx_IRQn, 3, 0);
    
    /* Enable the TIMx global Interrupt */
    HAL_NVIC_EnableIRQ(TIMx_IRQn);
    
  }
}

  /*##-1- Configure the TIM peripheral #######################################*/
  /* -----------------------------------------------------------------------
    In this example TIM3 input clock (TIM3CLK)  is set to APB1 clock (PCLK1) x2,
    since APB1 prescaler is set to 4 (0x100).
       TIM3CLK = PCLK1*2
       PCLK1   = HCLK/2
    => TIM3CLK = PCLK1*2 = (HCLK/2)*2 = HCLK = SystemCoreClock
    To get TIM3 counter clock at 10 KHz, the Prescaler is computed as following:
    Prescaler = (TIM3CLK / TIM3 counter clock) - 1
    Prescaler = (SystemCoreClock /10 KHz) - 1

    Note:
     SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f1xx.c file.
     Each time the core clock (HCLK) changes, user had to update SystemCoreClock
     variable value. Otherwise, any configuration based on this variable will be incorrect.
     This variable is updated in three ways:
      1) by calling CMSIS function SystemCoreClockUpdate()
      2) by calling HAL API function HAL_RCC_GetSysClockFreq()
      3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
  ----------------------------------------------------------------------- */
void Timer_Config()
{
  /* Compute the prescaler value to have TIMx counter clock equal to 10000 Hz */
  uwPrescalerValue = (uint32_t)(SystemCoreClock / 10000) - 1;
  
  /* Set TIMx instance */
  TimHandle.Instance = TIMx;

  /* Initialize TIMx peripheral as follows:
       + Period = 10000 - 1
       + Prescaler = (SystemCoreClock/10000) - 1
       + ClockDivision = 0
       + Counter direction = Up
  */
#if defined (USE_STM32L0XX_NUCLEO) || defined (USE_STM32F4XX_NUCLEO) || defined (USE_STM32L4XX_NUCLEO)
  TimHandle.Init.Period            = 100 - 1;
#endif
#if defined (USE_STM32F1xx_NUCLEO) 
  TimHandle.Init.Period            = 100 - 1;
#endif  
  TimHandle.Init.Prescaler         = uwPrescalerValue;
  TimHandle.Init.ClockDivision     = 0;
  TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
#ifdef USE_STM32F1xx_NUCLEO
  TimHandle.Init.RepetitionCounter = 0;
#endif 

  if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler(); 
  }

}

/**
  * @brief Push_Timer_Config
  *        This function configures the Push Timer
  * @param None
  * @retval None
  */
void Push_Timer_Config()
{
  /* Compute the prescaler value to have TIMx counter clock equal to 10000 Hz */
  uwPrescalerValue = (uint32_t)(SystemCoreClock / 10000) - 1;
  
  /* Set TIMx instance */
  PushTimHandle.Instance = TIMp;

  /* Initialize TIMx peripheral as follows:
       + Period = 10000 - 1
       + Prescaler = (SystemCoreClock/10000) - 1
       + ClockDivision = 0
       + Counter direction = Up
  */
  PushTimHandle.Init.Period            = 10 - 1;//10000
  PushTimHandle.Init.Prescaler         = uwPrescalerValue;
  PushTimHandle.Init.ClockDivision     = 0;
  PushTimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
#ifdef USE_STM32F1xx_NUCLEO
  PushTimHandle.Init.RepetitionCounter = 0;
#endif 

  if (HAL_TIM_Base_Init(&PushTimHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler(); 
  }

}

/**
* @brief  USART_Configuration
* WB_WIFI_UART configured as follow:
*      - BaudRate = 115200 baud  
*      - Word Length = 8 Bits
*      - One Stop Bit
*      - No parity
*      - Hardware flow control enabled (RTS and CTS signals)
*      - Receive and transmit enabled
*
* @param  None
* @retval None
*/
void UART_Configuration(uint32_t baud_rate)
{
  UartWiFiHandle.Instance             = WB_WIFI_UART;
  UartWiFiHandle.Init.BaudRate        = baud_rate;
  UartWiFiHandle.Init.WordLength      = UART_WORDLENGTH_8B;
  UartWiFiHandle.Init.StopBits        = UART_STOPBITS_1;
  UartWiFiHandle.Init.Parity          = UART_PARITY_NONE ;
  UartWiFiHandle.Init.HwFlowCtl       = UART_HWCONTROL_RTS;//UART_HWCONTROL_NONE;
  UartWiFiHandle.Init.Mode            = UART_MODE_TX_RX;
  UartWiFiHandle.Init.OverSampling    = UART_OVERSAMPLING_16;
  //UartWiFiHandle.Init.OneBitSampling  = UART_ONEBIT_SAMPLING_ENABLED;
  
  if(HAL_UART_DeInit(&UartWiFiHandle) != HAL_OK)
  {
    Error_Handler();
  }  
  if(HAL_UART_Init(&UartWiFiHandle) != HAL_OK)
  {
    Error_Handler();
  }  
 }


#ifdef USART_PRINT_MSG
void USART_PRINT_MSG_Configuration(uint32_t baud_rate)
{
  UartMsgHandle.Instance             = WIFI_UART_MSG;
  UartMsgHandle.Init.BaudRate        = baud_rate;
  UartMsgHandle.Init.WordLength      = UART_WORDLENGTH_8B;
  UartMsgHandle.Init.StopBits        = UART_STOPBITS_1;
  UartMsgHandle.Init.Parity          = UART_PARITY_NONE ;
  UartMsgHandle.Init.HwFlowCtl       = UART_HWCONTROL_NONE;// USART_HardwareFlowControl_RTS_CTS;
  UartMsgHandle.Init.Mode            = UART_MODE_TX_RX;

  if(HAL_UART_DeInit(&UartMsgHandle) != HAL_OK)
  {
    Error_Handler();
  }  
  if(HAL_UART_Init(&UartMsgHandle) != HAL_OK)
  {
    Error_Handler();
  }
      
}


void UART_Msg_Gpio_Init()
{ 
  GPIO_InitTypeDef  GPIO_InitStruct;

  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  USARTx_PRINT_TX_GPIO_CLK_ENABLE();
  USARTx_PRINT_RX_GPIO_CLK_ENABLE();


  /* Enable USARTx clock */
  USARTx_PRINT_CLK_ENABLE(); 
    __SYSCFG_CLK_ENABLE();
  /*##-2- Configure peripheral GPIO ##########################################*/  
  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = WiFi_USART_PRINT_TX_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
#if defined (USE_STM32L0XX_NUCLEO) || (USE_STM32F4XX_NUCLEO) || (USE_STM32L4XX_NUCLEO)
  GPIO_InitStruct.Alternate = PRINTMSG_USARTx_TX_AF;
#endif  
  HAL_GPIO_Init(WiFi_USART_PRINT_TX_GPIO_PORT, &GPIO_InitStruct);

  /* UART RX GPIO pin configuration  */
  GPIO_InitStruct.Pin = WiFi_USART_PRINT_RX_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
#if defined (USE_STM32L0XX_NUCLEO) || (USE_STM32F4XX_NUCLEO) || (USE_STM32L4XX_NUCLEO)
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Alternate = PRINTMSG_USARTx_RX_AF;
#endif 
  
  HAL_GPIO_Init(WiFi_USART_PRINT_RX_GPIO_PORT, &GPIO_InitStruct);
  
#ifdef WIFI_USE_VCOM
  /*##-3- Configure the NVIC for UART ########################################*/
  /* NVIC for USART */
  HAL_NVIC_SetPriority(USARTx_PRINT_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(USARTx_PRINT_IRQn);
#endif
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 


/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/

