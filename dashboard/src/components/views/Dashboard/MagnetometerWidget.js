import React from 'react';
import { connect } from 'react-redux';
import SimpleDimensionsChart from '../../common/SimpleDimensionsChart';
import { TESLAS } from '../../../scaleUnits';


const mapStateToProps = state => ({
    data: state.magnetometer.data,
});

const MagnetometerWidget = ({ data, yDomain, xDomain }) => (
    <SimpleDimensionsChart
        type="magnetometer"
        units={TESLAS}
        data={data}
    />
);

export default connect(mapStateToProps)(MagnetometerWidget);
