/**
 ******************************************************************************
 * @file    ring_buffer.c
 * @author  Central LAB
 * @version V2.0.0
 * @date    10-February-2016
 * @brief   Implements the Circular Buffer management of the Wi-Fi module
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

#include <stdio.h>
#include <stdlib.h>
#include "ring_buffer.h"
#include "wifi_module.h"

/** @addtogroup MIDDLEWARES
* @{
*/ 


/** @defgroup  NUCLEO_WIFI_UTILS
  * @brief Wi-Fi buffer utility
  * @{
  */ 


/** @defgroup NUCLEO_WIFI_UTILS_Private_Defines
  * @{
  */

/**
  * @}
  */

/** @defgroup NUCLEO_WIFI_UTILS_Private_Variables
  * @{
  */

extern uint8_t ring_buffer[RINGBUF_SIZE];
extern uint8_t pop_buffer[MAX_BUFFER_GLOBAL];
extern uint8_t prefetch_buffer[MAX_PREFETCH_BUFFER];
extern wifi_bool enable_receive_data_chunk;
extern uint32_t pop_buffer_size;

#define ELEMENT_SIZE 1

               
/**
  * @}
  */

/** @defgroup NUCLEO_WIFI_UTILS_Private_Functions
  * @{
  */

/**
  * @brief  init
  *         Initialize a circular buffer of type buffer_t
  * @param  None
  * @retval None
  */
void init(buffer_t *buffer, int size) 
{
    buffer->size = size;
    buffer->start = 0;
    buffer->count = 0;
    buffer->end = 0;
    buffer->element = ring_buffer;
}

/**
  * @brief  flush_buffer_queue
  *         flushes the buffer
  * @param  None
  * @retval None
  */ 
void flush_buffer_queue(buffer_t *buffer) 
{
  buffer->start = buffer->end;//the tail goes up to the head and buffer becomes empty
  buffer->count = 0;
}

/**
  * @brief  is_half_full
  *         checks if the buffer is half full (empty)
  * @param  None
  * @retval None
  */ 
int is_half_full(buffer_t *buffer)
{
  int bufsize = buffer->size;
  if (buffer->count >= bufsize - 100)
  {
    //printf("half full!");
    return 1;
  } 
  else 
  {
    return 0;
  }
}

/**
  * @brief  is_half_empty
  *         checks if the buffer is less than half
  * @param  None
  * @retval None
  */
int is_half_empty(buffer_t *buffer)
{
  //int bufsize = buffer->size;
  if (buffer->count <= 100)
  {
    return 1;
  }
  else 
  {
    return 0;
  }
}

/**
  * @brief  full
  *         indicates if the given buffer is full or not
  * @param  None
  * @retval None
  */
int full(buffer_t *buffer) 
{
  int bufsize = buffer->size;
  if (buffer->count == bufsize) 
  {
    return 1;
  }
  else 
  {
    return 0;
  }
}

/**
  * @brief  empty
  *         indicates if the given buffer is empty or not
  * @param  None
  * @retval None
  */
int empty(buffer_t *buffer) {
    if (buffer->count == 0) {
        return 1;
    } else {
        return 0;
    }
}

/**
  * @brief  prefetch_buffer_queue
  *         prefetches the pipeline upto xx bytes
  * @param  None
  * @retval None
  */ 
uint8_t * prefetch_buffer_queue(buffer_t *buffer)
{
  //int i = 0;
  uint8_t * element;
  int bufsize;

  memset(prefetch_buffer,0x00,MAX_PREFETCH_BUFFER);
  element = &prefetch_buffer[0];
  element[4] = '\0';

  if(!empty(buffer))
      {
        bufsize = (buffer->count);
        if(buffer->count <= MAX_PREFETCH_BUFFER)
            memcpy(element, &buffer->element[buffer->start], bufsize);
        else
            memcpy(element, &buffer->element[buffer->start], MAX_PREFETCH_BUFFER);
        return element;
      }
  else return NULL;
}

/**
  * @brief  push_buffer
  *         pushes a new item onto the circular buffer (queues it)
  * @param  None
  * @retval None
  */
