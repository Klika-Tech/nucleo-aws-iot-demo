import 'whatwg-fetch';

/**
 * Load aws metrics for the last 4h
 * @param apiUrl
 * @return {*|Promise.<Object>}
 */
export function fetchAwsMetrics({ apiUrl }) {
    const since = Math.round(Date.now() / 1000) - 14400;
    return fetch(`${apiUrl}getNucleoData?metric=temperature&since=${since}`)
        .then(response => response.json());
}
