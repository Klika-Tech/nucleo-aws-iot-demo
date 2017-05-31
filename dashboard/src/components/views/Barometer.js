import React, { Component } from 'react';
import { connect } from 'react-redux';
import { HYPER_PASCALS } from '../../scaleUnits';
import WeatherChart from '../common/WeatherChart';
import { pressureChangeChartType, pressureToggleVisibility } from '../../actions/pressure';

const mapStateToProps = state => ({
    chartType: state.pressure.chartType,
    sensorData: state.pressure.sensorData,
    markersData: state.pressure.markersData,
    citiesData: state.pressure.citiesData,
    displayedCitiesData: state.pressure.displayedCitiesData,
});

const mapDispatchToProps = dispatch => ({
    handleChangeType(type) {
        dispatch(pressureChangeChartType(type));
    },
    handleToggleVisibility(value) {
        dispatch(pressureToggleVisibility(value));
    },
});

const Barometer = ({
    sensorData, markersData, chartType, displayedCitiesData, citiesData, handleChangeType, handleToggleVisibility,
}) => (
    <WeatherChart
        type="pressure"
        units={HYPER_PASCALS}
        chartType={chartType}
        sensorData={sensorData}
        markersData={markersData}
        citiesData={citiesData}
        displayedCitiesData={displayedCitiesData}
        onChangeType={handleChangeType}
        onToggleVisibility={handleToggleVisibility}
    />
);

export default connect(mapStateToProps, mapDispatchToProps)(Barometer);
