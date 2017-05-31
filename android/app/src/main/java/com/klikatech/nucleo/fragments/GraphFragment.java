package com.klikatech.nucleo.fragments;

import android.content.res.Resources;
import android.graphics.Color;
import android.graphics.Typeface;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import android.widget.Toast;

import com.github.mikephil.charting.animation.Easing;
import com.github.mikephil.charting.charts.LineChart;
import com.github.mikephil.charting.components.LimitLine;
import com.github.mikephil.charting.components.XAxis;
import com.github.mikephil.charting.components.YAxis;
import com.github.mikephil.charting.data.Entry;
import com.github.mikephil.charting.data.LineData;
import com.github.mikephil.charting.data.LineDataSet;
import com.github.mikephil.charting.interfaces.datasets.ILineDataSet;
import com.klikatech.nucleo.Constants;
import com.klikatech.nucleo.NucleoApplication;
import com.klikatech.nucleo.R;
import com.klikatech.nucleo.activity.HomeActivity;
import com.klikatech.nucleo.custom.MyMarkerView;
import com.klikatech.nucleo.custom.XAxisValueFormat;
import com.klikatech.nucleo.custom.YAxisValueFormat;
import com.klikatech.nucleo.event.StartDataEvent;
import com.klikatech.nucleo.net.response.StartDataResponse;

import java.util.ArrayList;
import java.util.List;

import butterknife.Bind;
import butterknife.ButterKnife;
import butterknife.OnClick;
import de.greenrobot.event.EventBus;

public class GraphFragment extends Fragment {

    public static final String TAG = GraphFragment.class.getName();

    public int SENSOR;

    public boolean zoomOut = false;
    public boolean isStartChartAnimation = false;
    View fragmentView = null;

    @Bind(R.id.progressBar)
    public View pb;
    @Bind(R.id.tvSensorTitle)
    TextView tvSensorTitle;
    @Bind(R.id.chart1)
    LineChart mChart;

    public StartDataResponse mStartDataResponse;
    List<String> nameCity;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        fragmentView = inflater.inflate(R.layout.fragment_graph, null);
        ButterKnife.bind(this, fragmentView);
        pb.setVisibility(View.VISIBLE);

