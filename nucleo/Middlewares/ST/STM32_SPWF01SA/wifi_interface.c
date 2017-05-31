/**
 ******************************************************************************
 * @file    wifi_interface.c
 * @author  Central LAB
 * @version V2.0.0
 * @date    10-February-2016
 * @brief   User APIs implementation for X-CUBE-WIFI1
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

/** @addtogroup MIDDLEWARES
* @{
*/ 


/** @defgroup  NUCLEO_WIFI_INTERFACE
  * @brief Wi-Fi User API modules
  * @{
  */


/** @defgroup NUCLEO_WIFI_INTERFACE_Private_Defines
  * @{
  */


/**
  * @}
  */

/** @addtogroup NUCLEO_WIFI_INTERFACE_Private_Variables
  * @{
  */
/* Private variables ---------------------------------------------------------*/

#if defined (USE_STM32F4XX_NUCLEO) || (USE_STM32L4XX_NUCLEO)
extern uint8_t WiFi_AT_Cmd_Buff[2048];
#else
extern uint8_t WiFi_AT_Cmd_Buff[1024];
#endif

//extern uint8_t WiFi_AT_Cmd_Buff[1024];
extern char UserDataBuff[MAX_BUFFER_GLOBAL];
extern wifi_scan *wifi_scanned_list;//[15];
extern char print_msg_buff[MAX_BUFFER_GLOBAL];
extern UART_HandleTypeDef UartWiFiHandle;
extern uint8_t user_scan_number;
extern uint8_t no_of_open_client_sockets;
extern wifi_bool open_sockets[8];//Max open sockets allowed is 8. Each array element depicts one socket (true=open, false=closed)
extern wifi_bool Set_AT_Cmd_Response_False;
extern WiFi_AT_CMD_Response_t WiFi_Module_State;
extern volatile uint8_t wifi_client_connected;
extern wifi_bool switch_by_default_to_command_mode;
extern wifi_bool command_mode;
extern wifi_bool data_mode;
extern wifi_bool Scan_Ongoing;
extern wifi_bool WiFi_Configuration_Done;
extern WiFi_Config_HandleTypeDef WiFi_Config_Variables;
extern wifi_bool AT_Cmd_Ongoing;
extern GPIO_InitTypeDef  WAKEUP_InitStruct;
extern wifi_bool WiFi_Enabled;
extern uint8_t gpio_value, gpio_dir;
extern volatile uint8_t wifi_connected;
extern wifi_bool enable_sock_read;
extern uint32_t SockON_Data_Length;
extern uint8_t Socket_Open_ID, sockon_query_id;
extern wifi_bool enable_receive_data_chunk;
extern wifi_bool sock_read_ongoing;
extern wifi_bool prevent_push_OK_event;
extern wifi_bool stop_event_dequeue;
extern uint32_t WIND64_count;
extern wifi_bool prevent_push_WIFI_event;
extern wifi_bool do_not_reset_push_WIFI_event;
extern uint32_t process_buffer_index;
extern UART_HandleTypeDef UartMsgHandle;
extern uint32_t epoch_time;

/**
  * @}
  */
  
/** @defgroup NUCLEO_WIFI_INTERFACE_Private_Functions
  * @{
  */

#ifdef USART_PRINT_MSG

#define printf(arg...)    {sprintf((char*)print_msg_buff, arg);   \
HAL_UART_Transmit(&UartMsgHandle, (uint8_t*)print_msg_buff, strlen(print_msg_buff), 1000);}

#endif
/**
  * @brief  wifi_init
  *         User API for wifi init
  * @param  None
  * @retval None
  */
