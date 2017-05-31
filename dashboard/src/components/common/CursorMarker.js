import React, { PropTypes } from 'react';

const MARKER_SIZE = 10;

const CursorMarker = ({ data, y, children, marker }) => {
    let top = (data) ? y(data) : 0;
    top -= (MARKER_SIZE / 2);
    const style = {
        left: `${-(MARKER_SIZE / 2)}px`,
        top: `${top}px`,
    };
    return (
        <div className={`cursor-point ${marker ? 'marker' : ''}`} style={style}>{children}</div>
    );
};

CursorMarker.propTypes = {
    data: PropTypes.object,
    y: PropTypes.func,
    marker: PropTypes.bool,
};

export default CursorMarker;
