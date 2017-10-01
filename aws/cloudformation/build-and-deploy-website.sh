#!/usr/bin/env bash

echo ">>>>>>>> Deploy Website >>>>>>>>"

CURRENT_DIR=$(dirname "$0")
STACK_RESOURCE_FOLDER=${STACK_RESOURCE_FOLDER:=${CURRENT_DIR}/tmp}
CF_STACK_NAME=$(cat ${STACK_RESOURCE_FOLDER}/config.json | jq .stackName --raw-output)
AWS_REGION=$(cat ${STACK_RESOURCE_FOLDER}/config.json | jq .awsRegion --raw-output)
WEBSITE_BUCKET_NAME=$(aws --region=${AWS_REGION} cloudformation describe-stack-resources --stack-name ${CF_STACK_NAME} \
| jq -r '.[][] | select(.LogicalResourceId=="dashboardS3Bucket") | .PhysicalResourceId')

echo AWS S3 deploy bucket name: ${WEBSITE_BUCKET_NAME}

# build website
$(cd ${CURRENT_DIR}/../../dashboard/ && npm install && npm run build)

# upload website
aws s3 cp ${CURRENT_DIR}/../../dashboard/dist/ s3://${WEBSITE_BUCKET_NAME}/ --recursive --region ${AWS_REGION}