WiFi_Status_t wifi_init(wifi_config* config)
{
#ifndef WIFI_USE_VCOM
  uint8_t tx_level;
#endif

  WiFi_Status_t status = WiFi_MODULE_SUCCESS;

#if DEBUG_PRINT
  printf("\r\nInitializing SPWF01SA1 Interface..\r\n");
#endif  

  WiFi_Module_Init();

#ifndef WIFI_USE_VCOM  
  wifi_wakeup(WIFI_TRUE);//Prevent from going to sleep during configuration    

  /* Soft reset the module */
  wifi_reset();

  /* Set localecho1 to 0*/  
  status = SET_Configuration_Value(LOCALECHO1, 0);
  if(status != WiFi_MODULE_SUCCESS) return status;

  /* Restore default setting*/    
  Reset_AT_CMD_Buffer();  
  sprintf((char*)WiFi_AT_Cmd_Buff,AT_RESTORE_DEFAULT_SETTING);  
  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
  if(status == WiFi_MODULE_SUCCESS)
  {
    status = USART_Receive_AT_Resp(Process_Event);
    if(status != WiFi_MODULE_SUCCESS) return status;
  }

  /* Switch on HW Flow Control*/  
  status = SET_Configuration_Value(CONSOLE1_HWFC, 1);
  if(status != WiFi_MODULE_SUCCESS) return status; 
  
  if(config->wifi_baud_rate)
  {
    /* Set USART Speed*/  
    status = SET_Configuration_Value(CONSOLE1_SPEED, config->wifi_baud_rate);
    if(status != WiFi_MODULE_SUCCESS) return status; 
  }

  /* Set wifi_mode to idle*/  
  status = SET_Configuration_Value(WIFI_MODE, WiFi_IDLE_MODE);
  if(status != WiFi_MODULE_SUCCESS) return status;   

  switch(config->ht_mode)
  {
  case WIFI_FALSE:
    status = SET_Configuration_Value(WIFI_HT_MODE, 0);
    if(status != WiFi_MODULE_SUCCESS) return status;
    status = SET_Configuration_Addr(WIFI_OPR_RATE_MASK, "0x00003FCF");
    if(status != WiFi_MODULE_SUCCESS) return status;
    break;
  case WIFI_TRUE:
    status = SET_Configuration_Value(WIFI_HT_MODE, 1);
    if(status != WiFi_MODULE_SUCCESS) return status;
    status = SET_Configuration_Addr(WIFI_OPR_RATE_MASK, "0x003FFFCF");
    if(status != WiFi_MODULE_SUCCESS) return status;
    break;
  default:
    break;
  }

  switch(config->power)
  {
  case active:
    status = SET_Configuration_Value(WIFI_POWERSAVE, 0);
    if(status != WiFi_MODULE_SUCCESS) return status;
    status = SET_Configuration_Value(WIFI_SLEEP_ENABLED, 0);  
    if(status != WiFi_MODULE_SUCCESS) return status;
    break;
  case reactive:
    status = SET_Configuration_Value(WIFI_POWERSAVE, 1);
    if(status != WiFi_MODULE_SUCCESS) return status;
    status = SET_Configuration_Value(WIFI_OPERATIONAL_MODE, 11);  
    if(status != WiFi_MODULE_SUCCESS) return status;
    status = SET_Configuration_Value(WIFI_BEACON_WAKEUP, 0);
    if(status != WiFi_MODULE_SUCCESS) return status;
    status = SET_Configuration_Value(WIFI_LISTEN_INTERVAL, 0);
    if(status != WiFi_MODULE_SUCCESS) return status;
    status = SET_Configuration_Value(WIFI_SLEEP_ENABLED, 0);  
    if(status != WiFi_MODULE_SUCCESS) return status;
    break;
  case sleep:
    status = SET_Configuration_Value(WIFI_POWERSAVE, 1);
    if(status != WiFi_MODULE_SUCCESS) return status;
    status = SET_Configuration_Value(WIFI_OPERATIONAL_MODE, 12);  
    if(status != WiFi_MODULE_SUCCESS) return status;
    status = SET_Configuration_Value(WIFI_BEACON_WAKEUP, 10);
    if(status != WiFi_MODULE_SUCCESS) return status;
    status = SET_Configuration_Value(WIFI_LISTEN_INTERVAL, 1);
    if(status != WiFi_MODULE_SUCCESS) return status;
    status = SET_Configuration_Value(WIFI_SLEEP_ENABLED, 1);  
    if(status != WiFi_MODULE_SUCCESS) return status;
    break;
  default:
    break;
  }

  switch(config->power_level)
  {
  case low:
  case medium:
  case high:
  case max:
    tx_level=config->power_level*6;
    status = SET_Configuration_Value(WIFI_TX_POWER, tx_level);
    if(status != WiFi_MODULE_SUCCESS) return status;
    break;
  default:
    break;
  }

  switch(config->dhcp)
  {
  case off:
  case on:
  case custom:
    status = SET_Configuration_Value(IP_USE_DHCP_SERVER, config->dhcp);
    if(status != WiFi_MODULE_SUCCESS) return status;
    break;
  default:
    break;
  }

  /* Set IP address */
  if(config->ip_addr)
  {
    status = SET_Configuration_Addr(WIFI_IP_ADDRESS, config->ip_addr);
    if(status != WiFi_MODULE_SUCCESS) return status;
  }
  /* Set netmask address */
  if(config->netmask_addr)
  {
    status = SET_Configuration_Addr(WIFI_IP_NETMASK, config->netmask_addr);
    if(status != WiFi_MODULE_SUCCESS) return status;
  }
  /* Set default gateway address */
  if(config->gateway_addr)
  {
    status = SET_Configuration_Addr(WIFI_IP_DEFAULT_GATEWAY, config->gateway_addr);
    if(status != WiFi_MODULE_SUCCESS) return status;
  }
  /* Set dns address */
  if(config->dns_addr)
  {
    status = SET_Configuration_Addr(WIFI_IP_DNS, config->dns_addr);
    if(status != WiFi_MODULE_SUCCESS) return status;
  }
  /* Set hostname */
  if(config->host_name)
  {
    status = SET_Configuration_Addr(WIFI_IP_HOSTNAME, config->host_name);  
    if(status != WiFi_MODULE_SUCCESS) return status;
  }

  if(config->ap_domain_name)
  {
    status = SET_Configuration_Addr(WIFI_IP_APDOMAINNAME, config->ap_domain_name);
    if(status != WiFi_MODULE_SUCCESS) return status;
  }

  if(config->ap_config_page_name)
  {
    status = SET_Configuration_Addr(WIFI_IP_APREDIRECT, config->ap_config_page_name);
    if(status != WiFi_MODULE_SUCCESS) return status;
  }

  if(config->http_timeout)
  {
    status = SET_Configuration_Value(WIFI_IP_HTTP_TIMEOUT, config->http_timeout*1000);
    if(status != WiFi_MODULE_SUCCESS) return status;
  }
  if(config->dhcp_timeout)
  {
    status = SET_Configuration_Value(WIFI_IP_DHCP_TIMEOUT, config->dhcp_timeout);
    if(status != WiFi_MODULE_SUCCESS) return status;
  }   

#ifdef MODULE_VERSION_SPWF01Sx_1y
  Reset_AT_CMD_Buffer();  
  sprintf((char*)WiFi_AT_Cmd_Buff,AT_HTTPD, config->web_server);  
  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
  if(status == WiFi_MODULE_SUCCESS)
  {
    status = USART_Receive_AT_Resp(Process_Event);
    if(status != WiFi_MODULE_SUCCESS) return status;
  }  
#endif  
  
  /*AT+S.TLSCERT2=clean,all */
  Reset_AT_CMD_Buffer();  
  sprintf((char*)WiFi_AT_Cmd_Buff,"AT+S.TLSCERT2=clean,all\r");        
  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
  if(status == WiFi_MODULE_SUCCESS)
  {
    status = USART_Receive_AT_Resp(Process_Event);
    if(status != WiFi_MODULE_SUCCESS) return status;
  }  

  /* save current setting in flash */
  Reset_AT_CMD_Buffer();  
  sprintf((char*)WiFi_AT_Cmd_Buff,AT_SAVE_CURRENT_SETTING);  
  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
  if(status == WiFi_MODULE_SUCCESS)
  {
    status = USART_Receive_AT_Resp(Process_Event);
    if(status != WiFi_MODULE_SUCCESS) return status;
  }   

  if(config->wifi_baud_rate)
  {
    UART_Configuration(config->wifi_baud_rate); 
    Receive_Data();//Restart data reception
  }

  /* Soft reset the module, Do the second reset after setting all parameters and saving in flash */
  wifi_reset();

  wifi_wakeup(WIFI_FALSE);//De-assert wakeup signal (PC13) to allow sleep if enabled
#endif  //WIFI_USE_VCOM
  
#if DEBUG_PRINT
  printf("\r\nEnd of Initialization..\r\n");
#endif
  return status;
}


