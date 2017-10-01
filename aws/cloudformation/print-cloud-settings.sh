#!/usr/bin/env bash

echo ">>>>>>>>>>>>>>>>>>>>>>>"
echo ">>> Cloud Settings >>>>"
echo ">>>>>>>>>>>>>>>>>>>>>>>"
echo

STACK_RESOURCE_FOLDER=${STACK_RESOURCE_FOLDER:=./tmp}
AWS_REGION=$(cat ${STACK_RESOURCE_FOLDER}/config.json | jq .awsRegion --raw-output)
WEBSITE_URL=$(cat ${STACK_RESOURCE_FOLDER}/stacks.json | jq -r '.[][].Outputs[] | select(.OutputKey=="websiteURL") | .OutputValue')

echo Dashboard available by url: ${WEBSITE_URL}
echo