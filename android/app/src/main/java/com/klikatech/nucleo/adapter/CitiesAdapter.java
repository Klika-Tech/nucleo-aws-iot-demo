package com.klikatech.nucleo.adapter;

import android.content.Context;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.CheckBox;
import android.widget.CompoundButton;

import com.klikatech.nucleo.NucleoApplication;
import com.klikatech.nucleo.R;

import java.util.ArrayList;
import java.util.List;

public class CitiesAdapter extends ArrayAdapter<String> {

    private final Context context;
    private final List<String> names;

    private final List<String> selectedNames;

    public CitiesAdapter(Context context, int resource, List<String> names) {
        super(context, resource, names);

        this.context = context;
        this.names = names;

        if(NucleoApplication.getInstance().getSelectedCity()==null || NucleoApplication.getInstance().getSelectedCity().size()==0)
            selectedNames = new ArrayList<String>();
        else
            selectedNames = NucleoApplication.getInstance().getSelectedCity();

    }

    @Override
    public View getView(final int position, View convertView, ViewGroup parent) {

        //View rowView = super.getView(position, convertView, parent);

        LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        View rowView = inflater.inflate(R.layout.item_city, parent, false);

        CheckBox cb = (CheckBox) rowView.findViewById(R.id.cb_city);
        cb.setText(names.get(position));

        if(selectedNames.contains(names.get(position)))
            cb.setChecked(true);
        else
            cb.setChecked(false);

        cb.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if(isChecked){
                    if(names.size()!=0)
                        selectedNames.add(names.get(position));
                }
                else{
                    if(selectedNames.contains(names.get(position)))
                        selectedNames.remove(names.get(position));
                }
            }
        });

        return rowView;
    }

    public void saveSelected(){
        NucleoApplication.getInstance().setSelectedCity(selectedNames);
    }
}