/**
* @brief  wifi_socket_client_security
*         Set the security certificates and key for secure socket (TLS)
* @param  None
* @retval WiFi_Status_t : return status
*/
WiFi_Status_t wifi_socket_client_security(uint8_t* tls_mode, uint8_t* root_ca_server, uint8_t* client_cert, uint8_t* client_key, uint8_t* client_domain, uint32_t tls_epoch_time) 
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;

  /*AT+S.TLSCERT2=clean,all */
  Reset_AT_CMD_Buffer();  
  sprintf((char*)WiFi_AT_Cmd_Buff,"AT+S.TLSCERT2=clean,all\r");        
  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
  if(status == WiFi_MODULE_SUCCESS)
  {
    status = USART_Receive_AT_Resp(Process_Event);
    if(status != WiFi_MODULE_SUCCESS) return status;
  }

  /* AT+S.SETTIME=<seconds> */  
  Reset_AT_CMD_Buffer();
  if(tls_epoch_time==0) 
    epoch_time = EPOCH_TIME;
  else
    epoch_time = tls_epoch_time;
  
  sprintf((char*)WiFi_AT_Cmd_Buff,"AT+S.SETTIME=%lu\r",(unsigned long)epoch_time);        
  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
  if(status == WiFi_MODULE_SUCCESS)
  {
    status = USART_Receive_AT_Resp(Process_Event);
    if(status != WiFi_MODULE_SUCCESS) return status;
  }

  /*AT+S.TLSCERT=f_ca,<size><CR><data>*/
  Reset_AT_CMD_Buffer();
  sprintf((char*)WiFi_AT_Cmd_Buff,"AT+S.TLSCERT=f_ca,%d\r%s",strlen((const char *)root_ca_server) - 1, root_ca_server);        

  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
  if(status == WiFi_MODULE_SUCCESS)
  {
    status = USART_Receive_AT_Resp(Process_Event);
    if(status != WiFi_MODULE_SUCCESS) return status;
  }

  /*AT+S.TLSCERT=f_cert,<size><CR><data>*/
  if(tls_mode[0]=='m')
  {
    Reset_AT_CMD_Buffer();
    sprintf((char*)WiFi_AT_Cmd_Buff,"AT+S.TLSCERT=f_cert,%d\r%s",strlen((const char *)client_cert) - 1, client_cert);

    status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
    if(status == WiFi_MODULE_SUCCESS)
        {
            status = USART_Receive_AT_Resp(Process_Event);
            if(status != WiFi_MODULE_SUCCESS) return status;
        }

    /*AT+S.TLSCERT=f_key,<size><CR><data>*/
    Reset_AT_CMD_Buffer();
    sprintf((char*)WiFi_AT_Cmd_Buff,"AT+S.TLSCERT=f_key,%d\r%s",strlen((const char *)client_key) - 1, client_key);        

    status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
    if(status == WiFi_MODULE_SUCCESS)
        {
            status = USART_Receive_AT_Resp(Process_Event);
            if(status != WiFi_MODULE_SUCCESS) return status;
        }
  }

  /*AT+S.TLSDOMAIN=f_domain,<server domain>*/
  Reset_AT_CMD_Buffer();
  sprintf((char*)WiFi_AT_Cmd_Buff,"AT+S.TLSDOMAIN=f_domain,%s\r", client_domain);        
  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
  if(status == WiFi_MODULE_SUCCESS)
  {
    status = USART_Receive_AT_Resp(Process_Event);
    if(status != WiFi_MODULE_SUCCESS) return status;
  }  
 
  return status; 
}

/**
* @brief  wifi_socket_client_open
*         Open a network socket
* @param  Hostname hostname to connect to
*         portnumber portnumber of the Host to connect to
*         protocol tcp or udp protocol
*         sock_id socket id of the opened socket returned to the user
* @retval WiFi_Status_t : return status of socket open request
*/
WiFi_Status_t wifi_socket_client_open(uint8_t * hostname, uint32_t port_number, uint8_t * protocol, uint8_t * sock_id) 
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;

  Queue_Client_Open_Event(hostname,port_number,protocol);
  status = USART_Receive_AT_Resp(Process_Event);

  *sock_id = Socket_Open_ID; //return the socket id to the user

  return status; 
}

/**
* @brief  Open_Serial_Port
*         Open a network socket
* @param  None
* @retval WiFi_Status_t : Wifi status
*/
WiFi_Status_t Open_Serial_Port()
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;
  /* AT+S.SOCKOS=2<cr> */
  Reset_AT_CMD_Buffer();
  //sprintf((char*)WiFi_AT_Cmd_Buff,"\rAT+S.SOCKOS=%d\r",SerialPortNo);        
  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));

  if(status==WiFi_MODULE_SUCCESS)
  {
   status = USART_Receive_AT_Resp(Process_Event); 
  }
  return status;
}

/**
* @brief  wifi_socket_client_write
*         Write len bytes of data to socket
* @param  sock_id socket ID of the socket to write to
*         DataLength: data length to send
*         pData : pointer of data buffer to be written
* @retval WiFi_Status_t : return status of socket write request
*/
WiFi_Status_t wifi_socket_client_write(uint8_t sock_id, uint16_t DataLength, char * pData)
{
  /* AT+S.SOCKW=00,11<cr> */
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;

  //Check if sock_id is open
  if(!open_sockets[sock_id])
    return WiFi_NOT_READY;

  if(DataLength>=1021 || DataLength<=0)
    return WiFi_NOT_SUPPORTED;

  Queue_Client_Write_Event(sock_id,DataLength,pData);
  status = USART_Receive_AT_Resp(Process_Event);
  return status;
}

