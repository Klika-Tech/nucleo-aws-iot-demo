package com.klikatech.nucleo.net;

import com.google.gson.FieldNamingPolicy;
import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.klikatech.nucleo.NucleoApplication;
import com.klikatech.nucleo.event.NucleoEvents;
import com.klikatech.nucleo.net.exeption.NetworkException;
import com.klikatech.nucleo.net.exeption.ServerException;
import com.squareup.okhttp.OkHttpClient;
import java.util.concurrent.TimeUnit;

import de.greenrobot.event.EventBus;
import retrofit.ErrorHandler;
import retrofit.RequestInterceptor;
import retrofit.RetrofitError;
import retrofit.client.OkClient;
import retrofit.client.Response;
import retrofit.converter.GsonConverter;


public class RestAdapter {

    public static retrofit.RestAdapter getRestAdapter() {
        OkHttpClient okHttpClient = new OkHttpClient();
        okHttpClient.setConnectTimeout(NetConstant.TIMEOUT, TimeUnit.MILLISECONDS);
        okHttpClient.setReadTimeout(NetConstant.TIMEOUT, TimeUnit.MILLISECONDS);
        OkClient client = new OkClient(okHttpClient);
        return new retrofit.RestAdapter.Builder()
                .setRequestInterceptor(new RequestInterceptor() {
                    @Override
                    public void intercept(RequestFacade request) {
                        //request.addHeader(NetConstant.HEADER_CONTENT, NetConstant.CONTENT_TYPE);
                    }
                })
                .setClient(client)
                .setLogLevel(NucleoApplication.IS_DEBUG ? retrofit.RestAdapter.LogLevel.FULL : retrofit.RestAdapter.LogLevel.NONE)
                .setConverter(new GsonConverter(getGson()))
                .setErrorHandler(new ReffErrorHandler())
                //.setEndpoint(Api.HOST_0)
                .setEndpoint(NucleoApplication.getInstance().getEndPointHost())
                .build();
    }

    private static Gson getGson() {
        return new GsonBuilder()
                .setFieldNamingPolicy(FieldNamingPolicy.LOWER_CASE_WITH_UNDERSCORES)
                .setPrettyPrinting()
                .excludeFieldsWithoutExposeAnnotation()
                .create();
    }

    private static class ReffErrorHandler implements ErrorHandler {

        @Override
        public Throwable handleError(RetrofitError cause) {
            //logger.error(String.format("handle error in restadapter %1$s kind of %2$s", cause.getMessage(), cause.getKind()));
            if (cause.getKind() == RetrofitError.Kind.NETWORK) {
                EventBus.getDefault().post(NucleoEvents.Global.NETWORK_FAIL);
                return new NetworkException("network fails", cause);
            } else if (cause.getKind() == RetrofitError.Kind.CONVERSION) {
                throw new RuntimeException("conversion fails", cause);
            } else if (cause.getKind() == RetrofitError.Kind.HTTP) {

                Response response = cause.getResponse();
                int respCode = response.getStatus();

                if (respCode == NetConstant.CODE_500_SERVER_ERROR) {
                    EventBus.getDefault().post(NucleoEvents.Global.INTERNAL_SERVER_ERROR);
                    return new ServerException("server fails", cause);
                } else {
                    throw new RuntimeException("not handled", cause);
                }
            }
            return cause;
        }
    }
}
