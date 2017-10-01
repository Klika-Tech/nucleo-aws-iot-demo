#!/usr/bin/env bash

echo ">>>>>>>> Create Stack Parameters >>>>>>>>"

CURRENT_DIR=$(dirname "$0")
STACK_RESOURCE_FOLDER=${STACK_RESOURCE_FOLDER:=${CURRENT_DIR}/tmp}
OWM_API_KEY=$(cat ${STACK_RESOURCE_FOLDER}/config.json | jq .ownApiKey --raw-output)
CERT_ARN=$(cat ${STACK_RESOURCE_FOLDER}/keys.json | jq .certificateArn)
DEPLOY_BUCKET_NAME=$(cat ${STACK_RESOURCE_FOLDER}/config.json | jq .deployBucketName --raw-output)
WEATHER_LAMBDA_FILE=$(cat ${STACK_RESOURCE_FOLDER}/config.json | jq .weatherLambdaFile --raw-output)
GET_DATA_LAMBDA_FILE=$(cat ${STACK_RESOURCE_FOLDER}/config.json | jq .getDataLambdaFile --raw-output)

echo "[
   {
      \"ParameterKey\": \"owmApiKey\",
      \"ParameterValue\": \"${OWM_API_KEY}\"
   },
   {
      \"ParameterKey\": \"certificateARN\",
      \"ParameterValue\": ${CERT_ARN}
   },
   {
      \"ParameterKey\": \"deployS3Bucket\",
      \"ParameterValue\": \"${DEPLOY_BUCKET_NAME}\"
   },
   {
      \"ParameterKey\": \"deployS3weatherLambdaZip\",
      \"ParameterValue\": \"${WEATHER_LAMBDA_FILE}\"
   },
   {
      \"ParameterKey\": \"deployS3getDataLambdaZip\",
      \"ParameterValue\": \"${GET_DATA_LAMBDA_FILE}\"
   }
]" > ${STACK_RESOURCE_FOLDER}/parameters.json

echo Parameters file: ${STACK_RESOURCE_FOLDER}/parameters.json