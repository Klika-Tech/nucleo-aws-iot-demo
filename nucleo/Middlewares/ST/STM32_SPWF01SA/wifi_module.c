/**
 ******************************************************************************
 * @file    wifi_module.c
 * @author  Central LAB
 * @version V2.0.0
 * @date    10-February-2016
 * @brief   Enable Wi-Fi functionality using AT cmd set
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
//#include "stdio.h"
#include "string.h"
#include "event_buffer.h"

/** @addtogroup MIDDLEWARES
* @{
*/ 


/** @defgroup  NUCLEO_WIFI_MODULE
  * @brief Wi-Fi driver modules
  * @{
  */


/** @defgroup NUCLEO_WIFI_MODULE_Private_Defines
  * @{
  */


/**
  * @}
  */


/** @addtogroup NUCLEO_WIFI_MODULE_Private_Variables
  * @{
  */
/* Private variables ---------------------------------------------------------*/

/*All Buffers*/
//uint8_t WiFi_AT_Cmd_Buff[1024];
uint8_t process_buffer[MAX_BUFFER_GLOBAL];
char UserDataBuff[MAX_BUFFER_GLOBAL];
uint8_t USART_RxBuffer[64];//This buffer is only used in the Init phase (to receive "\r\nOK\r\n")
char print_msg_buff[MAX_BUFFER_GLOBAL];
char receive_msg_buff[MAX_BUFFER_GLOBAL];
volatile uint8_t ring_buffer[RINGBUF_SIZE];/* Default size for ring buffer */

#ifdef USE_STM32L0XX_NUCLEO
event_s_TypeDef event_buffer[10];
#else
event_s_TypeDef event_buffer[50];
#endif

volatile uint8_t pop_buffer[MAX_BUFFER_GLOBAL];
volatile uint8_t prefetch_buffer[MAX_PREFETCH_BUFFER];
uint8_t uart_byte[1];
buffer_t big_buff;
buffer_e event_buff;
event_s_TypeDef element;
uint8_t HTTP_Runway_Buff[6];//Used to store the last 6 bytes in between User Callbacks during HTTP tx
wifi_scan *wifi_scanned_list; //[MAX_WIFI_SCAN_NETWORK];

__IO uint32_t packet_sent=WIFI_TRUE;
__IO wifi_bool WiFi_Connected = WIFI_FALSE;
__IO wifi_bool IsBuffer1Full = WIFI_FALSE,IsBuffer2Full = WIFI_FALSE; 
__IO uint8_t	uTimeOut;
__IO ITStatus UartReady = RESET;
__IO ITStatus Uart2Ready = RESET;
__IO ITStatus TxUartReady = RESET;

wifi_bool USART2_MesgReadyBit = WIFI_FALSE;
wifi_bool Received_HTTPResponce = WIFI_FALSE;
wifi_bool Single_Digit_Indication = WIFI_FALSE;
wifi_bool WiFi_Enabled =WIFI_FALSE;
wifi_bool http_req_pending = WIFI_FALSE;
wifi_bool WiFi_In_Config_Mode = WIFI_FALSE;
wifi_bool WiFi_Configuration_Done = WIFI_FALSE;
wifi_bool IsConsoleActive = WIFI_FALSE ;
wifi_bool process_Indication_Completed = WIFI_TRUE,process_WiFi_Resp_Completed = WIFI_FALSE;
wifi_bool Timer_Running = WIFI_FALSE;
wifi_bool enable_dequeue = WIFI_TRUE;
wifi_bool stop_event_dequeue = WIFI_FALSE;
wifi_bool Standby_Timer_Running = WIFI_FALSE;
wifi_bool trigger_wakeup_callback = WIFI_FALSE;
wifi_bool Deep_Sleep_Enabled = WIFI_FALSE;
wifi_bool Standby_Enabled = WIFI_FALSE;
wifi_bool Low_Power_Enabled = WIFI_FALSE;
wifi_bool command_mode=WIFI_TRUE;
wifi_bool data_mode=WIFI_FALSE;
wifi_bool Scan_Ongoing = WIFI_FALSE;
wifi_bool AT_Cmd_Ongoing = WIFI_FALSE;
wifi_bool AT_Cmd_Processing = WIFI_FALSE;
wifi_bool Uartx_Rx_Processing = WIFI_FALSE;
wifi_bool Client_Connected = WIFI_FALSE;
wifi_bool Client_Disconnected = WIFI_FALSE;
wifi_bool switch_by_default_to_command_mode = WIFI_TRUE;
wifi_bool start_sock_read = WIFI_FALSE;
wifi_bool not_wind_message = WIFI_TRUE;
wifi_bool enable_receive_data_chunk = WIFI_FALSE;

wifi_bool data_pending_sockD=WIFI_FALSE;
wifi_bool enable_sock_read = WIFI_FALSE;
wifi_bool enable_query = WIFI_FALSE;
wifi_bool encoded_data_read = WIFI_FALSE;
wifi_bool Set_AT_Cmd_Response_False = WIFI_FALSE;
wifi_bool enable_fw_update_read = WIFI_FALSE;
wifi_bool Q_Contains_Message = WIFI_FALSE;
wifi_bool Q_Contains_Data = WIFI_FALSE;
wifi_bool enable_receive_http_response = WIFI_FALSE;
wifi_bool enable_receive_file_response = WIFI_FALSE;
wifi_bool enable_receive_wifi_scan_response = WIFI_FALSE;
wifi_bool prevent_push_OK_event = WIFI_FALSE;
wifi_bool client_socket_close_ongoing = WIFI_FALSE;
wifi_bool prevent_push_WIFI_event = WIFI_FALSE;
wifi_bool sock_read_ongoing = WIFI_FALSE;
wifi_bool enable_client_socket_write = WIFI_FALSE;
wifi_bool event_deQ_x_wind64 = WIFI_FALSE;
wifi_bool do_not_reset_push_WIFI_event = WIFI_FALSE;
wifi_bool message_pending = WIFI_FALSE;

uint32_t wind64_DQ_wait = 0;
uint32_t UserDataBuff_index;
volatile uint32_t tickcount;
uint8_t SocketId;
uint32_t SockON_Data_Len;
uint32_t Socket_Data_Length =0;
uint8_t Socket_Open_ID, sockon_query_id,sockon_id_user;
uint32_t SockON_Data_Length;
uint8_t enable_pending_data =0;
wifi_bool wifi_write_ongoing = WIFI_FALSE;

UART_HandleTypeDef UartWiFiHandle,UartMsgHandle;
WiFi_Config_HandleTypeDef WiFi_Config_Variables;

float fValue = 0;

extern TIM_HandleTypeDef    TimHandle;

__IO WiFi_WIND_State_TypeDef WiFi_WIND_State;//describes the current WIND number in processing

uint8_t WiFi_Resp_OK = 0;
uint32_t number_of_bytes=0;
uint32_t interim_number_of_bytes=0;
uint32_t Interim_SockON_Data_Len=0;
uint32_t bytes_to_be_read = 0;
uint32_t sock_total_count=0;
uint32_t sockD_total_count=0;
uint32_t ip_fragment_count=0;
uint32_t chunk_size;
uint32_t message_size;
uint32_t WIND55_count=0;
uint32_t WIND64_count=0;
uint8_t user_scan_number;
uint32_t pop_queue_length;
uint32_t pop_buffer_size=0;
uint32_t process_buffer_index = 5;
uint32_t epoch_time = 0;

WiFi_AT_CMD_Response_t WiFi_Module_State;
wifi_bool tempcheck = WIFI_FALSE;
uint8_t *WiFi_Scan_Buffer;
uint8_t * curr_hostname;
uint8_t * curr_path;
uint8_t * curr_pURL;
uint8_t * curr_protocol;
uint32_t  curr_port_number;
char * curr_data;
uint8_t curr_sockID;
uint8_t * curr_filename;
uint16_t curr_DataLength;
uint16_t sock_server_write_datalength;
char *sock_server_write_pdata;

wifi_bool Pending_SockON_Callback = WIFI_FALSE;
wifi_bool Pending_SockD_Callback = WIFI_FALSE;
wifi_bool SockON_Server_Closed_Callback = WIFI_FALSE;
wifi_bool Client_Socket_Close_Cmd = WIFI_FALSE;
wifi_bool standby_resume_callback = WIFI_FALSE;
wifi_bool HTTP_Data_available = WIFI_FALSE;
wifi_bool FILE_Data_available = WIFI_FALSE;
uint8_t socket_closed_id_callback;
uint8_t remote_socket_closed_id;
uint8_t client_socket_close_id;
volatile uint8_t wifi_ready = WIFI_FALSE;//Set once if wifi is ready for first time
volatile uint8_t wifi_connected = WIFI_FALSE;//Set once if wifi is connected for first time
volatile uint8_t wifi_client_connected = 0;//Set once if client is connected
volatile uint8_t wifi_client_disconnected = 0;//Set once if client is dis-connected
uint8_t gpio_value, gpio_dir, get_cfg_value[64];
WiFi_Status_t user_error_code = WiFi_MODULE_SUCCESS;
HAL_StatusTypeDef receive_status;
uint8_t no_of_open_client_sockets = 0;
wifi_bool open_sockets[8];              //Max open sockets allowed is 8. Each array element depicts one socket (true=open, false=closed)
uint8_t client_MAC_address[17];//current client MAC address store

typedef enum {
start_r_detected = 0x00,
started,
stopped
} process_state;

uint8_t line_pointer = 0;
process_state state = stopped;
uint8_t * temp;//pop buffer temporary
uint8_t enable_uart_byte_data_receive=1;
uint8_t uart_data_receive_ready=1;
wifi_bool resume_receive_data = WIFI_FALSE;

WiFi_Status_t AT_RESPONSE = WiFi_AT_CMD_RESP_ERROR;
__IO wifi_bool AT_Response_Received = WIFI_FALSE;
wifi_bool Received_Last_Byte_CB = WIFI_FALSE;
wifi_bool Deep_Sleep_Timer = WIFI_FALSE;
wifi_bool Deep_Sleep_Callback = WIFI_FALSE;
uint32_t sleep_count = 0;
uint32_t standby_time = 0;
uint8_t scanned_ssids = 0;
char * prefetch_str;

GPIO_InitTypeDef  GPIO_InitStruct;
GPIO_InitTypeDef  WAKEUP_InitStruct;

#if defined (USE_STM32F4XX_NUCLEO) || (USE_STM32L4XX_NUCLEO)
uint8_t WiFi_AT_Cmd_Buff[2048];
#else
uint8_t WiFi_AT_Cmd_Buff[1024];
#endif

#ifdef USART_PRINT_MSG

#define printf(arg...)    {sprintf((char*)print_msg_buff, arg);   \
HAL_UART_Transmit(&UartMsgHandle, (uint8_t*)print_msg_buff, strlen(print_msg_buff), 1000);}

#endif

#if defined (__CC_ARM)
size_t strnlen (const char* s, size_t maxlen);

size_t strnlen (const char* s, size_t maxlen)
    {
    size_t len = 0;

    while ((len <= maxlen) && (*s))
        {
            s++;
            len++;
        }

    return len;
    }
#endif

event_s_TypeDef event_s;
event_s_TypeDef * event_pop_s;

// [DLI]
#ifdef WIFI_USE_VCOM

uint8_t console_listen_char[1];
uint8_t console_input_char[1];
uint8_t console_send_char[1];
uint8_t console_echo_char[1];
__IO ITStatus console_send_ready = RESET;
__IO ITStatus console_echo_ready = SET;
__IO ITStatus console_push_ready = RESET;

// Virtual-COM UART
void console_input() {
  HAL_UART_Receive_IT(&UartMsgHandle, (uint8_t *)console_input_char, 1);
}

void wifi_vcom() {

  if (console_push_ready == SET) {
     push_buffer(&big_buff, uart_byte);
     console_push_ready = RESET;
     HAL_UART_Receive_IT(&UartWiFiHandle, (uint8_t *)uart_byte, 1);
  }
 if(console_echo_ready == SET) {
    temp = pop_buffer_queue(&big_buff);
    if(temp != NULL) {
      console_echo_ready = RESET;
      HAL_UART_Transmit_IT(&UartMsgHandle, temp, 1);
    }
 }
}

#endif

/**
  * @}
  */
  
/** @defgroup NUCLEO_WIFI_MODULE_Private_Functions
  * @{
  */

/**
  * @brief  WiFi_Module_Init
  *         Initialize wifi module
  * @param  None
  * @retval None
  */
void WiFi_Module_Init(void)
{
#ifdef WIFI_USE_VCOM
  console_input();
#else
  WiFi_Module_State = Process_Event;
#endif  
  init(&big_buff, 1024);//Init the ring buffer  
  wifi_ready = 0; //reset to get user callback on HW started
  wifi_connected = 0; //reset to get user callback on WiFi UP
  Receive_Data();
  
#ifdef USE_STM32L0XX_NUCLEO
  event_init(&event_buff, 10); //max 15 events can be Q'ed (Event Buffer is of size 15)
#else
  event_init(&event_buff, 50); //max 50 events can be Q'ed (Event Buffer is of size 50)
#endif
  
#ifndef WIFI_USE_VCOM
  Start_Timer();  
  memset(open_sockets,0x00, 8); //init the open socket array
  
  if (HAL_TIM_Base_Start_IT(&TimHandle) != HAL_OK)//Start the TIM timer
  {
    #if DEBUG_PRINT
    printf("Error");
    #endif
    Error_Handler();
  }
#endif
}

/**
* @brief  WiFi_Configuration
*         Default Wifi configuration parameters
* @param  None
* @retval None
*/
void WiFi_Configuration()
{
  WiFi_Config_Variables.blink_led               = 1;
  WiFi_Config_Variables.ip_use_dhcp             = 1 ;

  /* Set the network privacy mode 
    (0=none, 1=WEP, 2=WPA-Personal (TKIP/AES) or WPA2-Personal (TKIP/AES)) */  
  WiFi_Config_Variables.wifi_mode               =  WiFi_STA_MODE;
  WiFi_Config_Variables.wifi_priv_mode          =  WPA_Personal;
  WiFi_Config_Variables.wifi_ssid               =  "NETGEAR54" ;
  WiFi_Config_Variables.Wifi_Sec_key            =  "12341234";
  
   /*Power Management Settings*/
  WiFi_Config_Variables.sleep_enabled           = 0;//0=disabled, 1=enabled
  WiFi_Config_Variables.standby_enabled         = 1;
  WiFi_Config_Variables.standby_time            = 10;//in seconds
  WiFi_Config_Variables.wifi_powersave          = 1;//0=Active, 1=PS Mode, 2=Fast-PS Mode
  WiFi_Config_Variables.wifi_operational_mode   = 11;//11= Doze mode, 12= Quiescent mode
  WiFi_Config_Variables.wifi_listen_interval    = 0; //Wakeup every n beacon
  WiFi_Config_Variables.wifi_beacon_wakeup      = 1;         
}

/**
* @brief  wifi_reset
*         Reset WiFi module using PC12 gpio pin
* @param  None
* @retval None
*/
void wifi_reset(void)
{  
  RESET_WAKEUP_GPIO_CLK_ENABLE();
  __GPIOA_CLK_ENABLE();

  GPIO_InitStruct.Pin       = WiFi_RESET_GPIO_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;

  HAL_GPIO_Init(WiFi_RESET_GPIO_PORT, &GPIO_InitStruct);

  WiFi_Module_State = Process_Event; 
  WiFi_WIND_State.WiFiHWStarted = WIFI_FALSE;
  wifi_connected = 0; //reset wifi_connected to get user callback
  memset((void*)&WiFi_WIND_State,0x00,sizeof(WiFi_WIND_State)); /*reset the WIND State?*/

  /* ===   RESET PIN - PC12   ===*/
  HAL_GPIO_WritePin(WiFi_RESET_GPIO_PORT, WiFi_RESET_GPIO_PIN, GPIO_PIN_RESET);  
  HAL_Delay(50);  
  /* ===   SET PIN - PC12   ===*/
  HAL_GPIO_WritePin(WiFi_RESET_GPIO_PORT, WiFi_RESET_GPIO_PIN, GPIO_PIN_SET);
  HAL_Delay(50);
  HAL_GPIO_DeInit(WiFi_RESET_GPIO_PORT, WiFi_RESET_GPIO_PIN); 

  while(WiFi_WIND_State.WiFiHWStarted != WIFI_TRUE) {}
}

