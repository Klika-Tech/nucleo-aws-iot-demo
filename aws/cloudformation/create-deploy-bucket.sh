#!/usr/bin/env bash

CURRENT_DIR=$(dirname "$0")
STACK_RESOURCE_FOLDER=${STACK_RESOURCE_FOLDER:=${CURRENT_DIR}/tmp}
AWS_REGION=$(cat ${STACK_RESOURCE_FOLDER}/config.json | jq .awsRegion --raw-output)
DEPLOY_BUCKET_NAME=$(cat ${STACK_RESOURCE_FOLDER}/config.json | jq .deployBucketName --raw-output)

echo ">>>>>>>> Create Deploy Bucket >>>>>>>>"

echo AWS S3 deploy bucket name: ${DEPLOY_BUCKET_NAME}
echo AWS S3 region: ${AWS_REGION}

# create deploy bucket
if [ ${AWS_REGION} = "us-east-1" ];
then
    aws s3api create-bucket \
    --bucket ${DEPLOY_BUCKET_NAME} \
    --region ${AWS_REGION} 2> /dev/null
else
    aws s3api create-bucket \
    --bucket ${DEPLOY_BUCKET_NAME} \
    --region ${AWS_REGION} \
    --create-bucket-configuration LocationConstraint=${AWS_REGION} 2> /dev/null
fi

# zip & upload lambdas
for LAMBDA_DIR in $(find ${CURRENT_DIR}/lambdas -mindepth 1 -maxdepth 1 -type d);
do
    echo ${LAMBDA_DIR}
    LAMBDA_NAME=$(basename ${LAMBDA_DIR})
    ./zip-lambda.sh ${LAMBDA_NAME} ${LAMBDA_DIR}/index.js
    aws s3 cp ${LAMBDA_NAME}.zip s3://${DEPLOY_BUCKET_NAME} --region ${AWS_REGION}
    rm -f ${LAMBDA_NAME}.zip
done
