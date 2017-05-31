  /**
  ******************************************************************************
  * @file    stm32_spwf_wifi.h
  * @author  Central LAB
  * @version V2.0.0
  * @date    10-February-2016
  * @brief   Header file for HAL related functionality of X-CUBE-WIFI1
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __WIFI_MODULE_CONF_H
#define __WIFI_MODULE_CONF_H

/* Includes ------------------------------------------------------------------*/

#ifdef USE_STM32L0XX_NUCLEO
#include "stm32l0xx_hal.h"
#include "stm32l0xx_nucleo.h"
#include "stm32l0xx_hal_rcc.h"
#include "stm32l0xx_hal_rcc_ex.h"
#endif

#ifdef USE_STM32F4XX_NUCLEO  
#include "stm32f4xx_hal.h"
#include "stm32f4xx_nucleo.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_rcc_ex.h"
#endif

#ifdef USE_STM32F1xx_NUCLEO
#include "stm32f1xx_hal.h"
#include "stm32f1xx_nucleo.h"
#include "stm32f1xx_hal_rcc.h"
#include "stm32f1xx_hal_rcc_ex.h"
#endif

#ifdef USE_STM32L4XX_NUCLEO
#include "stm32l4xx_hal.h"
#include "stm32l4xx_nucleo.h"
#include "stm32l4xx_hal_rcc.h"
#include "stm32l4xx_hal_rcc_ex.h"
#endif

/* Exported macro ------------------------------------------------------------*/
#define DEBUG_PRINT 0
#define WiFi_ENABLE
#define FW_UPDATE_PACKET_SIZE   252
#define MAX_PREFETCH_BUFFER 20    

/********** TimeOUT *******************/
#define WIFI_TIME_OUT           40000   //4 sec
#define WIFI_HTTP_REQUEST_INTERVAL      60 //6sec

/********** Wi-Fi *******************/
#ifdef USART_PRINT_MSG

#ifdef USE_STM32L0XX_NUCLEO  

#define WIFI_UART_MSG                           USART2
#define USARTx_PRINT_CLK_ENABLE()              __USART2_CLK_ENABLE()
#define USARTx_PRINT_RX_GPIO_CLK_ENABLE()      __GPIOA_CLK_ENABLE()
#define USARTx_PRINT_TX_GPIO_CLK_ENABLE()      __GPIOA_CLK_ENABLE()

#define USARTx_PRINT_FORCE_RESET()             __USART2_FORCE_RESET()
#define USARTx_PRINT_RELEASE_RESET()           __USART2_RELEASE_RESET()

#define PRINTMSG_USARTx_TX_AF                       GPIO_AF4_USART2
#define PRINTMSG_USARTx_RX_AF                       GPIO_AF4_USART2

#endif //USE_STM32L0XX_NUCLEO

#if defined (USE_STM32F1xx_NUCLEO) || defined (USE_STM32F4XX_NUCLEO) || defined (USE_STM32L4XX_NUCLEO)

#define WIFI_UART_MSG                           USART2
#define USARTx_PRINT_CLK_ENABLE()              __HAL_RCC_USART2_CLK_ENABLE()
#define USARTx_PRINT_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_PRINT_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define USARTx_PRINT_FORCE_RESET()             __HAL_RCC_USART2_FORCE_RESET()
#define USARTx_PRINT_RELEASE_RESET()           __HAL_RCC_USART2_RELEASE_RESET()

#define PRINTMSG_USARTx_TX_AF                       GPIO_AF7_USART2
#define PRINTMSG_USARTx_RX_AF                       GPIO_AF7_USART2

#endif //(USE_STM32F1xx_NUCLEO) || (USE_STM32F4XX_NUCLEO)

#define WiFi_USART_PRINT_TX_PIN                    GPIO_PIN_2
#define WiFi_USART_PRINT_TX_GPIO_PORT              GPIOA
#define WiFi_USART_PRINT_RX_PIN                    GPIO_PIN_3
#define WiFi_USART_PRINT_RX_GPIO_PORT              GPIOA


/* Definition for USARTx's NVIC */
#define USARTx_PRINT_IRQn                      USART2_IRQn
#define USARTx_PRINT_IRQHandler                USART2_IRQHandler

