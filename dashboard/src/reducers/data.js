import { DATA_FETCHED } from '../actionTypes';

export default function (state = {}, { type }) {
    switch (type) {
    case DATA_FETCHED: {
        return { fetched: true };
    }
    default:
        return state;
    }
}
