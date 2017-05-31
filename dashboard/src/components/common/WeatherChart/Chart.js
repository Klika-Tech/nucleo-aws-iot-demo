import React, { Component, PropTypes } from 'react';
import * as d3 from 'd3';
import { scaleTime, scaleLinear } from 'd3-scale';
import _ from 'lodash';
import '../../common/style.scss';
import Chart from '../../common/Chart';
import Axis from '../../common/Axis';
import Area from '../../common/Area';
import Line from '../../common/Line';
import Curve from '../../common/Curve';
import Focus from '../../common/Focus';
import BrushX from '../../common/BrushX';
import WeatherCursor from './Cursor';
import CityLabel from './CityLabel';
import FocusMarker from '../../common/FocusMarker';
import ContextMarker from '../../common/ContextMarker';
import { LINE_CHART, AREA_CHART } from '../../../chartTypes';

const bisector = d3.bisector(d => d.date).right;
const zoomVelocity = 3;

class WeatherChart extends Component {
    constructor(props) {
        super(props);
        this.x = scaleTime();
        this.y = scaleLinear();
        this.x2 = scaleTime();
        this.y2 = scaleLinear();
        this.margin = { top: 0, right: 10, bottom: 100, left: 0 };
        this.margin2 = { right: 10, bottom: 20, left: 0 };
        this.state = {
            brush: false,
            selection: null,
            cursorData: null,
            cursorX: 0,
            cursorVisible: false,
        };
        this.getDefaultFocusDomain = this.getDefaultFocusDomain.bind(this);
        this.getCursorState = this.getCursorState.bind(this);
        this.calculateYDomain = this.calculateYDomain.bind(this);
        this.handleWheel = this.handleWheel.bind(this);
        this.handleMouseMove = this.handleMouseMove.bind(this);
        this.handleMouseOut = this.handleMouseOut.bind(this);
        this.handleBrushMount = this.handleBrushMount.bind(this);
        this.handleBrushStart = this.handleBrushStart.bind(this);
        this.handleBrush = this.handleBrush.bind(this);
        this.handleBrushEnd = this.handleBrushEnd.bind(this);
        this.updateContextDomains = this.updateContextDomains.bind(this);
        this.updateFocusDomains = this.updateFocusDomains.bind(this);
        this.updateCursor = this.updateCursor.bind(this);
        this.updateDimension = this.updateDimension.bind(this);
        this.updateD3(props);
    }

    componentWillReceiveProps(newProps) {
        this.updateD3(newProps, this.props);
    }

    getDefaultFocusDomain(last = 300000) { // for last 5 min
        const { contextDomain } = this;
        return [contextDomain[1] - last, contextDomain[1]];
    }

    getCursorState(props, xPos) {
        const { x } = this;
        const { data } = props;
        const datePos = x.invert(xPos);
        const index = d3.bisector(d => d.date).right(data, datePos);
        return {
            cursorData: data[index],
            cursorX: xPos,
        };
    }

    calculateYDomain(data) {
        const { type, units } = this.props;
        const minY = d3.min(data.map(d => d[type][units.key]));
        const maxY = d3.max(data.map(d => d[type][units.key]));
        return [
            Math.floor((minY - 0.3) * 30) / 30,
            Math.ceil((maxY + 0.3) * 30) / 30,
        ];
    }

    updateD3(newProps, oldProps = {}) {
        const state = this.state;
        const isDataChanged = oldProps.data !== newProps.data;
        const isSizeChanged = (oldProps.containerWidth !== newProps.containerWidth)
            || (oldProps.containerHeight !== newProps.containerHeight);
        const isBrush = state.brush;
        const isCursorVisible = state.cursorVisible;
        const isCitiesChanged = oldProps.citiesData !== newProps.citiesData;
        const isUnitsChanged = oldProps.units !== newProps.units;
        if (isDataChanged || isUnitsChanged) {
            this.updateContextDomains(newProps, state);
        }
        if (isBrush || isDataChanged || isCitiesChanged || isUnitsChanged) {
            this.updateFocusDomains(newProps, state);
        }
        if (isSizeChanged) {
            this.updateDimension(newProps, state);
        }
        if (!isBrush && isCursorVisible) {
            this.updateCursor(newProps, state);
        }
    }

