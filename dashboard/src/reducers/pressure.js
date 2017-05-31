import { update, changeType, toggleVisibility } from './common/weather';
import { PRESSURE_UPDATE, PRESSURE_CHANGE_CHART_TYRE, PRESSURE_TOGGLE_VISIBILITY } from '../actionTypes';
import { AREA_CHART } from '../chartTypes';

export default function (state = {
    chartType: AREA_CHART, citiesData: [], displayedCitiesData: [],
}, { type, payload }) {
    switch (type) {
    case PRESSURE_UPDATE: {
        return update(state, payload);
    }
    case PRESSURE_CHANGE_CHART_TYRE: {
        return changeType(state, payload);
    }
    case PRESSURE_TOGGLE_VISIBILITY: {
        return toggleVisibility(state, payload);
    }
    default:
        return state;
    }
}
