package com.kaze.app;

import android.app.Activity;
import android.content.Context;
import android.content.res.AssetManager;
import android.media.AudioManager;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.ArrayList;

public class Kaze
{
    public static void init(Activity activity)
    {
        nativeInit(activity);
        provideAssetManager(activity.getAssets());

        AudioManager am = (AudioManager)activity.getSystemService(Context.AUDIO_SERVICE);

        // Get the default audio sample rate and buffer size
        String sampleRateStr = am.getProperty(AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE);
        int sampleRate = Integer.parseInt(sampleRateStr);

        String framesPerBufferStr = am.getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER);
        int framesPerBuffer = Integer.parseInt(framesPerBufferStr);

        provideAudioDefaults(sampleRate == 0 ? 48000 : sampleRate, framesPerBuffer == 0 ? 256 : framesPerBuffer);

        java.io.File dataDir = activity.getFilesDir();
        provideDataDirectory(dataDir.toString());
    }

    public static void close()
    {
        nativeClose();
    }

    public static void sendHTTPRequest(String endpoint, String method, String mimeType, ArrayList<String> headers,
        long callback, long userptr)
    {
        new Thread(() -> {
            try {
                HttpResponse res = sendHTTPRequestSync(endpoint, method, mimeType, headers);
                if (res.ok())
                    doHttpCallback(res.status, res.body, res.headers, res.cookies, callback, userptr);
                else
                    doHttpCallback(res.status, res.error, res.headers, res.cookies, callback, userptr);
            }
            catch(Exception e)
            {
                // Status -1 means that a system exception was thrown
                doHttpCallback( -1, e.getMessage(), new ArrayList<String>(), new ArrayList<String>(), callback, userptr);
            }
        }).start();
    }

    public static HttpResponse sendHTTPRequestSync(String endpoint, String method, String mimeType, ArrayList<String> headers) {
        HttpResponse res = new HttpResponse();
        HttpURLConnection connection = null;

        try {
            URL url = new URL(endpoint);
            connection = (HttpURLConnection) url.openConnection();
            connection.setRequestMethod(method);
            connection.setRequestProperty("Content-Type", mimeType);

            for (int i = 0; i < headers.size() - 1; i += 2) {
                connection.setRequestProperty(headers.get(i), headers.get(i + 1));
            }

            int status = connection.getResponseCode();

            // Read the payload from stream
            BufferedReader reader;
            if (status == HttpURLConnection.HTTP_OK) {
                reader = new BufferedReader(
                    new InputStreamReader(connection.getInputStream())
                );
            } else {
                reader = new BufferedReader(
                    new InputStreamReader(connection.getErrorStream())
                );
            }

            StringBuilder payload = new StringBuilder();
            String line;

            while ((line = reader.readLine()) != null) {
                payload.append(line);
            }
            reader.close();

            res.status = status;
            if (status == HttpURLConnection.HTTP_OK)
                res.body = payload.toString();
            else
                res.error = payload.toString();

            // Get headers / cookies
            int i = 0;
            String headerKey;
            while ((headerKey = connection.getHeaderFieldKey(i)) != null)
            {
                headerKey = headerKey.toLowerCase();
                if (headerKey.equals("set-cookie"))
                {
                    String value = connection.getHeaderField(i);
                    if (value != null)
                        res.cookies.add(value);
                }
                else
                {
                    res.headers.add(headerKey);
                    String value = connection.getHeaderField(i);
                    res.headers.add( (value == null) ? "" : value);
                }
                ++i;
            }


            return res;
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            if (connection != null) {
                connection.disconnect();
            }
        }

        return new HttpResponse();
    }

    private static native void nativeInit(Activity activity);
    private static native void nativeClose();
    private static native void provideAssetManager(AssetManager mgr);
    private static native void provideAudioDefaults(int sampleRate, int framesPerBuffer);
    private static native void provideDataDirectory(String dataPath);
    private static native void doHttpCallback(int status, String body,
        ArrayList<String> headers, ArrayList<String> cookies, long callback, long userdata);
}
