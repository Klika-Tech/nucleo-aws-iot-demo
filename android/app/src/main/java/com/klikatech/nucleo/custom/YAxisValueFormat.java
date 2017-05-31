package com.klikatech.nucleo.custom;

import com.github.mikephil.charting.components.YAxis;
import com.github.mikephil.charting.formatter.YAxisValueFormatter;

import java.util.Locale;

public class YAxisValueFormat implements YAxisValueFormatter {

    @Override
    public String getFormattedValue(float value, YAxis yAxis) {
        String res = String.format(Locale.ENGLISH, "%.2f", value);
        return res.indexOf(".") < 0 ? res : res.replaceAll("0*$", "").replaceAll("\\.$", "");
    }
}
