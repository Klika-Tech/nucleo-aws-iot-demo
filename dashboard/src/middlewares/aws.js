import AWS from 'exports?AWS!aws-sdk/dist/aws-sdk';
import { AWS_CONNECT, MQTT_DISCONNECTED } from '../actionTypes';
import * as FetchService from '../services/fetch';
import config from '../config';
import { fetchData } from '../actions/data';
import * as SigV4Utils from '../services/awsSign';
import { connect } from '../actions/mqtt';

const awsMiddleware = store => next => (action) => {
    switch (action.type) {
    case AWS_CONNECT:
        FetchService.fetchAwsMetrics(config).then((data) => {
            store.dispatch(fetchData(data));
            identifyAndConnect(store.dispatch, config);
        });
        break;
    case MQTT_DISCONNECTED:
        setTimeout(() => {
            if (config.debug) console.log('MQTT: Reconnect...');
            identifyAndConnect(store.dispatch, config);
        }, 1000);
        break;
    default:
        return next(action);
    }
};

export default awsMiddleware;

function identifyAndConnect(dispatch, { awsRegion, cognitoIdentityPool, iotEndpoint }) {
    AWS.config.region = awsRegion;
    const awsCreds = new AWS.CognitoIdentityCredentials({
        IdentityPoolId: cognitoIdentityPool,
    });
    awsCreds.get((err) => {
        if (err) {
            console.error('[AWS]: ', err);
            return;
        }
        const url = SigV4Utils.getSignedUrl(
            'wss',
            iotEndpoint,
            '/mqtt',
            'iotdevicegateway',
            awsRegion,
            awsCreds.accessKeyId,
            awsCreds.secretAccessKey,
            awsCreds.sessionToken,
        );
        dispatch(connect(url, 2147483647));
    });
}
