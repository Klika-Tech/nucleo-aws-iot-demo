import React, { Component, PropTypes } from 'react';
import * as d3 from 'd3';
import _ from 'lodash';

class Area extends Component {
    constructor(props) {
        super(props);
        this.generator = getGenerator(props);
        this.renderCounter = props.skipRenderCount;
    }

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
        if (isDomainChanged) {
            this.generator = getGenerator(newProps);
        }
        return isSizeChanged || isDomainChanged || (isDataChanged && shouldRender);
    }

    render() {
        return (
            <path className="area" d={this.generator(this.props.data)} />
        );
    }
}

Area.propTypes = {
    data: PropTypes.array,
    domain: PropTypes.arrayOf(PropTypes.number),
    y0: PropTypes.func,
    y1: PropTypes.func,
    x: PropTypes.func,
    skipRenderCount: PropTypes.number,
};

Area.contextTypes = {
    containerHeight: PropTypes.number,
    containerWidth: PropTypes.number,
};

function getGenerator(props) {
    return d3.area()
        .y0(props.y0)
        .y1(props.y1)
        .x(props.x);
}

export default Area;
