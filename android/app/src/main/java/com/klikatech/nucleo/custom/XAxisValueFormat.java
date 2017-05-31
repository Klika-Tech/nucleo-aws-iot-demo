package com.klikatech.nucleo.custom;

import com.github.mikephil.charting.formatter.XAxisValueFormatter;
import com.github.mikephil.charting.utils.ViewPortHandler;

import java.sql.Timestamp;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;

public class XAxisValueFormat implements XAxisValueFormatter {
    @Override
    public String getXValue(String original, int index, ViewPortHandler viewPortHandler) {
        Date date = new Date(Long.parseLong(original) * 1000L);
        SimpleDateFormat sdf = new SimpleDateFormat("hh a");
        return sdf.format(date);
    }
}
