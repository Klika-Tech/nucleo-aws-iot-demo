import React from 'react';
import { connect } from 'react-redux';
import { PERCENTS } from '../../../scaleUnits';
import SimpleWeatherChart from '../../common/SimpleWeatherChart';

const mapStateToProps = state => ({
    data: state.humidity.sensorData,
});

const HumidityWidget = ({ data }) => (
    <SimpleWeatherChart
        type="humidity"
        units={PERCENTS}
        data={data}
    />
);

export default connect(mapStateToProps)(HumidityWidget);
