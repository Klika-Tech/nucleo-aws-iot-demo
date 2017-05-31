  /**
  ******************************************************************************
  * @file    ring_buffer.h
  * @author  Central LAB
  * @version V2.0.0
  * @date    10-February-2016
  * @brief   Header File for Circular Buffer management of the Wi-Fi module
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
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef __RING_BUFFER_H
#define __RING_BUFFER_H

/** @addtogroup BSP
* @{
*/ 


/** @defgroup  NUCLEO_WIFI_BUFFER_MGMT 
  * @brief Wi-Fi_driver modules
  * @{
  */ 


/** @defgroup NUCLEO_WIFI_BUFFER_MGMT_Private_TypesDefinitions
  * @{
  */

/**
  * @}
  */

/** @defgroup NUCLEO_WIFI_BUFFER_MGMT_Private_Defines
  * @{
  */

/**
  * @}
  */

/** @defgroup NUCLEO_WIFI_BUFFER_MGMT_Private_Macros
  * @{
  */



/**
  * @}
  */

/** @defgroup NUCLEO_WIFI_BUFFER_MGMT_Private_Variables
  * @{
  */

struct buffer {    
    volatile int start; // position of first data from USART
    volatile int end;  // position of last data from USART
    volatile int size; //Max size in terms of number of data packets (Total Bytes/size of each packet (8 bytes))
    volatile int count; // number of currently filled data packets (=size if full & =0 if empty)
    
    /*unsigned main buffer pointer*/
    uint8_t *element;
};
 
typedef struct buffer buffer_t;

void init(buffer_t *buffer, int size);
void flush_buffer_queue(buffer_t *buffer);
int is_half_full(buffer_t *buffer);
int is_half_empty(buffer_t *buffer);
int full(buffer_t *buffer);
int empty(buffer_t *buffer);
void push_buffer(buffer_t *buffer, uint8_t *data);
uint8_t * pop_buffer_queue(buffer_t *buffer);
uint8_t * popstack(buffer_t *buffer);
uint8_t * prefetch_buffer_queue(buffer_t *buffer);
void rewind_buffer_queue(buffer_t *buffer , int count);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
  
/**
  * @}
  */



#endif
