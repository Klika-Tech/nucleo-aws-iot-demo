package com.klikatech.nucleo.net.exeption;


public class ServerException extends Throwable {

    public ServerException(String detailMessage, Throwable cause) {
        super(detailMessage, cause);
    }
}
