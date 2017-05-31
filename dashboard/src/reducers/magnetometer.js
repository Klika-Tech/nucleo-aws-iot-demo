import { MAGNETOMETER_UPDATE } from '../actionTypes';
import { update } from './common/dimensions';

export default function (state = {}, { type, payload }) {
    switch (type) {
    case MAGNETOMETER_UPDATE: {
        return update(state, payload);
    }
    default:
        return state;
    }
}
