/**
 ******************************************************************************
 * @file    wifi_const.h
 * @author  Central LAB
 * @version V2.0.0
 * @date    10-February-2016
 * @brief   Describes the constants and defines in X-CUBE-WIFI1
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

/** @defgroup NUCLEO_WIFI_INTERFACE_Private_Defines
  * @{
  */
    
#define EPOCH_TIME                              1453727657//Human time (GMT): Mon, 25 Jan 2016 13:14:17 GMT
#define EXTI_CONF_TIMER                         1900 //millisec
#define PROCESS_WIFI_TIMER                      1
#define SLEEP_RESUME_PREVENT                    2000
#define RINGBUF_SIZE                            1024
#define MAX_BUFFER_GLOBAL                       512
#define MAX_WIFI_SCAN_NETWORK                   256
#define RxBufferSize                            64
#define AT_RESP_LEN_OK                          6 //\r\nOK\r\n
#define AT_RESP_LEN_GPIOR                       21//GPIO n = 0,in\r\n\r\nOK\r\n
#define AT_RESP_HELP_TEXT_LEN                   512
#define AT_ATTENTION                            "AT\r"
#define AT_WIFI_ENABLE                          "AT+S.WIFI=%d\r"
#define AT_GET_CONFIGURATION_VALUE              "AT+S.GCFG=%s\r"
#define AT_SET_CONFIGURATION_VALUE              "AT+S.SCFG=%s,%d\r"
#define AT_SET_CONFIGURATION_ADDRESS            "AT+S.SCFG=%s,%s\r"
//#define AT_GET_SSID                           "AT&F\r"
#define AT_SET_SSID                             "AT+S.SSIDTXT=%s\r"
#define AT_SET_SEC_KEY                          "AT+S.SCFG=wifi_wpa_psk_text,%s\r"
#define AT_RESTORE_DEFAULT_SETTING              "AT&F\r"
#define AT_SAVE_CURRENT_SETTING                 "AT&W\r"
#define AT_SET_POWER_STATE                      "AT+CFUN=%d\r"
#define AT_HELP_TEXT                            "AT+S.HELP\r"
#define AT_RESET_MSG                            "\r\n+WIND:2:Reset\r\n"

#define UNDEFINE_LENGTH                         0xFFFF
#define AT_WiFi_SCAN                            "AT+S.SCAN\r"
#define AT_SOCKET_OPEN                          "AT+S.SOCKON=%s,%d,%s,ind\r"
#define AT_SOCKET_WRITE                         "AT+S.SOCKW=%d,%d\r"
#define AT_SOCKET_READ                          "AT+S.SOCKR=%d,%d\r"
#define AT_SOCKET_CLOSE                         "AT+S.SOCKC=%d\r"
#define AT_SERVER_SOCKET_OPEN                   "AT+S.SOCKD=%d,%s,ind\r" //with indication option
#define AT_SERVER_SOCKET_CLOSE                  "AT+S.SOCKD=0\r"
#define AT_QUERY_PENDING_DATA                   "AT+S.SOCKQ=%d\r"
#define AT_DISPLAY_FILE_NAME                    "AT+S.FSL\r"
#define AT_DISPLAY_FILE_CONTENT                 "AT+S.FSP=/%s\r"
#define AT_CREATE_NEW_HTML_FILE                 "AT+S.FSC=/%s,%d\r"
#define AT_APPEND_FILE                          "AT+S.FSA=/%s,%d\r"
#define AT_DELETE_FILE                          "AT+S.FSD=/%s\r"
#define AT_DOWNLOAD_IMAGE_FILE                  "AT+S.HTTPDFSUPDATE=%s,/%s,%d\r"
#define AT_ERASE_FLASH_MEMORY                   "AT+S.HTTPDFSERASE\r"

#define AT_CMD_TO_DATA_MODE                     "AT+S.\r"
#define AT_DATA_TO_CMD_MODE                     "at+s." /* NOT \r */
#define AT_HTTPPOST_REQUEST                     "AT+S.HTTPPOST=%s\r"
#define AT_HTTPD                                "AT+S.HTTPD=%d\r"
    
/************Wi-Fi Config Variables**************/
    
#define BLINK_LED                               "blink_led"
#define LOCALECHO1                              "localecho1"
#define CONSOLE1_HWFC                           "console1_hwfc"
#define CONSOLE1_SPEED                          "console1_speed"
#define WIFI_PRIV_MODE                          "wifi_priv_mode"
#define IP_USE_DHCP_SERVER                      "ip_use_dhcp"
#define IP_USE_HTTPD                            "ip_use_httpd"
#define WIFI_MODE                               "wifi_mode"
#define WIFI_WPA_SECURITY                       "wifi_wpa_psk_text"  
#define WIFI_CHANNEL_NUMBER                     "wifi_channelnum"  
#define WIFI_IP_ADDRESS                         "ip_ipaddr"
#define WIFI_IP_DEFAULT_GATEWAY                 "ip_gw"
#define WIFI_IP_DNS                             "ip_dns"
#define WIFI_IP_NETMASK                         "ip_netmask"
#define WIFI_IP_HOSTNAME                        "ip_hostname"
#define WIFI_IP_APDOMAINNAME                    "ip_apdomainname"
#define WIFI_IP_APREDIRECT                      "ip_apredirect"
#define WIFI_IP_HTTP_TIMEOUT                    "ip_http_get_recv_timeout"
#define WIFI_IP_DHCP_TIMEOUT                    "ip_dhcp_timeout"
#define WIFI_MAC_ADDRESS												"nv_wifi_macaddr"
    
#define WIFI_SLEEP_ENABLED                      "sleep_enabled"
#define WIFI_HT_MODE                            "wifi_ht_mode"
#define WIFI_OPR_RATE_MASK                      "wifi_opr_rate_mask"
#define WIFI_POWERSAVE                          "wifi_powersave"
#define WIFI_OPERATIONAL_MODE                   "wifi_operational_mode"
#define WIFI_LISTEN_INTERVAL                    "wifi_listen_interval"
#define WIFI_BEACON_WAKEUP                      "wifi_beacon_wakeup"
#define WIFI_STANDBY_ENABLED                    "standby_enabled"
#define WIFI_STANDBY_TIME                       "standby_time" 
#define WIFI_TX_POWER                           "wifi_tx_power"
#define WIFI_IBSS_IP_ADDR                       "192.168.2.100"
#define WIFI_IBSS_DEFAULT_GATEWAY               "192.168.2.1"
#define WIFI_IBSS_IP_DNS_ADDR                   "192.168.2.1"
#define WIFI_IBSS_IP_MASK                       "255.255.255.0"
#define WIFI_IP_USE_DHCP                        0

/**
  * @}
  */
