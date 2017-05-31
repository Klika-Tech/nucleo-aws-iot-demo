Nucleo Board Demo Platform
=============

The platform demonstrates an [STM32 Nucleo board](doc/NUCLEO.md) and AWS integration use case.
The [back-end](doc/BACKEND.md) is entirely powered by [Amazon Web Services](https://aws.amazon.com/).
The [dashboard](doc/DASHBOARD.md) is a demo front-end implemented as a web application.
The [device](doc/NUCLEO.md) base on [STM32 Nucleo-64 board](http://www.st.com/en/evaluation-tools/stm32-mcu-nucleo.html?querycriteria=productId=LN1847).

You can see the live demo [here](http://iot-demo.nucleo.klika-tech.com/#/dashboard).

##### Contents

- [Overview](doc/OVERVIEW.md)
	- [Platform Scheme](doc/OVERVIEW.md#platform-scheme)
	- [Components](doc/OVERVIEW.md#components)
- [Back-end](doc/BACKEND.md)
	- [Components](doc/BACKEND.md#components)
	- [Amazon DynamoDB](doc/BACKEND.md#amazon-dynamodb)
	    - [Sensor Data](doc/BACKEND.md#sensor-data)
	    - [Weather Data](doc/BACKEND.md#weather-data)
	- [AWS IoT](doc/BACKEND.md#aws-iot)
	- [Certificates](doc/BACKEND.md#certificates)
	- [AWS Lambda](doc/BACKEND.md#aws-lambda)
	    - [Build Lambda Functions](doc/BACKEND.md#build-lambda-functions)
	    - [Create Lambda Functions](doc/BACKEND.md#create-lambda-functions)
            - [Get Nucleo Data](doc/BACKEND.md#get-nucleo-data)
        	- [Nucleo Fetch Weather](doc/BACKEND.md#nucleo-fetch-weather)
        	- [Generate Nucleo Data](doc/BACKEND.md#generate-nucleo-data)
	- [Amazon Cognito](doc/BACKEND.md#amazon-cognito)
	- [Amazon S3](doc/BACKEND.md#amazon-s3)
- [Device (STM32 Nucleo)](doc/NUCLEO.md)
	- [Hardware configurartion](doc/NUCLEO.md#hardware-configuration)
	- [Build Instruction](doc/NUCLEO.md#build-instruction)
		- [Setup AWS IoT connection options](doc/NUCLEO.md#setup-aws-iot-connection-options)
		- [Setup device certificates](doc/NUCLEO.md#setup-device-certificates)
		- [Setup WiFi credentials](doc/NUCLEO.md#setup-wifi-credentials)
		- [Deploy to device](doc/NUCLEO.md#deploy-to-device)	   		
- [Web Dashboard](doc/DASHBOARD.md)
	- [Build Instructions](doc/DASHBOARD.md#build-instructions)
   	- [Deployment (optional)](doc/DASHBOARD.md#deployment-optional)
- [Android app](doc/ANDROID.md)
	- [Overview](doc/ANDROID.md#overview)
	- [Build Instructions](doc/ANDROID.md#build-instructions)
   	
##### External Resources

- Build Tools
	- [Node.js](https://nodejs.org/en/)
	- [Webpack](https://webpack.github.io/)
	- [Keil uVision](http://www2.keil.com/mdk5/uvision/)
	- [System Workbench for STM32](http://www.st.com/en/development-tools/sw4stm32.html)
	- [Gradle](https://gradle.org/)
    - [Android Studio](https://developer.android.com/studio/index.html)
- Libraries
	- [React.js](https://facebook.github.io/react/)
	- [D3.js](https://d3js.org/)
	- [MQTT.js](https://github.com/mqttjs/MQTT.js)
	- [AWS SDK for Browser](https://aws.amazon.com/sdk-for-browser/)
	- [AWS SDK for Node.js](https://aws.amazon.com/sdk-for-node-js/)
	- [MPChartLib](https://github.com/PhilJay/MPAndroidChart/tree/master/MPChartLib)
	- [AWS SDK for Android](https://github.com/aws/aws-sdk-android)
- Drivers and MDK
    - [AWS IoT device SDK embedded C](https://github.com/aws/aws-iot-device-sdk-embedded-C)
    - [ST-LINK](http://www.st.com/en/embedded-software/stsw-link009.html)
    - [MDK-ARM](https://www.arm.com/products/tools/software-tools/mdk-arm/index.php)
- Cloud Services
	- [AWS IoT](https://aws.amazon.com/iot/)
	- [AWS Lambda](https://aws.amazon.com/lambda/) 
	- [Amazon DynamoDB](https://aws.amazon.com/dynamodb/)
	- [Amazon API Gateway](https://aws.amazon.com/api-gateway/)
	- [Amazon CloudWatch](http://aws.amazon.com/cloudwatch/)
	- [Amazon Cognito](http://aws.amazon.com/cognito/)
	- [Amazon S3](http://aws.amazon.com/s3/)
	- [OpenWeatherMap API](http://openweathermap.org/)
