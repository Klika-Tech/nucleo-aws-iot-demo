import React from 'react';
import { connect } from 'react-redux';
import SimpleDimensionsChart from '../../common/SimpleDimensionsChart';
import { G } from '../../../scaleUnits';

const mapStateToProps = state => ({
    data: state.accelerometer.data,
});

const AccelerometerWidget = ({ data }) => (
    <SimpleDimensionsChart
        type="accelerometer"
        units={G}
        data={data}
    />
);

export default connect(mapStateToProps)(AccelerometerWidget);