/**
* @brief  PowerUp_WiFi_Module
*         Power up Wi-Fi module,SET GPIO PA0 pin 
* @param  None
* @retval None
*/
void PowerUp_WiFi_Module(void)
{
  /* ===   SET PIN - PC12   ===*/
  HAL_GPIO_WritePin(WiFi_RESET_GPIO_PORT, WiFi_RESET_GPIO_PIN, GPIO_PIN_SET);
}

/**
* @brief  Receive_Data
*         Receive data from UART port
* @param  uint8_t number of bytes to be received
* @retval None
*/
void Receive_Data(void)
{
  HAL_GPIO_WritePin(WiFi_USART_RTS_GPIO_PORT, WiFi_USART_RTS_PIN, GPIO_PIN_RESET);//Assert RTS
  receive_status = HAL_UART_Receive_IT(&UartWiFiHandle, (uint8_t *)uart_byte, 1);
  if(receive_status!=HAL_OK)
    {
      #if DEBUG_PRINT
      printf("HAL_UARTx_Receive_IT Error");
      #endif
    }
  else {
    Uartx_Rx_Processing = WIFI_TRUE;
  }
}

/**
* @brief  Period elapsed callback in non blocking mode
*         This timer is used for calling back User registered functions with information
* @param  htim : TIM handle
* @retval None
*/
void Wifi_TIM_Handler(TIM_HandleTypeDef *htim)
{ 
  /**********************************************************************
  *                                                                     *
  *       Be careful not to make a blocking                             *
  *       call from this function, see                                  *
  *       example Socket_Read() and Socket_Close()                      *
  *                                                                     *
  **********************************************************************/
    WiFi_Status_t status = WiFi_MODULE_SUCCESS;

    if(stop_event_dequeue == WIFI_FALSE /*&& !enable_receive_data_chunk*/)
    {
        __disable_irq();
        event_pop_s = pop_eventbuffer_queue(&event_buff);
        __enable_irq();

        if(event_pop_s!=NULL && event_pop_s->event_pop == WIFI_TRUE)
          {
            switch(event_pop_s->event)
              {
                case WIFI_WIND_EVENT:
                        Process_WiFi_Indication_Cmd(event_pop_s);
                        break;

                case WIFI_OK_EVENT:
                        AT_Response_Received = WIFI_TRUE;
                        AT_RESPONSE = WiFi_MODULE_SUCCESS;
                        if(Set_AT_Cmd_Response_False == WIFI_TRUE)
                        {
                          Set_AT_Cmd_Response_False = WIFI_FALSE;
                          AT_Response_Received = WIFI_FALSE;
                        }
                        if(enable_client_socket_write)  //allows the OK received after socket client write to be processed
                        {
                          AT_Response_Received = WIFI_TRUE;
                          enable_client_socket_write = WIFI_FALSE;
                        }
                        break;

                case WIFI_SOCK_ID_EVENT:
                        /*check ID and update SocketID array*/
                        no_of_open_client_sockets++;

                        if(no_of_open_client_sockets > 8)  //Max number of clients is 8
                          {
                              AT_Response_Received = WIFI_TRUE;
                              AT_RESPONSE = WiFi_NOT_SUPPORTED;     
                              break;
                          }

                        open_sockets[event_pop_s->socket_id] = WIFI_TRUE;
                        Socket_Open_ID = event_pop_s->socket_id;
                        AT_Response_Received = WIFI_TRUE;
                        AT_RESPONSE = WiFi_MODULE_SUCCESS;
                        break;

                case WIFI_HTTP_EVENT:
                        Reset_AT_CMD_Buffer();  
                        memset(UserDataBuff, 0x00, MAX_BUFFER_GLOBAL);//Flush the buffer

                        if(curr_pURL) {
                            sprintf((char*)WiFi_AT_Cmd_Buff,AT_HTTPPOST_REQUEST,curr_pURL);
                        }
                        else {
                          if(curr_port_number!=0)
                            sprintf((char*)WiFi_AT_Cmd_Buff,"AT+S.HTTPGET=%s,%s,%d\r",curr_hostname, curr_path, (int)curr_port_number);
                          else 
                            sprintf((char*)WiFi_AT_Cmd_Buff,"AT+S.HTTPGET=%s,%s\r",curr_hostname, curr_path);
                        }

                        status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
                        if(status == WiFi_MODULE_SUCCESS) {
                          WiFi_Module_State = Process_Event;
                          stop_event_dequeue = WIFI_TRUE;
                          http_req_pending = WIFI_TRUE;                            
                        }
                        else {
                          #if DEBUG_PRINT
                            printf("\r\n ERROR IN HTTP \r\n");
                          #endif
                          AT_RESPONSE = WiFi_AT_CMD_RESP_ERROR;
                          AT_Response_Received = WIFI_TRUE;
                        }
                        break;

                case WIFI_CLIENT_SOCKET_WRITE_EVENT:
                        Reset_AT_CMD_Buffer();
                        /* AT+S.SOCKW=00,11<cr> */
                        sprintf((char*)WiFi_AT_Cmd_Buff,AT_SOCKET_WRITE,curr_sockID,curr_DataLength);
                        status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));

                        if(status == WiFi_MODULE_SUCCESS) {
                          Reset_AT_CMD_Buffer();
                          memcpy((char*)(char*)WiFi_AT_Cmd_Buff, (char*)curr_data,curr_DataLength);
                          WiFi_AT_Cmd_Buff[curr_DataLength]='\r';

                          status = USART_Transmit_AT_Cmd(curr_DataLength+2);
                          if(status == WiFi_MODULE_SUCCESS) {
                            WiFi_Module_State = Process_Event;
                          }
                          else {
                            #if DEBUG_PRINT
                              printf("\r\n ERROR IN SOCKET\r\n");
                            #endif
                            AT_Response_Received = WIFI_TRUE;
                            AT_RESPONSE = WiFi_AT_CMD_RESP_ERROR;
                          }
                        }
                        else {
                          #if DEBUG_PRINT
                            printf("\r\n ERROR In Socket\r\n");
                          #endif
                          AT_Response_Received = WIFI_TRUE;
                          AT_RESPONSE = WiFi_AT_CMD_RESP_ERROR;
                        }
                        break;
                 
                case WIFI_CLIENT_SOCKET_OPEN_EVENT:
                        Reset_AT_CMD_Buffer();

                        /* AT+S.SOCKON = myserver,1234,t <cr> */  
                        sprintf((char*)WiFi_AT_Cmd_Buff,AT_SOCKET_OPEN,curr_hostname,(int)curr_port_number,curr_protocol);        
                        status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
                        if(status != WiFi_MODULE_SUCCESS)
                        {
                          #if DEBUG_PRINT
                            printf("\r\n ERROR During Socket Open \r\n");
                          #endif
                          AT_Response_Received = WIFI_TRUE;
                          AT_RESPONSE = WiFi_AT_CMD_RESP_ERROR;
                        }
                        break;
                        
                case WIFI_CLIENT_SOCKET_CLOSE_EVENT:
                        Reset_AT_CMD_Buffer();

                         /* AT+S.SOCKC=00<cr> */
                        sprintf((char*)WiFi_AT_Cmd_Buff,AT_SOCKET_CLOSE,event_pop_s->socket_id);
                        status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
                        if(status == WiFi_MODULE_SUCCESS)
                        {
                          AT_RESPONSE = WiFi_MODULE_SUCCESS;
                          prevent_push_OK_event = WIFI_TRUE;  //prevent the OK received after socket close command to be Q'ed
                          stop_event_dequeue = WIFI_TRUE;
                          remote_socket_closed_id = event_pop_s->socket_id;
                          client_socket_close_ongoing = WIFI_TRUE; //used for making changes in the value of open_sockets[sock_id] if no error is returned
//                          SockON_Server_Closed_Callback = WIFI_TRUE;
                        }
                        else
                        {
                          #if DEBUG_PRINT
                            printf("\r\n ERROR During Socket Close \r\n");
                          #endif
                        }
                        break; 


                case WIFI_FILE_EVENT:
                        Reset_AT_CMD_Buffer();

                        if(curr_filename == NULL)
                        {
                            /* AT+S.FSL */
                            sprintf((char*)WiFi_AT_Cmd_Buff,AT_DISPLAY_FILE_NAME);
                        }
                        else if(curr_hostname == NULL)
                        {
                            /* AT+S.FSP=/index.html  */
                            sprintf((char*)WiFi_AT_Cmd_Buff,AT_DISPLAY_FILE_CONTENT,curr_filename);
                        }
                        else
                        {
                            /* AT+S.HTTPDFSUPDATE=%s,/outfile.img  */
                            sprintf((char*)WiFi_AT_Cmd_Buff,AT_DOWNLOAD_IMAGE_FILE,curr_hostname,curr_filename,(int)curr_port_number);
                        }
                        status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
                        if(status == WiFi_MODULE_SUCCESS)
                        {
                            WiFi_Module_State = Process_Event;
                            enable_receive_http_response = WIFI_TRUE;
                            enable_receive_data_chunk = WIFI_TRUE;
                            enable_receive_file_response = WIFI_TRUE;
                        }
                        else {
                          #if DEBUG_PRINT
                            printf("\r\n ERROR in FILE \r\n");
                          #endif
                          AT_Response_Received = WIFI_TRUE;
                          AT_RESPONSE = WiFi_AT_CMD_RESP_ERROR;
                        }
                        break;

              case WIFI_FW_UPDATE_EVENT:
                        Reset_AT_CMD_Buffer();
                        sprintf((char*)WiFi_AT_Cmd_Buff,"AT+S.FWUPDATE=%s,/%s,%d\r",curr_hostname,curr_filename,(int)curr_port_number);
                        status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
                        if(status == WiFi_MODULE_SUCCESS)
                        {
                          WiFi_Module_State = Process_Event;
                          enable_fw_update_read = WIFI_TRUE;
                          enable_receive_data_chunk = WIFI_TRUE;
                        }
                        else 
                        {
                          #if DEBUG_PRINT
                            printf("\r\n ERROR in Firmware \r\n");
                          #endif
                          AT_Response_Received = WIFI_TRUE;
                          AT_RESPONSE = WiFi_AT_CMD_RESP_ERROR;
                        }
                        break;

                case WIFI_ERROR_EVENT:
                        #if DEBUG_PRINT
                          printf("\r\n ERROR!\r\n");
                        #endif
                        AT_Response_Received = WIFI_TRUE;
                        AT_RESPONSE = WiFi_AT_CMD_RESP_ERROR;
                        break;

                case WIFI_GCFG_EVENT:
                case WIFI_GPIO_EVENT:
                        AT_Response_Received = WIFI_TRUE;
                        AT_RESPONSE = WiFi_MODULE_SUCCESS; 
                        break;

                case WIFI_STANDBY_CONFIG_EVENT:
                        #if DEBUG_PRINT
                            printf("\r\nGoing into standby..\r\n");  
                        #endif
                        WiFi_Module_State = Process_Event;
                        break;

                case WIFI_RESUME_CONFIG_EVENT:
                        #if DEBUG_PRINT
                            printf("\r\nResuming from standby..\r\n");
                        #endif
                        AT_Response_Received = WIFI_TRUE;//let main run-on
                        break;
                        
                 case WIFI_NO_EVENT:
                	break;
             }
          }
    }            
            /* If data is pending on client socket SOCKON, make read requests*/
            if(start_sock_read == WIFI_TRUE)
                    {
                        //flush_buffer_queue(&big_buff);
                        Socket_Read(Socket_Data_Length);
                        start_sock_read = WIFI_FALSE;
                    }

            /* Call Query, after notification for TLS is received */
            else if(enable_query == WIFI_TRUE && enable_dequeue == WIFI_TRUE)
                    {
                        //@TBD: Flushing the buffer may be detrimental if we have genuine follow on WIND55?
                        //flush_buffer_queue(&big_buff);  //Flush the buffer to remove WIND:55 in pipeline (This maybe a problem)
                        Socket_Pending_Data();
                        enable_query = WIFI_FALSE;
                    }

            else if(Pending_SockON_Callback==WIFI_TRUE)//for client socket
                    {
                        //Now callback to user with user_data pointer <UserDataBuff>              
                        ind_wifi_socket_data_received(sockon_id_user, (uint8_t *)UserDataBuff, message_size, chunk_size);
                        memset(UserDataBuff, 0x00, MAX_BUFFER_GLOBAL);//Flush the buffer
                        Resume_Dequeue();
                        Pending_SockON_Callback=WIFI_FALSE;
                    }

            else if(Pending_SockD_Callback == WIFI_TRUE)//for server socket
                    {
                        //if(command_mode)//if command_mode is achieved then callback else loop in this state
                        {
                        //Now callback to user with user_data pointer <UserDataBuff>
                        ind_wifi_socket_data_received(9, (uint8_t *)UserDataBuff, message_size, chunk_size);
                        memset(UserDataBuff, 0x00, MAX_BUFFER_GLOBAL);//Flush the buffer
                        Resume_Dequeue();
                        Pending_SockD_Callback=WIFI_FALSE;      
                        }
                    }

            else if(Client_Socket_Close_Cmd==WIFI_TRUE)//for client socket
                    {
                        //Close the socket
                        //Change State to AT_Cmd_Response before calling socket_close()
                        WiFi_Module_State = Process_Event;
                        wifi_socket_client_close(client_socket_close_id);
                        Client_Socket_Close_Cmd = WIFI_FALSE;
//                        SockON_Server_Closed_Callback = WIFI_TRUE;
                    }

            else if(SockON_Server_Closed_Callback==WIFI_TRUE)//for client socket
                    {
                        //callback the user
                        ind_wifi_socket_client_remote_server_closed(&socket_closed_id_callback);
                        SockON_Server_Closed_Callback = WIFI_FALSE;
                    }

            else if(HTTP_Data_available == WIFI_TRUE)
                    {
                        ind_wifi_http_data_available((uint8_t *)UserDataBuff,UserDataBuff_index);
                        memset(UserDataBuff, 0x00, MAX_BUFFER_GLOBAL);//Flush the buffer
//                        memcpy(&UserDataBuff, &HTTP_Runway_Buff, 6);
//                        memset(HTTP_Runway_Buff, 0x00, 6);
                        Resume_Dequeue();
                        HTTP_Data_available=WIFI_FALSE;
                    }

            else if (FILE_Data_available == WIFI_TRUE)
                    {
                        ind_wifi_file_data_available((uint8_t *) UserDataBuff);
                        memset(UserDataBuff, 0x00, MAX_BUFFER_GLOBAL);//Flush the buffer
                        Resume_Dequeue();
                        FILE_Data_available = WIFI_FALSE;
                    }
            else if(Client_Connected == WIFI_TRUE)
                    {
                            ind_socket_server_client_joined();
                            Client_Connected = WIFI_FALSE;
                    }

            else if(Client_Disconnected == WIFI_TRUE)
                    {
                        ind_socket_server_client_left();
                        Client_Disconnected = WIFI_FALSE;
                    }

            //Make callbacks from here to user for pending events
            
            if(WiFi_WIND_State.WiFiHWStarted==WIFI_TRUE)
                {
                    if(wifi_ready == 2)//Twice reset for User Callback
                        {
                            wifi_ready++;
                            ind_wifi_on();//Call this once only...This if for wifi_on (instead of console active
                        }
                }

            if(WiFi_WIND_State.WiFiUp == WIFI_TRUE)
                {
                    if(wifi_connected == 0)
                        {
                            wifi_connected = 1;
                            ind_wifi_connected();//wifi connected
                        }    
                    WiFi_WIND_State.WiFiUp = WIFI_FALSE;
                }

            else if(WiFi_WIND_State.WiFiStarted_MiniAPMode == WIFI_TRUE)
                {
                    ind_wifi_ap_ready();
                    WiFi_WIND_State.WiFiStarted_MiniAPMode = WIFI_FALSE;
                }

            else if(WiFi_WIND_State.WiFiAPClientJoined == WIFI_TRUE)
                {
                      ind_wifi_ap_client_joined(client_MAC_address);
                      WiFi_WIND_State.WiFiAPClientJoined = WIFI_FALSE;
                }

            else if(WiFi_WIND_State.WiFiAPClientLeft == WIFI_TRUE)
                {
                      ind_wifi_ap_client_left(client_MAC_address);
                      WiFi_WIND_State.WiFiAPClientLeft = WIFI_FALSE;
                }

            else if(Deep_Sleep_Callback == WIFI_TRUE)
                {
                      ind_wifi_resuming();
                      Deep_Sleep_Callback = WIFI_FALSE;
                }

            else if(standby_resume_callback == WIFI_TRUE)
                {
                      ind_wifi_resuming();
                      standby_resume_callback = WIFI_FALSE;
                }

            else if(WiFi_WIND_State.WiFiHWFailure==WIFI_TRUE)
                {      
                      WiFi_WIND_State.WiFiHWFailure=WIFI_FALSE;
                      ind_wifi_error(WiFi_HW_FAILURE_ERROR);//call with error number      
                }

            else if(WiFi_WIND_State.HardFault==WIFI_TRUE)
                {      
                      WiFi_WIND_State.HardFault=WIFI_FALSE;
                      ind_wifi_error(WiFi_HARD_FAULT_ERROR);//call with error number      
                }

            else if(WiFi_WIND_State.StackOverflow==WIFI_TRUE)
                {      
                      WiFi_WIND_State.StackOverflow=WIFI_FALSE;
                      ind_wifi_error(WiFi_STACK_OVERFLOW_ERROR);//call with error number      
                }

            else if(WiFi_WIND_State.MallocFailed==WIFI_TRUE)
                {      
                      WiFi_WIND_State.MallocFailed=WIFI_FALSE;
                      ind_wifi_error(WiFi_MALLOC_FAILED_ERROR);//call with error number      
                }

            else if(WiFi_WIND_State.InitFailure==WIFI_TRUE)
                {      
                      WiFi_WIND_State.InitFailure=WIFI_FALSE;
                      ind_wifi_error(WiFi_INIT_ERROR);//call with error number      
                }

            else if(WiFi_WIND_State.StartFailed==WIFI_TRUE)
                {      
                      WiFi_WIND_State.StartFailed=WIFI_FALSE;
                      ind_wifi_error(WiFi_START_FAILED_ERROR);//call with error number      
                }
            
            else if(WiFi_WIND_State.WiFiException==WIFI_TRUE)
                {      
                      WiFi_WIND_State.WiFiException=WIFI_FALSE;
                      ind_wifi_error(WiFi_EXCEPTION_ERROR);//call with error number      
                }

            else if(WiFi_WIND_State.PS_Mode_Failure==WIFI_TRUE)
                {      
                      WiFi_WIND_State.PS_Mode_Failure=WIFI_FALSE;
                      ind_wifi_warning(WiFi_POWER_SAVE_WARNING);//call with error number      
                }

            else if(WiFi_WIND_State.HeapTooSmall==WIFI_TRUE)
                {      
                      WiFi_WIND_State.HeapTooSmall=WIFI_FALSE;
                      ind_wifi_warning(WiFi_HEAP_TOO_SMALL_WARNING);//call with error number      
                }
            
            else if(WiFi_WIND_State.WiFiSignalLOW==WIFI_TRUE)
                {      
                      WiFi_WIND_State.WiFiSignalLOW=WIFI_FALSE;
                      ind_wifi_warning(WiFi_SIGNAL_LOW_WARNING);//call with error number      
                }    
            
            else if(WiFi_WIND_State.WiFiDeauthentication == WIFI_TRUE)
                {
                      WiFi_WIND_State.WiFiDeauthentication = WIFI_FALSE;
                      ind_wifi_connection_error(WiFi_DE_AUTH);
                }
            
            else if(WiFi_WIND_State.WiFiDisAssociation == WIFI_TRUE)
                {
                      WiFi_WIND_State.WiFiDisAssociation = WIFI_FALSE;
                      ind_wifi_connection_error(WiFi_DISASSOCIATION);
                }
            
            else if(WiFi_WIND_State.WiFiJoinFailed == WIFI_TRUE)
                {
                      WiFi_WIND_State.WiFiJoinFailed = WIFI_FALSE;
                      ind_wifi_connection_error(WiFi_JOIN_FAILED);
                }
            
            else if(WiFi_WIND_State.WiFiScanBlewUp == WIFI_TRUE)
                {
                      WiFi_WIND_State.WiFiScanBlewUp = WIFI_FALSE;
                      ind_wifi_connection_error(WiFi_SCAN_BLEWUP);  //@TBD to check if user made call, so not call callback if true
                }
            
            else if(WiFi_WIND_State.WiFiScanFailed == WIFI_TRUE)
                {
                      WiFi_WIND_State.WiFiScanFailed = WIFI_FALSE;
                      ind_wifi_connection_error(WiFi_SCAN_FAILED);  //@TBD to check if user made call, so not call callback if true
                }
            
            else if(WiFi_WIND_State.WiFiUnHandledInd == WIFI_TRUE)
                {
                      WiFi_WIND_State.WiFiUnHandledInd = WIFI_FALSE;
                      ind_wifi_packet_lost(WiFi_UNHANDLED_IND_ERROR);  //@TBD to check if user made call, so not call callback if true
                }
            
            else if(WiFi_WIND_State.WiFiRXMgmt == WIFI_TRUE)
                {
                      WiFi_WIND_State.WiFiRXMgmt = WIFI_FALSE;
                      ind_wifi_packet_lost(WiFi_RX_MGMT);  //@TBD to check if user made call, so not call callback if true
                }
            
            else if(WiFi_WIND_State.WiFiRXData == WIFI_TRUE)
                {
                      WiFi_WIND_State.WiFiRXData = WIFI_FALSE;
                      ind_wifi_packet_lost(WiFi_RX_DATA);  //@TBD to check if user made call, so not call callback if true
                }  
            
            else if(WiFi_WIND_State.WiFiRxUnk == WIFI_TRUE)
                {
                      WiFi_WIND_State.WiFiRxUnk = WIFI_FALSE;
                      ind_wifi_packet_lost(WiFi_RX_UNK);  //@TBD to check if user made call, so not call callback if true
                }  
            
            else if(WiFi_WIND_State.WiFiSockdDataLost == WIFI_TRUE)
                {
                      WiFi_WIND_State.WiFiSockdDataLost = WIFI_FALSE;
                      ind_wifi_socket_server_data_lost();  //@TBD to check if user made call, so not call callback if true
                }
      
}

