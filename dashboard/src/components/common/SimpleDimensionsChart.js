import React, { Component, PropTypes } from 'react';
import * as d3 from 'd3';
import { scaleTime, scaleLinear } from 'd3-scale';
import './style.scss';
import Chart from './Chart';
import Axis from './Axis';
import Line from './Line';

class SimpleDimensionsChart extends Component {
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
        const { type, data } = props;
        this.contextDomain = d3.extent(data.map(d => d.date));
        const minY = d3.min(data.map(
            d => Math.min(d[type].x, d[type].y, d[type].z)));
        const maxY = d3.max(data.map(
            d => Math.max(d[type].x, d[type].y, d[type].z)));
        this.focusYDomain = [
            Math.floor((minY - 0.3) * 30) / 30,
            Math.ceil((maxY + 0.3) * 30) / 30,
        ];
        y.domain(this.focusYDomain);
        x.domain(getDefaultDomain());
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
        const { containerWidth, containerHeight, data, type, units } = this.props;
        const { margin, x, y, height, width } = this;
        return (
            <div className="nucleo-chart-container">
                <div className="dimensions-chart">
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
                                <Line
                                    className="x"
                                    data={data}
                                    x={d => x(d.date)}
                                    y={d => y(d[type].x)}
                                />
                                <Line
                                    className="y"
                                    data={data}
                                    x={d => x(d.date)}
                                    y={d => y(d[type].y)}
                                />
                                <Line
                                    className="z"
                                    data={data}
                                    x={d => x(d.date)}
                                    y={d => y(d[type].z)}
                                />
                            </g>
                            <Axis
                                type="x"
                                scale={x}
                                data={data}
                                ticks={4}
                                translate={[0, height]}
                            />
                            <Axis
                                type="y"
                                scale={y}
                                data={data}
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

SimpleDimensionsChart.propTypes = {
    type: PropTypes.string,
    units: PropTypes.shape({
        key: PropTypes.string,
        label: PropTypes.string,
    }),
    data: PropTypes.arrayOf(PropTypes.shape({
        date: PropTypes.instanceOf(Date),
    })),
};

export default Chart(SimpleDimensionsChart);
