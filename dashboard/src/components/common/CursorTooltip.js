import React, { PropTypes } from 'react';

const offsetX = 15;
const offsetY = 20;

const CursorTooltip = ({ children, cursorX, containerWidth, marker }) => {
    const isLeft = cursorX - containerWidth < -200;
    const style = {
        left: (isLeft) ? `${offsetX}px` : null,
        right: (!isLeft) ? `${offsetX}px` : null,
        top: `${-offsetY}px`,
    };
    return (
        <div className={`cursor-tooltip ${marker ? 'marker' : ''}`} style={style}>{children}</div>
    );
};

CursorTooltip.propTypes = {
    cursorX: PropTypes.number,
    width: PropTypes.number,
    containerWidth: PropTypes.number,
    marker: PropTypes.bool,
};

export default CursorTooltip;