/**
* @brief  Start_Timer
*         Start Timer 
* @param  None
* @retval None
*/
void Start_Timer()
{
  tickcount = WIFI_FALSE;
  Timer_Running = WIFI_TRUE;
}

/**
* @brief  Stop_Timer
*         Stop Timer request
* @param  None
* @retval None
*/
void Stop_Timer()
{  
  tickcount = WIFI_FALSE;  
  Timer_Running = WIFI_FALSE;    
  UartReady = SET;
}

/**
* @brief  Stop_Dequeue
*         Stop dequeuing data from the ring buffer
* @param  None
* @retval None
*/
void Stop_Dequeue()
{
  enable_dequeue = WIFI_FALSE;
}

/**
* @brief  Resume_Dequeue
*         Resume dequeuing data from the ring buffer
* @param  None
* @retval None
*/
void Resume_Dequeue()
{
  enable_dequeue = WIFI_TRUE;
}

/**
* @brief  Wifi_SysTick_Isr
*         Function called every SysTick to process buffer
* @param  None
* @retval None
*/
void Wifi_SysTick_Isr()
{
    //Check if Data is Paused
    if((Timer_Running) && (enable_dequeue==WIFI_TRUE) /*&& ((tickcount++) >= PROCESS_WIFI_TIMER)*/)
        {    
            Process_WiFi();
        }

    if(resume_receive_data == WIFI_TRUE)
        {
            if(is_half_empty(&big_buff))
                {
                    resume_receive_data = WIFI_FALSE;
                    Receive_Data();
                }
        }
    
 
    if(Standby_Timer_Running) // module is in sleep and after expiry RX will be conf as EXTI
    {
        if((standby_time++) >= EXTI_CONF_TIMER)
            {
                Standby_Timer_Running=WIFI_FALSE;
                standby_time = 0;
                //configure_to_exti();
            }
    }

    /*A Resume WIND:70 has come and triggered this
    So checking here if after that resume we fall back to sleep (another WIND69) within SLEEP_RESUME_PREVENT time.
    If yes, we assume it is a false resume and hence do nothing and go back to sleep
    If no WIND69 (going into sleep) has come, we can assume the resume was genuine and then enable the callback
    */
    if((Deep_Sleep_Timer) && ( sleep_count++) >= SLEEP_RESUME_PREVENT)
        {
            if(Deep_Sleep_Enabled == WIFI_TRUE)//which means we have received another WIND69 in the 2 seconds
                {
                    //do nothing, go back to sleep
                    Deep_Sleep_Enabled = WIFI_TRUE;
                    Deep_Sleep_Callback = WIFI_FALSE;
                }
    else if (Deep_Sleep_Enabled == WIFI_FALSE) //which means we have not received any WIND69 during the last 2 seconds
        {
            //enable the user callback as it is a genuine WIND70
            Deep_Sleep_Callback = WIFI_TRUE;
        }
    Stop_DeepSleep_Timer();
    }
}

#if 0
/**
* @brief  RX_EXTI_Isr
*         
* @param  None
* @retval None
*/
void RX_EXTI_Isr(uint16_t GPIO_Pin)
{
  if(GPIO_Pin==WiFi_USART_RX_PIN && (HAL_GPIO_ReadPin(WiFi_USART_RX_GPIO_PORT, WiFi_USART_RX_PIN) == GPIO_PIN_SET))
  {
    HAL_NVIC_DisableIRQ(USARTx_EXTI_IRQn);
    UART_Configuration();//reconfigure the UART for WIND reception
    
    //Resume_Timer_Running = WIFI_TRUE;
    AT_Cmd_Processing = WIFI_FALSE;
    Receive_Data();
  }
}
#endif

/**
* @brief  HAL_UART_TxCpltCallback
*         Tx Transfer completed callback
* @param  UsartHandle: UART handle 
* @retval None
*/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandleArg)
{
#ifdef WIFI_USE_VCOM
  if (UartHandleArg==&UartMsgHandle)
    console_echo_ready = SET;
#else
  /* Set transmission flag: transfer complete */
  TxUartReady = SET; 
#endif
}

/**
* @brief  HAL_UART_RxCpltCallback
*         Rx Transfer completed callback
* @param  UsartHandle: UART handle 
* @retval None
*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandleArg)
{

#ifdef WIFI_USE_VCOM
  if (UartHandleArg==&UartWiFiHandle)
#endif  
  {
#ifndef WIFI_USE_VCOM
  Uartx_Rx_Processing = WIFI_FALSE;
  Stop_Timer();
  __disable_irq();
  push_buffer(&big_buff, uart_byte);
  __enable_irq();
  Start_Timer();
#else
    console_push_ready = SET;
#endif

#ifndef WIFI_USE_VCOM  
  if(is_half_full(&big_buff))
  {
    resume_receive_data = WIFI_TRUE;
    HAL_GPIO_WritePin(WiFi_USART_RTS_GPIO_PORT, WiFi_USART_RTS_PIN, GPIO_PIN_SET);//De-assert RTS
  } else
    {
      if(AT_Cmd_Processing == WIFI_FALSE)
      {//call Rx only if TX is not under processing (AT command)
        receive_status = HAL_UART_Receive_IT(&UartWiFiHandle, (uint8_t *)uart_byte, 1);
        if(receive_status!=HAL_OK)
        {
          #if DEBUG_PRINT 
          printf("HAL_UARTx_Receive_IT Error");
          #endif
        }
        else {
          Uartx_Rx_Processing = WIFI_TRUE;
        }
      }  //else if (AT_Cmd_Processing == WIFI_TRUE)
    }
#endif
  }
#ifdef WIFI_USE_VCOM
  else
  {
    console_send_char[0] = console_input_char[0];
    console_input();
    console_send_ready = SET;
    HAL_UART_Transmit_IT(&UartWiFiHandle, (uint8_t*)console_send_char, 1);
  }
#endif

// if (UartHandleArg->Instance == USART2)
 {
 //  Uart2Ready = SET;
 }
}

/**
* @brief  USART_Receive_AT_Resp
*         Receive and check AT cmd response
* @param  WiFi_AT_CMD_Response_t : WIFi module next state 
* @retval WiFi_Status_t : Response of AT cmd  
*/

WiFi_Status_t USART_Receive_AT_Resp(WiFi_AT_CMD_Response_t state)
{
  WiFi_Module_State = state;
  while(AT_Response_Received != WIFI_TRUE) {
		//do nothing
	}
  AT_Response_Received = WIFI_FALSE;
  return AT_RESPONSE;
}

/**
  * @brief  UART error callbacks
  * @param  UsartHandle: UART handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle)
{
    Error_Handler();
    //printf("\r\nuart cb error\r\n");
    //Receive_Data();
}

/**
* @brief  Process_WiFi
*         Pop a byte from the circular buffer and send the byte for processing
*         This function should be called from main or should be run with a periodic timer
* @param  None
* @retval None
*/
void Process_WiFi(void)
{
    __disable_irq();
    temp=pop_buffer_queue(&big_buff);   //contents of temp(pop_buffer) will not change till another de-queue is made
    __enable_irq();

    if(temp!=NULL) 
    {
      Process_Buffer(temp);
    }
    
   if(event_deQ_x_wind64)//if de-Q is stopped due to WIND64 wait
   {
     wind64_DQ_wait++;//1ms for each count
     if(wind64_DQ_wait>200)//wait for 50ms for example
     {
       wind64_DQ_wait=0;
       event_deQ_x_wind64 = WIFI_FALSE;
       //re-enable event Q after 200ms
       stop_event_dequeue = WIFI_FALSE;
     }
   }
}

/**
* @brief  Process_Buffer
*         Process and construct a Wind Line buffer
* @param  ptr: pointer to one single byte
* @retval None
*/

