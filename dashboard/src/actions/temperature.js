import {
    TEMPERATURE_UPDATE,
    TEMPERATURE_CHANGE_CHART_TYRE,
    TEMPERATURE_TOGGLE_VISIBILITY,
    TEMPERATURE_CHANGE_UNITS,
} from '../actionTypes';
import { changeChartType, toggleVisibility, push, fetch } from './common/weather';
import { CELSIUS, FAHRENHEITS } from './../scaleUnits';

export const temperatureChangeChartType = changeChartType(TEMPERATURE_CHANGE_CHART_TYRE);

export const temperatureToggleVisibility = toggleVisibility(TEMPERATURE_TOGGLE_VISIBILITY);

export const temperatureChangeUnits = units => ({
    type: TEMPERATURE_CHANGE_UNITS,
    payload: {
        units,
    },
});

const pdi = item => ({
    temperature: {
        [CELSIUS.key]: item.temperature,
        [FAHRENHEITS.key]: item.temperature * 9 / 5 + 32,
    },
    date: new Date(item.timestamp * 1000),
    marker: item.marker,
});

export const temperatureFetch = fetch(TEMPERATURE_UPDATE, 'tempData', pdi);

export const temperaturePush = push(TEMPERATURE_UPDATE, pdi);

