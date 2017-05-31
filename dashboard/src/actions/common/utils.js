import _ from 'lodash';

export function getActualData(data) {
    const currentTime = new Date();
    currentTime.setHours(currentTime.getHours() - 4); // sync with fetchAwsMetrics
    return _(data)
        .filter(item => item.date.getTime() >= currentTime.getTime())
        .value();
}
