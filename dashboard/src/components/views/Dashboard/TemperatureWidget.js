import React from 'react';
import { connect } from 'react-redux';
import { CELSIUS } from '../../../scaleUnits';
import SimpleWeatherChart from '../../common/SimpleWeatherChart';

const mapStateToProps = state => ({
    data: state.temperature.sensorData,
});

const TemperatureWidget = ({ data }) => (
    <SimpleWeatherChart
        type="temperature"
        units={CELSIUS}
        data={data}
    />
);

export default connect(mapStateToProps)(TemperatureWidget);
