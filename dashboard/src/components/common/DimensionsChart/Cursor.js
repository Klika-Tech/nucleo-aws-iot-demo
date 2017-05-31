import React, { Component, PropTypes } from 'react';
import * as d3 from 'd3';
import Cursor from '../../common/Cursor';
import CursorMarker from '../../common/CursorMarker';
import CursorTooltip from '../../common/CursorTooltip';

function DimensionsCursor({ data, cursorVisible, cursorX, margin, height, width, y, units, type }) {
    return (
        <Cursor
            y={margin.top}
            x={cursorX}
            height={height}
            visible={cursorVisible}
        >
            <CursorMarker
                data={data}
                y={d => y(d[type].x)}
                marker={data ? data.marker : false}
            >
                <CursorTooltip cursorX={cursorX} containerWidth={width} marker={data ? data.marker : false}>
                    {label(type, 'x', data, units)}
                </CursorTooltip>
            </CursorMarker>

            <CursorMarker
                data={data}
                y={d => y(d[type].y)}
                marker={data ? data.marker : false}
            >
                <CursorTooltip cursorX={cursorX} containerWidth={width} marker={data ? data.marker : false}>
                    {label(type, 'y', data, units)}
                </CursorTooltip>
            </CursorMarker>

            <CursorMarker
                data={data}
                y={d => y(d[type].z)}
                marker={data ? data.marker : false}
            >
                <CursorTooltip cursorX={cursorX} containerWidth={width} marker={data ? data.marker : false}>
                    {label(type, 'z', data, units)}
                </CursorTooltip>
            </CursorMarker>

        </Cursor>
    );
}

function label(type, axis, data, units) {
    const timeFormat = d3.timeFormat('%X');
    if (data) {
        const axisLabel = axis.toUpperCase();
        const value = Math.round(data[type][axis] * 100) / 100;
        const date = timeFormat(data.date);
        return `${axisLabel}: ${value}${units.label} @ ${date}`;
    }
    return '';
}

DimensionsCursor.propTypes = {
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

export default DimensionsCursor;

