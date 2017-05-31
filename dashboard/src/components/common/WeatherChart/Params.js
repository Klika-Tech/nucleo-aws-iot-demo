import React, { Component, PropTypes } from 'react';
import _ from 'lodash';
import { Form, FormGroup, ControlLabel, Radio, Checkbox } from '@sketchpixy/rubix';
import './style.scss';
import { LINE_CHART, AREA_CHART } from '../../../chartTypes';
import { CELSIUS, FAHRENHEITS } from '../../../scaleUnits';

class WeatherParams extends Component {
    constructor(props) {
        super(props);
        this.setChartType = this.setChartType.bind(this);
        this.setUnits = this.setUnits.bind(this);
        this.setShowWeatherFor = this.setShowWeatherFor.bind(this);
    }
    setChartType(event) {
        const { onChangeType } = this.props;
        onChangeType.call({}, event.target.value);
    }
    setUnits(event) {
        const { type, onChangeUnits } = this.props;
        if (type === 'temperature') {
            const val = (event.target.value === 'f') ? FAHRENHEITS : CELSIUS;
            onChangeUnits.call({}, val);
        }
    }
    setShowWeatherFor(event) {
        const { onToggleVisibility } = this.props;
        const value = parseInt(event.target.value, 0);
        onToggleVisibility.call({}, value);
    }
    render() {
        const { setShowWeatherFor } = this;
        const { chartType, selectedUnits, citiesData, displayedCitiesData, type } = this.props;
        const displayedCitiesIds = displayedCitiesData.map(d => d.cityId);
        return (
            <div className="weather-chart-params">
                <Form>
                    <FormGroup>
                        <ControlLabel>Chart Type</ControlLabel>
                        <Radio
                            checked={chartType === AREA_CHART}
                            value={AREA_CHART} onChange={this.setChartType}
                            inline
                        >
                            Area
                        </Radio>
                        <Radio
                            checked={chartType === LINE_CHART}
                            value={LINE_CHART}
                            onChange={this.setChartType}
                            inline
                        >
                            Line
                        </Radio>
                    </FormGroup>
                    {(type === 'temperature') && (
                        <FormGroup>
                            <ControlLabel>Units</ControlLabel>
                            <Radio
                                checked={selectedUnits.key === CELSIUS.key}
                                value="c"
                                onChange={this.setUnits}
                                inline
                            >{CELSIUS.label}</Radio>
                            <Radio
                                checked={selectedUnits.key === FAHRENHEITS.key}
                                value="f"
                                onChange={this.setUnits}
                                inline
                            >{FAHRENHEITS.label}</Radio>
                        </FormGroup>
                    )}
                    <FormGroup>
                        <ControlLabel>Weather Data</ControlLabel>
                        {_(citiesData).sortBy(d => d.cityName).map(d => (
                            <Checkbox
                                key={d.cityId}
                                checked={_.includes(displayedCitiesIds, d.cityId)}
                                value={d.cityId}
                                onChange={setShowWeatherFor}
                            >{d.cityName}</Checkbox>)).value()}
                    </FormGroup>
                </Form>
            </div>
        );
    }
}

WeatherParams.propTypes = {
    type: PropTypes.string,
    chartType: PropTypes.string,
    selectedUnits: PropTypes.shape({
        key: PropTypes.string,
        label: PropTypes.string,
    }),
    citiesData: PropTypes.arrayOf(PropTypes.shape({
        cityId: PropTypes.number,
        cityName: PropTypes.string,
    })),
    displayedCitiesData: PropTypes.arrayOf(PropTypes.shape({
        cityId: PropTypes.number,
        cityName: PropTypes.string,
    })),
    onChangeType: PropTypes.func,
    onToggleVisibility: PropTypes.func,
    onChangeUnits: PropTypes.func,
};

export default WeatherParams;
