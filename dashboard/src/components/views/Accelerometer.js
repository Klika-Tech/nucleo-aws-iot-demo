import React, { Component } from 'react';
import { connect } from 'react-redux';
import { G } from '../../scaleUnits';
import DimensionsChart from '../common/DimensionsChart';

const mapStateToProps = state => ({
    data: state.accelerometer.data,
    markersData: state.accelerometer.markersData,
});

const Accelerometer = ({ data, markersData }) => (
    <DimensionsChart
            type="accelerometer"
            units={G}
            data={data}
            markersData={markersData}
    />
);

export default connect(mapStateToProps)(Accelerometer);
