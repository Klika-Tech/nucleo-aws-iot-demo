import {
    MQTT_CONNECT,
    MQTT_CONNECTED,
    MQTT_DISCONNECTED,
} from '../actionTypes';

export function connect(url, reconnectPeriod) {
    return {
        type: MQTT_CONNECT,
        payload: {
            url,
            reconnectPeriod,
        },
    };
}

export function connected() {
    return {
        type: MQTT_CONNECTED,
    };
}

export function disconnected() {
    return {
        type: MQTT_DISCONNECTED,
    };
}