/**
* @brief  Socket_Read
*         Return len bytes of data from socket
* @param  DataLength: data length to read
* @retval WiFi_Status_t : return status of socket read request
*/
WiFi_Status_t Socket_Read(uint16_t DataLength)
{
  /* AT+S.SOCKR=01,12<cr> */
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;
  wait_for_command_mode();

  /* AT+S.SOCKON=myserver,1234,t<cr> */
  sprintf((char*)WiFi_AT_Cmd_Buff,AT_SOCKET_READ,sockon_query_id,DataLength);
  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
  if(status == WiFi_MODULE_SUCCESS)
  {
    if(stop_event_dequeue == WIFI_FALSE)
        stop_event_dequeue = WIFI_TRUE;
    SockON_Data_Length = DataLength;
    enable_sock_read = WIFI_TRUE;
    process_buffer_index =5;
    enable_receive_data_chunk = WIFI_TRUE;
    WiFi_Module_State = Process_Event;
    Set_AT_Cmd_Response_False = WIFI_TRUE;
    memset(UserDataBuff, 0x00, MAX_BUFFER_GLOBAL);
  }
  return status;
}

/**
* @brief  wifi_socket_client_close
*         The SOCKC command allows to close socket
* @param  sock_close_id the socket ID of the socket which needs to be closed.
* @retval WiFi_Status_t : return status of socket close request
*/
WiFi_Status_t wifi_socket_client_close(uint8_t sock_close_id)
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;
  
  if(open_sockets[sock_close_id])
  {
    Queue_Client_Close_Event(sock_close_id);
    return status; 
  }
  else
    return WiFi_MODULE_ERROR;
  
}

/**
* @brief  Socket_Pending_Data
*         Query pending data.It will returns the number of bytes of data waiting on socket
* @param None
* @retval uint8_t :number of bytes of data waiting on socket
*/
void Socket_Pending_Data()
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;
 
  /* AT+S.SOCKQ=01<cr> */
  Reset_AT_CMD_Buffer();

  wait_for_command_mode();

  if(open_sockets[sockon_query_id])
  {
      if(stop_event_dequeue == WIFI_FALSE)
        stop_event_dequeue = WIFI_TRUE;
      
      sprintf((char*)WiFi_AT_Cmd_Buff,AT_QUERY_PENDING_DATA,sockon_query_id);        
      status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
      if(status==WiFi_MODULE_SUCCESS)
      {
        /* EQ. Set state to Process_Event */
        WiFi_Module_State = Process_Event;
        Set_AT_Cmd_Response_False = WIFI_TRUE;
      }  
  }
}

/**
* @brief  wifi_socket_server_open
*         Open a Server socket
* @param  None
* @retval WiFi_Status_t : return status of server socket request
*/
WiFi_Status_t wifi_socket_server_open(uint32_t port_number, uint8_t * protocol) 
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;
  Reset_AT_CMD_Buffer();

  /* AT+S.SOCKD=portNo,t<cr> */  
  sprintf((char*)WiFi_AT_Cmd_Buff,AT_SERVER_SOCKET_OPEN,(int)port_number,protocol);        
  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
  if(status == WiFi_MODULE_SUCCESS)
  {
    status = USART_Receive_AT_Resp(Process_Event);
  }
  return status; 
}

/**
* @brief  wifi_socket_server_write
*         Write to a Server socket
* @param  None
* @retval WiFi_Status_t : return status of server socket request
*/
WiFi_Status_t wifi_socket_server_write(uint16_t DataLength,char * pData) 
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;
  /*Can only write if there is a client connected*/
  if(!wifi_client_connected)
  {
    return WiFi_NOT_READY;
  }
     __disable_irq();

  do_not_reset_push_WIFI_event = WIFI_TRUE;
  prevent_push_WIFI_event = WIFI_TRUE;
  __enable_irq();

  while(sock_read_ongoing || WIND64_count!= 0);//wait till any pending data is read

  wait_for_command_mode();

  /*to make sure that by default the mode is not switched to command mode from data mode*/
  switch_by_default_to_command_mode = WIFI_FALSE;

  /*Switch to Data Mode first*/
  if(!data_mode)
  {
    WiFi_switch_to_data_mode();//switch by default
    while(!data_mode)
    {
      //Wait till data_mode is active
    }
  }  

  /*Write the data on the uart*/
  if(HAL_UART_Transmit(&UartWiFiHandle, (uint8_t *)pData, DataLength,1000)!= HAL_OK)
  {
    Error_Handler();
    return WiFi_HAL_UART_ERROR;
  }
  //HAL_Delay(100);//Wait for tx before switching back to command mode

  /*Switch back to Command Mode*/
  if(!command_mode)
  {
    WiFi_switch_to_command_mode();//switch by default
    while(!command_mode)
    {
      //Wait till command_mode is active
    }
  }

  switch_by_default_to_command_mode = WIFI_TRUE;  /*back to default behaviour*/
  
  __disable_irq();
  prevent_push_WIFI_event = WIFI_FALSE;
  do_not_reset_push_WIFI_event = WIFI_FALSE;
  __enable_irq();
  
  return status;
}

/**
* @brief  Server Socket Close
*         Close a Server socket
* @param  None
* @retval WiFi_Status_t : return status of server socket request
*/
WiFi_Status_t wifi_socket_server_close() 
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;
  Reset_AT_CMD_Buffer();

  /* AT+S.SOCKD=portNo,t<cr> */  
  sprintf((char*)WiFi_AT_Cmd_Buff,AT_SERVER_SOCKET_CLOSE);        
  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
  if(status == WiFi_MODULE_SUCCESS)
  {
    status = USART_Receive_AT_Resp(Process_Event);
  }
  
  return status; 
}


/**
* @brief  wait_for_command_mode
*         Waits till we are in command mode
* @param  None
* @retval None
*/
void wait_for_command_mode(void)
{
    while(!command_mode)
        {
          //Make sure we are in command mode, ideally we should do this in every User API?
        }
}


