package com.klikatech.nucleo.custom;

import android.content.Context;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.RelativeLayout;

import com.klikatech.nucleo.R;

public class AccelerometerView extends RelativeLayout {

    View viewRoot;
    View positive;
    View negative;

    public AccelerometerView(Context context) {
        this(context, null);
    }

    public AccelerometerView(Context context, AttributeSet attrs) {
        super(context, attrs);
        View view = LayoutInflater.from(context).inflate(R.layout.view_accelerometer, this, true);
        viewRoot = view.findViewById(R.id.view);
        positive = view.findViewById(R.id.positive);
        negative = view.findViewById(R.id.negative);
    }

    public void setType(int xyz) {
        int colorID=R.color.text_home_x;
        switch (xyz) {
            case 0:
                colorID = R.color.text_home_x;
                break;
            case 1:
                colorID = R.color.text_home_y;
                break;
            case 2:
                colorID = R.color.text_home_z;
                break;
        }


        positive.setBackgroundColor(getResources().getColor(colorID));
        negative.setBackgroundColor(getResources().getColor(colorID));
    }

    public void setValue(float value) {
        float density = getResources().getDisplayMetrics().density;

        float maxHeight = (56 * density) / 2;

        if (value > 0) {
            RelativeLayout.LayoutParams paramsN = (RelativeLayout.LayoutParams) negative.getLayoutParams();
            paramsN.height = 0;
            negative.setLayoutParams(paramsN);

            RelativeLayout.LayoutParams paramsP = (RelativeLayout.LayoutParams) positive.getLayoutParams();
            paramsP.height = (int) (value * maxHeight);
            positive.setLayoutParams(paramsP);
        } else {
            RelativeLayout.LayoutParams paramsN = (RelativeLayout.LayoutParams) negative.getLayoutParams();
            paramsN.height = (int) (Math.abs(value) * maxHeight);
            negative.setLayoutParams(paramsN);

            RelativeLayout.LayoutParams paramsP = (RelativeLayout.LayoutParams) positive.getLayoutParams();
            paramsP.height = 0;
            positive.setLayoutParams(paramsP);
        }
    }
}
