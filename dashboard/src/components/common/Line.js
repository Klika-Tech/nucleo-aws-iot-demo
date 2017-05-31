import React, { Component, PropTypes } from 'react';
import _ from 'lodash';
import lineOptimized from '../../services/lineOptimized';

class Line extends Component {
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
            <path className={`line ${this.props.className || ''}`} d={this.generator(this.props.data)} />
        );
    }
}

Line.propTypes = {
    data: PropTypes.array,
    domain: PropTypes.arrayOf(PropTypes.number),
    x: PropTypes.func,
    y: PropTypes.func,
    className: PropTypes.string,
    skipRenderCount: PropTypes.number,
};

Line.contextTypes = {
    containerHeight: PropTypes.number,
    containerWidth: PropTypes.number,
};

function getGenerator(props) {
    return lineOptimized()
        .y(props.y)
        .x(props.x);
}

export default Line;
