import { update, changeType, toggleVisibility } from './common/weather';
import {
    TEMPERATURE_UPDATE,
    TEMPERATURE_CHANGE_CHART_TYRE,
    TEMPERATURE_TOGGLE_VISIBILITY,
    TEMPERATURE_CHANGE_UNITS,
} from '../actionTypes';
import { AREA_CHART } from '../chartTypes';
import { CELSIUS } from '../scaleUnits';

export default function (state = {
    chartType: AREA_CHART,
    citiesData: [],
    displayedCitiesData: [],
    units: CELSIUS,
}, { type, payload }) {
    switch (type) {
    case TEMPERATURE_UPDATE: {
        return update(state, payload);
    }
    case TEMPERATURE_CHANGE_CHART_TYRE: {
        return changeType(state, payload);
    }
    case TEMPERATURE_TOGGLE_VISIBILITY: {
        return toggleVisibility(state, payload);
    }
    case TEMPERATURE_CHANGE_UNITS: {
        return changeUnits(state, payload);
    }
    default:
        return state;
    }
}

function changeUnits(state, { units }) {
    return {
        ...state,
        units,
    };
}
