package com.kaze.app;

import static java.lang.Integer.parseInt;

import android.app.Activity;
import android.content.Context;
import android.content.res.AssetManager;
import android.media.AudioManager;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.nio.charset.StandardCharsets;
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
        int sampleRate = parseInt(sampleRateStr);

        String framesPerBufferStr = am.getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER);
        int framesPerBuffer = parseInt(framesPerBufferStr);

        provideAudioDefaults(sampleRate == 0 ? 48000 : sampleRate, framesPerBuffer == 0 ? 256 : framesPerBuffer);

        java.io.File dataDir = activity.getFilesDir();
        provideDataDirectory(dataDir.toString());

        provideIsEmulator(isEmulator());
    }

    public static void close()
    {
        nativeClose();
    }

    public static void sendHTTPRequest(String endpoint, String method, String mimeType,
                                       java.nio.ByteBuffer body, ArrayList<String> headers, long requestId)
    {
        new Thread(() -> {
            try {
                HttpResponse res = sendHTTPRequestSync(endpoint, method, mimeType, body, headers);
                if (res.ok())
                    doHttpCallback(res.status, res.body, res.headers, res.cookies, requestId);
                else
                    doHttpCallback(res.status, res.error, res.headers, res.cookies, requestId);
            }
            catch(Exception e)
            {
                // Status -1 means that a system exception was thrown
                doHttpCallback( -1, e.getMessage(), new ArrayList<String>(), new ArrayList<String>(), requestId);
            }
        }).start();
    }

    private static boolean passBodyToConnection(java.nio.ByteBuffer body, String mimeType, HttpURLConnection connection) {
        if (body == null) return true; // OK when no body - just return

        connection.setDoOutput(true);
        connection.setRequestProperty("Content-Type", mimeType);

        int CHUNK_SIZE = 8192;
        if (body.hasArray()) // use internal buffer's array
        {
            byte[] bodyArr = body.array();
            try (OutputStream os = connection.getOutputStream())
            {

                int i = 0;
                while(i < bodyArr.length)
                {
                    int bytesLeft = bodyArr.length - i;
                    int toWrite = Math.min(bytesLeft, CHUNK_SIZE);
                    os.write(bodyArr, 0, toWrite);
                    i += toWrite;
                }

                return true;
            }
            catch (IOException e)
            {
                return false;
            }
        }

        try (OutputStream os = connection.getOutputStream())
        {
            byte[] chunk = new byte[CHUNK_SIZE];

            while (body.hasRemaining())
            {
                int bytesToWrite = Math.min(body.remaining(), chunk.length);
                body.get(chunk, 0, bytesToWrite);
                os.write(chunk, 0, bytesToWrite);
            }
        }
        catch (IOException e)
        {
            return false;
        }

        return true;
    }

    public static HttpResponse sendHTTPRequestSync(String endpoint, String method, String mimeType, java.nio.ByteBuffer body, ArrayList<String> headers) {
        HttpResponse res = new HttpResponse();
        HttpURLConnection connection = null;

        try {
            URL url = new URL(endpoint);
            connection = (HttpURLConnection) url.openConnection();
            connection.setRequestMethod(method);

            for (int i = 0; i < headers.size() - 1; i += 2) {
                connection.setRequestProperty(headers.get(i), headers.get(i + 1));
            }

            passBodyToConnection(body, mimeType, connection);

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

    private static boolean isEmulator()
    {
        String model = android.os.Build.MODEL;
        String product = android.os.Build.PRODUCT;
        String hardware = android.os.Build.HARDWARE;
        String qemu = System.getProperty("ro.kernel.qemu");

        return model.contains("google_sdk") ||
            model.contains("Emulator") ||
            product.contains("sdk") ||
            hardware.contains("goldfish") ||
            (qemu != null && qemu.equals("1"));
    }

    private static native void nativeInit(Activity activity);
    private static native void nativeClose();
    private static native void provideAssetManager(AssetManager mgr);
    private static native void provideAudioDefaults(int sampleRate, int framesPerBuffer);
    private static native void provideDataDirectory(String dataPath);
    private static native void provideIsEmulator(boolean isEmulator);
    private static native void doHttpCallback(int status, String body,
        ArrayList<String> headers, ArrayList<String> cookies, long requestId);
}
