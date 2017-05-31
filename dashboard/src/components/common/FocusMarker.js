import React, { PropTypes } from 'react';

const FocusMarker = ({ x, y }) => (
    <circle className="marker" r="5" cx={x} cy={y} />
);

export default FocusMarker;
