package com.klikatech.nucleo.fragments;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.klikatech.nucleo.Constants;
import com.klikatech.nucleo.R;
import com.klikatech.nucleo.activity.HomeActivity;
import com.klikatech.nucleo.custom.AccelerometerView;
import com.klikatech.nucleo.custom.GyroscopeView;

import java.text.SimpleDateFormat;
import java.util.Locale;
import java.util.TimeZone;

import butterknife.Bind;
import butterknife.ButterKnife;
import butterknife.OnClick;

public class HomeFragment extends Fragment {
    public static final String TAG = HomeFragment.class.getName();
    @Bind(R.id.ivHome)
    View ivHome;
    @Bind(R.id.tvTitle)
    TextView tvTitle;
    @Bind(R.id.tvLastPushDate)
    TextView tvLastPushDate;

    /*Temperature*/
    @Bind(R.id.tvTemperatureF)
    TextView tvTemperatureF;
    @Bind(R.id.tvTemperatureC)
    TextView tvTemperatureC;

    /*Humidity*/
    @Bind(R.id.tvHumidityVal)
    TextView tvHumidityVal;

    /*Magnetometer*/
    @Bind(R.id.tvMagnetometerVal)
    TextView tvMagnetometerVal;
    @Bind(R.id.tvMagnetometerValX)
    TextView tvMagnetometerValX;
    @Bind(R.id.tvMagnetometerValY)
    TextView tvMagnetometerValY;
    @Bind(R.id.tvMagnetometerValZ)
    TextView tvMagnetometerValZ;

    /*Barometer*/
    @Bind(R.id.tvBarometerVal)
    TextView tvBarometerVal;

    /*Gyroscope*/
    @Bind(R.id.gyroX)
    GyroscopeView gyroX;
    @Bind(R.id.tvGyroX)
    TextView tvGyroX;
    @Bind(R.id.gyroY)
    GyroscopeView gyroY;
    @Bind(R.id.tvGyroY)
    TextView tvGyroY;
    @Bind(R.id.gyroZ)
    GyroscopeView gyroZ;
    @Bind(R.id.tvGyroZ)
    TextView tvGyroZ;

    /*Accelerometer*/
    @Bind(R.id.accX)
    AccelerometerView accX;
    @Bind(R.id.tvAccX)
    TextView tvAccX;
    @Bind(R.id.accY)
    AccelerometerView accY;
    @Bind(R.id.tvAccY)
    TextView tvAccY;
    @Bind(R.id.accZ)
    AccelerometerView accZ;
    @Bind(R.id.tvAccZ)
    TextView tvAccZ;

