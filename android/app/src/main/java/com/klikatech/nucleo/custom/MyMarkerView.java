
package com.klikatech.nucleo.custom;

import android.content.Context;
import android.widget.TextView;

import com.github.mikephil.charting.components.MarkerView;
import com.github.mikephil.charting.data.CandleEntry;
import com.github.mikephil.charting.data.Entry;
import com.github.mikephil.charting.highlight.Highlight;
import com.github.mikephil.charting.utils.Utils;
import com.klikatech.nucleo.Constants;
import com.klikatech.nucleo.NucleoApplication;
import com.klikatech.nucleo.R;
import com.klikatech.nucleo.net.response.StartDataResponse;

import java.text.SimpleDateFormat;
import java.util.Locale;
import java.util.TimeZone;

public class MyMarkerView extends MarkerView {

    private TextView tvContent;

    private StartDataResponse startDataResponse;

    public MyMarkerView(Context context, int layoutResource, StartDataResponse startDataResponse) {
        super(context, layoutResource);

        tvContent = (TextView) findViewById(R.id.tvContent);

        this.startDataResponse = startDataResponse;
    }

    // callbacks everytime the MarkerView is redrawn, can be used to update the
    // content (user-interface)
    @Override
    public void refreshContent(Entry e, Highlight highlight) {

        SimpleDateFormat dateFormatTime = new SimpleDateFormat("HH:mm:ss");
        dateFormatTime.setTimeZone(TimeZone.getTimeZone("UTC"));

        String time = "";
        if (startDataResponse.sensorData != null && startDataResponse.sensorData.size() != 0) {
            try {
                time = dateFormatTime.format(startDataResponse.sensorData.get(e.getXIndex()).timestamp);
            } catch (IndexOutOfBoundsException e1) {
                e1.printStackTrace();
            }
        }

        if (e instanceof CandleEntry) {

            CandleEntry ce = (CandleEntry) e;

            if (!NucleoApplication.getInstance().isFarengate)
                tvContent.setText(Utils.formatNumber(ce.getHigh(), 2, true) + " \u2103" + " @" + time);
            else {

                tvContent.setText(Utils.formatNumber(9 * ce.getHigh() / 5 + 32, 2, true) + " \u2109" + " @" + time);
            }


        } else {

            switch (NucleoApplication.getInstance().sensor_type) {
                case Constants.S_TEMP:
                    if (!NucleoApplication.getInstance().isFarengate)
                        tvContent.setText(String.format(Locale.ENGLISH, "%.2f", e.getVal()) + " \u2103" + " @" + time);
                    else
                        tvContent.setText(String.format(Locale.ENGLISH, "%.2f", 9 * e.getVal() / 5 + 32) + " \u2109" + " @" + time);
                    break;
                case Constants.S_HUM:
                    tvContent.setText(String.format(Locale.ENGLISH, "%.2f", e.getVal()) + " %" + " @" + time);
                    break;
                case Constants.S_MAGN:
                    tvContent.setText(String.format(Locale.ENGLISH, "%.2f", e.getVal()) + " \u00B5T" + " @" + time);
                    break;
                case Constants.S_BAR:
                    tvContent.setText(String.format(Locale.ENGLISH, "%.2f", e.getVal()) + " hPa" + " @" + time);
                    break;
                case Constants.S_G:
                    tvContent.setText(String.format(Locale.ENGLISH, "%.2f", e.getVal()) + " \u00B0" + " @" + time);
                    break;
                case Constants.S_A:
                    tvContent.setText(String.format(Locale.ENGLISH, "%.2f", e.getVal()) + " g" + " @" + time);
                    break;
                default:
                    break;
            }


        }
    }

    @Override
    public int getXOffset(float xpos) {
        // this will center the marker-view horizontally
        return -(getWidth() / 2);
    }

    @Override
    public int getYOffset(float ypos) {
        // this will cause the marker-view to be above the selected value
        return -getHeight();
    }
}
