/**
******************************************************************************
* @file    wifi_main.c
* @author  Central LAB
* @version V0.1.0
* @date    10-March-2016
* @brief   Wifi Main program body
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
#include "main.h"
//#include "stdio.h"

/** @defgroup NUCLEO_WIFI_MODULE
* @{ Wi-Fi driver modules
*/


char ssid[256];
char seckey[256];

#define DEFAULT_SSID 	"test_home"
#define DEFAULT_SECKEY 	"1234567890"
#define DEFAULT_MODE 	WPA_Personal

/**
* Private typedef -----------------------------------------------------------
*/

typedef enum {
	wifi_state_reset = 0,
	wifi_state_ready,
	wifi_state_idle,
	wifi_state_connected,
	wifi_state_connecting,
	wifi_state_disconnected,
	wifi_state_time_socket,
	wifi_open_tls,
	wifi_send_json,
	wifi_state_socket,
	wifi_state_error,
	wifi_undefine_state       = 0xFF,  
} wifi_state_t;

/**
* Private define ------------------------------------------------------------
*/
#define APPLICATION_DEBUG_MSG			0
#define APPLICATION_HARDCODE_SSID		1


#define MAX_SCAN_NETWORK 				15

/**
* Private macro -------------------------------------------------------------
*/

/**
* Private function prototypes -----------------------------------------------
*/
uint8_t time_get_time_from_time_server();

void (*p_ind_wifi_socket_data_received)(uint8_t * data_ptr, uint32_t message_size, uint32_t chunck_size);
void (*p_ind_wifi_socket_client_remote_server_closed)(uint8_t * socket_closed_id);

void time_ind_wifi_socket_data_received(uint8_t * data_ptr, uint32_t message_size, uint32_t chunck_size);
void time_ind_wifi_socket_client_remote_server_closed(uint8_t * socket_closed_id);

void wifi_print_status(WiFi_Status_t status);
/**
* Global Variables ---------------------------------------------------------
*/
static uint8_t TimeSocket;
static uint8_t TimeSocketClosed = 0;
int32_t last_ticks=0;
wifi_state_t wifi_state;
wifi_config config;
volatile uint32_t data_arrived_on_socket = 0;


/**
* Private functions ---------------------------------------------------------
*/


int findWifiNetwork(wifi_scan *scan_result, uint16_t max_scan_number) {
	uint16_t countNetwork = 0;

	printf("\r\rScanning wifi networks... ");
	if (wifi_network_scan(scan_result, max_scan_number) == WiFi_MODULE_SUCCESS){
		printf("Complete\r\n");
	} else {
		printf("Error!\r\n");
	}

	for (int i = 0; i < max_scan_number; ++i) {
		if (scan_result[i].ssid[0] == '\0')	continue;
		countNetwork++;
		printf("%d. AP Name: %s Channel: %d Security: wpa - %d wpa2 - %d wps - %d\r\n",
				i, scan_result[i].ssid, scan_result[i].channel_num,
				scan_result[i].sec_type.wpa, scan_result[i].sec_type.wpa2, scan_result[i].sec_type.wps);
	}
	return countNetwork;
}

char get_value[256];

