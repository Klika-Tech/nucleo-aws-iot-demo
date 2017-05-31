/*
* Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License").
* You may not use this file except in compliance with the License.
* A copy of the License is located at
*
*  http://aws.amazon.com/apache2.0
*
* or in the "license" file accompanying this file. This file is distributed
* on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
* express or implied. See the License for the specific language governing
* permissions and limitations under the License.
*/

/**
* @file subscribe_publish_sample.c
* @brief simple MQTT publish and subscribe on the same topic
*
* This example takes the parameters from the aws_iot_config.h file and establishes a connection to the AWS IoT MQTT Platform.
* It subscribes and publishes to the same topic - "sdkTest/sub"
*
* If all the certs are correct, you should see the messages received by the application in a loop.
*
* The application takes in the certificate path, host name , port and the number of times the publish should happen.
*
*/

/*******************************************************************************
* @file    subscribe_publish_sample_nucleo.c
* @author  Central LAB
* @version V0.1.0
* @date    10-March-2016
* @brief   simple MQTT publish and subscribe implementation for sample on Nucleo.
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
#include "wifi_const.h"
#include <ctype.h>
#include "string.h"
#include "timer_interface.h"

#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_interface.h"
#include "aws_iot_config.h"
#include "x_nucleo_iks01a1_hum_temp.h"
#include "x_nucleo_iks01a1_pressure.h"
#include "x_nucleo_iks01a1_magneto.h"
#include "x_nucleo_iks01a1_imu_6axes.h"

extern char receive_msg_buff[MAX_BUFFER_GLOBAL];
extern char print_msg_buff[MAX_BUFFER_GLOBAL];
extern UART_HandleTypeDef UartMsgHandle;

#if defined (__GNUC__)
#define printf(arg...)    {sprintf((char*)print_msg_buff, arg);   \
HAL_UART_Transmit(&UartMsgHandle, (uint8_t*)print_msg_buff, strlen(print_msg_buff), 1000);}
#else
#define printf(...)    {sprintf((char*)print_msg_buff, __VA_ARGS__);   \
HAL_UART_Transmit(&UartMsgHandle, (uint8_t*)print_msg_buff, strlen(print_msg_buff), 1000);}
#endif

#define PATH_MAX 800
char *optarg;
int optind, opterr, optopt;

extern char rootCA[];
extern char clientCRT[];
extern char clientKey[];
//extern uint8_t * rootCA;
//extern uint8_t * clientCRT;
//extern uint8_t * clientKey;
extern volatile uint32_t BpushButtonState;

//int getopt(int argc, char** argv, const char *optstring) {return 0;}
//char* getcwd( char* buffer, size_t size ) {return 0;}

//Debug levels
#define	LOG_NO_DEBUG 0
#define LOG_BRIEF 2
#define LOG_FULL 4


#define APPLICATION_DEBUG LOG_BRIEF

/**
* @brief Default cert location
*/
//char certDirectory[PATH_MAX + 1] = "../../certs";

/**
* @brief Default MQTT HOST URL is pulled from the aws_iot_config.h
*/
char HostAddress[255] = AWS_IOT_MQTT_HOST;

/**
* @brief Default MQTT port is pulled from the aws_iot_config.h
*/
uint32_t port = AWS_IOT_MQTT_PORT;

/**
* @brief This parameter will avoid infinite loop of publish and exit the program after certain number of publishes
*/
typedef struct
{
	float x;
	float y;
	float z;
}Vector3;

int Axes_Vector3(Axes_TypeDef *pSrcData, Vector3 *pDesData)
{
	pDesData->x = pSrcData->AXIS_X * 0.001f;
	pDesData->y = pSrcData->AXIS_Y * 0.001f;
	pDesData->z = pSrcData->AXIS_Z * 0.001f;
}

//HUM-TEMP sens
float Temperature;
float Humidity;
//PRESS-TEMP sens
float Pressure;
//6 axis GYRO-ACCEL
Vector3 Accelerometer;
Vector3 Gyroscope;
Vector3 Magnetometer;

//	char CurrentWD[PATH_MAX + 1];
char cafileName[] = AWS_IOT_ROOT_CA_FILENAME;
char clientCRTName[] = AWS_IOT_CERTIFICATE_FILENAME;
char clientKeyName[] = AWS_IOT_PRIVATE_KEY_FILENAME;


/**
* @brief MQTT subscriber callback hander
*
* called when data is received from AWS IoT Thing (message broker)
* @param MQTTCallbackParams type parameter
* @return no return
*/
int MQTTcallbackHandler(MQTTCallbackParams params) {

	INFO("Subscribe callback");
	INFO("%.*s\t%.*s",
	(int)params.TopicNameLen, params.pTopicName,
	(int)params.MessageParams.PayloadLen, (char*)params.MessageParams.pPayload);

	return 0;
}

