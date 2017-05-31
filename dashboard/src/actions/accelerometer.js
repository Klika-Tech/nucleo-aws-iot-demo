import { ACCELEROMETER_UPDATE } from '../actionTypes';
import { fetch, push } from './common/dimensions';

export const accelerometerFetch = fetch(ACCELEROMETER_UPDATE, 'accelerometer');

export const accelerometerPush = push(ACCELEROMETER_UPDATE, 'accelerometer');
