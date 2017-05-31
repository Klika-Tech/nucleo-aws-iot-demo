package com.klikatech.nucleo.activity;

import android.app.Activity;
import android.app.Dialog;
import android.content.res.Configuration;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;

import com.amazonaws.auth.CognitoCachingCredentialsProvider;
import com.amazonaws.mobileconnectors.iot.AWSIotKeystoreHelper;
import com.amazonaws.mobileconnectors.iot.AWSIotMqttClientStatusCallback;
import com.amazonaws.mobileconnectors.iot.AWSIotMqttLastWillAndTestament;
import com.amazonaws.mobileconnectors.iot.AWSIotMqttManager;
import com.amazonaws.mobileconnectors.iot.AWSIotMqttNewMessageCallback;
import com.amazonaws.mobileconnectors.iot.AWSIotMqttQos;
import com.amazonaws.regions.Region;
import com.amazonaws.services.iot.AWSIotClient;
import com.amazonaws.services.iot.model.AttachPrincipalPolicyRequest;
import com.amazonaws.services.iot.model.CreateKeysAndCertificateRequest;
import com.amazonaws.services.iot.model.CreateKeysAndCertificateResult;
import com.klikatech.nucleo.NucleoApplication;
import com.klikatech.nucleo.R;
import com.klikatech.nucleo.fragments.GraphFragment;
import com.klikatech.nucleo.fragments.HomeFragment;
import com.klikatech.nucleo.fragments.SettingFragment;
import com.klikatech.nucleo.job.StartDataJob;
import com.klikatech.nucleo.net.response.StartDataResponse;
import com.path.android.jobqueue.JobManager;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.UnsupportedEncodingException;
import java.security.KeyStore;
import java.util.List;
import java.util.UUID;

import butterknife.ButterKnife;

public class HomeActivity extends AppCompatActivity {

    String tagCurrentVisibleFragment = "";

    public final String LOG_TAG = "NUCLEO_TAG";

    AWSIotClient mIotAndroidClient;
    AWSIotMqttManager mqttManager;
    String clientId;
    String keystorePath;
    String keystoreName;
    String keystorePassword;
    Thread threadUpdate;
    KeyStore clientKeyStore = null;
    String certificateId;
    CustomDialogClass cdd;

    CognitoCachingCredentialsProvider credentialsProvider;

    JobManager jobManager = NucleoApplication.getInstance().getJobManager();
    private StartDataResponse mStartDataResponse;
    List<String> nameCity;

