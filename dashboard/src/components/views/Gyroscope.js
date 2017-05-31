import React, { Component } from 'react';
import { connect } from 'react-redux';
import { DEGREES } from '../../scaleUnits';
import DimensionsChart from '../common/DimensionsChart';

const mapStateToProps = state => ({
    data: state.gyroscope.data,
    markersData: state.gyroscope.markersData,
});

const Gyroscope = ({ data, markersData }) => (
    <DimensionsChart
            type="gyroscope"
            units={DEGREES}
            data={data}
            markersData={markersData}
    />
);

export default connect(mapStateToProps)(Gyroscope);
