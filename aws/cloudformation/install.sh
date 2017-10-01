#!/usr/bin/env bash

echo ">>>>>>>>>>>>>>>>>>>>>>>>>"
echo ">>>>>>>> INSTALL >>>>>>>>"
echo ">>>>>>>>>>>>>>>>>>>>>>>>>"
echo

CURRENT_DIR=$(dirname "$0")
export STACK_RESOURCE_FOLDER=${CURRENT_DIR}/tmp
export CF_STACK_NAME=$1
export AWS_REGION=$2
export OWM_API_KEY=$3

RANDOM_STR=$(cat /dev/urandom | tr -dc 'a-z' | head -c 8)

mkdir -p ${STACK_RESOURCE_FOLDER}

echo "{
  \"stackName\": \"${CF_STACK_NAME}\",
  \"awsRegion\": \"${AWS_REGION}\",
  \"ownApiKey\": \"${OWM_API_KEY}\",
  \"deployBucketName\": \"${CF_STACK_NAME}-cf-deploy-${RANDOM_STR}\",
  \"keyName\": \"${CF_STACK_NAME}\",
  \"weatherLambdaFile\": \"fetchWeatherLambda.zip\",
  \"getDataLambdaFile\": \"getNucleoData.zip\"
}" > ${STACK_RESOURCE_FOLDER}/config.json

${CURRENT_DIR}/create-keys.sh
${CURRENT_DIR}/create-deploy-bucket.sh
${CURRENT_DIR}/create-parameters.sh
${CURRENT_DIR}/create-stack.sh
${CURRENT_DIR}/create-dashboard-config.sh
${CURRENT_DIR}/build-and-deploy-website.sh
${CURRENT_DIR}/print-device-settings.sh
${CURRENT_DIR}/print-cloud-settings.sh

echo ">>>>>>>>>>>>>>>>>>>>>>>"
echo ">>>>>> COMPLETE >>>>>>>"
echo ">>>>>>>>>>>>>>>>>>>>>>>"
echo
