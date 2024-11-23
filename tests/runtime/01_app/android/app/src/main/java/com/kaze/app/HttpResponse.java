package com.kaze.app;

import java.util.ArrayList;

public class HttpResponse
{
    public HttpResponse() {

        this.status = -1;
        this.headers = new ArrayList<>();
        this.cookies = new ArrayList<>();
    }

    public int status;
    public String body;
    public String error;
    public ArrayList<String> headers;
    public ArrayList<String> cookies;

    public boolean ok()
    {
        return status >= 200 && status < 300;
    }
}
