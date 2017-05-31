package com.klikatech.nucleo.event;

import com.klikatech.nucleo.net.response.ErrorResponse;

public abstract class EventWrapper {

    private ErrorResponse errorResponse;

    public void setErrorResponse(ErrorResponse errorResponse) {
        this.errorResponse = errorResponse;
    }

    public ErrorResponse getErrorResponse() {
        return errorResponse;
    }

    public enum Result{OK,FAIL}
    private Result result;

    public Result getResult() {
        return result;
    }

    public void setResult(Result result) {
        this.result = result;
    }

    public abstract Object getEntity();
    public abstract void setEntity(Object object);

}