        EventBus.getDefault().register(this);
        return fragmentView;
    }

    public void updateTitle(String title) {
        tvSensorTitle.setText(String.valueOf(title));
    }

    @Override
    public void onStart() {
        super.onStart();
        ButterKnife.bind(this, fragmentView);
        if (!EventBus.getDefault().isRegistered(this)) EventBus.getDefault().register(this);
    }

    @OnClick(R.id.b_settings)
    public void onClickView(View v) {
        ((HomeActivity) getActivity()).showFragment(SettingFragment.TAG);
    }

    @Override
    public void onStop() {
        super.onStop();
        EventBus.getDefault().unregister(this);
        ButterKnife.unbind(this);
    }

    public void clearCities() {
        fragmentView.findViewById(R.id.tv_city2).setVisibility(View.GONE);
        fragmentView.findViewById(R.id.tv_city3).setVisibility(View.GONE);
        fragmentView.findViewById(R.id.tv_city4).setVisibility(View.GONE);
        fragmentView.findViewById(R.id.tv_city5).setVisibility(View.GONE);
        fragmentView.findViewById(R.id.tv_city6).setVisibility(View.GONE);
    }

    public void setCity() {

        clearCities();

        List<String> s = NucleoApplication.getInstance().getSelectedCity();

        TextView tcity2 = (TextView) fragmentView.findViewById(R.id.tv_city2);
        TextView tcity3 = (TextView) fragmentView.findViewById(R.id.tv_city3);
        TextView tcity4 = (TextView) fragmentView.findViewById(R.id.tv_city4);
        TextView tcity5 = (TextView) fragmentView.findViewById(R.id.tv_city5);
        TextView tcity6 = (TextView) fragmentView.findViewById(R.id.tv_city6);

        if (s != null && s.size() > 0) {
            if (s.size() < 3) {
                fragmentView.findViewById(R.id.l_city2).setVisibility(View.GONE);

                if (s.size() == 1) {
                    tcity2.setVisibility(View.VISIBLE);
                    tcity3.setVisibility(View.GONE);

                    tcity2.setText(s.get(0));

                } else {
                    tcity2.setVisibility(View.VISIBLE);
                    tcity3.setVisibility(View.VISIBLE);

                    tcity2.setText(s.get(0));
                    tcity3.setText(s.get(1));
                }


            } else {
                fragmentView.findViewById(R.id.l_city2).setVisibility(View.VISIBLE);

                tcity2.setVisibility(View.VISIBLE);
                tcity3.setVisibility(View.VISIBLE);

                tcity2.setText(s.get(0));
                tcity3.setText(s.get(1));

                if (s.size() == 3) {

                    tcity4.setVisibility(View.VISIBLE);
                    tcity5.setVisibility(View.GONE);
                    tcity6.setVisibility(View.GONE);

                    tcity4.setText(s.get(2));
                }

                if (s.size() == 4) {

                    tcity4.setVisibility(View.VISIBLE);
                    tcity5.setVisibility(View.VISIBLE);
                    tcity6.setVisibility(View.GONE);

                    tcity4.setText(s.get(2));
                    tcity5.setText(s.get(3));
                }

                if (s.size() == 5) {

                    tcity4.setVisibility(View.VISIBLE);
                    tcity5.setVisibility(View.VISIBLE);
                    tcity6.setVisibility(View.VISIBLE);

                    tcity4.setText(s.get(2));
                    tcity5.setText(s.get(3));
                    tcity6.setText(s.get(4));
                }
            }
        }
    }

    public void setChart(StartDataResponse startDataResponse) {
        mChart.setDrawGridBackground(false);
        mChart.setDescription("");
        mChart.setTouchEnabled(true);
        mChart.setDragEnabled(true);
        mChart.setScaleEnabled(true);
        mChart.setPinchZoom(true);
        mChart.setBackgroundColor(Color.TRANSPARENT);
        LimitLine llXAxis = new LimitLine(10f, "Index 10");
        llXAxis.setLineWidth(0f);
        llXAxis.enableDashedLine(10f, 10f, 0f);
        llXAxis.setLabelPosition(LimitLine.LimitLabelPosition.RIGHT_BOTTOM);
        llXAxis.setTextSize(10f);
        Typeface tf = Typeface.createFromAsset(getActivity().getAssets(), "OpenSans-Regular.ttf");
        mChart.getAxisLeft().setPosition(YAxis.YAxisLabelPosition.INSIDE_CHART);
        mChart.getXAxis().setPosition(XAxis.XAxisPosition.BOTTOM);
        mChart.getAxisLeft().setTypeface(tf);
        mChart.getXAxis().setTypeface(tf);
        XAxis xAxis = mChart.getXAxis();
        xAxis.setDrawGridLines(false);
        xAxis.setDrawAxisLine(false);
        YAxis leftAxis = mChart.getAxisLeft();
        leftAxis.removeAllLimitLines();
        leftAxis.enableGridDashedLine(10f, 10f, 0f);
        leftAxis.setDrawZeroLine(false);
        leftAxis.setDrawAxisLine(false);
        leftAxis.setDrawGridLines(false);
        leftAxis.setDrawLimitLinesBehindData(true);
        leftAxis.setValueFormatter(new YAxisValueFormat());
        mChart.getAxisRight().setEnabled(false);
        mChart.getXAxis().setPosition(XAxis.XAxisPosition.BOTTOM);
        mChart.getXAxis().setValueFormatter(new XAxisValueFormat());
        mChart.getLegend().setEnabled(false);
        if (!isStartChartAnimation) {
            setData(startDataResponse, true);
            isStartChartAnimation = true;
        } else
            setData(startDataResponse, false);
        MyMarkerView mv = new MyMarkerView(getActivity(), R.layout.custom_marker_view, startDataResponse);
        mChart.setMarkerView(mv);
        mChart.animateX(2500, Easing.EasingOption.EaseInOutQuart);
        mChart.invalidate();
    }

    public void setData(StartDataResponse startDataResponse, boolean isZoom) {
        if (getActivity() == null) return;
        switch (SENSOR) {
            case Constants.S_TEMP:
            case Constants.S_HUM:
            case Constants.S_BAR:
                setSimpleData(startDataResponse, isZoom);
                break;
            case Constants.S_MAGN:
            case Constants.S_G:
            case Constants.S_A:
                setXYZData(startDataResponse, isZoom);
                break;
        }
    }

    private List<Integer> getCityColors() {
        List<Integer> colors = new ArrayList<>();
        for (int i = 2; i <= 9; i++) {
            switch (i) {
                case 2:
                    colors.add(R.color.bg_city2);
                    break;
                case 3:
                    colors.add(R.color.bg_city3);
                    break;
                case 4:
                    colors.add(R.color.bg_city4);
                    break;
                case 5:
                    colors.add(R.color.bg_city5);
                    break;
                case 6:
                    colors.add(R.color.bg_city6);
                    break;
                case 7:
                    colors.add(R.color.bg_city7);
                    break;
                case 8:
                    colors.add(R.color.bg_city8);
                    break;
                case 9:
                    colors.add(R.color.bg_city9);
                    break;
            }
        }
        return colors;
    }

    public void onEventMainThread(StartDataEvent eventResult) {
        switch (eventResult.getResult()) {
            case OK:
                pb.setVisibility(View.GONE);
                mStartDataResponse = (StartDataResponse) eventResult.getEntity();
                if (mStartDataResponse.weatherData != null) {
                    if (nameCity != null) {
                        nameCity.clear();
                        nameCity = null;
                    }
                    nameCity = new ArrayList<>();
                    if (mStartDataResponse.weatherData != null && mStartDataResponse.weatherData.size() != 0) {
                        for (int i = 0; i < mStartDataResponse.weatherData.size(); i++) {
                            nameCity.add(mStartDataResponse.weatherData.get(i).cityName);
                        }
                        NucleoApplication.getInstance().setNames(nameCity);
                    }
                }
                zoomOut = false;
                if (mStartDataResponse.sensorData != null) {
                    if (!isStartChartAnimation)
                        setChart(mStartDataResponse);
                    else
                        setData(mStartDataResponse, false);
                }
                break;
            case FAIL:
                Toast.makeText(getActivity(), eventResult.getErrorResponse().errorMessage, Toast.LENGTH_SHORT).show();
                break;
        }
    }

    @Override
    public void onHiddenChanged(boolean hidden) {
        super.onHiddenChanged(hidden);
        if (!hidden) {
            NucleoApplication.getInstance().sensor_type = SENSOR;
            setCity();
            reInitView();
            reInitData();
        }
    }

    private void reInitData() {
        switch (SENSOR) {
            case Constants.S_TEMP:
                setCity();
                break;
            case Constants.S_HUM:
                clearCities();
                break;
            case Constants.S_MAGN:
                clearCities();
                break;
            case Constants.S_BAR:
                clearCities();
                break;
            case Constants.S_G:
                clearCities();
                break;
            case Constants.S_A:
                clearCities();
                break;
            default:
                break;
        }

        if (mStartDataResponse == null) return;
        setData(mStartDataResponse, true);
    }

    private void reInitView() {
        Resources resources = getResources();
        YAxis leftAxis = mChart.getAxisLeft();
        switch (SENSOR) {
            case Constants.S_TEMP:
                updateTitle(resources.getString(R.string.x_sensor, resources.getString(R.string.title_temperature)));
                leftAxis.setAxisMaxValue(50f);
                leftAxis.setAxisMinValue(-0f);
                setCity();
                break;
            case Constants.S_HUM:
                updateTitle(resources.getString(R.string.x_sensor, resources.getString(R.string.title_humidity)));
                leftAxis.setAxisMaxValue(100f);
                leftAxis.setAxisMinValue(-0f);
                break;
            case Constants.S_MAGN:
                updateTitle(resources.getString(R.string.x_sensor, resources.getString(R.string.title_magnetometer)));
                leftAxis.setAxisMaxValue(1f);
                leftAxis.setAxisMinValue(-1f);
                break;
            case Constants.S_BAR:
                updateTitle(resources.getString(R.string.x_sensor, resources.getString(R.string.title_barometer)));
                leftAxis.setAxisMaxValue(1100f);
                leftAxis.setAxisMinValue(-0f);
                break;
            case Constants.S_G:
                updateTitle(resources.getString(R.string.x_sensor, resources.getString(R.string.title_gyroscope)));
                leftAxis.setAxisMaxValue(7f);
                leftAxis.setAxisMinValue(-7f);
                break;
            case Constants.S_A:
                updateTitle(resources.getString(R.string.x_sensor, resources.getString(R.string.title_accelerometer)));
                leftAxis.setAxisMaxValue(1f);
                leftAxis.setAxisMinValue(-1f);
                break;
            default:
                break;
        }
    }

    private boolean isValueNull(StartDataResponse startDataResponse, int i) {
        boolean result = false;
        switch (SENSOR) {
            case Constants.S_TEMP:
                result = startDataResponse.sensorData.get(i).temperature == 0f;
                break;
            case Constants.S_HUM:
                result = startDataResponse.sensorData.get(i).humidity == 0f;
                break;
            case Constants.S_BAR:
                result = startDataResponse.sensorData.get(i).pressure == 0f;
                break;
            case Constants.S_MAGN:
                result = startDataResponse.sensorData.get(i).magnetometer == null;
                break;
            case Constants.S_G:
                result = startDataResponse.sensorData.get(i).gyroscope == null;
                break;
            case Constants.S_A:
                result = startDataResponse.sensorData.get(i).accelerometer == null;
                break;
            default:
                break;
        }
        return result;
    }

    private float getValueBySensor(StartDataResponse startDataResponse, int i) {
        float val = 0f;
        switch (SENSOR) {
            case Constants.S_TEMP:
                val = startDataResponse.sensorData.get(i).temperature;
                break;
            case Constants.S_HUM:
                val = startDataResponse.sensorData.get(i).humidity;
                break;
            case Constants.S_BAR:
                val = startDataResponse.sensorData.get(i).pressure;
                break;
            default:
                break;
        }
        return val;
    }

    private float getValueBySensor(StartDataResponse startDataResponse, int i, int xyz) {
        float val = 0f;
        try {
            switch (SENSOR) {
                case Constants.S_MAGN:
                    val = startDataResponse.sensorData.get(i).magnetometer.get(xyz);
                    break;
                case Constants.S_G:
                    val = startDataResponse.sensorData.get(i).gyroscope.get(xyz);
                    break;
                case Constants.S_A:
                    val = startDataResponse.sensorData.get(i).accelerometer.get(xyz);
                    break;
                default:
                    break;
            }
        } catch (NullPointerException e) {
            e.printStackTrace();
        }

        return val;
    }


    private void setSimpleData(StartDataResponse startDataResponse, boolean isZoom) {
        if (startDataResponse.sensorData != null && startDataResponse.sensorData.size() != 0) {
            ArrayList<String> xVals = new ArrayList<String>();
            for (int i = 0; i < startDataResponse.sensorData.size(); i++) {
                xVals.add(startDataResponse.sensorData.get(i).timestamp + "");
            }

            ArrayList<ILineDataSet> dataSets = new ArrayList<ILineDataSet>();
            ArrayList<Entry> yVals = new ArrayList<Entry>();
            for (int i = 0; i < startDataResponse.sensorData.size(); i++) {
                if (!isValueNull(startDataResponse, i))
                    yVals.add(new Entry(getValueBySensor(startDataResponse, i), i));
            }
            ArrayList<Entry> yCircleVals = new ArrayList<Entry>();
            for (int i = 0; i < startDataResponse.sensorData.size(); i++) {
                if (!isValueNull(startDataResponse, i))
                    if (startDataResponse.sensorData.get(i).marker && (getValueBySensor(startDataResponse, i) != 0f)) {
                        yCircleVals.add(new Entry(getValueBySensor(startDataResponse, i), i));
                    }
            }

            dataSets.add(getLineDataSet(yVals, R.color.bg_city1));
            dataSets.add(getCircleLineDataSet(yCircleVals));


            if (SENSOR == Constants.S_TEMP) {
                if (NucleoApplication.getInstance().getSelectedCity() != null && NucleoApplication.getInstance().getSelectedCity().size() != 0) {
                    List<Integer> colors = getCityColors();
                    if (startDataResponse.weatherData != null && startDataResponse.weatherData.size() != 0) {
                        for (int i = 0; i < startDataResponse.weatherData.size(); i++) {
                            if (NucleoApplication.getInstance().getSelectedCity().contains(startDataResponse.weatherData.get(i).cityName)) {
                                StartDataResponse.weatherData weatherData = startDataResponse.weatherData.get(i);
                                if (weatherData.tempData.size() > 0) {
                                    ArrayList<Entry> yValsTemp = new ArrayList<Entry>();
                                    yValsTemp.clear();
                                    for (int j = 0; j < weatherData.tempData.size(); j++) {
                                        if (j == 0) {
                                            yValsTemp.add(new Entry(weatherData.tempData.get(j).temperature, 0));
                                        } else if (j == weatherData.tempData.size() - 1) {
                                            yValsTemp.add(new Entry(weatherData.tempData.get(j).temperature, xVals.size() - 1));
                                        } else {
                                            int k = Math.round(xVals.size() * (weatherData.tempData.get(j).timestamp - startDataResponse.sensorData.get(0).timestamp) /
                                                    (startDataResponse.sensorData.get(startDataResponse.sensorData.size() - 1).timestamp - startDataResponse.sensorData.get(0).timestamp));
                                            if (k <= xVals.size() - 1) {
                                                yValsTemp.add(new Entry(weatherData.tempData.get(j).temperature, k));
                                            }
                                        }
                                    }
                                    if (i < 7)
                                        dataSets.add(getLineDataSet(yValsTemp, colors.get(i)));
                                    else
                                        dataSets.add(getLineDataSet(yValsTemp, colors.get(7)));
                                }
                            }
                        }
                    }
                }
            }
            LineData data = new LineData(xVals, dataSets);
            mChart.setData(data);
            mChart.notifyDataSetChanged();
            mChart.invalidate();
            if (isZoom)
                mChart.zoomAndCenterAnimated(24f, 24f, xVals.size() - 1, yVals.get(yVals.size() - 1).getVal(), YAxis.AxisDependency.LEFT, 0);
            if (zoomOut)
                do {
                    mChart.zoomOut();
                } while (!mChart.isFullyZoomedOut());
        }
    }

    private void setXYZData(StartDataResponse startDataResponse, boolean isZoom) {
        if (startDataResponse.sensorData != null && startDataResponse.sensorData.size() != 0) {
            ArrayList<String> xVals = new ArrayList<String>();
            for (int i = 0; i < startDataResponse.sensorData.size(); i++) {
                xVals.add(startDataResponse.sensorData.get(i).timestamp + "");
            }

            ArrayList<ILineDataSet> dataSets = new ArrayList<ILineDataSet>();
            ArrayList<Entry> yValsX = new ArrayList<Entry>();
            for (int i = 0; i < startDataResponse.sensorData.size(); i++) {
                if (!isValueNull(startDataResponse, i))
                    yValsX.add(new Entry(getValueBySensor(startDataResponse, i, 0), i));
            }
            ArrayList<Entry> yValsY = new ArrayList<Entry>();
            for (int i = 0; i < startDataResponse.sensorData.size(); i++) {
                if (!isValueNull(startDataResponse, i))
                    yValsY.add(new Entry(getValueBySensor(startDataResponse, i, 1), i));
            }
            ArrayList<Entry> yValsZ = new ArrayList<Entry>();
            for (int i = 0; i < startDataResponse.sensorData.size(); i++) {
                if (!isValueNull(startDataResponse, i))
                    yValsZ.add(new Entry(getValueBySensor(startDataResponse, i, 2), i));
            }

            ArrayList<Entry> yCircleVals = new ArrayList<Entry>();
            for (int i = 0; i < startDataResponse.sensorData.size(); i++) {
                if (!isValueNull(startDataResponse, i))
                    if (startDataResponse.sensorData.get(i).marker && (getValueBySensor(startDataResponse, i) != 0f)) {
                        yCircleVals.add(new Entry(getValueBySensor(startDataResponse, i), i));
                    }
            }

            dataSets.add(getLineDataSet(yValsX, R.color.text_home_x));
            dataSets.add(getLineDataSet(yValsY, R.color.text_home_y));
            dataSets.add(getLineDataSet(yValsZ, R.color.text_home_z));
            dataSets.add(getCircleLineDataSet(yCircleVals));

            LineData data = new LineData(xVals, dataSets);
            mChart.setData(data);
            mChart.notifyDataSetChanged();
            mChart.invalidate();
            if (isZoom){
                float scaleY=1f;
                mChart.zoomAndCenterAnimated(24f, scaleY, xVals.size() - 1, yValsX.get(yValsX.size() - 1).getVal(), YAxis.AxisDependency.LEFT, 0);}
            if (zoomOut)
                do {
                    mChart.zoomOut();
                } while (!mChart.isFullyZoomedOut());
        }
    }

    private LineDataSet getLineDataSet(ArrayList<Entry> yVals, int colorId) {
        LineDataSet set;
        set = new LineDataSet(yVals, null);
        set.setColor(getResources().getColor(colorId));
        set.setLineWidth(1f);
        set.setDrawCircles(false);
        set.setDrawCircleHole(false);
        set.setDrawValues(false);
        set.setDrawFilled(!NucleoApplication.getInstance().isLine);
        set.setDrawHighlightIndicators(false);
        return set;
    }

    private LineDataSet getCircleLineDataSet(ArrayList<Entry> yVals) {
        LineDataSet setCircle;
        setCircle = new LineDataSet(yVals, null);
        setCircle.setColor(getResources().getColor(android.R.color.transparent));
        setCircle.setLineWidth(0f);
        setCircle.setCircleColor(getResources().getColor(android.R.color.holo_red_dark));
        setCircle.setCircleRadius(2f);
        setCircle.setDrawCircles(true);
        setCircle.setDrawCircleHole(false);
        setCircle.setDrawValues(false);
        setCircle.setDrawFilled(false);
        setCircle.setDrawHighlightIndicators(false);
        return setCircle;
    }
}