    HomeFragment homeFragment;
    GraphFragment graphFragment;
    SettingFragment settingFragment;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_home);
        FragmentManager fm = getSupportFragmentManager();
        homeFragment = (HomeFragment) fm.findFragmentById(R.id.home_fragment);
        graphFragment = (GraphFragment) fm.findFragmentById(R.id.graph_fragment);
        settingFragment = (SettingFragment) fm.findFragmentById(R.id.setting_fragment);
        fm.beginTransaction().hide(graphFragment).hide(settingFragment).commit();

        tagCurrentVisibleFragment = homeFragment.TAG;

        ButterKnife.bind(this);

        if (!NucleoApplication.getInstance().isStart) {
            cdd = new CustomDialogClass(HomeActivity.this);
            cdd.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));
            cdd.show();
            NucleoApplication.getInstance().isStart = true;
        } else {

            long time = System.currentTimeMillis() - 14400000;
            StartDataJob startDataJob = new StartDataJob(String.valueOf(time / 1000));
            jobManager.addJob(startDataJob);
        }

        if (threadUpdate != null) {
            threadUpdate.interrupt();
            threadUpdate = null;
        }


        threadUpdate = new Thread(new Runnable() {
            @Override
            public void run() {

                synchronized (threadUpdate) {
                    try {
                        threadUpdate.wait(11000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }

                long time = System.currentTimeMillis() - 14400000;
                StartDataJob startDataJob = new StartDataJob(String.valueOf(time / 1000));
                jobManager.addJob(startDataJob);
            }
        });

        threadUpdate.start();

        startClient();
    }

    public void showFragment(String fragmentTag, int sens_type) {
        FragmentTransaction ft = getSupportFragmentManager().beginTransaction();
        if (fragmentTag.equals(HomeFragment.TAG)) {
            ft.show(homeFragment);
            ft.hide(graphFragment);
            ft.hide(settingFragment);
        } else if (fragmentTag.equals(GraphFragment.TAG)) {
            if (sens_type > -1)
                graphFragment.SENSOR = sens_type;
            ft.hide(homeFragment);
            ft.show(graphFragment);
            ft.hide(settingFragment);
        } else if (fragmentTag.equals(SettingFragment.TAG)) {
            ft.hide(homeFragment);
            ft.hide(graphFragment);
            ft.show(settingFragment);
        }
        ft.commit();
        tagCurrentVisibleFragment = fragmentTag;
    }

    public void showFragment(String fragmentTag) {
        showFragment(fragmentTag, -1);
    }

    private void startClient() {

        clientId = UUID.randomUUID().toString();

        credentialsProvider = new CognitoCachingCredentialsProvider(
                getApplicationContext(), // context
                NucleoApplication.getInstance().getPoolId(), // Identity Pool ID
                NucleoApplication.getInstance().getRegion() // Region
        );

        Region region = Region.getRegion(NucleoApplication.getInstance().getRegion());

        mqttManager = new AWSIotMqttManager(clientId, region, NucleoApplication.getInstance().getEndpointPrefix());

        mqttManager.setKeepAlive(10);

        AWSIotMqttLastWillAndTestament lwt = new AWSIotMqttLastWillAndTestament("mqtt",
                "Android client lost connection", AWSIotMqttQos.QOS0);
        mqttManager.setMqttLastWillAndTestament(lwt);

        mIotAndroidClient = new AWSIotClient(credentialsProvider);
        mIotAndroidClient.setRegion(region);

        keystorePath = getFilesDir().getPath();
        keystoreName = NucleoApplication.getInstance().getKeystoreName();
        keystorePassword = NucleoApplication.getInstance().getKeystorePass();
        certificateId = NucleoApplication.getInstance().getCertId();

        try {
            if (AWSIotKeystoreHelper.isKeystorePresent(keystorePath, keystoreName)) {
                if (AWSIotKeystoreHelper.keystoreContainsAlias(certificateId, keystorePath,
                        keystoreName, keystorePassword)) {
                    Log.i(LOG_TAG, "Certificate " + certificateId
                            + " found in keystore - using for MQTT.");

                    clientKeyStore = AWSIotKeystoreHelper.getIotKeystore(certificateId,
                            keystorePath, keystoreName, keystorePassword);

                    //Enable connect
                    connectClient();

                } else {
                    Log.i(LOG_TAG, "Key/cert " + certificateId + " not found in keystore.");
                }
            } else {
                Log.i(LOG_TAG, "Keystore " + keystorePath + "/" + keystoreName + " not found.");
            }
        } catch (Exception e) {
            Log.e(LOG_TAG, "An error occurred retrieving cert/key from keystore.", e);
        }

        if (clientKeyStore == null) {
            Log.i(LOG_TAG, "Cert/key was not found in keystore - creating new key and certificate.");

            new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        CreateKeysAndCertificateRequest createKeysAndCertificateRequest =
                                new CreateKeysAndCertificateRequest();
                        createKeysAndCertificateRequest.setSetAsActive(true);
                        final CreateKeysAndCertificateResult createKeysAndCertificateResult;
                        createKeysAndCertificateResult =
                                mIotAndroidClient.createKeysAndCertificate(createKeysAndCertificateRequest);
                        Log.i(LOG_TAG,
                                "Cert ID: " +
                                        createKeysAndCertificateResult.getCertificateId() +
                                        " created.");

                        AWSIotKeystoreHelper.saveCertificateAndPrivateKey(certificateId,
                                createKeysAndCertificateResult.getCertificatePem(),
                                createKeysAndCertificateResult.getKeyPair().getPrivateKey(),
                                keystorePath, keystoreName, keystorePassword);

                        clientKeyStore = AWSIotKeystoreHelper.getIotKeystore(certificateId,
                                keystorePath, keystoreName, keystorePassword);

                        AttachPrincipalPolicyRequest policyAttachRequest =
                                new AttachPrincipalPolicyRequest();
                        policyAttachRequest.setPolicyName(NucleoApplication.getInstance().getIotPolicyName());
                        policyAttachRequest.setPrincipal(createKeysAndCertificateResult
                                .getCertificateArn());
                        mIotAndroidClient.attachPrincipalPolicy(policyAttachRequest);

                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {

                                // Enable connect
                                connectClient();
                            }
                        });
                    } catch (Exception e) {
                        Log.e(LOG_TAG,
                                "Exception occurred when generating new private key and certificate.",
                                e);
                    }
                }
            }).start();
        }
    }

    private void connectClient() {
        Log.d(LOG_TAG, "clientId = " + clientId);
        try {
            mqttManager.connect(clientKeyStore, new AWSIotMqttClientStatusCallback() {
                @Override
                public void onStatusChanged(final AWSIotMqttClientStatus status,
                                            final Throwable throwable) {
                    Log.d(LOG_TAG, "Status = " + String.valueOf(status));

                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            if (status == AWSIotMqttClientStatus.Connecting) {

                            } else if (status == AWSIotMqttClientStatus.Connected) {

                                setSubscribe(NucleoApplication.getInstance().getTopic());

                            } else if (status == AWSIotMqttClientStatus.Reconnecting) {
                                if (throwable != null) {
                                    Log.e(LOG_TAG, "Connection error.", throwable);
                                }

                            } else if (status == AWSIotMqttClientStatus.ConnectionLost) {
                                if (throwable != null) {
                                    Log.e(LOG_TAG, "Connection error.", throwable);
                                }

                            } else {

                            }
                        }
                    });
                }
            });
        } catch (final Exception e) {
            Log.e(LOG_TAG, "Connection error.", e);

        }
    }

    private void setSubscribe(String topic) {

        Log.d(LOG_TAG, "topic = " + topic);

        try {
            mqttManager.subscribeToTopic(topic, AWSIotMqttQos.QOS0,
                    new AWSIotMqttNewMessageCallback() {
                        @Override
                        public void onMessageArrived(final String topic, final byte[] data) {
                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {

                                    try {
                                        String message = new String(data, "UTF-8");
                                        Log.d(LOG_TAG, "Message arrived:");
                                        Log.d(LOG_TAG, "   Topic: " + topic);
                                        Log.d(LOG_TAG, " Message: " + message);
                                        try {
                                            JSONObject tJson = new JSONObject(message);
                                            float temperature = (float) tJson.getDouble("temperature");
                                            long time = tJson.getLong("timestamp");

                                            float humidity = (float) tJson.getDouble("humidity");
                                            float pressure = (float) tJson.getDouble("pressure");

                                            float magnetometerX = (float) tJson.getJSONArray("magnetometer").getDouble(0);
                                            float magnetometerY = (float) tJson.getJSONArray("magnetometer").getDouble(1);
                                            float magnetometerZ = (float) tJson.getJSONArray("magnetometer").getDouble(2);

                                            float accelerometerX = (float) tJson.getJSONArray("accelerometer").getDouble(0);
                                            float accelerometerY = (float) tJson.getJSONArray("accelerometer").getDouble(1);
                                            float accelerometerZ = (float) tJson.getJSONArray("accelerometer").getDouble(2);

                                            float gyroscopeX = (float) tJson.getJSONArray("gyroscope").getDouble(0);
                                            float gyroscopeY = (float) tJson.getJSONArray("gyroscope").getDouble(1);
                                            float gyroscopeZ = (float) tJson.getJSONArray("gyroscope").getDouble(2);

                                            homeFragment.updateLastPushedDate(time);
                                            homeFragment.updateTemperature(temperature);
                                            homeFragment.updateBarometer(pressure);
                                            homeFragment.updateHumidity(humidity);

                                            homeFragment.updateMagnetometer(magnetometerX, magnetometerY, magnetometerZ);
                                            homeFragment.updateAccelerometer(accelerometerX, accelerometerY, accelerometerZ);
                                            homeFragment.updateGyroscope(gyroscopeX, gyroscopeY, gyroscopeZ);

                                            if (graphFragment.mStartDataResponse != null && graphFragment.mStartDataResponse.sensorData != null && graphFragment.mStartDataResponse.sensorData.size() != 0) {
                                                StartDataResponse.sensorData sensorData = graphFragment.mStartDataResponse.sensorData.get(0);
                                                sensorData.timestamp = time;
                                                sensorData.temperature = temperature;
                                                sensorData.humidity = humidity;
                                                sensorData.pressure = pressure;

                                                sensorData.accelerometer.set(0, accelerometerX);
                                                sensorData.accelerometer.set(1, accelerometerY);
                                                sensorData.accelerometer.set(2, accelerometerZ);

                                                sensorData.magnetometer.set(0, magnetometerX);
                                                sensorData.magnetometer.set(1, magnetometerY);
                                                sensorData.magnetometer.set(2, magnetometerZ);

                                                sensorData.gyroscope.set(0, gyroscopeX);
                                                sensorData.gyroscope.set(1, gyroscopeY);
                                                sensorData.gyroscope.set(2, gyroscopeZ);

                                                graphFragment.mStartDataResponse.sensorData.add(graphFragment.mStartDataResponse.sensorData.size() - 1, sensorData);
                                                graphFragment.mStartDataResponse.sensorData.remove(0);
                                                graphFragment.setData(graphFragment.mStartDataResponse, false);
                                            }

                                        } catch (NullPointerException | JSONException npe) {
                                            npe.printStackTrace();
                                        }
                                    } catch (UnsupportedEncodingException e) {
                                        Log.e(LOG_TAG, "Message encoding error.", e);
                                    }
                                }
                            });
                        }
                    });
        } catch (Exception e) {
            Log.e(LOG_TAG, "Subscription error.", e);
        }
    }


    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        if (tagCurrentVisibleFragment.equals(GraphFragment.TAG)) {
            if (graphFragment.mStartDataResponse != null)
                graphFragment.setChart(graphFragment.mStartDataResponse);
        }
    }

    @Override
    public void onStop() {
        super.onStop();
        if (cdd != null)
            cdd.dismiss();
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (tagCurrentVisibleFragment.equals(GraphFragment.TAG)) {
            if (graphFragment.mStartDataResponse != null)
                graphFragment.setChart(graphFragment.mStartDataResponse);
            graphFragment.setCity();
        }
    }


    public class CustomDialogClass extends Dialog implements
            View.OnClickListener {
        public Button start;

        public CustomDialogClass(Activity a) {
            super(a);
        }

        @Override
        protected void onCreate(Bundle savedInstanceState) {
            super.onCreate(savedInstanceState);
            requestWindowFeature(Window.FEATURE_NO_TITLE);
            setContentView(R.layout.dialog_start);

            start = (Button) findViewById(R.id.start);
            start.setOnClickListener(this);

        }

        @Override
        public void onClick(View v) {
            switch (v.getId()) {
                case R.id.start:
                    long time = System.currentTimeMillis() - 14400000;
                    StartDataJob startDataJob = new StartDataJob(String.valueOf(time / 1000));
                    jobManager.addJob(startDataJob);
                    dismiss();
                    break;
                default:
                    break;
            }
            dismiss();
        }
    }

    @Override
    public void onBackPressed() {
        if (tagCurrentVisibleFragment.equals(HomeFragment.TAG)) {
            super.onBackPressed();
        } else if (tagCurrentVisibleFragment.equals(GraphFragment.TAG)) {
            showFragment(HomeFragment.TAG);
        } else if (tagCurrentVisibleFragment.equals(SettingFragment.TAG)) {
            settingFragment.adapter.saveSelected();
            showFragment(GraphFragment.TAG);
        }
    }

    public GraphFragment getGraphFragment() {
        return graphFragment;
    }

    /*
    public void onEventMainThread(StartDataEvent eventResult) {

        switch (eventResult.getResult()) {

            case OK:

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

                break;
            case FAIL:
                Toast.makeText(HomeActivity.this, eventResult.getErrorResponse().errorMessage, Toast.LENGTH_SHORT).show();
                break;
        }
    }
    */
}
