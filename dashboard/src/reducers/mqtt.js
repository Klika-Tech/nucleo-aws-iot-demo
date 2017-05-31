import {
    MQTT_CONNECTED,
    MQTT_DISCONNECTED,
} from '../actionTypes';

export default function (state = {}, { type }) {
    switch (type) {
    case MQTT_CONNECTED: {
        return { connected: true };
    }
    case MQTT_DISCONNECTED: {
        return { connected: false };
    }
    default:
        return state;
    }
}
