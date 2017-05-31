package com.klikatech.nucleo.fragments;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CompoundButton;
import android.widget.ListView;
import android.widget.RadioButton;

import com.klikatech.nucleo.NucleoApplication;
import com.klikatech.nucleo.R;
import com.klikatech.nucleo.activity.HomeActivity;
import com.klikatech.nucleo.adapter.CitiesAdapter;

import butterknife.Bind;
import butterknife.ButterKnife;

public class SettingFragment extends Fragment {

    public static final String TAG = SettingFragment.class.getName();

    public CitiesAdapter adapter;

    View fragmentView;
    @Bind(R.id.vTempType)
    View vTempType;
    @Bind(R.id.textView3)
    View vCitiesTitle;
    @Bind(R.id.rb_line)
    RadioButton rbLine;
    @Bind(R.id.rb_area)
    RadioButton rbArea;
    @Bind(R.id.rb_celsius)
    RadioButton rbC;
    @Bind(R.id.rb_farengate)
    RadioButton rbF;
    @Bind(R.id.b_home)
    View home;
    @Bind(R.id.lv_cities)
    ListView lv;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        fragmentView = inflater.inflate(R.layout.fragment_settings, null);
        ButterKnife.bind(this, fragmentView);

        adapter = new CitiesAdapter(getActivity(), R.layout.item_city, NucleoApplication.getInstance().getNames());
        if (NucleoApplication.getInstance().getNames() != null)
            lv.setAdapter(adapter);

        home.findViewById(R.id.b_home).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                getActivity().onBackPressed();
                adapter.saveSelected();
            }
        });

        if (NucleoApplication.getInstance().isLine)
            rbLine.setChecked(true);
        else
            rbArea.setChecked(true);

        rbLine.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (rbLine.isChecked())
                    NucleoApplication.getInstance().isLine = true;
            }
        });

        rbArea.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (rbArea.isChecked())
                    NucleoApplication.getInstance().isLine = false;
            }
        });

        if (NucleoApplication.getInstance().isFarengate)
            rbF.setChecked(true);
        else
            rbC.setChecked(true);

        rbC.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (rbC.isChecked())
                    NucleoApplication.getInstance().isFarengate = false;
            }
        });

        rbF.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (rbF.isChecked())
                    NucleoApplication.getInstance().isFarengate = true;
            }
        });
        return fragmentView;
    }

    @Override
    public void onStart() {
        super.onStart();
        ButterKnife.bind(this, fragmentView);
    }

    @Override
    public void onStop() {
        super.onStop();
        ButterKnife.unbind(this);
    }

    @Override
    public void onHiddenChanged(boolean hidden) {
        super.onHiddenChanged(hidden);
        if (!hidden) {
            if (((HomeActivity) getActivity()).getGraphFragment().SENSOR != 0) {
                vTempType.setVisibility(View.GONE);
                lv.setVisibility(View.GONE);
                vCitiesTitle.setVisibility(View.GONE);
            } else {
                vTempType.setVisibility(View.VISIBLE);
                lv.setVisibility(View.VISIBLE);
                vCitiesTitle.setVisibility(View.VISIBLE);
                adapter = new CitiesAdapter(getActivity(), R.layout.item_city, NucleoApplication.getInstance().getNames());
                if (NucleoApplication.getInstance().getNames() != null)
                    lv.setAdapter(adapter);
            }
        }
    }
}
