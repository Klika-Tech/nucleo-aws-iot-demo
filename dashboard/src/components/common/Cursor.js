import React, { PropTypes } from 'react';

const CURSOR_OFFSET = 2;

const Cursor = ({ children, x, y, height, visible }) => {
    const style = {
        top: `${y}px`,
        left: `${x + CURSOR_OFFSET}px`,
        height: `${height}px`,
        display: (visible) ? 'block' : 'none',
    };
    return (
        <div className="cursor-focus" style={style}>
            {children}
        </div>
    );
};

Cursor.propTypes = {
    x: PropTypes.number,
    y: PropTypes.number,
    height: PropTypes.number,
    visible: PropTypes.bool,
};

export default Cursor;
