package com.klikatech.nucleo.event;

import com.klikatech.nucleo.net.response.StartDataResponse;

import java.util.List;

public class StartDataEvent extends EventWrapper {

    private StartDataResponse mEntity;

    @Override
    public Object getEntity() {
        return mEntity;
    }

    @Override
    public void setEntity(Object object) {
        mEntity = (StartDataResponse) object;
    }
}
