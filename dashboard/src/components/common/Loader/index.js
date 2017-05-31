import React from 'react';
import './style.scss';

const oval = require('./oval.svg');

const Loader = () => (
    <svg>
        <use xlinkHref={oval} />
    </svg>
);

export default Loader;
