import React, { Component, PropTypes } from 'react';
import ReactDOM from 'react-dom';
import * as d3 from 'd3';
import { axisBottom, axisRight } from 'd3-axis';
import _ from 'lodash';

class Axis extends Component {
    constructor(props) {
        super(props);
        this.renderAxis = this.renderAxis.bind(this);
        this.axis = (props.type === 'x') ? axisBottom() : axisRight();
        this.axis.scale(props.scale);
        this.renderCounter = props.skipRenderCount;
    }

    componentDidMount() { this.renderAxis(); }

    shouldComponentUpdate(newProps, newState, nextContext) {
        const isSizeChanged = (this.context.containerWidth !== nextContext.containerWidth)
            || (this.context.containerHeight !== nextContext.containerHeight);
        const isDomainChanged = !_.isEqual(this.props.domain, newProps.domain);
        const isDataChanged = (this.props.data !== newProps.data);
        let shouldRender = true;
        if (newProps.skipRenderCount !== undefined) {
            shouldRender = this.renderCounter <= 0;
            if (shouldRender) {
                this.renderCounter = newProps.skipRenderCount;
            } else {
                this.renderCounter -= 1;
            }
        }
        return isSizeChanged || isDomainChanged || (isDataChanged && shouldRender);
    }

    componentWillUpdate() {
        this.renderAxis();
    }

    renderAxis() {
        const { axis } = this;
        const { type, tickSize, ticks, tickFormat } = this.props;
        const node = ReactDOM.findDOMNode(this);
        const g = d3.select(node);
        if (ticks) {
            axis.ticks(ticks);
        }
        if (tickSize) {
            axis.tickSize(tickSize);
        }
        if (tickFormat) {
            axis.tickFormat(tickFormat);
        }
        g.call(axis);
        if (type === 'y') {
            g.selectAll('text').attr('x', 4).attr('dy', -4);
        }
    }

    render() {
        const { type, translate } = this.props;
        return (
            <g
                className={`${type} axis`}
                transform={(translate) ? `translate(${translate.join(',')})` : null}
            />
        );
    }
}

Axis.propTypes = {
    data: PropTypes.array,
    type: PropTypes.oneOf(['x', 'y']),
    domain: PropTypes.arrayOf(PropTypes.number),
    scale: PropTypes.func,
    tickFormat: PropTypes.func,
    tickSize: PropTypes.number,
    ticks: PropTypes.number,
    translate: PropTypes.arrayOf(PropTypes.number),
    skipRenderCount: PropTypes.number,
};

Axis.contextTypes = {
    containerHeight: PropTypes.number,
    containerWidth: PropTypes.number,
};

Axis.defaultProps = {
    translateX: 0,
    translateY: 0,
};

export default Axis;
