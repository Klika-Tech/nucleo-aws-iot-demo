import { HUMIDITY_UPDATE, HUMIDITY_CHANGE_CHART_TYRE, HUMIDITY_TOGGLE_VISIBILITY } from '../actionTypes';
import { changeChartType, toggleVisibility, prepareDataItem, push, fetch } from './common/weather';
import { PERCENTS } from './../scaleUnits';

export const humidityChangeChartType = changeChartType(HUMIDITY_CHANGE_CHART_TYRE);

export const humidityToggleVisibility = toggleVisibility(HUMIDITY_TOGGLE_VISIBILITY);

const pdi = prepareDataItem('humidity', PERCENTS);

export const humidityFetch = fetch(HUMIDITY_UPDATE, 'humidityData', pdi);

export const humidityPush = push(HUMIDITY_UPDATE, pdi);
