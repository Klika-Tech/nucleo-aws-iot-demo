package com.klikatech.nucleo.net.exeption;

import com.google.gson.Gson;
import com.google.gson.annotations.Expose;

import java.util.List;

import retrofit.RetrofitError;


public class ValidationException extends RuntimeException {

    public String detailMessage;

    public ValidationException(RetrofitError cause) {
        super(cause.getMessage(), cause);
        Gson gson = new Gson();
        ValidationErrors validationErrors = gson.fromJson(cause.getResponse().toString(), ValidationErrors.class);
        if (validationErrors.validationErrors != null && validationErrors.validationErrors.size() != 0) {
            detailMessage = validationErrors.validationErrors.get(0).message;
        }
    }

    private static class ValidationErrors {
        public List<ValidationError> validationErrors;
    }

    private static class ValidationError {
        @Expose
        public String field;
        @Expose
        public String message;
    }
}
