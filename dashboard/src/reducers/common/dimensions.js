import _ from 'lodash';

export function update(state, { data }) {
    const markersData = _.filter(data, d => d.marker);
    return {
        ...state,
        data,
        markersData,
    };
}
