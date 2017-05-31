import mqtt from 'mqtt';
import { MQTT_CONNECT } from '../actionTypes';
import { connect, connected, disconnected } from '../actions/mqtt';
import { pushData } from '../actions/data';
import config from '../config';

const mqttMiddleware = (function () {
    let url = null;
    let reconnectPeriod = null;
    let client = null;
    let interval = null;

    return (store) => {
        let messages = [];
        if (interval) clearInterval(interval);
        interval = setInterval(() => {
            if (messages.length) {
                if (config.debug) console.log('MQTT: dispatch batch update');
                store.dispatch(pushData(messages));
                messages = [];
            }
        }, 1000);
        return next => (action) => {
            switch (action.type) {
            case MQTT_CONNECT:
                if (client != null) {
                    client.end();
                }
                url = action.payload.url;
                reconnectPeriod = action.payload.reconnectPeriod;
                client = mqtt.connect(url, { reconnectPeriod });
                client.on('connect', () => {
                    if (config.debug) console.log('MQTT: client connected');
                    store.dispatch(connected());
                    client.subscribe(config.mqttTopic);
                    setTimeout(() => {
                        client.end();
                    }, 270000); // 4.5 minutes
                });
                client.on('close', () => {
                    if (config.debug) console.log('MQTT: client disconnected');
                    client.end();
                    store.dispatch(disconnected());
                });
                client.on('message', (topic, msg) => {
                    const message = msg.toString();
                    if (config.debug) console.info('MQTT: Message recieved.\nTopic: %s\nPayload: %s', topic, message);
                    if (topic === config.mqttTopic) {
                        messages.push(JSON.parse(message));
                    }
                });
                break;
            default:
                return next(action);
            }
        };
    };
}());

export default mqttMiddleware;
