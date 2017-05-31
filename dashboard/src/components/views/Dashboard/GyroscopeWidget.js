import React from 'react';
import { connect } from 'react-redux';
import SimpleDimensionsChart from '../../common/SimpleDimensionsChart';
import { DEGREES } from '../../../scaleUnits';

const mapStateToProps = state => ({
    data: state.gyroscope.data,
});

const GyroscopeWidget = ({ data }) => (
    <SimpleDimensionsChart
        type="gyroscope"
        units={DEGREES}
        data={data}
    />
);

export default connect(mapStateToProps)(GyroscopeWidget);