void Process_Buffer(uint8_t * ptr)
{
  static uint32_t Fillptr=0;//count=0;
  static uint8_t index, chan_value;
  unsigned char rxdata = 0;
  int rssi_value = 0;
  char SocketId_No[2];
  char databytes_No[4];
  char * pStr;

  rxdata =  *(ptr+0);  
  //printf(&rxdata);//check prints for debug...to be removed or kept in DEBUG statement
  if(enable_receive_data_chunk == WIFI_FALSE)
    process_buffer[Fillptr++] = rxdata;
   reset_event(&event_s);
    switch (WiFi_Module_State)
    {
    case Process_Event:

          if((process_buffer[Fillptr-2]==0xD) && (process_buffer[Fillptr-1]==0xA) && !enable_receive_http_response && !sock_read_ongoing)
            {
                if((strstr((const char *)process_buffer,"WIND:")) != NULL)
                {
                    if((strstr((const char *)process_buffer,"+WIND:67:")) !=NULL)
                        event_s.event = WIFI_STANDBY_CONFIG_EVENT;

                    else if ((strstr((const char *)process_buffer,"+WIND:68:"))!= NULL)
                        event_s.event = WIFI_RESUME_CONFIG_EVENT;

                    else
                        {
                            //end of msg received. Will not receive any other msg till we process this.
                            Stop_Timer();
                            #if defined (USE_STM32L0XX_NUCLEO) || (USE_STM32F4XX_NUCLEO) || (USE_STM32L4XX_NUCLEO)
                            __disable_irq();
                            #endif
                            Process_Wind_Indication(&process_buffer[0]);
                            #if defined (USE_STM32L0XX_NUCLEO) || (USE_STM32F4XX_NUCLEO) || (USE_STM32L4XX_NUCLEO)
                            __enable_irq();
                            #endif
                            Start_Timer();
                            sock_total_count=0;
                        }

                    if(!prevent_push_WIFI_event)
                        {
                            __disable_irq();
                            push_eventbuffer(&event_buff, event_s);
                             __enable_irq();
                            reset_event(&event_s);
                        }

                    if (!do_not_reset_push_WIFI_event) prevent_push_WIFI_event = WIFI_FALSE;
                    if(enable_sock_read)
                        {
                            sock_read_ongoing = WIFI_FALSE;//we finished processing a message and now sock read will commence
                            Q_Contains_Message = WIFI_FALSE;
                        }
                    Fillptr=0;
                    memset(process_buffer, 0x00, strlen((const char*)process_buffer));
                }

                else if((strstr((const char *)process_buffer,"\r\nOK\r\n")) != NULL)
                {
                    /*Now Check to which AT Cmd response this OK belongs to so that correct parsing can be done*/

                    // SOCKON ID (Open a client socket)
                    if(((pStr=(strstr((const char *)process_buffer,"ID: "))) != NULL))      
                    {
                            SocketId_No[0]    = *(pStr + 4) ;
                            SocketId_No[1]    = *(pStr + 5) ;
                            SocketId          = (((SocketId_No[0] - '0') * 10 ) + (SocketId_No[1] - '0'));
                            event_s.socket_id = SocketId;
                            event_s.event     =  WIFI_SOCK_ID_EVENT;
                            __disable_irq();
                            push_eventbuffer(&event_buff, event_s);
                             __enable_irq();
                            reset_event(&event_s);
                            if(enable_sock_read)
                            {
                                sock_read_ongoing = WIFI_FALSE;  //we finished processing a message and now sock read will commence
                                Q_Contains_Message = WIFI_FALSE;
                            }
                            Fillptr=0;
                            sock_total_count=0;
                            memset(process_buffer, 0x00, MAX_BUFFER_GLOBAL);
                     }

                      // DATALEN from SOCKQ
                     else if((pStr=(strstr((const char *)process_buffer,"DATALEN: "))) != NULL)
                     {
                            //Find the DataLength and do a socket read
                            databytes_No[0] = *(pStr + 9);
                            databytes_No[1] = *(pStr + 10);
                            databytes_No[2] = *(pStr + 11);
                            databytes_No[3] = *(pStr + 12);

                            if( databytes_No[1] == '\r')
                            {
                                SockON_Data_Len = databytes_No[0] - '0'; 
                            }
                            else if( databytes_No[2] == '\r')
                            {
                                SockON_Data_Len = (((databytes_No[0] - '0') * 10 ) + (databytes_No[1] - '0'));
                            }
                            else if( databytes_No[3] == '\r')
                            {
                                SockON_Data_Len = (((databytes_No[0] - '0') * 100 ) + ((databytes_No[1] - '0') * 10 ) + (databytes_No[2] - '0'));
                            }
                            else //it's a 4-digit number
                            {
                               SockON_Data_Len  = ((databytes_No[0] - '0') * 1000 ) + ((databytes_No[1] - '0') * 100 ) + ((databytes_No[2] - '0') * 10) + (databytes_No[3] - '0');
                            }
                            Socket_Data_Length = SockON_Data_Len;
                            if(Socket_Data_Length != 0)
                            {
                                  start_sock_read = WIFI_TRUE;
                            }
                            else if(Socket_Data_Length == 0)
                            {
                                stop_event_dequeue = WIFI_FALSE;      //continue popping events if nothing to read
                                Set_AT_Cmd_Response_False = WIFI_FALSE;
                            }
                            Fillptr = 0;
                            sock_total_count = 0;
                            if(enable_sock_read)
                            {
                                sock_read_ongoing = WIFI_FALSE;  //we finished processing a message and now sock read will commence
                                Q_Contains_Message = WIFI_FALSE;
                                pop_buffer_size = 0;
                                pop_buffer[0]='\0';     // required as this byte is already used in process_buffer
                            }
                            memset(process_buffer, 0x00, MAX_BUFFER_GLOBAL);
                     }

                      else if((((strstr((const char *)process_buffer," = "))) != NULL))
                      {
                            // AT command GCFG
                            pStr = (char *) strstr((const char *)process_buffer," = ");
                            event_s.event = WIFI_GCFG_EVENT;
                            memcpy(get_cfg_value, pStr+2, (strlen(pStr)-2));
                            __disable_irq();
                            push_eventbuffer(&event_buff, event_s);
                             __enable_irq();
                            reset_event(&event_s);
                            if(enable_sock_read)
                              {
                                  sock_read_ongoing = WIFI_FALSE;    //we finished processing a message and now sock read will commence
                                  Q_Contains_Message = WIFI_FALSE;
                              }
                            Fillptr=0;
                            memset(process_buffer, 0x00, MAX_BUFFER_GLOBAL);
                      }

                      else
                      {
                            //This is a standalone OK
                            /*Cases possible
                            - TLSCERT,TLSCERT2, TLSDOMAIN, SETTIME
                            - S.SOCKW, SOCKR, S.SOCKC, S.SOCKD (open a server socket)
                            - File Operations
                            - S.GPIOC and S.GPIOW
                            */
                            //Push a simple OK Event, if this is an OK event required to be pushed to Q 
                            if(prevent_push_OK_event)
                                {
                                    //This OK is not to be handled, hence the pop action on OK completion to be done here
                                    //since pop will not happen
//                                  stop_event_dequeue=WIFI_FALSE;
                                    
                                    if(client_socket_close_ongoing)
                                      {
                                          if(no_of_open_client_sockets > 0)
                                            no_of_open_client_sockets--;
                                          prevent_push_OK_event = WIFI_FALSE;
                                          open_sockets[remote_socket_closed_id] = WIFI_FALSE;
                                          socket_closed_id_callback = remote_socket_closed_id;
                                          Set_AT_Cmd_Response_False = WIFI_FALSE;
                                          client_socket_close_ongoing = WIFI_FALSE;
                                          SockON_Server_Closed_Callback = WIFI_TRUE;
                                          stop_event_dequeue=WIFI_FALSE;
                                      }
                                    
                                }

                            else
                                {
                                        event_s.ok_eval = WIFI_TRUE;
                                        event_s.event = WIFI_OK_EVENT;
                                        __disable_irq();
                                        push_eventbuffer(&event_buff, event_s);
                                         __enable_irq();
                                        reset_event(&event_s);
                                }
                            prevent_push_OK_event = WIFI_FALSE;
                            memset(process_buffer, 0x00, Fillptr);
                            Fillptr=0;
                            sock_total_count=0;
                            if(enable_sock_read)
                                {
                                    sock_read_ongoing = WIFI_FALSE;//we finished processing a message and now sock read will commence
                                    Q_Contains_Message = WIFI_FALSE;
                                }
                            break;
                        }
                }

               else if((((strstr((const char *)process_buffer,"ERROR"))) != NULL))
                {
                    //This is an ERROR
                    //There can be only ONE outstanding AT command and hence this ERROR belongs to that
                    //HTTP -> ERROR: host not found
                    //@TBD: Check all Errors Possible here???
                    if((strstr((const char *)process_buffer,"\r\nERROR: Pending data\r\n")) != NULL)    //if Error after sock close command and not 'OK'
                    {
                        printf("\r\nERROR: Socket could not be closed..PENDING DATA\r\n");
                        prevent_push_OK_event = WIFI_FALSE;     //prevent the OK received after socket close command to be Q'ed
                        client_socket_close_ongoing = WIFI_FALSE;
                        stop_event_dequeue = WIFI_FALSE;
                    }
                    else
                    {
                        event_s.event = WIFI_ERROR_EVENT;
                        __disable_irq();
                        push_eventbuffer(&event_buff, event_s);
                         __enable_irq();
                        reset_event(&event_s);
                        if(stop_event_dequeue)
                          /*ERROR:Illegal Socket ID*/
                        stop_event_dequeue= WIFI_FALSE;//continue popping events if nothing to read
                    }
                    Fillptr=0;
                    sock_total_count=0;
                    if(enable_sock_read)
                    {
                        sock_read_ongoing = WIFI_FALSE;//we finished processing a message and now sock read will commence
                        Q_Contains_Message = WIFI_FALSE;
                    }
                    memset(process_buffer, 0x00, MAX_BUFFER_GLOBAL);
                }

               else if((((strstr((const char *)process_buffer,"GPIO "))) != NULL)) 
                {
                      // Receive GPIO Read
                      pStr = (char *) strstr((const char *)process_buffer,"= 0,");                                  
                      if(pStr != NULL)
                          {
                              gpio_value = 0;
                          } 
                      else 
                          gpio_value = 1;   
                      
                      pStr = (char *) strstr((const char *)process_buffer,"out");                                  
                      if(pStr != NULL)
                        {
                            gpio_dir= 0;    //out
                        }
                      else
                        {
                            gpio_dir= 1;    //in
                        }
                      //Push GPIO Read Event on Event_Queue
                      event_s.event = WIFI_GPIO_EVENT;
                      __disable_irq();
                      push_eventbuffer(&event_buff, event_s);
                       __enable_irq();
                      reset_event(&event_s);
                      if(enable_sock_read)
                        {
                            sock_read_ongoing = WIFI_FALSE;     //we finished processing a message and now sock read will commence
                            Q_Contains_Message = WIFI_FALSE;
                        }
                      Fillptr = 0;
                      memset(process_buffer, 0x00, strlen((const char*)process_buffer));
                      break;
                }

               else
               {
                  if(enable_sock_read && Q_Contains_Message && !message_pending)
                  {
                      printf((const char*)&process_buffer[0]);
                      Q_Contains_Message = WIFI_FALSE;
                      Q_Contains_Data = WIFI_TRUE;
                      //enable_sock_read = WIFI_FALSE;
                      //stop_event_dequeue= WIFI_FALSE;
                  }

                  //if in data mode, reset on \r\n
                  if(!sock_read_ongoing && data_mode)
                  {
                    Fillptr = 0;
                    memset(process_buffer, 0x00, strlen((const char*)process_buffer));
                  }
               }
            }

            else if (http_req_pending)   //HTTP Response Check
            {
                 if((strstr((const char *)&process_buffer[0],"200 OK\r")) != NULL || (strstr((const char *)&process_buffer[0],"400 Bad Request\r")) != NULL 
                        || (strstr((const char *)&process_buffer[0],"401 Unauthorized\r")) != NULL || (strstr((const char *)&process_buffer[0],"403 Forbidden\r")) != NULL 
                        || (strstr((const char *)&process_buffer[0],"404 Not Found\r")) != NULL || (strstr((const char *)&process_buffer[0],"408 Request Timeout\r")) != NULL 
                        || (strstr((const char *)&process_buffer[0],"500 Internal Server Error\r")) != NULL  || (strstr((const char *)&process_buffer[0],"502 Bad Gateway\r")) != NULL 
                        || (strstr((const char *)&process_buffer[0],"504 Gateway Timeout\r")) != NULL)
                {
                    enable_receive_http_response = WIFI_TRUE;
                    enable_receive_data_chunk = WIFI_TRUE;
                    http_req_pending = WIFI_FALSE;
                    pop_buffer[0]='\0';
                    pop_buffer_size=0;
                    process_buffer_index =6;
                }
            }

          else if ((process_buffer[Fillptr-1]==0x09) && (process_buffer[Fillptr-2]==':') && (process_buffer[Fillptr-3]=='1'))//<ht> Horizontal Tab for Scan Result?
            {
                enable_receive_wifi_scan_response = WIFI_TRUE;
            }

          if(!Pending_SockON_Callback && !HTTP_Data_available && !FILE_Data_available)
          {
              //Check Process Buffer for any pending message
              if(enable_receive_data_chunk)
                {
                    if(!Q_Contains_Data && pop_buffer_size)
                    {
                      pop_queue_length = pop_buffer_size;
                      if(Fillptr + pop_queue_length > 511)
                      {
                         uint32_t length = (Fillptr + pop_queue_length) - 511;
                        __disable_irq();
                         rewind_buffer_queue(&big_buff,length);
                         __enable_irq();
                         memset(ptr+(511 - Fillptr), 0x00, length);
                         pop_queue_length = 511 - Fillptr;
                      }
                      memcpy(process_buffer+Fillptr,(char const *)pop_buffer, pop_queue_length);
                      Fillptr = Fillptr + pop_queue_length;

                      if(Fillptr > 512)
                      {
                          printf("\r\nFillptr corrupted!!\r\n");
                      }

                      if((strstr((const char *)process_buffer,"ERROR: ")) != NULL)
                      {
                           Q_Contains_Message = WIFI_FALSE;
                           message_pending = WIFI_FALSE;
                      }
                      else if(!sock_read_ongoing && !enable_receive_http_response) 
                      {
                          if(process_buffer[0]!='\0') 
                          {
                            if(((process_buffer[0]==0xD) && (process_buffer[1]==0xA)) && process_buffer[2] != 0xD) 
                            {
                              message_pending = WIFI_TRUE;
                              if((pStr = (strstr((const char *)process_buffer+2,"\r\n"))) != NULL) 
                              {                    // process buffer has complete message
                                    int wind_length = ((uint8_t *)pStr - (uint8_t *)process_buffer)+2;  

                                    if(strstr((const char *)process_buffer+2,"DATALEN:")) 
                                    {
                                          pStr = strstr((const char *)process_buffer + wind_length,"\r\nOK\r\n"); //find OK, as DATALEN has to be terminated by OK
                                          if(pStr!=NULL)
                                          {
                                              wind_length = ((uint8_t *)pStr-(uint8_t *)process_buffer)+6;
                                          }
                                    }

                                    if(Fillptr-wind_length)
                                    {
                                          __disable_irq();
                                        rewind_buffer_queue(&big_buff, Fillptr - wind_length);             
                                        __enable_irq();
                                        memset(process_buffer + wind_length,0x00,Fillptr - wind_length);
                                        Fillptr = wind_length;
                                    }
                                    message_pending = WIFI_FALSE;
                                }
                                Q_Contains_Message = WIFI_TRUE;
                              }
                          }
                      }
                   }
                }

              if(!Q_Contains_Message && enable_sock_read && pop_buffer_size)     /*read is enabled*/
              {
                  sock_read_ongoing = WIFI_TRUE;
                  sock_total_count = sock_total_count + pop_queue_length;

                  /* Check for "ERROR: Not enough data in buffer " */
                  pStr = (char *) strstr((const char *)&process_buffer,"ERROR: ");

                  if (pStr != NULL)
                  {
                      if((process_buffer[Fillptr-2]==0xD) && (process_buffer[Fillptr-1]==0xA))
                      {
                        if((pStr = (char *)strstr((const char *)&process_buffer,"\r\nERROR: Too many sockets\r\n")) !=NULL)
                        {
                           #if DEBUG_PRINT
                             printf("\r\nERROR: TOO MANY SOCKETS \r\n");
                           #endif
                          
                            if(*(pStr+27)!='\0')
                            {
                                int len = (uint8_t *)pStr - (uint8_t *)process_buffer;
                                int extra_bytes = Fillptr - (len+27);
                                __disable_irq();
                                rewind_buffer_queue(&big_buff, extra_bytes);
                                __enable_irq();
                            }
                            Fillptr=0;
                            sock_total_count =0;
                            memset(process_buffer, 0x00, MAX_BUFFER_GLOBAL);
                            AT_Response_Received = WIFI_TRUE;
                            AT_RESPONSE = WiFi_AT_CMD_RESP_ERROR;
                            break;
                        }
                        else if((pStr = (char *)strstr((const char *)&process_buffer,"\r\nERROR: Pending data\r\n")) !=NULL)
                        {
                           #if DEBUG_PRINT
                             printf("\r\nERROR: PENDING DATA \r\n");
                           #endif
                          
                            if(*(pStr+23)!='\0')
                            {
                                int len = (uint8_t *)pStr - (uint8_t *)process_buffer;
                                int extra_bytes = Fillptr - (len+23);
                                __disable_irq();
                                rewind_buffer_queue(&big_buff, extra_bytes);
                                __enable_irq();
                            }
                            Fillptr=0;
                            sock_total_count =0;
                            memset(process_buffer, 0x00, MAX_BUFFER_GLOBAL);
                            AT_Response_Received = WIFI_TRUE;
                            AT_RESPONSE = WiFi_AT_CMD_RESP_ERROR;
                            break;
                        }

                        printf("\rERROR DURING SOCK READ\r\n");
                        sock_total_count=0;
                        SockON_Data_Len=0;
                        SockON_Data_Length=0;
                        enable_sock_read = WIFI_FALSE;
                        stop_event_dequeue = WIFI_FALSE;
                        Q_Contains_Message = WIFI_FALSE;
                        Q_Contains_Data = WIFI_FALSE;
                        enable_pending_data = 0;
                        enable_receive_data_chunk = WIFI_FALSE;
                        WIND64_count=0;
                        process_buffer_index =5;
                        //enable_query = TRUE;  //do we have more data?
                        Fillptr=0;
                        sock_read_ongoing = WIFI_FALSE;
                        if(data_pending_sockD)
                        {
                            data_pending_sockD=WIFI_FALSE;
                            number_of_bytes=0;
                            switch_by_default_to_command_mode=WIFI_TRUE;
                            WiFi_switch_to_command_mode(); //switch by default
                        }

                        memset(process_buffer, 0x00, MAX_BUFFER_GLOBAL);
                        break;
                     }
                  }
                  
                  /*now check if end of msg received*/
                  if(sock_total_count >= SockON_Data_Length)
                  {
                    #if DEBUG_PRINT
                     printf("\nReached SockON_Data_Len \r\n");
                    #endif

                    if(sock_total_count > SockON_Data_Length)
                    {
                        int databytes = sock_total_count - SockON_Data_Length;
                        __disable_irq();
                        rewind_buffer_queue(&big_buff, databytes);
                        __enable_irq();
                        memset(process_buffer+(Fillptr - databytes), 0x00, databytes);
                        Fillptr = Fillptr - databytes;
                    }
                    chunk_size = Fillptr;
                    message_size = SockON_Data_Length;
                    memcpy(UserDataBuff, process_buffer, Fillptr);
                    Fillptr = 0;
                    sock_total_count = 0;
                    SockON_Data_Len = 0;
                    Q_Contains_Data = WIFI_FALSE;
                    Q_Contains_Message = WIFI_FALSE;
                    enable_receive_data_chunk = WIFI_FALSE;
                    enable_pending_data = 0;

                    if(data_pending_sockD)
                    {
//                        data_pending_sockD=WIFI_FALSE;
                        number_of_bytes=0;
                        sockon_id_user = 0;
                        if(WIND64_count>0)
                            WIND64_count--;                         //decrease the number of pending WIND:64 Events
                        if(WIND64_count==0) {
                        switch_by_default_to_command_mode=WIFI_TRUE;
                        WiFi_switch_to_command_mode();              //switch by default
                        }
                        enable_query = WIFI_FALSE;
                    }
                    else
                    {
                        enable_query = WIFI_TRUE;
                        sockon_id_user = sockon_query_id;
                        /*@TODO: Do not need to prevent OK push in case of server socket*/
                        prevent_push_OK_event = WIFI_TRUE;    //prevent the qeueuing of the OK after this read operation
                    }

                    enable_sock_read = WIFI_FALSE;
                    sock_read_ongoing = WIFI_FALSE;
                    Stop_Dequeue();                           //Stop dequeue till user callback returns
                    Pending_SockON_Callback = WIFI_TRUE;      //set this to callback to user with User Buffer pointer
                                  //do we have more data?
                    if(data_pending_sockD)
                    {
                      stop_event_dequeue = WIFI_FALSE;
                      data_pending_sockD = WIFI_FALSE;
                    }
                    memset(process_buffer, 0x00, MAX_BUFFER_GLOBAL);
                    break;
                  }

                  if(Fillptr >= MAX_BUFFER_GLOBAL-1)
                  {
                    message_size = SockON_Data_Length;
                    chunk_size = 511;
                    memcpy(UserDataBuff, process_buffer, Fillptr);
                    Fillptr = 0;
                    process_buffer_index = 5;
                    Stop_Dequeue();
                    memset(process_buffer, 0x00, MAX_BUFFER_GLOBAL); 
                    Pending_SockON_Callback = WIFI_TRUE;      //set this to callback to user with User Buffer pointer
                  }
                  break;
              }

              if (enable_receive_http_response && pop_buffer_size) // http response enabled
              {
                  sock_read_ongoing = WIFI_TRUE;
                  if((pStr = (strstr((const char *)process_buffer + process_buffer_index - 6,"\r\nOK\r\n"))) != NULL)  
                    {
                        #if DEBUG_PRINT
                          printf("\r\nOK\r\n");         //http response completed
                        #endif

                        if(*(pStr+7) != '\0')
                        {
                            int len = (uint8_t *)pStr - (uint8_t *)process_buffer;
                            int extra_bytes = Fillptr - (len+6);
                            __disable_irq();
                            rewind_buffer_queue(&big_buff, extra_bytes);
                            __enable_irq();
                            
                            memset(process_buffer+len+7, 0x00, extra_bytes);
                            Fillptr = Fillptr - extra_bytes;
                        }

                        memcpy(UserDataBuff, process_buffer, Fillptr);
                        UserDataBuff_index = Fillptr;
                        AT_Response_Received = WIFI_TRUE;
                        enable_receive_data_chunk = WIFI_FALSE;
                        enable_receive_http_response = WIFI_FALSE;
                        Stop_Dequeue();
                        AT_RESPONSE = WiFi_MODULE_SUCCESS;
                        sock_read_ongoing = WIFI_FALSE;
                        Q_Contains_Data = WIFI_FALSE;
                        Q_Contains_Message = WIFI_FALSE;
                        stop_event_dequeue = WIFI_FALSE;
                        process_buffer_index = 6;
                        memset(process_buffer, 0x00, Fillptr);
                        Fillptr=0;

                        if(enable_receive_file_response)
                          FILE_Data_available = WIFI_TRUE;
                        else
                          HTTP_Data_available=WIFI_TRUE;
                    }

                    else if(((strstr((const char *)process_buffer + process_buffer_index-6,"ERROR"))) != NULL)
                    {
                        #if DEBUG_PRINT
                        printf("\r\nERROR\r\n");
                        #endif
                        AT_Response_Received = WIFI_TRUE;
                        AT_RESPONSE = WiFi_AT_CMD_RESP_ERROR;
                        sock_read_ongoing = WIFI_FALSE;
                        enable_receive_data_chunk = WIFI_FALSE;
                        Q_Contains_Data = WIFI_FALSE;
                        Q_Contains_Message = WIFI_FALSE;
                        Fillptr=0;
                        process_buffer_index=6;
                        enable_receive_http_response = WIFI_FALSE;
                        memset(process_buffer, 0x00, MAX_BUFFER_GLOBAL);

                        if(enable_receive_file_response)
                          FILE_Data_available = WIFI_FALSE;
                        else
                          HTTP_Data_available=WIFI_FALSE;
                    }

                    process_buffer_index = Fillptr;
                    if(Fillptr == MAX_BUFFER_GLOBAL-1 )
                    {
                          memcpy(UserDataBuff, process_buffer, Fillptr);
                          memcpy(&HTTP_Runway_Buff, &UserDataBuff[505], 6);
                          memset(&UserDataBuff[505], 0x00, 6);
                          memset(process_buffer, 0x00, MAX_BUFFER_GLOBAL);
                          memcpy(&process_buffer, &HTTP_Runway_Buff, 6);
                          memset(HTTP_Runway_Buff, 0x00, 6);
                          Fillptr = 6;
                          process_buffer_index = 6;
                          UserDataBuff_index = 505;

                          if(enable_receive_file_response)
                            FILE_Data_available = WIFI_TRUE;
                          else
                            HTTP_Data_available=WIFI_TRUE;
                    }
                    break;
              }
          }
          if((!Q_Contains_Message) && enable_receive_wifi_scan_response)
          {
                /*now check if end of msg received*/
                if((process_buffer[Fillptr-2]==0xD) && (process_buffer[Fillptr-1]==0xA))
                {       
                  if(scanned_ssids < user_scan_number)
                    {
                        pStr = (char *) strstr((const char *)&process_buffer,"CHAN:");            
                        if(pStr != NULL)
                            {
                                databytes_No[0] = *(pStr + 6) ;
                                databytes_No[1] = *(pStr + 7) ;
                      
                                chan_value = (((databytes_No[0] - '0') * 10 ) + (databytes_No[1] - '0'));
                            }

                        wifi_scanned_list[scanned_ssids].channel_num = chan_value;

                        pStr = (char *) strstr((const char *)&process_buffer,"RSSI:");            
                        if(pStr != NULL)
                            {
                                databytes_No[0] = *(pStr + 7) ;
                                databytes_No[1] = *(pStr + 8) ;
                      
                                rssi_value = (((databytes_No[0] - '0') * 10 ) + (databytes_No[1] - '0'));
                            }

                        wifi_scanned_list[scanned_ssids].rssi = -(rssi_value);

                        pStr = (char *) strstr((const char *)&process_buffer,"SSID:");
                        if(pStr != NULL)
                            {
                                index = 7;
                                while(*(pStr + index) != 0x27)
                                    {
                                        wifi_scanned_list[scanned_ssids].ssid[index-7] = *(pStr + index);
                                        index++;
                                        if(index==35) break; //max ssid lenght is 30 characters
                                    }                                
                            }

                        pStr = (char *) strstr((const char *)&process_buffer,"WPA ");            
                        if(pStr != NULL)
                            {
                                wifi_scanned_list[scanned_ssids].sec_type.wpa = WIFI_TRUE;
                            } else
                                wifi_scanned_list[scanned_ssids].sec_type.wpa = WIFI_FALSE;
                        
                        pStr = (char *) strstr((const char *)&process_buffer,"WPA2 ");            
                        if(pStr != NULL)
                            {
                                wifi_scanned_list[scanned_ssids].sec_type.wpa2 = WIFI_TRUE;
                            } else
                                wifi_scanned_list[scanned_ssids].sec_type.wpa2 = WIFI_FALSE;
                        
                        pStr = (char *) strstr((const char *)&process_buffer,"WPS ");            
                        if(pStr != NULL)
                            {
                                wifi_scanned_list[scanned_ssids].sec_type.wps = WIFI_TRUE;
                            } else
                                wifi_scanned_list[scanned_ssids].sec_type.wps = WIFI_FALSE;

                        scanned_ssids++;//increment total_networks
                    }

                  //end of one line from SCAN result       
                  pStr = (char *) strstr((const char *)&process_buffer,"ERROR");
                  if(pStr != NULL)
                  {
                   #if DEBUG_PRINT
                   printf("ERROR Scan Failed"); 
                   #endif
                   memset(process_buffer, 0x00, MAX_BUFFER_GLOBAL);
                   Fillptr=0;
                   AT_Response_Received = WIFI_TRUE;
                   enable_receive_wifi_scan_response = WIFI_FALSE;
                   Scan_Ongoing = WIFI_FALSE; //Enable next scan
                   AT_RESPONSE = WiFi_AT_CMD_RESP_ERROR;     
                   break;
                  }

                  #if DEBUG_PRINT
                  printf((const char*)process_buffer);
                  #endif

                  if(((strstr((const char *)process_buffer,"OK\r\n"))) != NULL /*|| scanned_ssids==10*/)/*Max 10 networks supported*/
                  {
                    //print and go for next line
                    //If Any part of scan line contains "OK" this will exit!!
                    #if DEBUG_PRINT
                      printf("\nOK\r\n");   
                    #endif
                    Scan_Ongoing = WIFI_FALSE; //Enable next scan             
                    scanned_ssids=0;                       
                    Fillptr=0;
                    AT_Response_Received = WIFI_TRUE;
                    enable_receive_wifi_scan_response = WIFI_FALSE;
                    AT_RESPONSE = WiFi_MODULE_SUCCESS;
                    memset(process_buffer, 0x00, MAX_BUFFER_GLOBAL);    
                    break;
                  }

                  Fillptr=0;
                  memset(process_buffer, 0x00, MAX_BUFFER_GLOBAL);          
                }

                if(Fillptr>=MAX_BUFFER_GLOBAL-1)
                {
                  #if DEBUG_PRINT
                  printf("\rHTTP: process_buffer Max Buffer Size reached\r\n");
                  #endif          
                  Fillptr=0;          
                  memset(process_buffer, 0x00, MAX_BUFFER_GLOBAL); 
                }
          }

          if((!Q_Contains_Message) && enable_fw_update_read)
          {
                  #ifdef DEBUG_PRINT
                  printf("*");     //print * till finish    
                  #endif

                  pStr = (char *) strstr((const char *)process_buffer,"Complete!");

                  if(pStr != NULL)
                  {
                        #ifdef DEBUG_PRINT
                        printf("\r\nUpdate complete\r\n");
                        #endif
                        AT_Response_Received = WIFI_TRUE;
                        AT_RESPONSE = WiFi_MODULE_SUCCESS;
                        enable_fw_update_read = WIFI_FALSE;
                        Fillptr=0;
                        memset(process_buffer, 0x00, strlen((char const *)process_buffer));
                  } 
                  if(Fillptr>=MAX_BUFFER_GLOBAL)
                  {
                      Fillptr=0;
                      memset(process_buffer, 0x00, strlen((char const *)process_buffer));
                  }

                  //No change of state till we get "+WIND:17:F/W update complete!"
          }

          /*else no other case is satisfied at least do the following*/
          {
                if(Fillptr>=MAX_BUFFER_GLOBAL-1)
                  {
                      Fillptr=0;
                      memset(process_buffer, 0x00,MAX_BUFFER_GLOBAL-1);
                  }
          }
  }
}

