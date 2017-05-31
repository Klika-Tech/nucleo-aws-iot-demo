import { createStore } from 'redux';
import reducers from './reducers';
import middlewares from './middlewares';

export default function (initialState = {}) {
    return createStore(reducers, initialState, middlewares);
}
