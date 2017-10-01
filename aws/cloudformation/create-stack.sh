#!/usr/bin/env bash

echo ">>>>>>>> Create Stack >>>>>>>>"

CURRENT_DIR=$(dirname "$0")
STACK_RESOURCE_FOLDER=${STACK_RESOURCE_FOLDER:=${CURRENT_DIR}/tmp}
CF_STACK_NAME=$(cat ${STACK_RESOURCE_FOLDER}/config.json | jq .stackName --raw-output)
AWS_REGION=$(cat ${STACK_RESOURCE_FOLDER}/config.json | jq .awsRegion --raw-output)

echo AWS CloudFormation stack name: ${CF_STACK_NAME}

aws --region ${AWS_REGION} cloudformation create-stack --stack-name ${CF_STACK_NAME} \
    --template-body file://${CURRENT_DIR}/stack.yml \
    --capabilities CAPABILITY_IAM \
    --parameters file://${STACK_RESOURCE_FOLDER}/parameters.json || exit 1

echo "Waiting while stack creations complete ..."

aws --region ${AWS_REGION}  cloudformation wait stack-create-complete --stack-name ${CF_STACK_NAME} || exit 1