/**
* @brief  wifi_file_delete
*         Delete a file
* @param  pFileName : File Name to be deleted
* @retval WiFi_Status_t : return status of delete file request
*/
WiFi_Status_t wifi_file_delete(char * pFileName)
{
  /* AT+S.FSD: delete an existing file */  
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;  
  Reset_AT_CMD_Buffer(); 
  /* AT+S.FSL */
  sprintf((char*)WiFi_AT_Cmd_Buff,AT_DELETE_FILE,pFileName);  
  
  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
  if(status == WiFi_MODULE_SUCCESS)
  {
    status = USART_Receive_AT_Resp(Process_Event);
  }
  return status;
}

/**
* @brief  wifi_file_list
*         List existing filename
* @param  None
* @retval WiFi_Status_t : return status of AT cmd request
*/

WiFi_Status_t wifi_file_list()
{
    WiFi_Status_t status = WiFi_MODULE_SUCCESS;  
    Queue_Wifi_File_Event(NULL,NULL,0);
    status = USART_Receive_AT_Resp(Process_Event);
    return status;
}

/**
* @brief  wifi_file_show
*         Print the contents of an existing file
* @param  pFileName : pinter of file name
* @retval WiFi_Status_t : return status of AT cmd request
*/

WiFi_Status_t wifi_file_show(uint8_t * pFileName)
{
    if(pFileName==NULL)
        return WiFi_MODULE_ERROR;

    WiFi_Status_t status = WiFi_MODULE_SUCCESS;  
    Queue_Wifi_File_Event(NULL,pFileName,0);
    status = USART_Receive_AT_Resp(Process_Event);
    return status; 
}

/**
* @brief  wifi_file_create
*         Create file for HTTP server
* @param  pFileName : pointer of file name to be created
*         alength   : length of file
* @retval WiFi_Status_t : return status of AT cmd request
*/

WiFi_Status_t wifi_file_create(char *pFileName, uint16_t alength, char * pUserFileBuff)
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;

  if(alength >1024)
    return WiFi_AT_FILE_LENGTH_ERROR;

  Reset_AT_CMD_Buffer();

  /* AT+S.FSC=/index.html  */
  sprintf((char*)WiFi_AT_Cmd_Buff, AT_CREATE_NEW_HTML_FILE, pFileName, alength);

  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
  if(status == WiFi_MODULE_SUCCESS)
  {
    status = USART_Receive_AT_Resp(Process_Event);
    int len = strlen(pUserFileBuff);

    if(len >= 1024)
       return WiFi_AT_FILE_LENGTH_ERROR;

    /* AT+S.FSA=/index.html  */
    sprintf((char*)WiFi_AT_Cmd_Buff,AT_APPEND_FILE,pFileName,len);

    status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
    if(status == WiFi_MODULE_SUCCESS)
    {
      memset(WiFi_AT_Cmd_Buff, 0x00, sizeof WiFi_AT_Cmd_Buff);
      memcpy((char*)(char*)WiFi_AT_Cmd_Buff, (char*) pUserFileBuff,len);
      WiFi_AT_Cmd_Buff[len+1]='\r';
      status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
      if(status == WiFi_MODULE_SUCCESS)
      {
        status = USART_Receive_AT_Resp(Process_Event);
      }
    }
  }
  return status; 
}

/**
* @brief  wifi_http_get
*         Issue an HTTP GET of the given path to the specified host
* @param  None
* @retval WiFi_Status_t : return status of AT cmd response
*/

WiFi_Status_t wifi_http_get(uint8_t * hostname, uint8_t * path, uint32_t port_number)
{
  if(hostname == NULL || path == NULL)
    return WiFi_MODULE_ERROR;
  
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;

  while(sock_read_ongoing || WIND64_count!= 0); //wait till any pending data is read

  // AT+S.HTTPGET=host.example.com,/index.html, port_number<cr>
  //Queue the http-get command
  Queue_Http_Event(hostname, path, port_number,NULL);

  //Make the user wait anyway
  status = USART_Receive_AT_Resp(Process_Event);

  return status; 

}

/**
* @brief  wifi_http_post
*         Issue an HTTP GET of the given path to the specified host
* @param  None
* @retval WiFi_Status_t : status of Http Post Request
*/

WiFi_Status_t wifi_http_post(uint8_t * pURL_path)
{
  if(pURL_path == NULL)
    return WiFi_MODULE_ERROR;
  
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;
  
  while(sock_read_ongoing || WIND64_count!= 0);//wait till any pending data is read
  
  // AT+S.HTTPPOST = posttestserver.com,/post.php,name=demo&email=mymail&subject=subj&body=message<cr>
  Queue_Http_Event(NULL,NULL,0,pURL_path);
  
  //Make the user wait anyway
  status = USART_Receive_AT_Resp(Process_Event);
  return status;
}

/**
* @brief  wifi_file_image_create
*         Downloads an updated file system via a single HTTP GET request to the
*         named host and path.
* @param  None
* @retval WiFi_Status_t
*/
WiFi_Status_t wifi_file_image_create(uint8_t * pHostName, uint8_t * pFileName, uint32_t port_number)
{
  if(pHostName == NULL || pFileName == NULL ||  port_number ==0)
    return WiFi_MODULE_ERROR;
  
    WiFi_Status_t status = WiFi_MODULE_SUCCESS;
    Queue_Wifi_File_Event(pHostName,pFileName,port_number);
    status = USART_Receive_AT_Resp(Process_Event);
    
    /* Soft reset the module */
    SET_Power_State(PowerSave_State);
    return status;
}

/**
* @brief  wifi_file_erase_external_flash
*         This API allows to erase the content of the external flash
* @param  None
* @retval WiFi_Status_t
*/
WiFi_Status_t wifi_file_erase_external_flash()
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;  

  Reset_AT_CMD_Buffer();
  ResetBuffer();

  /* AT+S.HTTPDFSERASE */
  sprintf((char*)WiFi_AT_Cmd_Buff,AT_ERASE_FLASH_MEMORY);  
  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));

  /* Soft reset the module */
  SET_Power_State(PowerSave_State);

   return status;
}