#endif //USART_PRINT_MSG


/* User can use this section to tailor USARTx/UARTx instance used and associated 
resources */
/* Definition for USARTx clock resources */
#ifdef USE_STM32L0XX_NUCLEO
#if 0
#define WB_WIFI_UART                     LPUART1 
#define USARTx_CLK_ENABLE()              __LPUART1_CLK_ENABLE() 
#define USARTx_RX_GPIO_CLK_ENABLE()      __GPIOC_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __GPIOC_CLK_ENABLE()

#define USARTx_FORCE_RESET()             __LPUART1_FORCE_RESET() 
#define USARTx_RELEASE_RESET()           __LPUART1_RELEASE_RESET() 
#endif
#if 1 /*For exp hardware revision*/
#define WB_WIFI_UART                     USART1 
#define USARTx_CLK_ENABLE()              __USART1_CLK_ENABLE() 
#define USARTx_RX_GPIO_CLK_ENABLE()      __GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __GPIOA_CLK_ENABLE()

#define USARTx_FORCE_RESET()             __USART1_FORCE_RESET() 
#define USARTx_RELEASE_RESET()           __USART1_RELEASE_RESET() 
#endif
#endif //USE_STM32L0XX_NUCLEO  

#ifdef USE_STM32F1xx_NUCLEO

#define WB_WIFI_UART                     USART1
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE()
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define USARTx_FORCE_RESET()             __HAL_RCC_USART1_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __HAL_RCC_USART1_RELEASE_RESET()

#endif //USE_STM32F1xx_NUCLEO

#ifdef USE_STM32F4XX_NUCLEO

#define WB_WIFI_UART                     USART1
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE()
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define USARTx_FORCE_RESET()             __HAL_RCC_USART1_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __HAL_RCC_USART1_RELEASE_RESET()

#endif //USE_STM32F4XX_NUCLEO  

#ifdef USE_STM32L4XX_NUCLEO
#define WB_WIFI_UART                     USART1
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE()
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define USARTx_FORCE_RESET()             __HAL_RCC_USART1_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __HAL_RCC_USART1_RELEASE_RESET()

#endif //USE_STM32L4XX_NUCLEO 

/* Definition for USARTx Pins */
#define WiFi_USART_TX_PIN                    GPIO_PIN_9
#define WiFi_USART_TX_GPIO_PORT              GPIOA
#define WiFi_USART_RX_PIN                    GPIO_PIN_10
#define WiFi_USART_RX_GPIO_PORT              GPIOA

#define WiFi_USART_RTS_PIN                    GPIO_PIN_12
#define WiFi_USART_RTS_GPIO_PORT              GPIOA
#define WiFi_USART_CTS_PIN                    GPIO_PIN_11
#define WiFi_USART_CTS_GPIO_PORT              GPIOA

#ifdef USE_STM32L0XX_NUCLEO
#if 0
#define WiFi_USARTx_TX_AF                    GPIO_AF0_LPUART1
#define WiFi_USARTx_RX_AF                    GPIO_AF0_LPUART1
#endif
#if 1 /*For exp hardware revision*/
#define WiFi_USARTx_TX_AF                    GPIO_AF4_USART1
#define WiFi_USARTx_RX_AF                    GPIO_AF4_USART1
#endif

#endif    

#if defined (USE_STM32L4XX_NUCLEO) || defined(USE_STM32F4XX_NUCLEO)
#define WiFi_USARTx_TX_AF                    GPIO_AF7_USART1
#define WiFi_USARTx_RX_AF                    GPIO_AF7_USART1
#endif    

#if 1
#define  WiFi_RESET_GPIO_PIN              GPIO_PIN_12
#else
#define  WiFi_RESET_GPIO_PIN              GPIO_PIN_0
#endif

#define  WiFi_RESET_GPIO_PORT              GPIOC /*PC12*/

#define  WiFi_WAKEUP_GPIO_PIN              GPIO_PIN_8
#define  WiFi_WAKEUP_GPIO_PORT             GPIOC /*PC8*/

#if defined (USE_STM32L4XX_NUCLEO) || defined (USE_STM32F4XX_NUCLEO)
#define RESET_WAKEUP_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()
#endif