/**
* @brief  Process_Wind_Indication_Cmd
*         Process Wind indication command
* @param  process_buff_ptr: pointer of WiFi indication buffer
* @retval None
*/

void Process_Wind_Indication(uint8_t *process_buff_ptr)
{
  char * pStr = (char*)process_buff_ptr;
  char Indication_No[2]; 
  char databytes_No[4]; 
  #if DEBUG_PRINT
  printf((const char*)process_buff_ptr);
  #endif
  char * ptr_offset;
  int i;

  uint32_t wind55_data_len=0;
  WiFi_Indication_t Wind_No = Undefine_Indication;
  
  if(pStr != NULL)
  {
      pStr = (char *) strstr((const char *)(pStr),"WIND:");///////
      
      if(pStr != NULL)
      {
          Indication_No[0] = *(pStr + 5) ;
          Indication_No[1] = *(pStr + 6) ;
          
          if( Indication_No[1] == ':')
          {
            Single_Digit_Indication = WIFI_TRUE;
            /*  Convert char to integer */
            Wind_No = (WiFi_Indication_t)(Indication_No[0] - '0'); 
          }
          else
          {
            Single_Digit_Indication = WIFI_FALSE;
            /* Convert char to integer */   
            Wind_No = (WiFi_Indication_t)(((Indication_No[0] - '0') * 10 ) + (Indication_No[1] - '0'));
          }
          
          event_s.wind = Wind_No;
          event_s.event = WIFI_WIND_EVENT;
          
          switch (Wind_No)
          {
          case SockON_Data_Pending: /*WIND:55*/
              
                  /*+WIND:55:Pending Data:%d:%d*/   
                  ptr_offset = (char *) strstr((const char *)&process_buffer,"Data:");
            
                  /*Need to find out which socket ID has data pending*/
                  databytes_No[0] = *(ptr_offset + 5);
                  
                  SocketId = (databytes_No[0] - '0');//Max number of sockets is 8 (so single digit)
                  event_s.socket_id = SocketId; 
                  enable_pending_data = 1;
            
                  /* EQ. Check for ENC string to identify TLS case. Set enable_query */ 
                  if ( (*(ptr_offset + 7) == 'E') && (*(ptr_offset + 8) == 'N') && (*(ptr_offset + 9) == 'C') )
                  {
                        event_s.enc = WIFI_TRUE;
                  }
                  else
                  {                
                        //And find the length of the data
                        databytes_No[0] = *(ptr_offset + 7);
                        databytes_No[1] = *(ptr_offset + 8);
                        databytes_No[2] = *(ptr_offset + 9);
                        databytes_No[3] = *(ptr_offset + 10);
            
                         if( databytes_No[1] == '\r')
                         {      
                                        wind55_data_len = databytes_No[0] - '0'; 
                         }
                         else if( databytes_No[2] == '\r')
                         {
                                        wind55_data_len = (((databytes_No[0] - '0') * 10 ) + (databytes_No[1] - '0'));
                         }
                         else if( databytes_No[3] == '\r') //it's a 3-digit number
                         {
                                        wind55_data_len = (((databytes_No[0] - '0') * 100 ) + ((databytes_No[1] - '0') * 10 ) + (databytes_No[2] - '0'));
                         }
                         else   //it's a 4-digit number
                         {
                                        wind55_data_len = ((databytes_No[0] - '0') * 1000 ) + ((databytes_No[1] - '0') * 100 ) + ((databytes_No[2] - '0') * 10) + (databytes_No[3] - '0');			  
                         }                
                         
                           event_s.data_length = wind55_data_len;
                           wind55_data_len = 0;
                   }
                   break;

              case SockON_Server_Socket_Closed:
                    //Find the id of the socket closed
                    //ptr_offset = (char *) strstr((const char *)pStr,"+WIND:58");
                    databytes_No[0] = *(pStr + 22) ;
                    databytes_No[1] = *(pStr + 23) ;
                    if( databytes_No[1] == '\r')
                    {
                          remote_socket_closed_id = databytes_No[0] - '0'; 
                    }
                    else
                    {
                          remote_socket_closed_id = (((databytes_No[0] - '0') * 10 ) + (databytes_No[1] - '0'));
                    }
                    event_s.socket_id = remote_socket_closed_id;
                    break;      
                    
              case SockD_Pending_Data:
                    if(prevent_push_WIFI_event) 
                    {
                      #ifdef DEBUG_PRINT
                      printf(">>IG\r\n");
                      #endif
                      return; ///exit if push is prevented
                    }

                  /* @TODO: Do something to delay the de-Q of pending packets so that any further
                            pending WIND64 packets are allowed to arrive and get Q-ed */

                    if(event_deQ_x_wind64)      //which means there is already a previous WIND64
                    {
                      wind64_DQ_wait=0;         //reset the timer
                    }
                    else
                    {
                      stop_event_dequeue = WIFI_TRUE;//Stop the event de-Q
                      event_deQ_x_wind64 = WIFI_TRUE;//Set the flag
                    }
                    //Start Reading data from Client Here.    
                    // +WIND:64:Sockd Pending Data:1:130:130
                    ptr_offset = (char *) strstr((const char *)&process_buffer,"Data:");

                    //Store the packet number
                    databytes_No[0] = *(ptr_offset + 5) ;
                    event_s.wind64_pending_packet_no = databytes_No[0] - '0';

                    //And now find the data length
                    databytes_No[0] = *(ptr_offset + 8) ;//points to number just after 2nd colon
                    databytes_No[1] = *(ptr_offset + 9) ;
                    databytes_No[2] = *(ptr_offset + 10) ;
                    databytes_No[3] = *(ptr_offset + 11) ;
          
                    if( databytes_No[0] == ':')//then it is a 1 digit number
                    {
                      databytes_No[0] = *(ptr_offset + 7) ;
                      databytes_No[1] = *(ptr_offset + 8) ;
                    }
                    else if(databytes_No[1] == ':')//two digit number
                    {
                      databytes_No[0] = *(ptr_offset + 7) ;
                      databytes_No[1] = *(ptr_offset + 8) ;
                      databytes_No[2] = *(ptr_offset + 9) ;
                      //databytes_No[3] = *(ptr_offset + 13) ;
                    }
                    else if(databytes_No[2] == ':')//three digit number
                    {
                      databytes_No[0] = *(ptr_offset + 7) ;
                      databytes_No[1] = *(ptr_offset + 8) ;
                      databytes_No[2] = *(ptr_offset + 9) ;
                      databytes_No[3] = *(ptr_offset + 10) ;
                    }
                    else if(databytes_No[3] == ':')//four digit number
                    {
                      databytes_No[0] = *(ptr_offset + 7) ;
                      databytes_No[1] = *(ptr_offset + 8) ;
                      databytes_No[2] = *(ptr_offset + 9) ;
                      databytes_No[3] = *(ptr_offset + 10) ;
                    }

                    if( databytes_No[1] == ':')
                    {
                      interim_number_of_bytes = databytes_No[0] - '0'; 
                    }
                    else if( databytes_No[2] == ':')
                    {
                      interim_number_of_bytes = (((databytes_No[0] - '0') * 10 ) + (databytes_No[1] - '0'));
                    }
                    else if( databytes_No[3] == ':')
                    {
                      interim_number_of_bytes = (((databytes_No[0] - '0') * 100 ) + ((databytes_No[1] - '0') * 10 ) + (databytes_No[2] - '0'));
                    }
                    else //it's a 4-digit number
                    {
                      interim_number_of_bytes = (((databytes_No[0] - '0') * 1000 ) + ((databytes_No[1] - '0') * 100 ) + ((databytes_No[2] - '0') * 10 ) + (databytes_No[3] - '0'));
                    }            

                    if(WIND64_count >= 1) /*WIND:64 came after pop of previous event and switch to data mode was issued*/
                                       /*WIND:64 came before pop of previous event and switch to data mode was issued*/
                        {
                            event_s.data_length = interim_number_of_bytes;// - (730*WIND64_count);
                            WIND64_count++;           //Count of the number of queued WIND:64 Events
                            interim_number_of_bytes = 0;
                            break;
                        }
                    else
                        {
                            event_s.data_length = interim_number_of_bytes;
                            WIND64_count++; //Count of the number of queued WIND:64 Events
                            interim_number_of_bytes = 0;
                        }
                    break;

                case In_Command_Mode:
                    command_mode= WIFI_TRUE;
                    data_mode= WIFI_FALSE;
                    WIND64_count=0;     //reset the WIND64 count since the previous data mode would have consumed all data
                    break;

                case In_Data_Mode:
                    command_mode = WIFI_FALSE;
                    data_mode = WIFI_TRUE;

                    if(switch_by_default_to_command_mode == WIFI_TRUE)
                    {
                      if(!command_mode)
                      {
                        WiFi_switch_to_command_mode();//switch by default
                      }    
                    }
                    if(data_pending_sockD == WIFI_TRUE)
                      {
                          
                         process_buffer_index =5;
                         enable_sock_read = WIFI_TRUE;//now data will start coming
                        //Set the data-length to read
                        SockON_Data_Length = number_of_bytes;
                      }
                    break;
                    
                case WiFi__MiniAP_Associated:
                    //Find out which client joined by parsing the WIND //+WIND:28
                    ptr_offset = (char *) strstr((const char *)&process_buffer,"+WIND:28");
                    for(i=17;i<=33;i++)
                    client_MAC_address[i-17] = *(ptr_offset + i) ;    
                    WiFi_WIND_State.WiFiAPClientJoined = WIFI_TRUE;
                    break;
                    
                case WiFi_MiniAP_Disassociated:
                    //Find out which client left by parsing the WIND //+WIND:72
                    ptr_offset = (char *) strstr((const char *)&process_buffer,"+WIND:72");
                    for(i=17;i<=33;i++)
                    client_MAC_address[i-17] = *(ptr_offset + i) ;
                    WiFi_WIND_State.WiFiAPClientLeft = WIFI_TRUE;
                    break;

                case Console_Active:
                case Poweron :
                case WiFi_Reset:
                case Watchdog_Running:
                case Heap_Too_Small:
                case WiFi_Hardware_Dead:
                case Watchdog_Terminating:
                case SysTickConfigure:
                case Hard_Fault:
                case StackOverflow:
                case MallocFailed:
                case Error:
                case WiFi_PS_Mode_Failure:
                case CopyrightInfo:
                case WiFi_BSS_Regained:
                case WiFi_Signal_LOW:
                case WiFi_Signal_OK:
                case FW_update:
                case Encryption_key_Not_Recognized:
                case WiFi_Join:
                case JOINFAILED:
                case WiFi_Scanning:
                case SCANBLEWUP:
                case SCANFAILED:
                case WiFi_Up:
                case WiFi_Association_Successful:
                case WiFi_Started_MiniAP_Mode:
                case Start_Failed :
                case WiFi_EXCEPTION :
                case WiFi_Hardware_Started :
                case WiFi_BSS_LOST:
                case WiFi_Unhandled_Event:
                case Scan_Complete:
                case WiFi_UNHANDLED_IND:
                case WiFi_UNHANDLED:
                case WiFi_Powered_Down:
                case WiFi_MiniAP_Mode :
                case WiFi_Deauthentication:
                case WiFi_Disassociation:
                case RX_MGMT:
                case RX_DATA:
                case RX_UNK:
                case DOT11_AUTHILLEGAL:
                case Creating_PSK:
                case WPA_Terminated :
                case WPA_Supplicant_Failed:
                case WPA_Handshake_Complete:
                case GPIO_line:
                case Wakeup:
                case Factory_debug:
                case Remote_Configuration:
                     break;
                default:
                     break;
            }
         memset(process_buffer, 0x00, MAX_BUFFER_GLOBAL);
      }
   }
}

