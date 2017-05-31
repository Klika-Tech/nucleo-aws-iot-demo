Overview
=============

The platform demonstrates a [Nucleo board](http://www.st.com/en/evaluation-tools/stm32-mcu-nucleo.html?querycriteria=productId=LN1847) with [AWS IoT](https://aws.amazon.com/iot/) integration use case.

##### Contents

- [Platform Scheme](#platform-scheme)
- [Components](#components)

## Platform Scheme

<p align="left">
  <img src="./assets/scheme.png" atl="Nucleo Board Demo Platform Scheme" />
</p>

## Components

- [STM32 Nucleo-64 board](http://www2.st.com/content/st_com/en/products/evaluation-tools/product-evaluation-tools/mcu-eval-tools/stm32-mcu-eval-tools/stm32-mcu-nucleo.html?querycriteria=productId=LN1847) with [WiFi](http://www2.st.com/content/st_com/en/products/ecosystems/stm32-open-development-environment/stm32-nucleo-expansion-boards/stm32-ode-connect-hw/x-nucleo-idw01m1.html) and [sensors](http://www2.st.com/content/st_com/en/products/ecosystems/stm32-open-development-environment/stm32-nucleo-expansion-boards/stm32-ode-sense-hw/x-nucleo-iks01a1.html) expansions. The board is a "thing" for the AWS IoT service. It updates its shadow with the sensors data every second. 
- [AWS-powered back-end](BACKEND.md) receives the data, stores it in a DB and provides an API for the data retrieval.
- [Web dashboard](DASHBOARD.md) uses the API to fetch and visualize the data.
- [Android app](ANDROID.md) is another front-end. It fetches and displays the sensors data as well.