import React, { Component, PropTypes } from 'react';

class Focus extends Component {
    constructor(props) {
        super(props);
        this.handleRect = this.handleRect.bind(this);
        this.handleMouseMove = this.handleMouseMove.bind(this);
        this.handleMouseOut = this.handleMouseOut.bind(this);
        this.handleWheel = this.handleWheel.bind(this);
    }
    handleRect(e) {
        if (e) this.rect = e.getBoundingClientRect();
    }
    handleMouseMove(e) {
        const { rect } = this;
        const { onMouseMove, margin } = this.props;
        let [x, y] = [e.clientX, e.clientY];
        x -= margin.left + rect.left;
        y -= margin.top + rect.top;
        onMouseMove.call({}, x, y);
    }
    handleMouseOut(e) {
        const { onMouseOut } = this.props;
        onMouseOut.call();
    }
    handleWheel(e) {
        const { onWheel } = this.props;
        const deltaY = e.deltaY === undefined ? e.wheelDeltaY : e.deltaY;
        onWheel.call({}, deltaY);
    }
    render() {
        const { margin, children, width, height } = this.props;
        return (
            <g
                className="focus"
                transform={`translate(${margin.left},${margin.top})`}
                onMouseMove={this.handleMouseMove}
                onMouseOut={this.handleMouseOut}
                onWheel={this.handleWheel}
            >
                <rect className="focus-bg" x="0" y="0" width={width} height={height} ref={this.handleRect} />
                {children}
            </g>
        );
    }
}

Focus.propTypes = {
    margin: PropTypes.shape({
        left: PropTypes.number,
        top: PropTypes.number,
        right: PropTypes.number,
        bottom: PropTypes.number,
    }),
    height: PropTypes.number,
    width: PropTypes.number,
    onMouseMove: PropTypes.func,
    onMouseOut: PropTypes.func,
    onWheel: PropTypes.func,
};


export default Focus;