/**
* @brief MQTT disconnect callback hander
*
* @param no parameter
* @return no return
*/

void disconnectCallbackHandler(void) {
	WARN("MQTT Disconnect");
}

/**
* @brief main entry function to AWS IoT code
*
* @param no parameter
* @return IoT_Error_t status return
*/

int aws_main() {
	
	IoT_Error_t rc = NONE_ERROR;
	int32_t i = 0;

#if APPLICATION_DEBUG != LOG_NO_DEBUG
	INFO("HUMID sens      %s", ((int)BSP_HUM_TEMP_Init()== 0)? "OK" : "ERROR");
	INFO("PRESS sens      %s", ((int)BSP_PRESSURE_Init() == 0)? "OK" : "ERROR");
	INFO("GYRO|ACCEL sens %s", ((int)BSP_IMU_6AXES_Init() == 0)? "OK" : "ERROR");
	INFO("MAGNETO sens    %s", ((int)BSP_MAGNETO_Init() == 0)? "OK" : "ERROR");
#endif	
	BSP_IMU_6AXES_Enable_Free_Fall_Detection_Ext();

	INFO("\nAWS IoT SDK Version %d.%d.%d-%s\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_TAG);


	MQTTConnectParams connectParams = MQTTConnectParamsDefault;

	connectParams.KeepAliveInterval_sec = 30;
	connectParams.isCleansession = true;
	connectParams.MQTTVersion = MQTT_3_1_1;
	connectParams.pClientID = AWS_IOT_MQTT_CLIENT_ID;//"CSDK-test-device";
	connectParams.pHostURL = HostAddress;						 //Assign a Host Name
	connectParams.port = port;											 //8883
	connectParams.isWillMsgPresent = false;
	connectParams.pRootCALocation = rootCA;
	connectParams.pDeviceCertLocation = clientCRT;
	connectParams.pDevicePrivateKeyLocation = clientKey;
	connectParams.mqttCommandTimeout_ms = 15000;//2000 //ST
	connectParams.tlsHandshakeTimeout_ms = 15000;//5000; //ST
	connectParams.isSSLHostnameVerify = true;// ensure this is set to true for production
	//connectParams.isSSLHostnameVerify = false;// ensure this is set to true for production
	connectParams.disconnectHandler = disconnectCallbackHandler;

	INFO("Connecting AWS IoT...");
	rc = aws_iot_mqtt_connect(&connectParams);
	if (NONE_ERROR != rc) {
		ERROR("Error(%d) connecting to %s:%d", rc, connectParams.pHostURL, connectParams.port);
	}
	else
	{
		DEBUG("aws_iot_mqtt_connect: connected to %s:%d", connectParams.pHostURL, connectParams.port);
	}

	MQTTSubscribeParams subParams = MQTTSubscribeParamsDefault;
	subParams.mHandler = MQTTcallbackHandler;
	subParams.pTopic = "Nucleo/test";
	subParams.qos = QOS_0;

	if (NONE_ERROR == rc) {
		INFO("Subscribing to AWS IoT...");
		rc = aws_iot_mqtt_subscribe(&subParams);
		if (NONE_ERROR != rc) {
			ERROR("Error subscribing");
		} else {
			DEBUG("aws_iot_mqtt_subscribe PASS");
		}
	}

	MQTTMessageParams Msg = MQTTMessageParamsDefault;
	Msg.qos = QOS_0;
	char cPayload[2000];
	sprintf(cPayload, "%s : %d ", "hello from STM", i);
	Msg.pPayload = (void *) cPayload;


	MQTTPublishParams Params = MQTTPublishParamsDefault;
	//Params.pTopic = "Nucleo/shadow/update";


	#define BUTTONDELAY 50 //Delay in ms to check the button state
	#define DELAYRATIO 1		//Multiplyer for autopublish/buttonpress
	int delays = DELAYRATIO;
	bool Publish = false;
	
	Axes_TypeDef temp_axes;
	
	while (NONE_ERROR == rc) {
		delays--;
		//Max time the yield function will wait for read messages
		rc = aws_iot_mqtt_yield(2000); //100
		DEBUG("-->sleep");
		
		fsleep(BUTTONDELAY); //Sleeping for BUTTONDELAY ms
		
		//Getting values of sensors
		if(BSP_HUM_TEMP_GetTemperature((float *)&Temperature) != HUM_TEMP_OK)
			ERROR("Sensor TEMP0 reading error\r\n");
		
		if(BSP_HUM_TEMP_GetHumidity((float *)&Humidity) != HUM_TEMP_OK)
			ERROR("Sensor HUMID reading error\r\n");
		
		if(BSP_PRESSURE_GetPressure((float *)&Pressure) != PRESSURE_OK)
			ERROR("Sensor PRESS reading error\r\n");
		
		if(BSP_IMU_6AXES_X_GetAxes((Axes_TypeDef *)&temp_axes) != IMU_6AXES_OK)
			ERROR("Sensor ACCEL reading error\r\n");
		Axes_Vector3(&temp_axes, &Accelerometer);
		
		if(BSP_IMU_6AXES_G_GetAxes((Axes_TypeDef *)&temp_axes) != IMU_6AXES_OK)
			ERROR("Sensor GYROS reading error\r\n");
		Axes_Vector3(&temp_axes, &Gyroscope);
		
		if(BSP_MAGNETO_M_GetAxes((Axes_TypeDef *)&temp_axes) != MAGNETO_OK)
			ERROR("Sensor MAGNET reading error\r\n");
		Axes_Vector3(&temp_axes, &Magnetometer);

		if(BpushButtonState) //Polling button state each BUTTONDELAY ms
		{
			BpushButtonState = 0;
			Publish = true;
						
			// Here is subscription topic.
			Params.pTopic = "Nucleo/data";
			
			sprintf(cPayload, "{\"temperature\": %f, \"humidity\": %f, \"pressure\": %f, \"accelerometer\": [%f, %f, %f], \"gyroscope\": [%f, %f, %f], \"magnetometer\": [%f, %f, %f], \"marker\": true}",
				Temperature, Humidity, Pressure, Accelerometer.x, Accelerometer.y, Accelerometer.z, Gyroscope.x, Gyroscope.y, Gyroscope.z, Magnetometer.x, Magnetometer.y, Magnetometer.z);
#if APPLICATION_DEBUG != LOG_NO_DEBUG
			INFO("[BTTN] Publishing...");
			#if APPLICATION_DEBUG == LOG_FULL
			INFO("       [TEMPE]: %f", Temperature);
			INFO("       [HUMID]: %f", Humidity);
			INFO("       [PRESS]: %f", Pressure);
			INFO("       [ACCEL]: (%f, %f, %f)", Accelerometer.x, Accelerometer.y, Accelerometer.z);
			INFO("       [GYROS]: (%f, %f, %f)", Gyroscope.x, Gyroscope.y, Gyroscope.z);
			INFO("       [MAGNE]: (%f, %f, %f)", Magnetometer.x, Magnetometer.y, Magnetometer.z);
			#endif
#endif
		}
		else
		{
			if(delays <= 0)
			{
				Publish = true;
				// Here is subscription topic for shadow.
				Params.pTopic = "$aws/things/Nucleo/shadow/update";
				
				sprintf(cPayload, "{\"state\": {\"reported\": {\"temperature\": %f, \"humidity\": %f, \"pressure\": %f, \"accelerometer\": [%f, %f, %f], \"gyroscope\": [%f, %f, %f], \"magnetometer\": [%f, %f, %f]}}}",
					Temperature, Humidity, Pressure, Accelerometer.x, Accelerometer.y, Accelerometer.z, Gyroscope.x, Gyroscope.y, Gyroscope.z, Magnetometer.x, Magnetometer.y, Magnetometer.z);

#if APPLICATION_DEBUG != LOG_NO_DEBUG
			INFO("[AUTO] Publishing...");
			#if APPLICATION_DEBUG == LOG_FULL				
				INFO("       [TEMPE]: %f", Temperature);
				INFO("       [HUMID]: %f", Humidity);
				INFO("       [PRESS]: %f", Pressure);
				INFO("       [ACCEL]: (%f, %f, %f)", Accelerometer.x, Accelerometer.y, Accelerometer.z);
				INFO("       [GYROS]: (%f, %f, %f)", Gyroscope.x, Gyroscope.y, Gyroscope.z);
				INFO("       [MAGNE]: (%f, %f, %f)", Magnetometer.x, Magnetometer.y, Magnetometer.z);
				#endif
#endif
				delays = DELAYRATIO;
			}
			
		}
		if(Publish)
		{
			Publish = false;
			Msg.PayloadLen = strlen(cPayload);
			Params.MessageParams = Msg;
			rc = aws_iot_mqtt_publish(&Params);
		}
		
				
	}

	if(NONE_ERROR != rc){
		ERROR("An error occurred in the loop.\n");
	}
	else{
		INFO("Publish done");
	}
	/* Error or No Error, disconnect from MQTT server*/
	aws_iot_mqtt_disconnect();
	
	return rc;
}