/**
* @brief  Wifi Main program
* @param  None
* @retval WiFi_Status_t value
*/
int wifi_main(void)
{    
	WiFi_Status_t status = WiFi_MODULE_SUCCESS;   
	uint32_t cnt=0;
	WiFi_Priv_Mode mode;


	/* configure the timers  */
	Timer_Config( );

	printf("\n\rWiFi is initializing....\n\r");

	p_ind_wifi_socket_data_received = time_ind_wifi_socket_data_received;
	p_ind_wifi_socket_client_remote_server_closed = time_ind_wifi_socket_client_remote_server_closed;

	memset(&config,0,sizeof(config));
	config.power=active;
	config.power_level=max;//high;//max?
	config.dhcp=on;
	config.ht_mode=WIFI_FALSE;
	config.web_server=WIFI_TRUE;

	wifi_state = wifi_state_idle;

	/* Init the wi-fi module */
	status = wifi_init(&config);

    #if APPLICATION_DEBUG_MSG

		GET_Status_Value(WIFI_SW_VERSION, get_value);
		printf("%s = %s\n", WIFI_SW_VERSION, get_value);

		GET_Status_Value(WIFI_HW_VERSION, get_value);
		printf("%s = %s\n", WIFI_HW_VERSION, get_value);


	    GET_Configuration_Value(WIFI_IP_HTTP_TIMEOUT, get_value);
	    printf("%s = %s\n", WIFI_IP_HTTP_TIMEOUT, get_value);

	    GET_Configuration_Value(WIFI_IP_DHCP_TIMEOUT, get_value);
	    printf("%s = %s\n", WIFI_IP_DHCP_TIMEOUT, get_value);

	    GET_Configuration_Value(WIFI_IP_HOSTNAME, get_value);
	    printf("%s = %s\n", WIFI_IP_HOSTNAME, get_value);

		GET_Configuration_Value(WIFI_BAS_RATE_MASK, get_value);
		printf("%s = %s\n", WIFI_BAS_RATE_MASK, get_value);

	    GET_Configuration_Value(WIFI_OPR_RATE_MASK, get_value);
	    printf("%s = %s\n", WIFI_OPR_RATE_MASK, get_value);
	#endif
	
	if(status!=WiFi_MODULE_SUCCESS)
	{
		printf("Error in WIFI  Initialization\r\n");
		#if APPLICATION_DEBUG_MSG
			wifi_print_status(status);
		#endif
		return status;
	}
	printf("WiFi is initialized.\r\n");

	char mac[6];
	status = wifi_mac_get((uint32_t*)mac);
	if(status != WiFi_MODULE_SUCCESS)
	{
		printf("Error of getting MAC address\r\n");
		#if APPLICATION_DEBUG_MSG
			wifi_print_status(status);
		#endif
		return status;
	}
	printf("Current MAC Adress: %s\r\n", mac);
	
#if !APPLICATION_HARDCODE_SSID
	uint16_t ssid_number = 0, security_number = 0;
	uint16_t countNetwork = 0;
	wifi_scan scan_result[MAX_SCAN_NETWORK];

	memset(scan_result, 0x00, sizeof(scan_result[0]) * MAX_SCAN_NETWORK);

	countNetwork = findWifiNetwork(scan_result, MAX_SCAN_NETWORK);

	printf("\r\nChoose WI-FI Hotspot # [0-%d]: ", countNetwork-1);

	do {
		getInputInt(&ssid_number);
		if (ssid_number > countNetwork) {
			printf("\r\nPlease insert a valid number\r\n");
			continue;
		}
	} while(0);

	sprintf(ssid, "%s", scan_result[ssid_number].ssid);

	printf("\r\nWi-Fi Security Mode\r\nNone\t\t= 0\r\nWEP\t\t= 1\r\nWPA_Personal\t= 2\r\nChoose WI-FI Security [0-2]: ");

	do {
		getInputInt(&security_number);
		if (security_number > 2) {
			printf("\r\nPlease insert a valid number\r\n");
			continue;
		}
	} while(0);

	mode = (WiFi_Priv_Mode)security_number;

	if (mode != None) {
		printf("\r\nSecurity Key: ");
		getInputStr(seckey);
	}

#else
	sprintf(ssid, "%s", 	DEFAULT_SSID);
	sprintf(seckey, "%s", 	DEFAULT_SECKEY);
	mode = 					DEFAULT_MODE;

	printf("ssid:   %s\r\n", ssid);
	printf("seckey: %s\r\n", seckey);
#endif

#if APPLICATION_DEBUG_MSG
	printf("Connecting to AP: %s\r\n", ssid);
#endif
	
	status = wifi_connect(ssid, mode == None? NULL : seckey, mode);
	if(status != WiFi_MODULE_SUCCESS)
	{
		printf("Error in AP Connection");
		#if APPLICATION_DEBUG_MSG
			wifi_print_status(status);
		#endif
		return status;
	}

	/* wait here to be connected */
	while(wifi_state != wifi_state_connected)
	{
		printf(".");
		HAL_Delay(1000);
	}
	printf("\r\n");

	/* date/time in unix secs past 1-Jan-70 */
	while (time_get_time_from_time_server() != WiFi_MODULE_SUCCESS){
		HAL_Delay(1000);
	}
	
	printf("Wait for NTP server Connection to be closed\r\n");

	__disable_irq();
	
	uint32_t temp = 0;
	
	while ((TimeSocketClosed == WIFI_FALSE)&& (temp++ < 50))
	{
		__enable_irq();
		
		printf(".");
		HAL_Delay(500);
		__disable_irq();
	}
	__enable_irq();
	
	if(TimeSocketClosed == WIFI_FALSE)
	{
		status = wifi_socket_client_close(TimeSocket);
		
		if(status!=WiFi_MODULE_SUCCESS)
		{
			printf("Error in Closer of NTP Server Socket\r\n");
			#if APPLICATION_DEBUG_MSG
				wifi_print_status(status);
			#endif
			return status;
		}
	}
	return status;
}
/**
* @brief Opens a socket to NTP server to read time
*
* 
* @param no parameter
* @return WiFi_Status_t value
*/
/*********************************************************************************//**
The Time Protocol may be implemented over the Transmission Control Protocol (TCP) or 
the User Datagram Protocol (UDP). A host connects to a server that supports the Time 
Protocol on port 37. The server then sends the time as a 32-bit unsigned integer in 
binary format and in network byte order, representing the number of seconds since 00:00 
(midnight) 1 January, 1900 GMT, and closes the connection. Operation over UDP requires 
the sending of any datagram to the server port, as there is no connection setup for UDP.

The fixed 32-bit data format means that the timestamp rolls over approximately every 136 years, 
with the first such occurrence on 7 February 2036. Programs that use the Time Protocol must be 
carefully designed to use context-dependent information to distinguish these dates from those in 1900.

********************************************************************************************/

