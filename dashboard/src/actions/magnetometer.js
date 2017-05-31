import { MAGNETOMETER_UPDATE } from '../actionTypes';
import { fetch, push } from './common/dimensions';

export const magnetometerFetch = fetch(MAGNETOMETER_UPDATE, 'magnetometer');

export const magnetometerPush = push(MAGNETOMETER_UPDATE, 'magnetometer');