/**
* @brief  Process_Dequeued_Wind_Indication
*         Process Wind Indication after popping from Queue
* @param  event_pop_s1 popped event contents
* @retval None
*/

void Process_WiFi_Indication_Cmd(event_s_TypeDef * event_pop_s1)
{
  switch(event_pop_s1->wind)
  { 
    case Console_Active:
        WiFi_WIND_State.ConsoleActive = WIFI_TRUE;
      break;
    case Poweron :
        WiFi_WIND_State.WiFiPowerON = WIFI_TRUE;
      break;
    case WiFi_Reset:
        WiFi_WIND_State.WiFiReset = WIFI_TRUE;
      break;
    case Watchdog_Running:
      break;
    case Heap_Too_Small:
          WiFi_WIND_State.HeapTooSmall=WIFI_TRUE;
      break;
    case WiFi_Hardware_Dead:
        WiFi_WIND_State.WiFiHWFailure = WIFI_TRUE;
      break;
    case Watchdog_Terminating:
      break;
    case SysTickConfigure:
      break;
    case Hard_Fault:
        WiFi_WIND_State.HardFault = WIFI_TRUE;
      break;   
    case StackOverflow:
      WiFi_WIND_State.StackOverflow = WIFI_TRUE;
      break;
    case MallocFailed:
      WiFi_WIND_State.MallocFailed = WIFI_TRUE;
      break;
    case Error:
      WiFi_WIND_State.InitFailure = WIFI_TRUE;
      break;
    case WiFi_PS_Mode_Failure:
      WiFi_WIND_State.PS_Mode_Failure = WIFI_TRUE;
      break;
    case CopyrightInfo:
      break;
    case WiFi_BSS_Regained:
      break;
    case WiFi_Signal_LOW:
          WiFi_WIND_State.WiFiSignalLOW = WIFI_TRUE;
      break;
    case WiFi_Signal_OK :
      break;
    case FW_update:
      break;
    case Encryption_key_Not_Recognized:
      break;
    case WiFi_Join :
        WiFi_WIND_State.WiFiJoin = WIFI_TRUE;
      break;
    case JOINFAILED :
      WiFi_WIND_State.WiFiJoinFailed = WIFI_TRUE;
      break;
    case WiFi_Scanning :
        WiFi_WIND_State.WiFiScanning = WIFI_TRUE;
      break;
    case SCANBLEWUP:
      WiFi_WIND_State.WiFiScanBlewUp = WIFI_TRUE;
      break;
    case SCANFAILED:
      WiFi_WIND_State.WiFiScanFailed = WIFI_TRUE;
      break;
    case WiFi_Up:
        WiFi_WIND_State.WiFiUp = WIFI_TRUE;
      break;
    case WiFi_Association_Successful:
        WiFi_WIND_State.WiFiAssociation = WIFI_TRUE;
      break;
    case WiFi_Started_MiniAP_Mode:
        WiFi_WIND_State.WiFiStarted_MiniAPMode = WIFI_TRUE;
      break;
    case Start_Failed :
        WiFi_WIND_State.StartFailed = WIFI_TRUE;  
      break;
    case WiFi_EXCEPTION :
        WiFi_WIND_State.WiFiException = WIFI_TRUE;
      break;
    case WiFi_Hardware_Started :
        wifi_ready++;
        WiFi_Enabled = WIFI_TRUE;
        WiFi_WIND_State.WiFiHWStarted = WIFI_TRUE;
        /*If this is a start-up after standby*/
        if(trigger_wakeup_callback == WIFI_TRUE)
          {
            trigger_wakeup_callback = WIFI_FALSE;
            Standby_Enabled = WIFI_FALSE;
            standby_resume_callback = WIFI_TRUE;
          }      
      break;
    case WiFi_BSS_LOST:
      break;
    case WiFi_Unhandled_Event:
      break;
    case Scan_Complete:
        WiFi_WIND_State.WiFiScanComplete = WIFI_TRUE;
        Scan_Ongoing = WIFI_FALSE;
      break;
    case WiFi_UNHANDLED_IND:
      WiFi_WIND_State.WiFiUnHandledInd = WIFI_TRUE;
      break;
    case WiFi_UNHANDLED:
      break;
    case WiFi_Powered_Down:
        WiFi_Enabled = WIFI_FALSE;      
        //wifi_ready = 0;
        WiFi_WIND_State.WiFiHWStarted = WIFI_FALSE;
        WiFi_WIND_State.WiFiPowerDown = WIFI_TRUE;
      break;
    case WiFi_MiniAP_Mode :
        WiFi_WIND_State.WiFiMiniAPMode = WIFI_TRUE;
      break;
    case WiFi_Deauthentication:
        WiFi_WIND_State.WiFiDeauthentication = WIFI_TRUE;
      break;    
    case WiFi_Disassociation:
      WiFi_WIND_State.WiFiDisAssociation = WIFI_TRUE;
      break;
    case RX_MGMT:
      WiFi_WIND_State.WiFiRXMgmt = WIFI_TRUE;
      break;
    case RX_DATA:
      WiFi_WIND_State.WiFiRXData = WIFI_TRUE;
      break;
    case RX_UNK:
      WiFi_WIND_State.WiFiRxUnk = WIFI_TRUE;
      break;
    case DOT11_AUTHILLEGAL:
      break;
    case Creating_PSK:
      break; 
    case WPA_Terminated :
      break;
    case WPA_Supplicant_Failed:
      break;
    case WPA_Handshake_Complete:
      break;
    case GPIO_line:
      break;
    case Wakeup:
      break;
    case Factory_debug:
      break;  

    case SockON_Data_Pending:

      /* +WIND:55:Pending Data:%d:%d */
      if (enable_sock_read == WIFI_TRUE)
      {       
          #if DEBUG_PRINT
            printf ("\nAlert!\r\n");
          #endif
          enable_sock_read = WIFI_FALSE;
          enable_receive_data_chunk = WIFI_FALSE;
          //break;
      }
      sockon_query_id = event_pop_s1->socket_id;
      if(open_sockets[sockon_query_id])
      {
          enable_query = WIFI_TRUE;
          stop_event_dequeue = WIFI_TRUE;
      }

       break;

    case SockON_Server_Socket_Closed:
      client_socket_close_id = event_pop_s1->socket_id;
      Client_Socket_Close_Cmd = WIFI_TRUE;
      break;

    case SockD_Pending_Data:
      number_of_bytes = event_pop_s1->data_length;
      data_pending_sockD = WIFI_TRUE;
      stop_event_dequeue = WIFI_TRUE;             //Stop any more event de-queue
      enable_receive_data_chunk = WIFI_TRUE;       // read data in chunk now from ring buffer

      if(!data_mode)
        {
          if(event_pop_s1->wind64_pending_packet_no == 1) { //If this is the first WIND64 pending event de-Q'ed
              switch_by_default_to_command_mode = WIFI_FALSE; //we don't want to switch back to command mode after changing to data mode here
              WiFi_switch_to_data_mode();     //switch by default
          }
        }
      else //already data is coming from previous WIND:64
      {
          process_buffer_index =5;
          enable_sock_read = WIFI_TRUE;//n
          SockON_Data_Length = number_of_bytes;
      }
      break;

    case Incoming_socket_client:
      Client_Connected = WIFI_TRUE;
      wifi_client_connected=1;  //Set this so that the callback can be made to the user
      break;

    case Outgoing_socket_client:
      Client_Disconnected = WIFI_TRUE;
      wifi_client_disconnected=0;//Set this so that the callback can be made to the user
      wifi_client_connected = 0;
      break;

    case SockD_Dropping_Data:
      WiFi_WIND_State.WiFiSockdDataLost = WIFI_TRUE;
      break;

    case Low_Power_Mode_Enabled:
      Low_Power_Enabled = WIFI_TRUE;
      break;

    case Going_Into_Standby:
      Standby_Enabled = WIFI_TRUE;
      break;

    case Resuming_From_Standby:
      Standby_Enabled = WIFI_FALSE;
      standby_resume_callback = WIFI_TRUE;
      break;    
    case Going_Into_DeepSleep:
      Deep_Sleep_Enabled = WIFI_TRUE;    
      break;
    case Resuming_From_DeepSleep:
      Deep_Sleep_Enabled = WIFI_FALSE;
      Start_DeepSleep_Timer();
      break;
    default:
      break;
  }
}

