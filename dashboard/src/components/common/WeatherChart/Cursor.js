import React, { Component, PropTypes } from 'react';
import * as d3 from 'd3';
import Cursor from '../../common/Cursor';
import CursorMarker from '../../common/CursorMarker';
import CursorTooltip from '../../common/CursorTooltip';

function WeatherCursor({ data, cursorVisible, cursorX, margin, height, width, y, units, type }) {
    return (
        <Cursor
            y={margin.top}
            x={cursorX}
            height={height}
            visible={cursorVisible}
        >
            <CursorMarker
                data={data}
                y={d => y(d[type][units.key])}
                marker={data ? data.marker : false}
            >
                <CursorTooltip
                    cursorX={cursorX}
                    containerWidth={width}
                    width={190}
                    marker={data ? data.marker : false}
                >
                    {label(type, data, units)}
                </CursorTooltip>

            </CursorMarker>

        </Cursor>
    );
}

function label(type, data, units) {
    const timeFormat = d3.timeFormat('%X');
    if (data) {
        const value = Math.round(data[type][units.key] * 100) / 100;
        const date = timeFormat(data.date);
        return `${value}${units.label} @ ${date}`;
    }
    return '';
}

WeatherCursor.propTypes = {
    type: PropTypes.string,
    units: PropTypes.shape({
        key: PropTypes.string,
        label: PropTypes.string,
    }),
    data: PropTypes.shape({
        date: PropTypes.instanceOf(Date),
    }),
    cursorX: PropTypes.number,
    cursorVisible: PropTypes.bool,
    margin: PropTypes.shape({
        left: PropTypes.number,
        top: PropTypes.number,
        right: PropTypes.number,
        bottom: PropTypes.number,
    }),
    height: PropTypes.number,
    width: PropTypes.number,
    y: PropTypes.func,
};

export default WeatherCursor;