uint8_t *ntp_server = (uint8_t*)"time-d.nist.gov";

uint8_t time_get_time_from_time_server()
{
	WiFi_Status_t status = WiFi_MODULE_SUCCESS;
	
	printf("Connecting to NTP server %s\r\n", ntp_server);
	
	//status = wifi_socket_client_open("time-d.nist.gov", 37, "t", &TimeSocket);
	status = wifi_socket_client_open( ntp_server, 37, "t", &TimeSocket);
	if(status == WiFi_MODULE_SUCCESS) {
#if APPLICATION_DEBUG_MSG   
		printf("NTP Server Connected\r\n");
#endif
		// Wait for NTP socket close callbacks
	}
	else
	{
		printf("NTP Server connection Error\r\n");
		#if APPLICATION_DEBUG_MSG
			wifi_print_status(status);
		#endif
	}
	return status;

}


/**
****** WiFi Indication User Callback ********
*/

/**
* @brief Called asychronously by wifi driver when data arrives on wifi interface
*
* This function calls a function pointer to read data. According to socket requirement NTP read or AWS read is called
* @param unsigned pointer to char - pointer to data buffer to be read.
* @param signed int - total message data size to be received
* @param signed int - partial message data size received of total message size
* @return no return
*/

void ind_wifi_socket_data_received(uint8_t socket_id,uint8_t * data_ptr, uint32_t message_size, uint32_t chunck_size)
{
	(*p_ind_wifi_socket_data_received)(data_ptr,message_size,chunck_size);
}

/**
* @brief Called asychronously by wifi driver when client socket is closed by NTP server
*
* 
* @param unsigned pointer to char - pointer to socket id
*/
void ind_wifi_socket_client_remote_server_closed(uint8_t * socket_closed_id)
{
	(*p_ind_wifi_socket_client_remote_server_closed)(socket_closed_id);
}


/**
* @brief Called asychronously by wifi driver when wifi device is initialized
*
* 
* @param no parameter
* @return no return
*/

void ind_wifi_on() //WIFI ready
{
	/*Critical. Never change it*/
	wifi_state = wifi_state_ready; 
}