    View v;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        v = inflater.inflate(R.layout.fragment_home, null);
        ButterKnife.bind(this, v);
        initFragment();
        return v;
    }

    @Override
    public void onStart() {
        super.onStart();
        ButterKnife.bind(this, v);
    }

    @Override
    public void onStop() {
        super.onStop();
        ButterKnife.unbind(this);
    }

    private void initFragment() {
        updateTemperature(0);
        updateHumidity(0);
        updateMagnetometer(0, 0, 0);
        updateBarometer(0);
        updateGyroscope(0, 0, 0);
        updateAccelerometer(0, 0, 0);

        gyroX.setType(1);
        gyroY.setType(2);
        gyroZ.setType(3);
    }

    public void updateTemperature(float t) {
        if (tvTemperatureF == null || tvTemperatureC == null) return;
        tvTemperatureF.setText(String.format(Locale.ENGLISH, "%.2f", 9 * t / 5 + 32) + " \u00B0");
        tvTemperatureC.setText(String.format(Locale.ENGLISH, "%.2f", t) + " \u00B0");
    }

    public void updateHumidity(float h) {
        if (tvHumidityVal == null) return;
        tvHumidityVal.setText(String.format(Locale.ENGLISH, "%.2f", h)+" ");
    }

    public void updateMagnetometer(float x, float y, float z) {
        if (tvMagnetometerVal == null) return;
        tvMagnetometerVal.setText(String.format(Locale.ENGLISH, "%.2f", (float) Math.sqrt(x * x + y * y + z * z))+" ");
        if (tvMagnetometerValX == null) return;
        tvMagnetometerValX.setText(String.format(Locale.ENGLISH, "%.2f", x));
        if (tvMagnetometerValY == null) return;
        tvMagnetometerValY.setText(String.format(Locale.ENGLISH, "%.2f", y));
        if (tvMagnetometerValZ == null) return;
        tvMagnetometerValZ.setText(String.format(Locale.ENGLISH, "%.2f", z));
    }

    public void updateBarometer(float p) {
        if (tvBarometerVal == null) return;
        tvBarometerVal.setText(String.format(Locale.ENGLISH, "%.2f", p)+" ");
    }

    public void updateGyroscope(float x, float y, float z) {
        if (gyroX == null || tvGyroX == null) return;

        gyroX.setAngle(x);
        tvGyroX.setText(getResources().getString(R.string.angle, String.format(Locale.ENGLISH, "%.2f", x)));

        if (gyroY == null || tvGyroY == null) return;
        gyroY.setAngle(y);
        tvGyroY.setText(getResources().getString(R.string.angle, String.format(Locale.ENGLISH, "%.2f", y)));

        if (gyroZ == null || tvGyroZ == null) return;
        gyroZ.setAngle(z);
        tvGyroZ.setText(getResources().getString(R.string.angle, String.format(Locale.ENGLISH, "%.2f", z)));
    }

    public void updateAccelerometer(float x, float y, float z) {
        if (accX == null || tvAccX == null) return;

        accX.setValue(x);
        accX.setType(0);
        tvAccX.setText(String.format(Locale.ENGLISH, "%.2f", x));
        if (accY == null || tvAccY == null) return;
        accY.setValue(y);
        accY.setType(1);
        tvAccY.setText(String.format(Locale.ENGLISH, "%.2f", y));
        if (accZ == null || tvAccZ == null) return;
        accZ.setValue(z);
        accZ.setType(2);
        tvAccZ.setText(String.format(Locale.ENGLISH, "%.2f", z));
    }

    public void updateLastPushedDate(long timestamp) {
        if (tvLastPushDate == null) return;

        SimpleDateFormat dateFormatTime1 = new SimpleDateFormat("MMMM dd, yyyy");
        dateFormatTime1.setTimeZone(TimeZone.getTimeZone("UTC"));

        SimpleDateFormat dateFormatTime2 = new SimpleDateFormat("HH:mm");
        dateFormatTime1.setTimeZone(TimeZone.getTimeZone("UTC"));

        tvLastPushDate.setText(dateFormatTime1.format(timestamp*1000) + " at " + dateFormatTime2.format(timestamp*1000));
    }

    @OnClick({R.id.vTemperature, R.id.vHumidity, R.id.vMagnetometer, R.id.vBarometer,
            R.id.vGyroX, R.id.vGyroY, R.id.vGyroZ,
            R.id.vAccX, R.id.vAccY, R.id.vAccZ})
    public void onItemClicked(View view) {
        switch (view.getId()) {
            case R.id.vTemperature:
                ((HomeActivity) getActivity()).showFragment(GraphFragment.TAG, Constants.S_TEMP);
                break;
            case R.id.vHumidity:
                ((HomeActivity) getActivity()).showFragment(GraphFragment.TAG, Constants.S_HUM);
                break;
            case R.id.vMagnetometer:
                ((HomeActivity) getActivity()).showFragment(GraphFragment.TAG, Constants.S_MAGN);
                break;
            case R.id.vBarometer:
                ((HomeActivity) getActivity()).showFragment(GraphFragment.TAG, Constants.S_BAR);
                break;
            case R.id.vGyroX:
            case R.id.vGyroY:
            case R.id.vGyroZ:
                ((HomeActivity) getActivity()).showFragment(GraphFragment.TAG, Constants.S_G);
                break;
            case R.id.vAccX:
            case R.id.vAccY:
            case R.id.vAccZ:
                ((HomeActivity) getActivity()).showFragment(GraphFragment.TAG, Constants.S_A);
                break;
            default:
                break;
        }
    }
}