    updateContextDomains(props) {
        const { x2, y2 } = this;
        const { data } = props;
        this.contextDomain = d3.extent(data.map(d => d.date));
        x2.domain(this.contextDomain);
        this.contextYDomain = this.calculateYDomain(data);
        y2.domain(this.contextYDomain);
    }

    updateFocusDomains(props, state = {}) {
        const { y, x, x2, getDefaultFocusDomain } = this;
        const { selection } = state;
        const { type, data, citiesData, units } = props;
        const fd = (selection) ? (selection.map(x2.invert)) : getDefaultFocusDomain();
        const [minX, maxX] = fd;
        this.focusData = data.slice(
            Math.max(0, bisector(data, minX) - 1),
            Math.min(data.length, bisector(data, maxX) + 1),
        );
        this.focusDomain = d3.extent(this.focusData.map(d => d.date));
        this.defaultSelection = this.focusDomain.map(d => x2(d));
        x.domain(this.focusDomain);
        // ------------------ //
        citiesData.forEach((i) => {
            i.focus = {
                data: i.data.slice(
                    Math.max(0, bisector(i.data, minX) - 1),
                    Math.min(i.data.length, bisector(i.data, maxX) + 1),
                ),
            };
            if (i.focus.data.length >= 2) {
                const last = i.focus.data[i.focus.data.length - 1];
                const preLast = i.focus.data[i.focus.data.length - 2];
                const diffValue = last[type][units.key] - preLast[type][units.key];
                const dxDomainTime = this.focusDomain[1] - preLast.date;
                let diffTime = last.date - preLast.date;
                diffTime = diffTime || 1;
                i.focus.labelYData = preLast[type][units.key] + (dxDomainTime * diffValue / diffTime);
            }
        });
        const dataUnion = _(citiesData)
            .map(d => d.focus.data)
            .push(this.focusData)
            .flatten()
            .value();
        this.focusYDomain = this.calculateYDomain(dataUnion);
        y.domain(this.focusYDomain);
    }

    updateCursor(props, state = {}) {
        const { cursorX } = state;
        if (cursorX) {
            const cs = this.getCursorState(props, cursorX);
            this.state = {
                ...state,
                ...cs,
                cursorX,
            };
        }
    }

    updateDimension(props) {
        const { x, y, x2, y2 } = this;
        const { containerWidth, containerHeight } = props;
        this.margin2.top = containerHeight - 70;
        this.width = containerWidth - this.margin.left - this.margin.right;
        this.height = containerHeight - this.margin.top - this.margin.bottom;
        this.height2 = containerHeight - this.margin2.top - this.margin2.bottom;
        y.range([this.height, 0]);
        x.range([0, this.width]);
        x2.range([0, this.width]);
        y2.range([this.height2, 0]);
    }

    handleWheel(deltaY) {
        const { state, width, moveBrush } = this;
        if (!moveBrush) return;

        const selection = state.selection ? state.selection : this.defaultSelection;
        const isZoomIn = deltaY < 0;

        const zoomedSelection = (isZoomIn)
            ? [selection[0] - zoomVelocity, selection[1] + zoomVelocity]
            : [selection[0] + zoomVelocity, selection[1] - zoomVelocity];

        let validatedSelection = [ // extra zoom out
            ((zoomedSelection[0] < 0) ? 0 : zoomedSelection[0]),
            ((zoomedSelection[1] > width) ? width : zoomedSelection[1]),
        ];

        if (validatedSelection[0] >= validatedSelection[1]) { // extra zoom in
            validatedSelection = [validatedSelection[0], validatedSelection[0]];
        }

        moveBrush.call({}, validatedSelection);
    }

    handleMouseMove(xPos) {
        const { getCursorState, props } = this;
        const cs = getCursorState(props, xPos);
        this.setState({
            ...cs,
            cursorVisible: true,
        });
    }

    handleMouseOut() {
        this.setState({
            cursorVisible: false,
        });
    }

    handleBrushStart(selection) {
        this.setState({
            brush: true,
            selection,
        });
    }

    handleBrush(selection) {
        this.setState({
            selection,
        });
    }

    handleBrushEnd(selection) {
        this.setState({
            brush: false,
            selection,
        });
    }

    handleBrushMount({ moveBrush }) {
        this.moveBrush = moveBrush;
    }

