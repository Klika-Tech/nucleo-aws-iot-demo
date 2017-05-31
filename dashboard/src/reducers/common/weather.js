import _ from 'lodash';

export function update(state, { sensorData, citiesData }) {
    const displayedCitiesIds = state.displayedCitiesData.map(d => d.cityId);
    const displayedCitiesData = state.citiesData.filter(d => _.includes(displayedCitiesIds, d.cityId));
    const markersData = _.filter(sensorData, d => d.marker);
    return {
        ...state,
        sensorData,
        citiesData,
        displayedCitiesData,
        markersData,
    };
}

export function changeType(state, { chartType }) {
    return {
        ...state,
        chartType,
    };
}

export function toggleVisibility(state, { value }) {
    const displayedCitiesIds = state.displayedCitiesData.map(d => d.cityId);
    let showFor;
    if (_.includes(displayedCitiesIds, value)) {
        showFor = _.without(displayedCitiesIds, value);
    } else {
        showFor = _.union(displayedCitiesIds, [value]);
    }
    const displayedCitiesData = state.citiesData.filter(d => _.includes(showFor, d.cityId));
    return {
        ...state,
        displayedCitiesData,
    };
}