/**
* @brief Called asychronously by wifi driver when wifi device is connected to AP
*
* 
* @param no parameter
* @return no return
*/

void ind_wifi_connected() //connected to AP
{
	printf("\r\nwifi connected to AP: %s\r\n", ssid);
	wifi_state = wifi_state_connected;
}

/**
* @brief Called asychronously by wifi driver when wifi device resumes from suspend
*
* 
* @param no parameter
* @return no return
*/

void ind_wifi_resuming()
{
	printf("\r\nwifi resuming from sleep user callback... \r\n");
	//Change the state to connect to socket if not connected
	wifi_state = wifi_state_socket;
}

void ind_wifi_warning(WiFi_Status_t warning_code)
{
	#if APPLICATION_DEBUG_MSG
			wifi_print_status(warning_code);
	#endif
}
void ind_wifi_error(WiFi_Status_t error_code)
{
	#if APPLICATION_DEBUG_MSG
			wifi_print_status(error_code);
	#endif

	if (WiFi_MALLOC_FAILED_ERROR == error_code)
	{
		(void)HAL_NVIC_SystemReset();
	}
}
void ind_wifi_connection_error(WiFi_Status_t status_code)
{
	#if APPLICATION_DEBUG_MSG
	printf("\r\n Connection Error");
			wifi_print_status(status_code);
	#endif
	if (WiFi_DISASSOCIATION == status_code)
	{
		(void)HAL_NVIC_SystemReset();
	}
//	wifi_state = wifi_state_disconnected;
//	wifi_main();
}

///
/////////////////////WiFi Indication User Callback for NTP server//////////////////////////////////////
///

/**
* @brief Called asychronously by wifi driver when data arrives on NTP Server socket
*
* 
* @param unsigned pointer to char - pointer to data buffer to be read.
* @param signed int - total message data size to be received
* @param signed int - partial message data size received of total message size
* @return no return
*/

void time_ind_wifi_socket_data_received(uint8_t * data_ptr, uint32_t message_size, uint32_t chunck_size)
{
#if APPLICATION_DEBUG_MSG
	printf("\r\nTime Received from NTP Server\r\n");
#endif
	///	
	/// Time Protocol provides the time as a binary number of seconds since 1900,
	/// 
	/// 2,208,988,800 corresponds to 00:00  1 Jan 1970 GMT from 12:00:01 am on 1 January 1900 GMT
	///
	last_ticks = ((data_ptr[0]<<24 )|(data_ptr[1]<<16)|(data_ptr[2]<<8)| data_ptr[3]) - 2208988800ul;	    
}

/**
* @brief Called asychronously by wifi driver when client socket is closed by NTP server
*
* 
* @param unsigned pointer to char - pointer to socket id
*/
void time_ind_wifi_socket_client_remote_server_closed(uint8_t * socket_closed_id)
{
	uint8_t id = *socket_closed_id;

	printf("\r\nNTP server socket closed\r\n");
	
	__disable_irq();

	TimeSocketClosed= WIFI_TRUE;
	
	__enable_irq();

	// Call here 
	wifi_state = wifi_state_idle;
}




///////////////////////////////////////////////////////////




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
* @}
*/

#if APPLICATION_DEBUG_MSG