/**
* @brief  Queue_Http_Get_ Event
*         Queue an HTTP-Request Event (GET/POST)
* @param  hostname hostname for HTTP-GET/POST
* @param  path path for HTTP-GET
* @param  port_number port_number for HTTP-GET
* @param  pURL_path full URL for HTTP-POST
* @retval None
*/

void Queue_Http_Event(uint8_t * hostname, uint8_t * path, uint32_t port_number, uint8_t * pURL_path) 
{
   Wait_For_Sock_Read_To_Complete();
   if(pURL_path == NULL)
    {
      curr_hostname = hostname;
      curr_path = path;
      curr_port_number = port_number;
    }
    else
      curr_pURL = pURL_path;

    event_s.event = WIFI_HTTP_EVENT;
    __disable_irq();
    push_eventbuffer(&event_buff, event_s);
     __enable_irq();
    
    reset_event(&event_s);
}

/**
* @brief  Queue_Client_Write_Event
*         Queues a Client Socket write event.
* @param  sock_id socket ID to write to
* @param  DataLength length of the data to be written
* @param  pData pointer to data
* @retval None
*/
void Queue_Client_Write_Event(uint8_t sock_id, uint16_t DataLength, char * pData)
{
    Wait_For_Sock_Read_To_Complete();
    curr_DataLength = DataLength;
    curr_data = pData;
    curr_sockID = sock_id;
    enable_client_socket_write = WIFI_TRUE;

    event_s.event = WIFI_CLIENT_SOCKET_WRITE_EVENT;
    __disable_irq();
    push_eventbuffer(&event_buff, event_s);
    __enable_irq();

    reset_event(&event_s);
}

/**
* @brief Queue_Wifi_File_Image_Create_Event
*        Queue a File Image Create Event
* @param pHostName hostname
* @param pFileName filename within host
* @param port_number port number to connect to
* @retval None
*/
void Queue_Wifi_File_Event(uint8_t * pHostName, uint8_t * pFileName, uint32_t port_number)
{
    Wait_For_Sock_Read_To_Complete();
    event_s.event = WIFI_FILE_EVENT;
    curr_filename = pFileName;
    curr_hostname = pHostName;
    curr_port_number = port_number;

    __disable_irq();
    push_eventbuffer(&event_buff, event_s);
    __enable_irq();

    reset_event(&event_s);
}

/**
* @brief Queue_Wifi_FW_Update_Event
*        Queue a Firmware update Event
* @param hostname hostname
* @param filename_path filename and path within host
* @param port_number port number to connect to
* @retval None
*/
void Queue_Wifi_FW_Update_Event(uint8_t * hostname, uint8_t * filename_path, uint32_t port_number)
{
    Wait_For_Sock_Read_To_Complete();
    event_s.event = WIFI_FW_UPDATE_EVENT;
    curr_filename = filename_path;
    curr_hostname = hostname;
    curr_port_number = port_number;

    __disable_irq();
    push_eventbuffer(&event_buff, event_s);
    __enable_irq();

    reset_event(&event_s);
}

void Queue_Client_Open_Event(uint8_t * hostname, uint32_t port_number, uint8_t * protocol)
{
    Wait_For_Sock_Read_To_Complete();
    curr_hostname = hostname;
    curr_port_number = port_number;
    curr_protocol = protocol;
    
    event_s.event = WIFI_CLIENT_SOCKET_OPEN_EVENT;
     __disable_irq();
    push_eventbuffer(&event_buff, event_s);
    __enable_irq();

    reset_event(&event_s);
}

void Queue_Client_Close_Event(uint8_t sock_id)
{
    Wait_For_Sock_Read_To_Complete();

    event_s.event = WIFI_CLIENT_SOCKET_CLOSE_EVENT;
    event_s.socket_id = sock_id;
     __disable_irq();
    push_eventbuffer(&event_buff, event_s);
    __enable_irq();

    reset_event(&event_s);
}

/**
* @brief Wait_For_Sock_Read_To_Complete
*        Wait till sock is over and the OK of read arrives
* @param None
* @retval None
*/
void Wait_For_Sock_Read_To_Complete(void)
{
  //wait if read is ongoing or read OK is yet to arrive
  while(sock_read_ongoing==WIFI_TRUE || prevent_push_OK_event==WIFI_TRUE);
}

/**
* @brief  Reset_AT_CMD_Buffer
*         Clear USART2 Rx buffer and Wi-Fi AT cmd buffer
* @param  None
* @retval None
*/
void Reset_AT_CMD_Buffer()
{
  memset(WiFi_AT_Cmd_Buff, 0x00, sizeof WiFi_AT_Cmd_Buff); 
}

#ifdef  USE_FULL_ASSERT

/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {
  }
}

#endif


/**
* @brief  search_buffer
*         search for substring in a buffer that contains null
* @param  pSourceBuff   : pointer of source buffer
*         sourceBuffLen : length of string buffer
*         pSearchStringBuff : pointer of search string buffer
*         seartchStringLen  : length of search string buffer
* @retval pointer of source buffer
*/
char *search_buffer(char *pSourceBuff, uint16_t sourceBuffLen, char *pSearchStringBuff, uint16_t seartchStringLen)
{   /* warning: O(n^2) */
    int searchlen = sourceBuffLen - seartchStringLen + 1;
    for ( ; searchlen-- > 0; pSourceBuff++)
        if (!memcmp(pSourceBuff, pSearchStringBuff, seartchStringLen))
            return pSourceBuff;
    return NULL;
}

/**
* @brief  Delete_Colon
*         delete colon from input buffer
* @param  input : pointer of input buffer
* @retval return pointer of updated buffer
*/
char* Delete_Colon(char* input)                                         
{
    int i,j;
    char *output=input;
    
    /* Delete Colon */
    for (i = 0, j = 0; i<strlen(input); i++,j++)          
    {
#if 0     
      if (input[i]!=':') && (input[i]!='\r')&& (input[i]!='\n')&& (input[i]!='O')&& (input[i]!='K'))
            output[j]=input[i];                     
        else
            j--;                                     
#else
      if ((input[i] ==':') || (input[i]=='\r')|| (input[i]=='\n')|| (input[i]=='O')|| (input[i]=='K'))
            j--;                                     
        else
          output[j]=input[i];                            
    }
    //output[j]=NULL;    
#endif  
    return output;
}

/**
* @brief  Read_WiFi_SSID
*         Read SSID of WiFi module store in flash 
* @param  string : pointer of SSID
* @retval return status of AT cmd request
*/
WiFi_Status_t Read_WiFi_SSID(char *string)
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;
  char * ssid = "wifi_ssid";
  char * pStr;
  /* AT+S.GCFG=wifi_ssid read SSID */
  Reset_AT_CMD_Buffer();

  /* AT : send AT command */  
  sprintf((char*)WiFi_AT_Cmd_Buff,AT_GET_CONFIGURATION_VALUE,ssid);   

  /* 
    Response :
    # wifi_ssid = 41:6E:53:53:49:44:00:00:00:00:00:00:00:00:00:00:00:00:00:
    00:00:00:00:00:00:00:00:00:00:00:00:00<cr><lf>
    <cr><lf>OK<cr><lf>
    */

  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
  if(status == WiFi_MODULE_SUCCESS)
  {
    status = USART_Receive_AT_Resp(Process_Event);
  }

    /* find a substring (# wifi_ssid = )inside a USART2_RxBuffer that may contain nulls */
    uint16_t sourceBuffLen = sizeof(get_cfg_value)-1; /* exclude null terminator from length */
    char searchStringBuff[] = "#  wifi_ssid = ";
    uint16_t stringBuffLen = sizeof(searchStringBuff)-1; /* exclude null terminator from length */    
    char *res = search_buffer((char *)&get_cfg_value, sourceBuffLen, searchStringBuff, stringBuffLen);

    pStr = (char *) (strstr((const char *)res,"= "));
    if(pStr != NULL)
    {
      strcat( string, (pStr + 2));      
      /* Remove colon,\r,\n,OK strings */
      memcpy(string, Delete_Colon(string) , 32); 
    }
  return status;    
}

/**
* @brief  Read_WiFi_SecKey
*         Read Security key of WiFi module store in flash 
* @param  string : pointer of Security key
* @retval return status of AT cmd request
*/
WiFi_Status_t Read_WiFi_SecKey(char *string)
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;
  char *seckey = "wifi_wpa_psk_text";
  char *pStr;
  
  /* AT+S.GCFG=wifi_ssid read SSID */
  Reset_AT_CMD_Buffer();
  
  /* AT : send AT command */  
  sprintf((char*)WiFi_AT_Cmd_Buff,AT_GET_CONFIGURATION_VALUE,seckey);  
  
  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
  if(status == WiFi_MODULE_SUCCESS)
  {
    status = USART_Receive_AT_Resp(Process_Event);
  }


    /* find a substring (wifi_wpa_psk_text = )inside a USART2_RxBuffer that may contain nulls */
    uint16_t sourceBuffLen = sizeof(get_cfg_value)-1; /* exclude null terminator from length */
    char searchStringBuff[] = "wifi_wpa_psk_text = ";
    uint16_t stringBuffLen = sizeof(searchStringBuff)-1; /* exclude null terminator from length */    
    char *res = search_buffer((char *)&get_cfg_value, sourceBuffLen, searchStringBuff, stringBuffLen);
    
    pStr = (char *) (strstr((const char *)res,"= "));
    if(pStr != NULL)
    {
      strcat( string, (pStr + 2));      
      /* Remove colon,\r,\n,OK strings */
      memcpy(string, Delete_Colon(string) , 32); 
    }
    
  return status;    
}

/**
* @brief  Read_WiFi_Mode
*         Read Wi-Fi mode 0: idle,1 =STA,2 =IBSS,3 =MiniAP
* @param  string : return wifi mode type
* @retval return status of AT cmd request
*/
WiFi_Status_t Read_WiFi_Mode(char *string)
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;
  char *mode = "wifi_mode";
  char *pStr;
  
    /* AT+S.GCFG=wifi_mode */
  Reset_AT_CMD_Buffer();
  
  /* AT : send AT command */  
  sprintf((char*)WiFi_AT_Cmd_Buff,AT_GET_CONFIGURATION_VALUE,mode);  
  
  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
  if(status == WiFi_MODULE_SUCCESS)
  {
    status = USART_Receive_AT_Resp(Process_Event);
  }
  
  pStr = (char *) strstr((const char *)&get_cfg_value,"wifi_mode = ");
  if(pStr != NULL)
  {
    string[0] = *(pStr + 12) ;
  }

  return status ;
}

/**
* @brief  Write_WiFi_SSID
*         Store SSID in flash memory of WiFi module
* @param  string : pointer of SSID
* @retval return status of AT cmd request
*/
WiFi_Status_t Write_WiFi_SSID(char *string)
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;  
  Reset_AT_CMD_Buffer(); 
  
  /* AT+S.SSIDTXT=abcd <ExampleSSID> //set SSID */
  sprintf((char*)WiFi_AT_Cmd_Buff,AT_SET_SSID,string);  
  
  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
  if(status == WiFi_MODULE_SUCCESS)
  {
    status = USART_Receive_AT_Resp(Process_Event);
  }
  
  /* AT&W :Save the settings on the flash memory */
  Reset_AT_CMD_Buffer();
  Save_Current_Setting();
  
  return status; 
  
}


/**
* @brief  Write_WiFi_SecKey
*         Store security key in flash memory of WiFi module
* @param  string : pointer of security key
* @retval return status of AT cmd request
*/
WiFi_Status_t Write_WiFi_SecKey(char *string)
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;  
  Reset_AT_CMD_Buffer(); 
  
  /* AT+S.SCFG=wifi_wpa_psk_text,helloworld : set password */
  sprintf((char*)WiFi_AT_Cmd_Buff,"AT+S.SCFG=wifi_wpa_psk_text,%s\r",string);
  
  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
  if(status == WiFi_MODULE_SUCCESS)
  {
    status = USART_Receive_AT_Resp(Process_Event);
  }
  
  /* AT&W :Save the settings on the flash memory */
  Reset_AT_CMD_Buffer();
  Save_Current_Setting();
  
  return status;     
}



/**
* @brief  PrintErrorMsg
*         Print error message on UART terminal
* @param  None
* @retval None
*/
void PrintErrorMsg (void)
{
  Print_Msg("error in AT cmd",sizeof("error in AT cmd"));
}

/**
  * @brief  Print_Msg
  *         Print messages on UART terminal
  * @param  msgBuff : Contains data that need to be print
  * @param  length  : leghth of the data
  * @retval None
  */
void Print_Msg(char * msgBuff,uint8_t length)
{

}

/**
* @brief  Error_Handler
*         This function is executed in case of error occurrence.
* @param  None
* @retval None
*/
void Error_Handler(void)
{
  /* Turn LED2 on */
  BSP_LED_On(LED2);
  //The following while(1) is commented as it prevents standby functionality
  /*while(1)
  {
    //Error if LED2 is slowly blinking (1 sec. period)
    BSP_LED_Toggle(LED2); 
    HAL_Delay(500);
  } */
  Receive_Data();
}


