import { PRESSURE_UPDATE, PRESSURE_CHANGE_CHART_TYRE, PRESSURE_TOGGLE_VISIBILITY } from '../actionTypes';
import { changeChartType, toggleVisibility, prepareDataItem, push, fetch } from './common/weather';
import { HYPER_PASCALS } from './../scaleUnits';

export const pressureChangeChartType = changeChartType(PRESSURE_CHANGE_CHART_TYRE);

export const pressureToggleVisibility = toggleVisibility(PRESSURE_TOGGLE_VISIBILITY);

const pdi = prepareDataItem('pressure', HYPER_PASCALS);

export const pressureFetch = fetch(PRESSURE_UPDATE, 'pressureData', pdi);

export const pressurePush = push(PRESSURE_UPDATE, pdi);
