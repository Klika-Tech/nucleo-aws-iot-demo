#!/usr/bin/env bash

CURRENT_DIR=$(dirname "$0")
STACK_RESOURCE_FOLDER=${STACK_RESOURCE_FOLDER:=${CURRENT_DIR}/tmp}
CF_STACK_NAME=$(cat ${STACK_RESOURCE_FOLDER}/config.json | jq .stackName --raw-output)
AWS_REGION=$(cat ${STACK_RESOURCE_FOLDER}/config.json | jq .awsRegion --raw-output)
DEPLOY_BUCKET_NAME=$(cat ${STACK_RESOURCE_FOLDER}/config.json | jq .deployBucketName --raw-output)
WEBSITE_BUCKET_NAME=$(aws --region=${AWS_REGION} cloudformation describe-stack-resources --stack-name ${CF_STACK_NAME} \
| jq -r '.[][] | select(.LogicalResourceId=="dashboardS3Bucket") | .PhysicalResourceId')
CERT_ID=$(cat ${STACK_RESOURCE_FOLDER}/keys.json | jq .certificateId --raw-output)

echo "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"
echo "<<<<<<<< DELETE STACK <<<<<<<<"
echo "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"
echo

echo AWS CloudFormation stack name: ${CF_STACK_NAME}
echo AWS Website bucket name: ${WEBSITE_BUCKET_NAME}

aws --region ${AWS_REGION} iot update-certificate --certificate-id ${CERT_ID} --new-status INACTIVE 2> /dev/null
aws --region ${AWS_REGION} iot delete-certificate --certificate-id ${CERT_ID} 2> /dev/null

aws --region ${AWS_REGION} s3 rm s3://${DEPLOY_BUCKET_NAME} --recursive
aws --region ${AWS_REGION} s3api delete-bucket --bucket ${DEPLOY_BUCKET_NAME}

aws --region ${AWS_REGION} s3 rm s3://${WEBSITE_BUCKET_NAME} --recursive

aws --region ${AWS_REGION} cloudformation delete-stack --stack-name ${CF_STACK_NAME} || exit 1

echo "Waiting while stack deleting ..."

aws --region ${AWS_REGION} cloudformation wait stack-delete-complete --stack-name ${CF_STACK_NAME} || exit 1

rm -rf ${STACK_RESOURCE_FOLDER}

echo "<<<<<<<<<<<<<<<<<<<<<<<<<<"
echo "<<<<<<<< COMPLETE <<<<<<<<"
echo "<<<<<<<<<<<<<<<<<<<<<<<<<<"
echo