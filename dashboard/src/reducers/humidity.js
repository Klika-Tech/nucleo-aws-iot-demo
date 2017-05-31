import { update, changeType, toggleVisibility } from './common/weather';
import { HUMIDITY_UPDATE, HUMIDITY_CHANGE_CHART_TYRE, HUMIDITY_TOGGLE_VISIBILITY } from '../actionTypes';
import { AREA_CHART } from '../chartTypes';

export default function (state = {
    chartType: AREA_CHART, citiesData: [], displayedCitiesData: [],
}, { type, payload }) {
    switch (type) {
    case HUMIDITY_UPDATE: {
        return update(state, payload);
    }
    case HUMIDITY_CHANGE_CHART_TYRE: {
        return changeType(state, payload);
    }
    case HUMIDITY_TOGGLE_VISIBILITY: {
        return toggleVisibility(state, payload);
    }
    default:
        return state;
    }
}