/**
* @brief  wifi_fw_update
*         Issue an HTTP GET of the given path to the specified host and get the firmware updated
* @param  None
* @retval None
*/
WiFi_Status_t wifi_fw_update(uint8_t * hostname, uint8_t * filename_path, uint32_t port_number)
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;
  Queue_Wifi_FW_Update_Event(hostname,filename_path,port_number);
  status = USART_Receive_AT_Resp(Process_Event);
  /* Soft reset the module */
  SET_Power_State(PowerSave_State);
  return status;
}

/**
* @brief  wifi_network_scan
*         Performs an immediate scan for available network
* @param  None
* @retval WiFi_Status_t : WiFi status error
*/
WiFi_Status_t wifi_network_scan(wifi_scan *scan_result, uint16_t max_scan_number)
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;
  wifi_scanned_list = scan_result;
  if(max_scan_number>MAX_WIFI_SCAN_NETWORK)
    return WiFi_NOT_SUPPORTED;
  user_scan_number = max_scan_number;
  
  if(Scan_Ongoing)
  {
    return WiFi_AT_CMD_BUSY;
  }
  
  Scan_Ongoing = WIFI_TRUE;
  
#if 0   //TBD
  if(WiFi_Param.WiFi_Module_State == WiFi_MiniAP_MODE)
    return WiFi_NOT_SUPPORTED;
#endif  
  
  /* AT+S.SCAN: performs an immediate scan for available networks */
  Reset_AT_CMD_Buffer();
  sprintf((char*)WiFi_AT_Cmd_Buff,AT_WiFi_SCAN);  
  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
  if(status == WiFi_MODULE_SUCCESS)
  {
    status = USART_Receive_AT_Resp(Process_Event);
  }
  
  /*At this point we have WiFi_Scan_Buffer filled with RSSI and SSID values*/
  return status;
}

/**
* @brief  Set_MiniAP_Mode
*         Configure Wi-Fi module in AP mode.
          MiniAP is always configured in open mode (WEP not supported)
* @param  None
* @retval WiFi_Status_t : status of AT cmd 
*/
WiFi_Status_t wifi_ap_start(uint8_t * ssid, uint8_t channel_num)
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;  
  
  /* Set the SSID : AT+S.SSIDTXT=<SSID>*/    
  if(ssid)
      status = SET_SSID((char*)ssid);
  else 
    /* default SSID : AT+S.SSIDTXT=SPWF_AP*/
    status = SET_SSID(WiFi_Config_Variables.wifi_ssid);
  
  if(status != WiFi_MODULE_SUCCESS)
    return WiFi_SSID_ERROR;

  /* Set the network privacy mode : AT+S.SCFG=wifi_priv_mode,0*/ 
   status = SET_Configuration_Value(WIFI_PRIV_MODE, None);
  if(status != WiFi_MODULE_SUCCESS)
    return WiFi_CONFIG_ERROR;
  
  /* Set the network mode (1 = STA, 2 = IBSS, 3 = MiniAP) :: AT+S.SCFG=wifi_mode,3*/  
   status = SET_Configuration_Value(WIFI_MODE, WiFi_MiniAP_MODE);
   if(status != WiFi_MODULE_SUCCESS)
    return WiFi_CONFIG_ERROR;
   
  /* Set the channel number */  
   status = SET_Configuration_Value(WIFI_CHANNEL_NUMBER, channel_num);
   if(status != WiFi_MODULE_SUCCESS)
    return WiFi_CONFIG_ERROR; 
 
  /* Save the settings on the flash memory : AT&W*/ 
  Save_Current_Setting();
  
  WiFi_Configuration_Done = WIFI_TRUE;
  WiFi_Module_State = Process_Event;

  /* Soft reset the module */
  SET_Power_State(PowerSave_State);
  
  return status; 
}

   
/**
* @brief  SET_WiFi_STA_Mode
*         Configure Wi-Fi module in STA mode
* @param  SSID     : SSID name
* @param  sec_key  : security key
* @param  priv_mode : network privecy mode
* @retval WiFi_Status_t : status of AT cmd 
*/
WiFi_Status_t wifi_connect(char * ssid, char * sec_key, WiFi_Priv_Mode priv_mode)
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;
   if(AT_Cmd_Ongoing == WIFI_FALSE)
    AT_Cmd_Ongoing = WIFI_TRUE;
  else 
  {
    return WiFi_AT_CMD_BUSY;
  }
  
  if(sec_key) {
    status = SET_WiFi_SecKey((char*)sec_key);
    if(status != WiFi_MODULE_SUCCESS)
        return WiFi_SecKey_ERROR;
  }
 
  /* Set the SSID : AT+S.SSIDTXT=<SSID>*/    
  if(ssid)
      status = SET_SSID((char*)ssid);
  else 
    /* default SSID : AT+S.SSIDTXT=SPWF_STA*/
    status = SET_SSID(WiFi_Config_Variables.wifi_ssid);
  
  if(status != WiFi_MODULE_SUCCESS)
    return WiFi_SSID_ERROR;

  /* Set the network privacy mode : AT+S.SCFG=wifi_priv_mode,2*/ 
   status = SET_Configuration_Value(WIFI_PRIV_MODE, priv_mode);
  if(status != WiFi_MODULE_SUCCESS)
    return WiFi_CONFIG_ERROR;
  
  /* Set the network mode (1 = STA, 2 = IBSS, 3 = MiniAP) :: AT+S.SCFG=wifi_mode,1*/  
   status = SET_Configuration_Value(WIFI_MODE, WiFi_STA_MODE);
   if(status != WiFi_MODULE_SUCCESS)
    return WiFi_CONFIG_ERROR;

  /* Save the settings on the flash memory : AT&W*/ 
  Save_Current_Setting();

  WiFi_Configuration_Done = WIFI_TRUE;
  //WiFi_Module_State = WiFi_Process_Event;

  /* Soft reset the module */
  SET_Power_State(PowerSave_State);
  
  AT_Cmd_Ongoing = WIFI_FALSE;

  return status; 
}

