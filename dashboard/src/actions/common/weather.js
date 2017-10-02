import _ from 'lodash';
import { getActualData } from '../common/utils';

export const changeChartType = actionType => chartType => ({
    type: actionType,
    payload: {
        chartType,
    },
});

export const toggleVisibility = actionType => value => ({
    type: actionType,
    payload: {
        value,
    },
});

export function getUpdatedCitiesDataStub(citiesData, sensorData) {
    citiesData.forEach((d) => {
        const last = _.last(d.data);
        last.date = _.last(sensorData).date;
        d.data.push(last);
        d.data = getActualData(d.data);
    });
    return citiesData;
}


export function prepareDataItem(type, scaleUnit) {
    return item => ({
        [type]: {
            [scaleUnit.key]: item[type],
        },
        date: new Date(item.timestamp * 1000),
        marker: item.marker,
    });
}

export const fetch = (actionType, type, pdi) => (fullData) => {
    const sensorData = _.get(fullData, 'sensorData', []).map(pdi);
    const citiesData = _.get(fullData, 'citiesData', []).map(d => ({
        cityId: d.cityId,
        cityName: d.cityName,
        data: d[type].map(pdi),
    }));
    return {
        type: actionType,
        payload: {
            sensorData,
            citiesData,
        },
    };
};


export const push = (actionType, pdi) => (chunks, state) => {
    let sensorData = state.sensorData;
    sensorData = sensorData.concat(chunks.map(pdi));
    sensorData = getActualData(sensorData);
    const citiesData = getUpdatedCitiesDataStub(state.citiesData, sensorData);
    return {
        type: actionType,
        payload: {
            sensorData,
            citiesData,
        },
    };
};

