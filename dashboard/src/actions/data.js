import _ from 'lodash';
import { batchActions } from 'redux-batched-actions';
import { DATA_FETCHED } from '../actionTypes';
import { accelerometerFetch, accelerometerPush } from './accelerometer';
import { gyroscopeFetch, gyroscopePush } from './gyroscope';
import { magnetometerFetch, magnetometerPush } from './magnetometer';
import { pressureFetch, pressurePush } from './pressure';
import { humidityFetch, humidityPush } from './humidity';
import { temperatureFetch, temperaturePush } from './temperature';

export function fetchData(data) {
    return (dispatch) => {
        const pd = prepareData(data);
        dispatch(batchActions([
            accelerometerFetch(pd),
            gyroscopeFetch(pd),
            magnetometerFetch(pd),
            pressureFetch(pd),
            humidityFetch(pd),
            temperatureFetch(pd),
            { type: DATA_FETCHED },
        ]));
    };
}

export function pushData(chunks) {
    return (dispatch, getState) => {
        const pds = chunks.map(prepareDataItem);
        const { accelerometer, gyroscope, magnetometer, pressure, humidity, temperature } = getState();
        dispatch(batchActions([
            accelerometerPush(pds, accelerometer),
            gyroscopePush(pds, gyroscope),
            magnetometerPush(pds, magnetometer),
            pressurePush(pds, pressure),
            humidityPush(pds, humidity),
            temperaturePush(pds, temperature),
        ]));
    };
}

function prepareData(data) {
    _.forEach(data.weatherData, (d) => {
        d.tempData = _.map(d.tempData, prepareDataItem);
        d.humidityData = _.map(d.humidityData, prepareDataItem);
        d.pressureData = _.map(d.pressureData, prepareDataItem);
    });
    return data;
}

function prepareDataItem(dataItem) {
    const result = {
        timestamp: dataItem.timestamp === undefined
            ? Math.round(Date.now() / 1000)
            : +dataItem.timestamp,
        marker: dataItem.marker,
    };

    if (dataItem.temperature !== undefined) result.temperature = parseFloat(dataItem.temperature);
    if (dataItem.humidity !== undefined) result.humidity = parseFloat(dataItem.humidity);
    if (dataItem.pressure !== undefined) result.pressure = parseFloat(dataItem.pressure);
    if (dataItem.magnetometer !== undefined) result.magnetometer = dataItem.magnetometer;
    if (dataItem.accelerometer !== undefined) result.accelerometer = dataItem.accelerometer;
    if (dataItem.gyroscope !== undefined) result.gyroscope = dataItem.gyroscope;

    return result;
}
