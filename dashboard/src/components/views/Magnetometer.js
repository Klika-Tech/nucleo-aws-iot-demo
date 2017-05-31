import React, { Component } from 'react';
import { connect } from 'react-redux';
import { TESLAS } from '../../scaleUnits';
import DimensionsChart from '../common/DimensionsChart';

const mapStateToProps = state => ({
    data: state.magnetometer.data,
    markersData: state.magnetometer.markersData,
});

const Magnetometer = ({ data, markersData }) => (
    <DimensionsChart
            type="magnetometer"
            units={TESLAS}
            data={data}
            markersData={markersData}
    />
);

export default connect(mapStateToProps)(Magnetometer);