/**
* @brief  SET_WiFi_IBSS_Mode
*         Configure Wi-Fi module in IBSS mode
* @param  SSID     : SSID name
* @param  priv_mode : network privecy mode
* @retval WiFi_Status_t : status of AT cmd
*/
WiFi_Status_t wifi_adhoc_create(uint8_t * ssid, WiFi_Priv_Mode priv_mode)
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;
  
  Attention_Cmd();
  /* Set the SSID : AT+S.SSIDTXT=<SSID>*/    
  status = SET_SSID((char*)ssid);  
  if(status != WiFi_MODULE_SUCCESS)
    return WiFi_SSID_ERROR;
  
  /* Set the network privacy mode : AT+S.SCFG=wifi_priv_mode,0*/ 
    status = SET_Configuration_Value(WIFI_PRIV_MODE, priv_mode);
    if(status != WiFi_MODULE_SUCCESS)
      return WiFi_CONFIG_ERROR;
  
  
  /* Set the network mode (1 = STA, 2 = IBSS, 3 = MiniAP) :: AT+S.SCFG=wifi_mode,2*/  
    status = SET_Configuration_Value(WIFI_MODE, WiFi_IBSS_MODE);
    if(status != WiFi_MODULE_SUCCESS)
      return WiFi_CONFIG_ERROR;
  
  /* Set IP address */
    status = SET_Configuration_Addr(WIFI_IP_ADDRESS, WIFI_IBSS_IP_ADDR);
    if(status != WiFi_MODULE_SUCCESS)
      return WiFi_CONFIG_ERROR;
  
  /* Set IP default gateway */
    status = SET_Configuration_Addr(WIFI_IP_DEFAULT_GATEWAY, WIFI_IBSS_DEFAULT_GATEWAY);
    if(status != WiFi_MODULE_SUCCESS)
      return WiFi_CONFIG_ERROR;
  
  
  /* Set IP DNS */
    status = SET_Configuration_Addr(WIFI_IP_DNS, WIFI_IBSS_IP_DNS_ADDR);
    if(status != WiFi_MODULE_SUCCESS)
      return WiFi_CONFIG_ERROR;
  
  /* Set IP netmask */
    status = SET_Configuration_Addr(WIFI_IP_NETMASK, WIFI_IBSS_IP_MASK);
    if(status != WiFi_MODULE_SUCCESS)
      return WiFi_CONFIG_ERROR;
  
  /* Turn OFF the DHCP */
    SET_Configuration_Value(IP_USE_DHCP_SERVER, WIFI_IP_USE_DHCP);   
    if(status != WiFi_MODULE_SUCCESS)
      return WiFi_CONFIG_ERROR;
  
  /* Save the settings on the flash memory : AT&W*/ 
  Save_Current_Setting();
    
  /* Soft reset the module */
  SET_Power_State(PowerSave_State); 
  return status;
}


/**
* @brief  wifi_standby
*         Configured WiFi module to enter standby
* @param  arg_standby_time: standby time
* @retval WiFi_Status_t : status of AT cmd
*/
WiFi_Status_t wifi_standby(uint8_t arg_standby_time)
{
  /*
  For Standby, the presence of Jumpers on JP4 and JP3 has the following behaviour:
  JP3 (middle and bottom): prevents standby and immediately wakes-up module
  JP3 (middle and top): no effect on standby
  JP4 (middle and right): prevents wakeup and standby runs forever
  JP4 (middle and left): no effect on standby
  */
  
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;
  
  /*if(arg_standby_time<2)
    return WiFi_NOT_SUPPORTED;*/
      
  SET_Configuration_Value(WIFI_SLEEP_ENABLED, 0);  
  SET_Configuration_Value(WIFI_STANDBY_ENABLED, 1);  
  status = SET_Configuration_Value(WIFI_STANDBY_TIME, arg_standby_time);  
  
  /* save current setting in flash */
  Save_Current_Setting();
  
  /* AT : send AT command */
  sprintf((char*)WiFi_AT_Cmd_Buff,AT_SET_POWER_STATE,1);  //cfun=4
  WiFi_Module_State = Process_Event; //make sure the WiFi module is in this state to receive WINDS after wakeup
  status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
  
  if(status == WiFi_MODULE_SUCCESS)//if transmit is success, prepare for resume
  {
    /*
    AT_Cmd_Processing = WIFI_TRUE;//We do not want the next UART data_byte fetch to be done
    HAL_NVIC_DisableIRQ(USARTx_IRQn);//Disable UART IRQ  
    Standby_Timer_Running=WIFI_TRUE;  
    printf("\r\nGoing into Standby Mode...\r\n");*/
  }
  
  return status;
  
}

/**
* @brief  wifi_wakeup
*         wakeup the module from sleep by setting the GPIO6 through PC13
*         or allow it to go to sleep
*         Jumper needed on JP4
* @param  wakeup wakeup (WIFI_TRUE) or allow sleep(WIFI_FALSE)
* @retval WiFi_Status_t : status of function call
*/
WiFi_Status_t wifi_wakeup(wifi_bool wakeup)
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;    
  
  RESET_WAKEUP_GPIO_CLK_ENABLE();
  
  WAKEUP_InitStruct.Pin       = WiFi_WAKEUP_GPIO_PIN;
  WAKEUP_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
  WAKEUP_InitStruct.Pull      = GPIO_NOPULL;
  WAKEUP_InitStruct.Speed     = GPIO_SPEED_HIGH;

  HAL_GPIO_Init(WiFi_WAKEUP_GPIO_PORT, &WAKEUP_InitStruct);
  
  if(wakeup)
    HAL_GPIO_WritePin(WiFi_WAKEUP_GPIO_PORT, WiFi_WAKEUP_GPIO_PIN, GPIO_PIN_SET);
  else
    HAL_GPIO_WritePin(WiFi_WAKEUP_GPIO_PORT, WiFi_WAKEUP_GPIO_PIN, GPIO_PIN_RESET);
  
  return status;
}


