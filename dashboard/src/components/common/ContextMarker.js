import React, { PropTypes } from 'react';

const ContextMarker = ({ x, y1, y2 }) => (
    <line className="context-marker" x1={x} y1={y1} x2={x} y2={y2} />
);

export default ContextMarker;
