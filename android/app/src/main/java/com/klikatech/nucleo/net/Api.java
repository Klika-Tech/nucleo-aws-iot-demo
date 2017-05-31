package com.klikatech.nucleo.net;

import com.klikatech.nucleo.net.response.StartDataResponse;

import java.util.Map;

import retrofit.http.GET;
import retrofit.http.POST;
import retrofit.http.QueryMap;


public class Api {

    //public static final String HOST_0 ="https://v7yns2sew7.execute-api.us-east-1.amazonaws.com";

    public interface DataService {
        @GET("/prod/getNucleoData")
        StartDataResponse startData(@QueryMap Map<String, String> options);
    }
}
