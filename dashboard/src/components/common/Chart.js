import React, { Component, PropTypes } from 'react';
import Dimensions from 'react-dimensions';

const ChartHOC = (ComposedComponent) => {
    const c = class extends Component {
        getChildContext() {
            return {
                containerHeight: this.props.containerHeight,
                containerWidth: this.props.containerWidth,
            };
        }
        render() {
            return (<ComposedComponent {...this.props} />);
        }
    };
    c.childContextTypes = {
        containerHeight: PropTypes.number,
        containerWidth: PropTypes.number,
    };
    return c;
};

const Chart = _ => (Dimensions()(ChartHOC(_)));

export default Chart;
