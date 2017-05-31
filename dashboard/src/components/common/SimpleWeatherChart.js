import React, { Component, PropTypes } from 'react';
import * as d3 from 'd3';
import { scaleTime, scaleLinear } from 'd3-scale';
import './style.scss';
import Chart from './Chart';
import Axis from './Axis';
import Area from './Area';

const bisector = d3.bisector(d => d.date).right;

class SimpleWeatherChart extends Component {
    constructor(props) {
        super(props);
        this.x = scaleTime();
        this.y = scaleLinear();
        this.margin = { top: 0, right: 0, bottom: 60, left: 0 };
        this.getDefaultDomain = this.getDefaultDomain.bind(this);
        this.updateDomains = this.updateDomains.bind(this);
        this.updateDimension = this.updateDimension.bind(this);
        this.updateD3(props);
    }

    componentWillReceiveProps(newProps) {
        this.updateD3(newProps, this.props);
    }

    getDefaultDomain(last = 300000) { // for last 5 min
        const { contextDomain } = this;
        return [contextDomain[1] - last, contextDomain[1]];
    }

    updateD3(newProps, oldProps = {}) {
        const isDataChanged = oldProps.data !== newProps.data;
        const isSizeChanged = (oldProps.containerWidth !== newProps.containerWidth)
            || (oldProps.containerHeight !== newProps.containerHeight);
        if (isDataChanged) {
            this.updateDomains(newProps);
        }
        if (isSizeChanged) {
            this.updateDimension(newProps);
        }
    }

    updateDomains(props) {
        const { y, x, getDefaultDomain } = this;
        const { type, data, units } = props;
        this.contextDomain = d3.extent(data.map(d => d.date));
        const focusDomain = getDefaultDomain();
        this.focusData = data.slice(
            Math.max(0, bisector(data, focusDomain[0]) - 1),
            Math.min(data.length, bisector(data, focusDomain[1]) + 1),
        );
        const minY = d3.min(this.focusData.map(d => d[type][units.key]));
        const maxY = d3.max(this.focusData.map(d => d[type][units.key]));
        this.focusYDomain = [
            Math.floor((minY - 0.3) * 30) / 30,
            Math.ceil((maxY + 0.3) * 30) / 30,
        ];
        y.domain(this.focusYDomain);
        x.domain(focusDomain);
    }

    updateDimension(props) {
        const { x, y } = this;
        const { containerWidth, containerHeight } = props;
        this.width = containerWidth - this.margin.left - this.margin.right;
        this.height = containerHeight - this.margin.top - this.margin.bottom;
        y.range([this.height, 0]);
        x.range([0, this.width]);
    }

    render() {
        const { containerWidth, containerHeight, type, units } = this.props;
        const { margin, x, y, height, width, focusData, focusYDomain } = this;
        return (
            <div className="nucleo-chart-container">
                <div className="weather-chart">
                    <svg width={containerWidth} height={containerHeight}>
                        <defs>
                            <clipPath id="clip">
                                <rect width={width} height={height} />
                            </clipPath>
                        </defs>
                        <g
                            className="focus"
                            transform={`translate(${margin.left},${margin.top})`}
                        >
                            <g className="zoom">
                                <Area
                                    data={focusData}
                                    domain={focusYDomain}
                                    y0={d => y(focusYDomain[0])}
                                    y1={d => y(d[type][units.key])}
                                    x={d => x(d.date)}
                                />
                            </g>
                            <Axis
                                type="x"
                                scale={x}
                                data={focusData}
                                ticks={4}
                                translate={[0, height]}
                            />
                            <Axis
                                type="y"
                                scale={y}
                                data={focusData}
                                domain={focusYDomain}
                                tickSize={0}
                                ticks={4}
                                tickFormat={v => (`${y.tickFormat()(v)}${units.label}`)}
                            />
                        </g>
                    </svg>
                </div>
            </div>
        );
    }
}

SimpleWeatherChart.propTypes = {
    type: PropTypes.string,
    units: PropTypes.shape({
        key: PropTypes.string,
        label: PropTypes.string,
    }),
    data: PropTypes.arrayOf(PropTypes.shape({
        date: PropTypes.instanceOf(Date),
    })),
};

export default Chart(SimpleWeatherChart);
