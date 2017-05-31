import React, { Component, PropTypes } from 'react';
import * as d3 from 'd3';
import _ from 'lodash';

class Curve extends Component {
    constructor(props) {
        super(props);
        this.generator = getGenerator(props);
    }

    shouldComponentUpdate(newProps, newState, nextContext) {
        const isSizeChanged = (this.context.containerWidth !== nextContext.containerWidth)
            || (this.context.containerHeight !== nextContext.containerHeight);
        const isDomainChanged = !_.isEqual(this.props.domain, newProps.domain);
        const isDataChanged = (this.props.data !== newProps.data);
        if (isDomainChanged) {
            this.generator = getGenerator(newProps);
        }
        return isSizeChanged || isDomainChanged || isDataChanged;
    }

    render() {
        return (
            <path className={`line ${this.props.className || ''}`} d={this.generator(this.props.data)} />
        );
    }
}

Curve.propTypes = {
    data: PropTypes.array,
    domain: PropTypes.arrayOf(PropTypes.number),
    x: PropTypes.func,
    y: PropTypes.func,
    className: PropTypes.string,
};

Curve.contextTypes = {
    containerHeight: PropTypes.number,
    containerWidth: PropTypes.number,
};


function getGenerator(props) {
    return d3.line()
        .curve(d3.curveBasis)
        .y(props.y)
        .x(props.x);
}


export default Curve;
