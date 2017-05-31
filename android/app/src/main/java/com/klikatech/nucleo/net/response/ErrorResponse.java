package com.klikatech.nucleo.net.response;

public class ErrorResponse {

    public int errorCode;
    public String errorMessage = "No data from server";

    public String getMessage() {return errorMessage;}

}