import { getActualData } from './utils';

export function fetch(type, key) {
    const pdi = prepareDataItem(key);
    return (fullData) => {
        const items = _.get(fullData, 'sensorData', []);
        const data = items.map(pdi);
        return {
            type,
            payload: { data },
        };
    };
}

export function push(type, key) {
    const pdi = prepareDataItem(key);
    return (chunks, state) => {
        let data = _.get(state, 'data', []);
        data = data.concat(chunks.map(pdi));
        data = getActualData(data);
        return {
            type,
            payload: {
                data,
            },
        };
    };
}

function prepareDataItem(key) {
    return item => ({
        [key]: {
            x: item[key][0],
            y: item[key][1],
            z: item[key][2],
        },
        date: new Date(item.timestamp * 1000),
        marker: item.marker,
    });
}