#ifdef USE_STM32F1xx_NUCLEO
#define RESET_WAKEUP_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()
#endif

#ifdef USE_STM32L0XX_NUCLEO
#define RESET_WAKEUP_GPIO_CLK_ENABLE()      __GPIOC_CLK_ENABLE()
#endif

/* Definition for USARTx's NVIC */
#ifdef USE_STM32L0XX_NUCLEO
#if 0
#define USARTx_IRQn                     LPUART1_IRQn 
#define USARTx_IRQHandler               LPUART1_IRQHandler
#endif
#if 1 /*For exp hardware revision*/
#define USARTx_IRQn                     USART1_IRQn 
#define USARTx_IRQHandler               USART1_IRQHandler
#endif
#define USARTx_EXTI_IRQn                EXTI4_15_IRQn
#define USARTx_EXTI_IRQHandler          EXTI4_15_IRQHandler

#endif //USE_STM32L0XX_NUCLEO
#ifdef USE_STM32F1xx_NUCLEO
#define USARTx_IRQn                     USART1_IRQn
#define USARTx_IRQHandler               USART1_IRQHandler

//#define USARTx_EXTI_IRQn                EXTI15_10_IRQn
//#define USARTx_EXTI_IRQHandler          EXTI15_10_IRQHandler

#endif //USE_STM32L0XX_NUCLEO
#if defined (USE_STM32L4XX_NUCLEO) || defined(USE_STM32F4XX_NUCLEO)
#define USARTx_IRQn                     USART1_IRQn
#define USARTx_IRQHandler               USART1_IRQHandler

#define USARTx_EXTI_IRQn                EXTI15_10_IRQn
#define USARTx_EXTI_IRQHandler          EXTI15_10_IRQHandler

#endif //USE_STM32F4XX_NUCLEO

#if defined (USE_STM32F1xx_NUCLEO)
#define TIMx                           TIM3
#define TIMx_CLK_ENABLE()              __HAL_RCC_TIM3_CLK_ENABLE()
#define TIMp                           TIM2
#define TIMp_CLK_ENABLE()              __HAL_RCC_TIM2_CLK_ENABLE()

/* Definition for TIMx's NVIC */
#define TIMx_IRQn                      TIM3_IRQn
#define TIMx_IRQHandler                TIM3_IRQHandler
#define TIMp_IRQn                      TIM2_IRQn
#define TIMp_IRQHandler                TIM2_IRQHandler
#endif

#if defined (USE_STM32L4XX_NUCLEO) || defined(USE_STM32F4XX_NUCLEO)
#define TIMx                           TIM3
#define TIMx_CLK_ENABLE()              __HAL_RCC_TIM3_CLK_ENABLE()
#define TIMp                           TIM2
#define TIMp_CLK_ENABLE()              __HAL_RCC_TIM2_CLK_ENABLE()

/* Definition for TIMx's NVIC */
#define TIMx_IRQn                      TIM3_IRQn
#define TIMx_IRQHandler                TIM3_IRQHandler
#define TIMp_IRQn                      TIM2_IRQn
#define TIMp_IRQHandler                TIM2_IRQHandler
#endif

#ifdef USE_STM32L0XX_NUCLEO
#define TIMx                           TIM2
#define TIMx_CLK_ENABLE()              __TIM2_CLK_ENABLE()
#define TIMp                           TIM21
#define TIMp_CLK_ENABLE()              __TIM21_CLK_ENABLE()

/* Definition for TIMx's NVIC */
#define TIMx_IRQn                      TIM2_IRQn
#define TIMx_IRQHandler                TIM2_IRQHandler
#define TIMp_IRQn                      TIM21_IRQn
#define TIMp_IRQHandler                TIM21_IRQHandler
#endif

/* Exported functions ------------------------------------------------------- */
void Push_Timer_Config(void);

/* Size of Trasmission buffer */
#define TXBUFFERSIZE                    (COUNTOF(aTxBuffer) - 1)
/* Size of Reception buffer */
#define RXBUFFERSIZE                    TXBUFFERSIZE

#define  WiFi_USART_BAUD_RATE                115200

#endif //__WIFI_MODULE_CONF_H