    render() {
        const { containerWidth, containerHeight, data, markersData, citiesData, type, units, chartType } = this.props;
        const {
            margin, margin2, x, y, x2, y2,
            focusData, focusYDomain, contextYDomain,
            height, height2, width, state,
        } = this;
        return (
            <div>
                <svg width={containerWidth} height={containerHeight}>
                    <defs>
                        <clipPath id="clip">
                            <rect width={width} height={height} />
                        </clipPath>
                    </defs>
                    <Focus
                        margin={margin}
                        height={height}
                        width={width}
                        onMouseMove={_.throttle(this.handleMouseMove, 100)}
                        onMouseOut={this.handleMouseOut}
                        onWheel={this.handleWheel}
                    >
                        <g className="zoom">
                            {chartType === AREA_CHART && (
                                <Area
                                    data={focusData}
                                    domain={focusYDomain}
                                    y0={d => y(focusYDomain[0])}
                                    y1={d => y(d[type][units.key])}
                                    x={d => x(d.date)}
                                />
                            )}
                            {chartType === LINE_CHART && (
                                <Line
                                    data={focusData}
                                    domain={focusYDomain}
                                    y={d => y(d[type][units.key])}
                                    x={d => x(d.date)}
                                />
                            )}
                            {citiesData.map(city => (
                                <Curve
                                    key={city.cityId}
                                    className={`city-${city.cityId}`}
                                    data={city.focus.data}
                                    domain={focusYDomain}
                                    x={d => x(d.date)}
                                    y={d => y(d[type][units.key])}
                                />
                            ))}
                            {markersData.map(d => (
                                <FocusMarker
                                    key={d.date.toISOString()}
                                    y={y(d[type][units.key])}
                                    x={x(d.date)}
                                />
                            ))}
                        </g>
                        <Axis
                            type="x"
                            scale={x}
                            data={focusData}
                            translate={[0, height]}
                        />
                        <Axis
                            type="y"
                            scale={y}
                            data={focusData}
                            domain={focusYDomain}
                            tickSize={width}
                            tickFormat={v => (`${y.tickFormat()(v)}${units.label}`)}
                        />
                    </Focus>
                    <g className="context" transform={`translate(${margin2.left},${margin2.top})`}>
                        <BrushX
                            width={width}
                            height={height2}
                            onBrushMount={this.handleBrushMount}
                            onBrushStart={this.handleBrushStart}
                            onBrush={this.handleBrush}
                            onBrushEnd={this.handleBrushEnd}
                        >
                            <Area
                                data={data}
                                domain={contextYDomain}
                                y0={d => y2(contextYDomain[0])}
                                y1={d => y2(d[type][units.key])}
                                x={d => x2(d.date)}
                            />
                            {markersData.map(d => (
                                <ContextMarker
                                    key={d.date.toISOString()}
                                    y1={y2(contextYDomain[0])}
                                    y2={y2(contextYDomain[1])}
                                    x={x2(d.date)}
                                />
                            ))}
                        </BrushX>
                        <Axis
                            type="x"
                            data={data}
                            scale={this.x2}
                            translate={[0, height2]}
                            skipRenderCount={10}
                        />
                    </g>
                </svg>
                <WeatherCursor
                    type={type}
                    units={units}
                    data={state.cursorData}
                    cursorX={state.cursorX}
                    cursorVisible={state.cursorVisible}
                    margin={margin}
                    height={height}
                    width={width}
                    y={y}
                />
                {citiesData.map(city => (
                    <CityLabel
                        key={city.cityId}
                        data={city}
                        margin={margin}
                        top={y(city.focus.labelYData)}
                    />
                ))}
            </div>
        );
    }
}

WeatherChart.propTypes = {
    type: PropTypes.string,
    chartType: PropTypes.string,
    units: PropTypes.shape({
        key: PropTypes.string,
        label: PropTypes.string,
    }),
    data: PropTypes.arrayOf(PropTypes.shape({
        date: PropTypes.instanceOf(Date),
    })),
    markersData: PropTypes.arrayOf(PropTypes.shape({
        date: PropTypes.instanceOf(Date),
    })),
    citiesData: PropTypes.arrayOf(PropTypes.shape({
        cityId: PropTypes.number,
        cityName: PropTypes.string,
    })),
};

export default Chart(WeatherChart);
