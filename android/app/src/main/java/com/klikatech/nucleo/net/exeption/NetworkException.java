package com.klikatech.nucleo.net.exeption;


public class NetworkException extends Throwable {

    public NetworkException(String detailMessage, Throwable cause) {
        super(detailMessage, cause);
    }
}