/**
* @brief  wifi_disconnect
*         disconnect the module from any AP
* @param  None
* @retval WiFi_Status_t : status of AT cmd
*/
WiFi_Status_t wifi_disconnect(void)
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;
  
  /* Set wifi_mode to idle*/
  status = SET_Configuration_Value(WIFI_MODE, WiFi_IDLE_MODE);
  
  /*If module was connected, reset the status*/
  if(wifi_connected == 1)
      {
        wifi_connected = 0;//this will allow the TIM handler to make the callback on connection(WiFiUp)
      }  
  
  /* save current setting in flash */
  Save_Current_Setting();
  
  /* Soft reset the module */
  status = SET_Power_State(PowerSave_State);//CFUN=1
       
  return status;
}

/**
* @brief  wifi_enable
*         Enable/Disable the Wi-Fi interface
* @param  enable enable Wi-Fi (WIFI_TRUE) disable Wi-Fi (WIFI_FALSE)
* @retval WiFi_Status_t : status of AT cmd
*/
WiFi_Status_t wifi_enable(wifi_bool enable)
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;
  
  /* Enable or Disable wifi interface*/  
  Reset_AT_CMD_Buffer();  
  sprintf((char*)WiFi_AT_Cmd_Buff,AT_WIFI_ENABLE, enable);  
  
  if(HAL_UART_Transmit(&UartWiFiHandle, (uint8_t *)WiFi_AT_Cmd_Buff, strlen((char*)WiFi_AT_Cmd_Buff),1000)!= HAL_OK)
  {
    Error_Handler();
    #if DEBUG_PRINT
    printf("HAL_UART_Transmit Error");
    #endif
    return WiFi_HAL_UART_ERROR;
  }  
  
  status = USART_Receive_AT_Resp(Process_Event);  
  
  //wait for power down/hw started
  if(enable)
  while(WiFi_Enabled != WIFI_TRUE) {}
  else
    while(WiFi_Enabled != WIFI_FALSE) {}
  
  return status;
        
}

/**
* @brief  wifi_mac_get
*         Obtaining Module MAC address
* @param  Pointer to 6 bytes char array
* @retval WiFi_Status_t : status of AT cmd
*/
WiFi_Status_t wifi_mac_get(uint32_t * mac)
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;
   
  /* Check the MAC Address AT+S.GCFG=nv_wifi_macaddr*/
 
	if (mac)
		status = GET_Configuration_Value(WIFI_MAC_ADDRESS, mac);
  
  if(status != WiFi_MODULE_SUCCESS)
    return WiFi_AT_CMD_RESP_ERROR;

  return status; 
}


/**
* @brief  wifi_restore
*         Restore the Wi-Fi with default values.
* @param  None
* @retval WiFi_Status_t : status of AT cmd
*/
WiFi_Status_t wifi_restore()
{
  WiFi_Status_t status = WiFi_MODULE_SUCCESS;
  
  /* Restore default setting*/
  Restore_Default_Setting();  
  
  /* Set wifi_mode to idle*/
  SET_Configuration_Value(WIFI_MODE, WiFi_IDLE_MODE);
  
  /* set the local echo */
  SET_Configuration_Value(LOCALECHO1, 0);
  
  /* save current setting in flash */
  Save_Current_Setting();
  
  /* Soft reset the module */
  status = SET_Power_State(PowerSave_State);//CFUN=1
       
  return status;
}

/*GPIO Configuration Functions*/

/**
* @brief  wifi_gpio_init
*         Configure a GPIO pin as in or out with IRQ setting
* @param  pin GPIO pin number
* @param  irq configuration of the pin
* @retval WiFi_Status_t : status of AT cmd
*/
uint8_t wifi_gpio_init(GpioPin pin, char* dir, char irq)
{
    WiFi_Status_t status = WiFi_MODULE_SUCCESS;
    
    /* AT+S.GPIOC=pin,dir,irq */
    Reset_AT_CMD_Buffer();
    
    memset(WiFi_AT_Cmd_Buff, 0x00, sizeof WiFi_AT_Cmd_Buff);
    
    if(irq!=GPIO_Off)
      sprintf((char*)WiFi_AT_Cmd_Buff,"AT+S.GPIOC=%d,%s,%c\r", pin, dir, irq);
    else
      sprintf((char*)WiFi_AT_Cmd_Buff,"AT+S.GPIOC=%d,%s\r", pin, dir);
    
    status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
    if(status == WiFi_MODULE_SUCCESS)
      {
        status = USART_Receive_AT_Resp(Process_Event);
      }
    
    
    return status; 
   
}

/**
* @brief  wifi_gpio_read
*         Read the configuration of a GPIO pin
* @param  pin GPIO pin number
* @param  val value returned
* @param  dir configuration direction returned
* @retval WiFi_Status_t : status of AT cmd
*/
uint8_t wifi_gpio_read(GpioPin pin, uint8_t *val, uint8_t *dir)
{
    WiFi_Status_t status = WiFi_MODULE_SUCCESS;    

    /* AT+S.GPIOR=pin */
    Reset_AT_CMD_Buffer();  

    sprintf((char*)WiFi_AT_Cmd_Buff,"AT+S.GPIOR=%d\r", pin);

    status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
    if(status == WiFi_MODULE_SUCCESS)
      {
        status = USART_Receive_AT_Resp(Process_Event);
      }
    *val = gpio_value;
    *dir = gpio_dir;

    return status;
}

/**
* @brief  wifi_gpio_write
*         Read the value of a GPIO pin
* @param  pin GPIO pin number
* @param  val value to be configured
* @retval WiFi_Status_t : status of AT cmd
*/
uint8_t wifi_gpio_write(GpioPin pin, GpioWriteValue value)
{
    WiFi_Status_t status = WiFi_MODULE_SUCCESS;

    /* AT+S.GPIOW=pin,value */
    Reset_AT_CMD_Buffer(); 

    sprintf((char*)WiFi_AT_Cmd_Buff,"AT+S.GPIOW=%d,%d\r", pin, value);

    status = USART_Transmit_AT_Cmd(strlen((char*)WiFi_AT_Cmd_Buff));
    if(status == WiFi_MODULE_SUCCESS)
      {
        status = USART_Receive_AT_Resp(Process_Event);
      }
    
    return status;
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

