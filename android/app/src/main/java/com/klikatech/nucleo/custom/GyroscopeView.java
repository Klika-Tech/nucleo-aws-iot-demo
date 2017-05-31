package com.klikatech.nucleo.custom;

import android.content.Context;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.ImageView;

import com.klikatech.nucleo.R;

public class GyroscopeView extends FrameLayout {

    ImageView gyro;

    public GyroscopeView(Context context) {
        this(context, null);
    }

    public GyroscopeView(Context context, AttributeSet attrs) {
        super(context, attrs);
        View view = LayoutInflater.from(context).inflate(R.layout.view_gyroscope, this, true);
        gyro = (ImageView) view.findViewById(R.id.value);
    }

    public void setAngle(float angle) {
        gyro.setRotation(angle);
    }

    public void setType(int t){
        switch (t){
            case 1:
                gyro.setImageResource(R.mipmap.gyro_x);
                break;
            case 2:
                gyro.setImageResource(R.mipmap.gyro_y);
                break;
            case 3:
                gyro.setImageResource(R.mipmap.gyro_z);
                break;
            default:
                break;
        }
    }
}
