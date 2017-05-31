import React, { Component, PropTypes } from 'react';
import ReactDOM from 'react-dom';
import * as d3 from 'd3';

class BrushX extends Component {
    constructor(props) {
        super(props);
        this.brush = d3.brushX();
        this.renderBrush = this.renderBrush.bind(this);
        this.handleBrushStart = this.handleBrushStart.bind(this);
        this.handleBrush = this.handleBrush.bind(this);
        this.handleBrushEnd = this.handleBrushEnd.bind(this);
    }
    componentDidMount() { this.renderBrush(); }
    handleBrushStart() {
        const { onBrushStart } = this.props;
        onBrushStart.call({}, d3.event.selection);
    }
    handleBrush() {
        const { onBrush } = this.props;
        onBrush.call({}, d3.event.selection);
    }
    handleBrushEnd() {
        const { onBrushEnd } = this.props;
        onBrushEnd.call({}, d3.event.selection);
    }
    renderBrush() {
        const { brush } = this;
        const { width, height, onBrushMount, onBrushStart, onBrush, onBrushEnd } = this.props;
        const node = ReactDOM.findDOMNode(this);
        const g = d3.select(node);
        brush.extent([[0, 0], [width, height]]);
        if (onBrushStart) {
            brush.on('start', this.handleBrushStart);
        }
        if (onBrush) {
            brush.on('brush', this.handleBrush);
        }
        if (onBrushEnd) {
            brush.on('end', this.handleBrushEnd);
        }
        g.call(brush);
        const moveBrush = function (selection) {
            brush.move(g, selection);
        };
        onBrushMount.call({}, { moveBrush });
    }
    render() {
        const { children } = this.props;
        return (
            <g className="brush">
                {children}
            </g>
        );
    }
}

BrushX.propTypes = {
    height: PropTypes.number,
    width: PropTypes.number,
    onBrushMount: PropTypes.func,
    onBrushStart: PropTypes.func,
    onBrush: PropTypes.func,
    onBrushEnd: PropTypes.func,
};

export default BrushX;
