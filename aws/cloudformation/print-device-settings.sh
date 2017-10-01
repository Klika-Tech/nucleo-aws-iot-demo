#!/usr/bin/env bash

echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
echo ">>> Print Device Settings >>>>"
echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
echo

ROOT_CA_URL="https://www.symantec.com/content/en/us/enterprise/verisign/roots/VeriSign-Class%203-Public-Primary-Certification-Authority-G5.pem"
STACK_RESOURCE_FOLDER=${STACK_RESOURCE_FOLDER:=./tmp}
AWS_REGION=$(cat ${STACK_RESOURCE_FOLDER}/config.json | jq .awsRegion --raw-output)
KEY_NAME=$(cat ${STACK_RESOURCE_FOLDER}/config.json | jq .keyName --raw-output)
AWS_IOT_MQTT_HOST=$(aws --region ${AWS_REGION} iot describe-endpoint | jq .endpointAddress --raw-output)
AWS_IOT_MY_THING_NAME=$(cat ${STACK_RESOURCE_FOLDER}/stacks.json | jq -r '.[][].Parameters[] | select(.ParameterKey=="thingName") | .ParameterValue')
AWS_IOT_MQTT_CLIENT_ID=${AWS_IOT_MY_THING_NAME}
AWS_IOT_MQTT_TOPIC_DATA=$(cat ${STACK_RESOURCE_FOLDER}/stacks.json | jq -r '.[][].Parameters[] | select(.ParameterKey=="mqttDataTopic") | .ParameterValue')

echo AWS_IOT_MQTT_HOST = ${AWS_IOT_MQTT_HOST}
echo AWS_IOT_MQTT_CLIENT_ID = ${AWS_IOT_MQTT_CLIENT_ID}
echo AWS_IOT_MY_THING_NAME = ${AWS_IOT_MY_THING_NAME}
echo AWS_IOT_MQTT_TOPIC_DATA = ${AWS_IOT_MQTT_TOPIC_DATA}
echo AWS_IOT_MQTT_TOPIC_SHADOW = "\$aws/things/${AWS_IOT_MY_THING_NAME}/shadow/update"
echo

echo "THING rootCA: "
curl ${ROOT_CA_URL}
echo
echo

echo "THING clientCRT: "
cat ${STACK_RESOURCE_FOLDER}/${KEY_NAME}-certificate.pem.crt
echo

echo "THING clientKey: "
cat ${STACK_RESOURCE_FOLDER}/${KEY_NAME}.key.pem
echo