void push_buffer(buffer_t *buffer, uint8_t *data) 
{
  int bufsize;

  if (full(buffer)) 
  {     
    //Buffer overflow and no more space
    //MPD: No Action taken here; in case of buffer overflow, do we need to overwrite last buffer?
    //printf("\r\nRing Buffer Full!!\r\n");
    //printf(data);
    return;
  } 
  else 
  {
    buffer->count++;    
    memcpy(&buffer->element[buffer->end], data, ELEMENT_SIZE);
    buffer->end = buffer->end + ELEMENT_SIZE;
    
    //wrap around if max size is reached
    bufsize = (buffer->size);
    if (buffer->end >= bufsize) 
    {
      buffer->end = 0;
    }
  }
}
 
/**
  * @brief  pop_buffer_queue
  *         dequeues the circular buffer
  * @param  None
  * @retval None
  */ 
uint8_t * pop_buffer_queue(buffer_t *buffer) 
{
  uint8_t * element;
  int bufsize;

  element = &pop_buffer[0];
  if (empty(buffer)) 
  {
    //printf("\r\nRing Buffer Empty!!\r\n");
    return NULL;
  } 
  else 
  {
      memset(pop_buffer, 0x00 , MAX_BUFFER_GLOBAL);
      if(enable_receive_data_chunk)
      {
        int buf_end = buffer->end;
        // int buf_start = buffer->start;
        if(buffer->count < 512)
        {
            pop_buffer_size = buffer->count;
            if(buf_end >= buffer->start)
            {
                  memcpy(element, &buffer->element[buffer->start], pop_buffer_size);
            }
            else
            {
                  int buf_start = buffer->start;
                  memcpy(element, &buffer->element[buffer->start], 1024 - buf_start);
                  memcpy(element+(1024-buffer->start), &buffer->element[0], buf_end);
            }
            buffer->start = buffer->end;
            buffer->count = 0;
        }
        else
        {
            if(buf_end >= buffer->start)
            {
                  memcpy(element, &buffer->element[buffer->start], 511);
                  buffer->start = buffer->start + 511;
                  buffer->count = buf_end - buffer->start;
            }
            else
            {
                  if(buffer->start + 511 < 1024)
                  {
                      memcpy(element, &buffer->element[buffer->start], 511);
                      buffer->start = buffer->start + 511;
                      buffer->count = (1024 - buffer->start)+ buf_end;
                  }
                  else
                  {
                      int buf_start = buffer->start;
                      memcpy(element, &buffer->element[buffer->start], 1024-buf_start);
                      memcpy(element+(1024-buffer->start), &buffer->element[0], buf_start-513);  //(buffer->start + 511) - 1024;
                      buffer->start = (buffer->start-513);
                      buffer->count = buf_end - buffer->start;
                  }
            }
            pop_buffer_size = 511;
        }
    }
    else
    {
        /* First in First Out*/
        memcpy(element, &buffer->element[buffer->start], ELEMENT_SIZE);
        buffer->start = buffer->start + ELEMENT_SIZE;
        buffer->count--;
        pop_buffer_size = 1;
        bufsize = (buffer->size);
        if (buffer->start >= bufsize)
        {
          buffer->start = 0;
        } 
    }
    return element;
  }
}

/**
  * @brief  rewinds_buffer_queue
  *         rewinds the circular buffer
  * @param  None
  * @retval None
  */ 
void rewind_buffer_queue(buffer_t *buffer , int count)
{
    int buf_end = buffer->end;
    if(buffer->start - count >= 0)
    {
        buffer->start = buffer->start - count;
        if(buf_end > buffer->start) {
          buffer->count = buf_end - buffer->start;
        }
        else {
          buffer->count = (1024-buffer->start)+buf_end;
        }
    }
    else
    {
        buffer->start = 1024 - (count - buffer->start);
        buffer->count = (1024 - buffer->start)+ buf_end;
    }
}

/**
  * @}
  */ 

/**
  * @}
  */ 


/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
