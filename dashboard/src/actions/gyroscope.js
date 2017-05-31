import { GYROSCOPE_UPDATE } from '../actionTypes';
import { fetch, push } from './common/dimensions';

export const gyroscopeFetch = fetch(GYROSCOPE_UPDATE, 'gyroscope');

export const gyroscopePush = push(GYROSCOPE_UPDATE, 'gyroscope');

