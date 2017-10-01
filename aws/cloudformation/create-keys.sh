#!/usr/bin/env bash

echo ">>>>>>>> Create Keys >>>>>>>>"

CURRENT_DIR=$(dirname "$0")
STACK_RESOURCE_FOLDER=${STACK_RESOURCE_FOLDER:=${CURRENT_DIR}/tmp}
KEY_NAME=$(cat ${STACK_RESOURCE_FOLDER}/config.json | jq .keyName --raw-output)
AWS_REGION=$(cat ${STACK_RESOURCE_FOLDER}/config.json | jq .awsRegion --raw-output)

openssl ecparam -genkey -name prime256v1 -out ${STACK_RESOURCE_FOLDER}/${KEY_NAME}.key.pem
openssl req -new -sha256 -key ${STACK_RESOURCE_FOLDER}/${KEY_NAME}.key.pem \
    -subj "/C=EU/ST=Nucleo/L=Minsk/O=Dis/CN=www.example.com" \
    -out ${STACK_RESOURCE_FOLDER}/${KEY_NAME}.csr

CSR=$(cat ${STACK_RESOURCE_FOLDER}/${KEY_NAME}.csr)

aws --region ${AWS_REGION} iot create-certificate-from-csr --set-as-active \
    --certificate-pem-outfile ${STACK_RESOURCE_FOLDER}/${KEY_NAME}-certificate.pem.crt \
    --certificate-signing-request "${CSR}" > ${STACK_RESOURCE_FOLDER}/keys.json

echo Your Certificate ARN is: $(cat ${STACK_RESOURCE_FOLDER}/keys.json | jq .certificateArn)