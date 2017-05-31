package com.klikatech.nucleo.job;

import com.klikatech.nucleo.event.EventWrapper;
import com.path.android.jobqueue.Job;
import com.path.android.jobqueue.Params;


public abstract class BaseJob<T extends EventWrapper> extends Job {
    protected T event;

    protected BaseJob(Params params) {
        super(params);
    }
}
