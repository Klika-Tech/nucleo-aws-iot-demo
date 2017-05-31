import React from 'react';
import { connect } from 'react-redux';
import { HYPER_PASCALS } from '../../../scaleUnits';
import SimpleWeatherChart from '../../common/SimpleWeatherChart';

const mapStateToProps = state => ({
    data: state.pressure.sensorData,
});

const BarometerWidget = ({ data }) => (
    <SimpleWeatherChart
        type="pressure"
        units={HYPER_PASCALS}
        data={data}
    />
);

export default connect(mapStateToProps)(BarometerWidget);