/**
* @brief  USART_Transmit_AT_Cmd
*         send AT cmd on UART port of wifi module.
* @param  size size of the
*  AT command
* @retval WiFi_Status_t : status of AT cmd
*/
WiFi_Status_t USART_Transmit_AT_Cmd(uint16_t size)
{
  //Check for Hardware Started
  if(WiFi_Enabled == WIFI_FALSE) 
    return WiFi_NOT_READY;
  //Check for Deep-Sleep or Standby Mode, return error if true
  if (Standby_Enabled == WIFI_TRUE || Deep_Sleep_Enabled == WIFI_TRUE)
    return WiFi_IN_LOW_POWER_ERROR;

  AT_Cmd_Processing = WIFI_TRUE;//Stop Any Rx between the TX call

  if (size == 0)
    {
        printf("ERROR in USART_Transmit_AT_Cmd!");
        return WiFi_UNHANDLED_IND_ERROR;
    }

#if defined(USART3_INT_MODE)
  if(HAL_UART_Transmit_IT(&UartWiFiHandle, (uint8_t *)WiFi_AT_Cmd_Buff, size)!= HAL_OK)
  {
    Error_Handler();
    return WiFi_HAL_UART_ERROR;
  }
  while (UartReady != SET);
  UartReady = RESET; 

#elif defined(USART3_POLLING_MODE)
  //while(Uartx_Rx_Processing!=WIFI_FALSE);
  if(HAL_UART_Transmit(&UartWiFiHandle, (uint8_t *)WiFi_AT_Cmd_Buff, size, 1000)!= HAL_OK)
  {
    Error_Handler();
    #if DEBUG_PRINT
    printf("HAL_UART_Transmit Error");
    #endif
    return WiFi_HAL_UART_ERROR;
  }

#else
 #error "Please select USART mode in your application (in wifi_module.h file)"
#endif
 
  AT_Cmd_Processing = WIFI_FALSE;//Re-enable Rx for UART
  if(Uartx_Rx_Processing == WIFI_FALSE)
    Receive_Data();//Start receiving Rx from the UART again, if and only if it was stopped in the previous Uartx_Rx_Handler
  return WiFi_MODULE_SUCCESS;
}

/**
* @brief  Start_DeepSleep_Timer
*         start the deep sleep timer.
* @param  None
* @retval void
*/
void Start_DeepSleep_Timer(void)
{
  Deep_Sleep_Timer = WIFI_TRUE;
  sleep_count = 0;
}

/**
* @brief  Stop_DeepSleep_Timer
*         stop the deep sleep timer.
* @param  None
* @retval void
*/
void Stop_DeepSleep_Timer()
{
  Deep_Sleep_Timer = WIFI_FALSE;
  sleep_count = 0;
}

#if 0
/**
* @brief  configure_to_exti
*         Configured the USART Rx pin to EXTI pin to capture standby wakeup interrupt
* @param  None
* @retval None
*/
void configure_to_exti()
{
  /*configure the pin*/
  
  HAL_NVIC_DisableIRQ(USARTx_IRQn);//Disable UART IRQ
  
  /* USART_RX Pin as EXTI IRQ*/
  GPIO_InitTypeDef  GPIO_InitStruct;
  GPIO_InitStruct.Pin = WiFi_USART_RX_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
#if defined (USE_STM32L0XX_NUCLEO) || (USE_STM32F4XX_NUCLEO) 
  GPIO_InitStruct.Alternate = 0;
#endif
  HAL_GPIO_Init(WiFi_USART_RX_GPIO_PORT, &GPIO_InitStruct);
  
  /* Configure the NVIC for EXTI */  
  HAL_NVIC_SetPriority(USARTx_EXTI_IRQn, 3, 0);    
  HAL_NVIC_EnableIRQ(USARTx_EXTI_IRQn);
}
#endif

/**
* @brief  WiFi_switch_to_command_mode
*         switch to command mode from data mode
* @param  None
* @retval None
*/
void WiFi_switch_to_command_mode(void)
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;

  /* AT+S.*/  
  Reset_AT_CMD_Buffer();  

  sprintf((char*)WiFi_AT_Cmd_Buff,AT_DATA_TO_CMD_MODE);   //Notice the lower case

  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
  if(status == WiFi_MODULE_SUCCESS)
  {
    //nothing to do
  }
}


/**
* @brief  WiFi_switch_to_data_mode
*         switch to data mode from command mode
* @param  None
* @retval None
*/
void WiFi_switch_to_data_mode(void)
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;
  
  /* AT+S.*/  
  Reset_AT_CMD_Buffer();    
  
  sprintf((char*)WiFi_AT_Cmd_Buff,AT_CMD_TO_DATA_MODE);   //Notice the upper case
  
  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
  
  if(status == WiFi_MODULE_SUCCESS)
  {
    //nothing to do
  }
   
}


/**
* @brief  Attention_Cmd
*         Attention command
* @param  None
* @retval WiFi_Status_t : status of AT cmd Request
*/
WiFi_Status_t Attention_Cmd()
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;
  
  Reset_AT_CMD_Buffer(); 
  
  /* AT : send AT command */
  sprintf((char*)WiFi_AT_Cmd_Buff,AT_ATTENTION);  

  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
  if(status == WiFi_MODULE_SUCCESS)
  {
    status = USART_Receive_AT_Resp(Process_Event);
  }
  return status; 
}


/**
* @brief  SET_Power_State
*         SET power mode of wifi module
* @param  state : power mode of wi-fi module i.e active,sleep,standby,powersave
* @retval WiFi_Status_t : status of AT cmd Request
*/
WiFi_Status_t SET_Power_State(WiFi_Power_State_t state)
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;
  
#if DEBUG_PRINT  
  printf("\r\n >>Soft Reset Wi-Fi module\r\n");
#endif
  
  Reset_AT_CMD_Buffer();
  
  /* AT : send AT command */
  sprintf((char*)WiFi_AT_Cmd_Buff,AT_SET_POWER_STATE,state);  
//  WiFi_Module_State = Process_Event; 
//  WiFi_WIND_State.WiFiReset = WIFI_FALSE;
  WiFi_WIND_State.WiFiHWStarted = WIFI_FALSE;
  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
  if(status != WiFi_MODULE_SUCCESS) 
    return status;
  memset((void*)&WiFi_WIND_State,0x00,sizeof(WiFi_WIND_State)); /*reset the WIND State?*/
  /* AT+CFUN=1 //Soft reset */
  while(WiFi_WIND_State.WiFiHWStarted != WIFI_TRUE);
  return status;
}
  

/**
* @brief  Display_Help_Text
*         this function will print a list of all commands supported with a brief help text for each cmd
* @param  None
* @retval WiFi_Status_t : status of AT cmd Request
*/
WiFi_Status_t Display_Help_Text()
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;
  Reset_AT_CMD_Buffer();

  /* AT : send AT command */
  sprintf((char*)WiFi_AT_Cmd_Buff,AT_HELP_TEXT);  

  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
  if(status == WiFi_MODULE_SUCCESS)
  {
    status = USART_Receive_AT_Resp(Process_Event);
  }
  return status; 
}

/**
* @brief  GET_Configuration_Value
*         Get a wifi status value from the module
* @param  sVar_name : Name of the status variable
*         aValue    : value of status variable to be returned to user
* @retval WiFi_Status_t : status of AT cmd Request
*/
WiFi_Status_t GET_Status_Value(char* sVar_name,uint32_t *aValue)
{
  int cfg_value_length;
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;

  Reset_AT_CMD_Buffer();

  /* AT : send AT command */
  sprintf((char*)WiFi_AT_Cmd_Buff,AT_GET_STATUS_VALUE,sVar_name);

  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
  if(status == WiFi_MODULE_SUCCESS)
  {
    status = USART_Receive_AT_Resp(Process_Event);
    cfg_value_length = strlen((const char*)get_cfg_value);
    memcpy(aValue,get_cfg_value,cfg_value_length);   //copy user pointer to get_cfg_value
    memset(get_cfg_value, 0x00,cfg_value_length);
  }
  return status;
}

/**
* @brief  GET_Configuration_Value
*         Get a wifi configuration value from the module
* @param  sVar_name : Name of the config variable
*         aValue    : value of config variable to be returned to user
* @retval WiFi_Status_t : status of AT cmd Request
*/
WiFi_Status_t GET_Configuration_Value(char* sVar_name,uint32_t *aValue)
{
  int cfg_value_length;
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;

  Reset_AT_CMD_Buffer(); 

  /* AT : send AT command */  
  sprintf((char*)WiFi_AT_Cmd_Buff,AT_GET_CONFIGURATION_VALUE,sVar_name);   

  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
  if(status == WiFi_MODULE_SUCCESS)
  {
    status = USART_Receive_AT_Resp(Process_Event);
    cfg_value_length = strlen((const char*)get_cfg_value);
    memcpy(aValue,get_cfg_value,cfg_value_length);   //copy user pointer to get_cfg_value
    memset(get_cfg_value, 0x00,cfg_value_length);
  }
  return status; 
}

/**
* @brief  SET_Configuration_Addr
*         Get a wifi configuration address from the module
* @param  sVar_name : Name of the config variable
*         addr    : value of config address to be returned to user
* @retval WiFi_Status_t : status of AT cmd Request
*/
WiFi_Status_t SET_Configuration_Addr(char* sVar_name,char* addr)
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;

  Reset_AT_CMD_Buffer(); 

  /* AT : send AT command */
  sprintf((char*)WiFi_AT_Cmd_Buff,AT_SET_CONFIGURATION_ADDRESS,sVar_name,addr);  

  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
  if(status == WiFi_MODULE_SUCCESS)
  {
    status = USART_Receive_AT_Resp(Process_Event);
  }
  return status;
}

/**
* @brief  SET_Configuration_Value
*         SET the value of configuration variable
* @param  sVar_name : Name of the config variable
*         aValue    : value of config variable
* @retval WiFi_Status_t : status of AT cmd Request
*/
WiFi_Status_t SET_Configuration_Value(char* sVar_name,uint32_t aValue)
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;

  Reset_AT_CMD_Buffer(); 

  /* AT : send AT command */
  sprintf((char*)WiFi_AT_Cmd_Buff,AT_SET_CONFIGURATION_VALUE,sVar_name,(int)aValue);  

  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
  if(status == WiFi_MODULE_SUCCESS)
  {
    status = USART_Receive_AT_Resp(Process_Event);
  }
  return status; 
}

/**
* @brief  SET_SSID
*         SET SSID in flash memory of Wi-Fi module
* @param  ssid : pointer of SSID
* @retval WiFi_Status_t : status of AT cmd Request
*/
WiFi_Status_t SET_SSID(char* ssid)
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;
  
  Reset_AT_CMD_Buffer(); 
  
  /* AT+S.SSIDTXT=abcd <ExampleSSID>  */
  sprintf((char*)WiFi_AT_Cmd_Buff,AT_SET_SSID,ssid);  

  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
  if(status == WiFi_MODULE_SUCCESS)
  {
    status = USART_Receive_AT_Resp(Process_Event);
  }
  return status; 
}


/**
* @brief  SET_WiFi_SecKey
*         SET wifi security key
* @param  seckey : pointer of security key
* @retval WiFi_Status_t : status of AT cmd Request
*/
WiFi_Status_t SET_WiFi_SecKey(char* seckey)
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;
  
  Reset_AT_CMD_Buffer(); 
  
  /* AT+S.SCFG=wifi_wpa_psk_text,helloworld : set password */
  sprintf((char*)WiFi_AT_Cmd_Buff,AT_SET_SEC_KEY,seckey);  

  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
  if(status == WiFi_MODULE_SUCCESS)
  {
    status = USART_Receive_AT_Resp(Process_Event);
  }
  return status;    
}


/**
* @brief  Restore_Default_Setting
*         Restore the factory default values of the configuration variables 
*         and writes them to non volatile storage
* @param  None
* @retval WiFi_Status_t : status of AT cmd Request
*/
WiFi_Status_t Restore_Default_Setting()
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;
  
  //Reset_AT_CMD_Buffer(); 
  
  /* AT&F: restore default setting */
  sprintf((char*)WiFi_AT_Cmd_Buff,AT_RESTORE_DEFAULT_SETTING);  

  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
  
  if(status == WiFi_MODULE_SUCCESS)
  {
    status = USART_Receive_AT_Resp(Process_Event);
  }
  return status; 
  
}

/**
* @brief  Save_Current_Setting
*         Store the current RAM-based setting to non-volatile storage
* @param  None
* @retval WiFi_Status_t : status of AT cmd Request
*/
WiFi_Status_t Save_Current_Setting()
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;
  
  Reset_AT_CMD_Buffer(); 
  
  /* AT&W :Save the settings on the flash memory */
  sprintf((char*)WiFi_AT_Cmd_Buff,AT_SAVE_CURRENT_SETTING);  

  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
  if(status == WiFi_MODULE_SUCCESS)
  {
    status = USART_Receive_AT_Resp(Process_Event);
  }
  return status; 
}


/**
* @brief  ResetBuffer
*         Reset receive data/indication msg buffer
* @param  None
* @retval None
*/
void ResetBuffer()
{  
  
}


/**
* @brief  config_init_value
*         initalize config values before reset
* @param  sVar_name : Name of the config variable
*         aValue    : value of config variable
* @retval None
*/
WiFi_Status_t config_init_value(char* sVar_name,uint8_t aValue)
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;
  Reset_AT_CMD_Buffer();   
  sprintf((char*)WiFi_AT_Cmd_Buff,AT_SET_CONFIGURATION_VALUE,sVar_name,aValue);
  if(HAL_UART_Transmit(&UartWiFiHandle, (uint8_t *)WiFi_AT_Cmd_Buff, strlen((char*)WiFi_AT_Cmd_Buff),1000)!= HAL_OK)
  {
    Error_Handler();    
    return WiFi_HAL_UART_ERROR;
  }
  
  status = WaitForResponse(AT_RESP_LEN_OK);
  return status;
}

/**
* @brief  config_init_addr
*         initalize config strings/addresses before reset
* @param  sVar_name : Name of the config variable
*         addr    : value of config address to be returned to user
* @retval None
*/
WiFi_Status_t config_init_addr(char* sVar_name,char* addr)
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;
  Reset_AT_CMD_Buffer();   
  sprintf((char*)WiFi_AT_Cmd_Buff,AT_SET_CONFIGURATION_ADDRESS,sVar_name,addr);
  if(HAL_UART_Transmit(&UartWiFiHandle, (uint8_t *)WiFi_AT_Cmd_Buff, strlen((char*)WiFi_AT_Cmd_Buff),1000)!= HAL_OK)
  {
    Error_Handler();    
    return WiFi_HAL_UART_ERROR;
  }
  
  status = WaitForResponse(AT_RESP_LEN_OK);
  return status;

}


/**
* @brief  WaitForResponse
*         Wait for OK response
* @param  alength length of the data to be received
* @retval None
*/
WiFi_Status_t WaitForResponse(uint16_t alength)
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;
  
  if(alength <= RxBufferSize)
  {
    if(HAL_UART_Receive(&UartWiFiHandle, (uint8_t *)USART_RxBuffer, alength,5000)!= HAL_OK)
    {
      Error_Handler();
      return WiFi_HAL_UART_ERROR;
    }
    
    if(((strstr((const char *)&USART_RxBuffer,"OK"))) == NULL)
    {
      return WiFi_AT_CMD_RESP_ERROR;
    }
    
  }
  
  return status;  
}
/**** Wi-Fi indication call back *************/
__weak void ind_wifi_warning(WiFi_Status_t warning_code)
{
}
	
__weak void ind_wifi_error(WiFi_Status_t error_code)
{
}

__weak void ind_wifi_connection_error(WiFi_Status_t status_code)
{
}

__weak void ind_wifi_connected(void)
{
}
	
__weak void ind_wifi_ap_ready(void)
{
}

__weak void ind_wifi_ap_client_joined(uint8_t * client_mac_address)
{
}

__weak void ind_wifi_ap_client_left(uint8_t * client_mac_address)
{
}

__weak void ind_wifi_on(void)
{
}

__weak void ind_wifi_packet_lost(WiFi_Status_t status_code)
{
}

__weak void ind_wifi_gpio_changed(void)
{
}

__weak void ind_wifi_socket_data_received(uint8_t socket_id, uint8_t * data_ptr, uint32_t message_size, uint32_t chunk_size)
{
}

__weak void ind_wifi_socket_client_remote_server_closed(uint8_t * socketID)
{
}

__weak void ind_wifi_socket_server_data_lost(void)
{
}

__weak void ind_socket_server_client_joined(void)
{
}

__weak void ind_socket_server_client_left(void)
{
}

__weak void ind_wifi_http_data_available(uint8_t * data_ptr,uint32_t message_size)
{
}

__weak void ind_wifi_file_data_available(uint8_t * data_ptr)
{
}
__weak void ind_wifi_resuming(void)
{
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

/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/