void wifi_print_status(WiFi_Status_t status)
{
	switch ((int)status)
	{
		case WiFi_MODULE_SUCCESS				: printf("\r\n WiFi_MODULE_SUCCESS\r\n");
		break;
		case WiFi_TIME_OUT_ERROR				: printf("\r\n WiFi_TIME_OUT_ERROR\r\n");
		break;
		case WiFi_MODULE_ERROR					: printf("\r\n WiFi_MODULE_ERROR\r\n");
		break;
		case WiFi_HAL_OK								: printf("\r\n WiFi_HAL_OK\r\n");
		break;
		case WiFi_NOT_SUPPORTED					: printf("\r\n WiFi_NOT_SUPPORTED\r\n");
		break;
		case WiFi_NOT_READY							: printf("\r\n WiFi_NOT_READY\r\n");
		break;
		case WiFi_SCAN_FAILED						: printf("\r\n WiFi_SCAN_FAILED\r\n");;
		break;
		case WiFi_AT_CMD_BUSY 					: printf("\r\n WiFi_AT_CMD_BUSY\r\n");;
		break;
		case WiFi_SSID_ERROR						: printf("\r\n WiFi_SSID_ERROR\r\n");;
		break;
		case WiFi_SecKey_ERROR					: printf("\r\n WiFi_SecKey_ERROR\r\n");;
		break;
		case WiFi_CONFIG_ERROR					: printf("\r\n WiFi_CONFIG_ERROR\r\n");;
		break;
		case WiFi_STA_MODE_ERROR				: printf("\r\n WiFi_STA_MODE_ERROR\r\n");;
		break;
		case WiFi_AP_MODE_ERROR					: printf("\r\n WiFi_AP_MODE_ERROR\r\n");;
		break;
		case WiFi_AT_CMD_RESP_ERROR			: printf("\r\n WiFi_AT_CMD_RESP_ERROR\r\n");;
		break;
		case WiFi_AT_FILE_LENGTH_ERROR	: printf("\r\n WiFi_AT_FILE_LENGTH_ERROR\r\n");;
		break;
		case WiFi_HAL_UART_ERROR 				: printf("\r\n WiFi_HAL_UART_ERROR\r\n");;
		break;
		case WiFi_IN_LOW_POWER_ERROR		: printf("\r\n WiFi_IN_LOW_POWER_ERROR\r\n");;
		break;
		case WiFi_HW_FAILURE_ERROR			: printf("\r\n WiFi_HW_FAILURE_ERROR\r\n");;
		break;
		case WiFi_HEAP_TOO_SMALL_WARNING: printf("\r\n WiFi_HEAP_TOO_SMALL_WARNING\r\n");;
		break;
		case WiFi_STACK_OVERFLOW_ERROR	: printf("\r\n WiFi_STACK_OVERFLOW_ERROR\r\n");;
		break;
		case WiFi_HARD_FAULT_ERROR			: printf("\r\n WiFi_HARD_FAULT_ERROR\r\n");;
		break;
		case WiFi_MALLOC_FAILED_ERROR		: printf("\r\n WiFi_MALLOC_FAILED_ERROR\r\n");;
		break;
		case WiFi_INIT_ERROR						: printf("\r\n WiFi_INIT_ERROR\r\n");;
		break;
		case WiFi_POWER_SAVE_WARNING		: printf("\r\n WiFi_POWER_SAVE_WARNING\r\n");;
		break;
		case WiFi_SIGNAL_LOW_WARNING		: printf("\r\n WiFi_SIGNAL_LOW_WARNING\r\n");;
		break;
		case WiFi_JOIN_FAILED						: printf("\r\n WiFi_JOIN_FAILED\r\n");;
		break;
		case WiFi_SCAN_BLEWUP						: printf("\r\n WiFi_SCAN_BLEWUP\r\n");;
		break;
		case WiFi_START_FAILED_ERROR		: printf("\r\n WiFi_START_FAILED_ERROR\r\n");;
		break;
		case WiFi_EXCEPTION_ERROR				: printf("\r\n WiFi_EXCEPTION_ERROR\r\n");;
		break;
		case WiFi_DE_AUTH								: printf("\r\n WiFi_DE_AUTH\r\n");;
		break;
		case WiFi_DISASSOCIATION				: printf("\r\n WiFi_DISASSOCIATION\r\n");;
		break;
		case WiFi_UNHANDLED_IND_ERROR		: printf("\r\n WiFi_UNHANDLED_IND_ERROR\r\n");;
		break;
		case WiFi_RX_MGMT								: printf("\r\n WiFi_RX_MGMT\r\n");;
		break;
		case WiFi_RX_DATA								: printf("\r\n WiFi_RX_DATA\r\n");;
		break;
		case WiFi_RX_UNK								: printf("\r\n WiFi_RX_UNK\r\n");;
		break;
	}
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
