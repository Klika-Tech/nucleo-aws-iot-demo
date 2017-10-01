#!/usr/bin/env bash

ZIP_NAME=$1
LAMBDA_FILE=$2

echo Zipping ${LAMBDA_FILE}

zip -j ${ZIP_NAME}.zip ${LAMBDA_FILE}