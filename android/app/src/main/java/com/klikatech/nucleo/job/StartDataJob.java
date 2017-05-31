package com.klikatech.nucleo.job;

import com.klikatech.nucleo.NucleoApplication;
import com.klikatech.nucleo.R;
import com.klikatech.nucleo.event.EventWrapper;
import com.klikatech.nucleo.event.StartDataEvent;
import com.klikatech.nucleo.net.Api;
import com.klikatech.nucleo.net.RestAdapter;
import com.klikatech.nucleo.net.exeption.NetworkException;
import com.klikatech.nucleo.net.response.ErrorResponse;
import com.klikatech.nucleo.net.response.StartDataResponse;
import com.path.android.jobqueue.Params;

import java.util.HashMap;
import java.util.Map;

import de.greenrobot.event.EventBus;
import retrofit.RetrofitError;


public class StartDataJob extends BaseJob {

    public static final int PRIORITY = 1;
    private Map<String, String> options = new HashMap<>();

    public StartDataJob(String since) {
        super(new Params(PRIORITY));
        event = new StartDataEvent();

        //?metric=temperature&since=1460367696"

        this.options.put("metric", "temperature");
        this.options.put("since", since);
    }

    @Override
    public void onAdded() {

    }

    @Override
    public void onRun() throws Throwable {

        Api.DataService startDataService = RestAdapter.getRestAdapter().create(Api.DataService.class);

        StartDataResponse startDataResponse = startDataService.startData(options);

        event.setResult(EventWrapper.Result.OK);
        event.setEntity(startDataResponse);

        EventBus.getDefault().post(event);
    }

    @Override
    protected void onCancel() {

    }

    @Override
    protected boolean shouldReRunOnThrowable(Throwable throwable) {

        ErrorResponse error = new ErrorResponse();
        error.errorMessage = NucleoApplication.getContext().getString(R.string.server_error_text);


        event.setErrorResponse(error);

        event.setResult(EventWrapper.Result.FAIL);
        EventBus.getDefault().post(event);

        if (throwable != null) {
            if (throwable instanceof RetrofitError) {
                RetrofitError retrofitError = (RetrofitError) throwable;
                if (retrofitError.getKind() == RetrofitError.Kind.HTTP) {

                    /*
                    if (retrofitError.getResponse().getStatus() == NetConstants.CODE_400_BAD_REQUEST) {
                        message = PricePatrolApplication.getContext().getString(R.string.message_wrong_confirm_code);
                    }
                    */

                } else if (retrofitError.getKind() == RetrofitError.Kind.NETWORK) {
                    return false; // network error already handled in RestAdapter
                }
            } else if (throwable.getCause() instanceof NetworkException) {
                return false; // network error already handled in RestAdapter
            }
        }

        return false;
    }


}
