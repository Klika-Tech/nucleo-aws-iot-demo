
#include "network_interface.h"
#include "timer_interface.h"
#include "wifi_module.h"
#include "tls_error_nucleo.h"
#include "aws_iot_log.h"
#include "aws_iot_config.h"

extern void (*p_ind_wifi_socket_data_received)(uint8_t * data_ptr, uint32_t message_size, uint32_t chunck_size);
extern void (*p_ind_wifi_socket_client_remote_server_closed)(uint8_t * socket_closed_id);

void data_ind_wifi_socket_data_received(uint8_t * data_ptr, uint32_t message_size, uint32_t chunck_size);
void data_ind_wifi_socket_client_remote_server_closed(uint8_t * socket_closed_id);

extern volatile uint32_t data_arrived_on_socket;
extern int32_t last_ticks;

#ifdef USART_PRINT_MSG

extern char receive_msg_buff[MAX_BUFFER_GLOBAL];
extern char print_msg_buff[MAX_BUFFER_GLOBAL];
extern UART_HandleTypeDef UartMsgHandle;

#define printf(arg...)    {sprintf((char*)print_msg_buff, arg);   \
HAL_UART_Transmit(&UartMsgHandle, (uint8_t*)print_msg_buff, strlen(print_msg_buff), 1000);}

#endif


void delay(uint32_t loop)
{
	volatile uint32_t loopcount = loop;
	
	while(loop--)
	{
	};
}
	

/**
 * @brief Initialize the TLS implementation
 *
 * Perform any initialization required by the TLS layer.
 * Connects the interface to implementation by setting up
 * the network layer function pointers to platform implementations.
 *
 * @param pNetwork - Pointer to a Network struct defining the network interface.
 * @return integer defining successful initialization or TLS error
 */
int iot_tls_init(Network *pNetwork)
{
	pNetwork->my_socket = 0;
	pNetwork->mqttread = iot_tls_read;
	pNetwork->mqttwrite = iot_tls_write;
	pNetwork->disconnect = iot_tls_disconnect;
	
	p_ind_wifi_socket_data_received = data_ind_wifi_socket_data_received;
	p_ind_wifi_socket_client_remote_server_closed = data_ind_wifi_socket_client_remote_server_closed;
	
	return TLS_E_SUCCESS;
}

/**
 * @brief Create a TLS socket and open the connection
 *
 * Creates an open socket connection including TLS handshake.
 *
 * @param pNetwork - Pointer to a Network struct defining the network interface.
 * @param TLSParams - TLSConnectParams defines the properties of the TLS connection.
 * @return integer - successful connection or TLS error
 */
int iot_tls_connect(Network *pNetwork, TLSConnectParams TLSParams)
{
	uint32_t retrycount = 5;	
	
	INFO("CA: %d, DC: %d, PC: %d", strlen((const char *)TLSParams.pRootCALocation), strlen((const char *)TLSParams.pDeviceCertLocation), strlen((const char *)TLSParams.pDevicePrivateKeyLocation));
	
	if ( wifi_socket_client_security(
			"m", //o->one-way Auth, m-> mutual Auth
			TLSParams.pRootCALocation,
			TLSParams.pDeviceCertLocation,
			TLSParams.pDevicePrivateKeyLocation,
			AWS_IOT_MQTT_HOST,  //"SERVER"
			last_ticks
			) != WiFi_MODULE_SUCCESS)
	{
		ERROR("wifi_socket_client_security FAIL");
		return TLS_E_SOCKETS_INIT_ERROR;
	}

	label_client_open: ;
	WiFi_Status_t wc = wifi_socket_client_open(TLSParams.pDestinationURL, TLSParams.DestinationPort, "s",(uint8_t *)&pNetwork->my_socket); 
	if (wc != WiFi_MODULE_SUCCESS) {
		ERROR("wifi_socket_client_open FAIL");
		retrycount --;
		if(retrycount) goto label_client_open;
		
		return TLS_E_SOCKETS_INIT_ERROR;
	}

	return TLS_E_SUCCESS;

}

/**
 * @brief Write bytes to the network socket
 *
 * @param Network - Pointer to a Network struct defining the network interface.
 * @param unsigned char pointer - buffer to write to socket
 * @param integer - number of bytes to write
 * @param integer - write timeout value in milliseconds
 * @return integer - number of bytes written or TLS error
 */
int iot_tls_write(Network* pNetwork, unsigned char* writebuffer, int bytestowrite, int timeout_ms)
{
	WiFi_Status_t status = WiFi_MODULE_SUCCESS;
	
//	INFO("+iot_tls_write. bytes to write = %d within  timeout_ms = %d",bytestowrite,timeout_ms);

	
	if ( (status = wifi_socket_client_write(pNetwork->my_socket, bytestowrite, writebuffer)) == WiFi_MODULE_SUCCESS)
	{
//		INFO("+iot_tls_write bytes written = %d. ",bytestowrite);

		return bytestowrite;
	}
	else
	{
//		INFO("+iot_tls_write failed");
		return status;
	}
}

/**
 * @brief Read bytes from the network socket
 *
 * @param Network - Pointer to a Network struct defining the network interface.
 * @param unsigned char pointer - pointer to buffer where read bytes should be copied
 * @param integer - number of bytes to read
 * @param integer - read timeout value in milliseconds
 * @return integer - number of bytes read or TLS error
 */

int iot_tls_read(Network* pNetwork, unsigned char* readbuffer, int bytestoread, int timeout_ms)
{
//WiFi_Status_t status = WiFi_MODULE_SUCCESS;
	uint32_t i = 0;
	
	uint32_t tickstart = 0;
  tickstart = HAL_GetTick();

//	INFO("+iot_tls_read. bytes to read = %d within  timeout_ms = %d",bytestoread,timeout_ms);
	

	while(i < bytestoread )
	{
		if(DequeueRx(&readbuffer[i]))
		{
			i++;
		}
		
#if defined ( __GNUC__ )
		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
#else
		__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
#endif
		if((HAL_GetTick() - tickstart) > timeout_ms)
		{
			break;
		}
		
	}

//	INFO("+iot_tls_read. bytes read = %d",i);
	
	return i;
	
}


/**
 * @brief Disconnect from network socket
 *
 * @param Network - Pointer to a Network struct defining the network interface.
 */
void iot_tls_disconnect(Network *pNetwork)
{
	wifi_socket_client_close(pNetwork->my_socket);
}

/**
 * @brief Perform any tear-down or cleanup of TLS layer
 *
 * Called to cleanup any resources required for the TLS layer.
 *
 * @param Network - Pointer to a Network struct defining the network interface.
 * @return integer - successful cleanup or TLS error
 */
int iot_tls_destroy(Network *pNetwork)
{
	pNetwork->my_socket = 0;
	pNetwork->mqttread = NULL;
	pNetwork->mqttwrite = NULL;
	pNetwork->disconnect = NULL;
	
	return TLS_E_SUCCESS;
}
