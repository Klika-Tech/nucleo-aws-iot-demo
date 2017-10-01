#!/usr/bin/env bash

echo ">>>>>>>> Create Dashboard Config >>>>>>>>"

CURRENT_DIR=$(dirname "$0")
STACK_RESOURCE_FOLDER=${STACK_RESOURCE_FOLDER:=${CURRENT_DIR}/tmp}
CF_STACK_NAME=$(cat ${STACK_RESOURCE_FOLDER}/config.json | jq .stackName --raw-output)
AWS_REGION=$(cat ${STACK_RESOURCE_FOLDER}/config.json | jq .awsRegion --raw-output)

aws --region=${AWS_REGION} cloudformation describe-stacks --stack-name ${CF_STACK_NAME} > ${STACK_RESOURCE_FOLDER}/stacks.json

API_URL=$(cat ${STACK_RESOURCE_FOLDER}/stacks.json | jq -r '.[][].Outputs[] | select(.OutputKey=="apiUrl") | .OutputValue')
COGNITO_IDENTITY_POOL=$(cat ${STACK_RESOURCE_FOLDER}/stacks.json | jq -r '.[][].Outputs[] | select(.OutputKey=="cognitoIdentityPool") | .OutputValue')
IOT_ENDPOINT=$(cat ${STACK_RESOURCE_FOLDER}/stacks.json | jq -r '.[][].Outputs[] | select(.OutputKey=="iotEndpoint") | .OutputValue')
MQTT_TOPIC=$(cat ${STACK_RESOURCE_FOLDER}/stacks.json | jq -r '.[][].Outputs[] | select(.OutputKey=="mqttTopic") | .OutputValue')

echo "module.exports = {
    awsRegion: \"${AWS_REGION}\",
    apiUrl: \"${API_URL}\",
    cognitoIdentityPool: \"${COGNITO_IDENTITY_POOL}\",
    iotEndpoint: \"${IOT_ENDPOINT}\",
    mqttTopic: \"${MQTT_TOPIC}\",
    debug: false
};" > ${CURRENT_DIR}/../../dashboard/src/config.js

echo Dashboard config file: ../../dashboard/src/config.json