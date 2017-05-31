import React, { Component } from 'react';
import { connect } from 'react-redux';
import WeatherChart from '../common/WeatherChart';
import {
    temperatureChangeChartType,
    temperatureToggleVisibility,
    temperatureChangeUnits,
} from '../../actions/temperature';

const mapStateToProps = state => ({
    chartType: state.temperature.chartType,
    units: state.temperature.units,
    sensorData: state.temperature.sensorData,
    markersData: state.temperature.markersData,
    citiesData: state.temperature.citiesData,
    displayedCitiesData: state.temperature.displayedCitiesData,
});

const mapDispatchToProps = dispatch => ({
    handleChangeType(type) {
        dispatch(temperatureChangeChartType(type));
    },
    handleToggleVisibility(value) {
        dispatch(temperatureToggleVisibility(value));
    },
    handleChangeUnits(units) {
        dispatch(temperatureChangeUnits(units));
    },
});

const Temperature = ({
    sensorData, markersData, chartType, units, displayedCitiesData,
    citiesData, handleChangeType, handleToggleVisibility,
    handleChangeUnits,
}) => (
    <WeatherChart
        type="temperature"
        units={units}
        chartType={chartType}
        sensorData={sensorData}
        markersData={markersData}
        citiesData={citiesData}
        displayedCitiesData={displayedCitiesData}
        onChangeType={handleChangeType}
        onToggleVisibility={handleToggleVisibility}
        onChangeUnits={handleChangeUnits}
    />
);

export default connect(mapStateToProps, mapDispatchToProps)(Temperature);
