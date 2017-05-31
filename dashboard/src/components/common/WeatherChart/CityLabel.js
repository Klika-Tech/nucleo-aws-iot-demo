import React, { PropTypes } from 'react';

const offsetY = 10;
const offsetX = 5;

const CityLabel = ({ data, top, margin }) => {
    const style = {
        top: `${top - offsetY}px`,
        right: `${margin.right + offsetX}px`,
    };
    return (
        <div className={`city-label city-${data.cityId}`} style={style}>{data.cityName}</div>
    );
};

CityLabel.propTypes = {
    top: PropTypes.number,
    margin: PropTypes.shape({
        right: PropTypes.number,
    }),
    data: PropTypes.shape({
        cityId: PropTypes.number,
        cityName: PropTypes.string,
    }),
};

export default CityLabel;
