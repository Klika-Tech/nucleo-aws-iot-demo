package com.klikatech.nucleo;

import android.app.Application;
import android.content.Context;
import android.content.pm.ApplicationInfo;

import com.amazonaws.regions.Regions;
import com.path.android.jobqueue.JobManager;
import com.path.android.jobqueue.config.Configuration;
import com.path.android.jobqueue.log.CustomLogger;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
import java.io.InputStream;
import java.util.List;

public class NucleoApplication extends Application {

    private static NucleoApplication applicationInst;
    private JobManager jobManager;
    private static Context mContext;
    public static boolean IS_DEBUG;

    public boolean isStart = false;
    public boolean isLine = false;
    public boolean isFarengate = false;
    public int sensor_type = -1;

    private List<String> names;
    private List<String> selectedNames;


    private String endpoint_hostname;
    private String topic;

    private String customer_specific_endpoint_prefix;
    private String cognito_pool_id;
    private String aws_iot_policy_name;
    private Regions my_region;
    private String keystore_name;
    private String keystore_password;
    private String certificate_id;

    public NucleoApplication() {
        super();
    }

    public static NucleoApplication getInstance() {
        return applicationInst;
    }


    @Override
    public void onCreate() {
        super.onCreate();

        IS_DEBUG = (0 != (getApplicationInfo().flags & ApplicationInfo.FLAG_DEBUGGABLE));

        applicationInst = this;
        configureJobManager();
        mContext = getApplicationContext();

        try {
            JSONObject settings = (new JSONObject(loadSettingsJSONFromAsset())).getJSONObject("settings");

            endpoint_hostname = settings.getString("endpoint_hostname");
            topic = settings.getString("topic");

            my_region = Regions.fromName(settings.getString("region"));

            customer_specific_endpoint_prefix = settings.getString("customer_specific_endpoint_prefix");
            cognito_pool_id = settings.getString("cognito_pool_id");
            aws_iot_policy_name = settings.getString("aws_iot_policy_name");
            keystore_name = settings.getString("keystore_name");
            keystore_password = settings.getString("keystore_password");
            certificate_id = settings.getString("certificate_id");

        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    public static Context getContext() {
        return mContext;
    }

    private void configureJobManager() {

        Configuration configuration = new Configuration.Builder(this)
                .customLogger(new CustomLogger() {
                    @Override
                    public boolean isDebugEnabled() {
                        return IS_DEBUG;
                    }

                    @Override
                    public void d(String text, Object... args) {
                    }

                    @Override
                    public void e(Throwable t, String text, Object... args) {
                    }

                    @Override
                    public void e(String text, Object... args) {
                    }
                })
                .minConsumerCount(0)//always keep at least one consumer alive
                .maxConsumerCount(7)//up to 5 consumers at a time
                .loadFactor(2)//3 jobs per consumer
                .consumerKeepAlive(15)//wait 15 seconds
                .build();
        jobManager = new JobManager(this, configuration);
    }

    public JobManager getJobManager() {
        return jobManager;
    }

    public List<String> getNames() {
        return names;
    }

    public void setNames(List<String> names) {
        this.names = names;
    }

    public void setSelectedCity(List<String> selected) {

        this.selectedNames = selected;
    }

    public List<String> getSelectedCity() {

        return this.selectedNames;
    }

    private String loadSettingsJSONFromAsset() {

        String json = null;
        try {
            InputStream is = getAssets().open("settings.json");
            int size = is.available();
            byte[] buffer = new byte[size];
            is.read(buffer);
            is.close();
            json = new String(buffer, "UTF-8");
        } catch (IOException ex) {
            ex.printStackTrace();
            return null;
        }

        return json;
    }

    public String getTopic() {
        return topic;
    }

    public String getEndPointHost() {
        return endpoint_hostname;
    }

    public Regions getRegion() {
        return my_region;
    }

    public String getEndpointPrefix() {
        return customer_specific_endpoint_prefix;
    }

    public String getPoolId() {
        return cognito_pool_id;
    }

    public String getIotPolicyName() {
        return aws_iot_policy_name;
    }

    public String getKeystoreName() {
        return keystore_name;
    }

    public String getKeystorePass() {
        return keystore_password;
    }

    public String getCertId() {
        return certificate_id;
    }

}

