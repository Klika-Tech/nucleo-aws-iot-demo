package com.klikatech.nucleo.net.response;

import com.google.gson.annotations.Expose;
import com.google.gson.annotations.SerializedName;

import java.util.List;

public class StartDataResponse {

    @Expose
    @SerializedName("sensorData")
    public List<sensorData> sensorData;

    @Expose
    @SerializedName("weatherData")
    public List<weatherData> weatherData;

    public class sensorData{

        @Expose
        @SerializedName("temperature")
        public float temperature;

        @Expose
        @SerializedName("timestamp")
        public long timestamp;

        @Expose
        @SerializedName("marker")
        public boolean marker;

        @Expose
        @SerializedName("humidity")
        public float humidity;

        @Expose
        @SerializedName("pressure")
        public float pressure;

        @Expose
        @SerializedName("accelerometer")
        public List<Float> accelerometer;

        @Expose
        @SerializedName("gyroscope")
        public List<Float> gyroscope;

        @Expose
        @SerializedName("magnetometer")
        public List<Float> magnetometer;
    }

    public class weatherData{

        @Expose
        @SerializedName("cityName")
        public String cityName;

        @Expose
        @SerializedName("cityId")
        public String cityId;

        @Expose
        @SerializedName("tempData")
        public List<tempData> tempData;

    }

    public class tempData{

        @Expose
        @SerializedName("temperature")
        public float temperature;

        @Expose
        @SerializedName("timestamp")
        public long timestamp;

    }
}
