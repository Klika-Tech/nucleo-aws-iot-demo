import { ACCELEROMETER_UPDATE } from '../actionTypes';
import { update } from './common/dimensions';

export default function (state = {}, { type, payload }) {
    switch (type) {
    case ACCELEROMETER_UPDATE: {
        return update(state, payload);
    }
    default:
        return state;
    }
}
