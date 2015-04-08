package communication.Helpers;

import android.app.Service;
import android.content.Intent;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;

import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.HttpVersion;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.conn.ClientConnectionManager;
import org.apache.http.conn.scheme.PlainSocketFactory;
import org.apache.http.conn.scheme.Scheme;
import org.apache.http.conn.scheme.SchemeRegistry;
import org.apache.http.conn.ssl.SSLSocketFactory;
import org.apache.http.entity.StringEntity;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.impl.conn.tsccm.ThreadSafeClientConnManager;
import org.apache.http.params.BasicHttpParams;
import org.apache.http.params.HttpParams;
import org.apache.http.params.HttpProtocolParams;
import org.apache.http.protocol.HTTP;
import org.json.JSONArray;
import org.json.JSONObject;
import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.URLEncoder;
import java.security.KeyStore;
import java.util.ArrayList;
import java.util.List;

import communication.Communication;
import main.Application.SessionData;
import tester.TestService;
import tester.helpers.TestMsg;
import tester.helpers.TestSettings;

import static android.os.Message.obtain;

//This handler is running on the Comm Thread and will handle all communication related events
public class CommMessageHandler extends Handler {

    private final String TAG = "CommMessageHandler";
    private final String LoginURL ="/api/auth/login";
    private final String StartTestURL = "/api/test_set/create";
    private final String StartMobileURL = "/api/start_test/mobile";
    private final String StartRouterURL = "/api/start_test/router";

    private SessionData data;

    public CommMessageHandler()
    {
        data = SessionData.getInstance();
    }

    //This handles all messages sent from the service
    //They are processed on the Comm Thread
	@Override
	public void handleMessage(Message msg) {
        switch (msg.what) {
            // things that this thread should do
        
            case CommMsg.QUIT:
                Looper.myLooper().quit();
                break;

            case CommMsg.LOGIN:
                loginToServer( (Service) msg.obj);
                break;

            case CommMsg.REQUEST_TEST:
                serverStartTest((Handler) msg.obj);
        }
	}

    //The helper method that is used to start a test. It reaches out to the controller and gets
    //Test configurations. It is run on the Communication Thread
    private void serverStartTest(Handler sender)
    {
        int id = getSetId(sender);

        //Each method will send an error if it breaks
        if( id >= 0)
        {
            //Get the acctual settings
            TestSettings settings = new TestSettings();
            settings.setSetId(id);

            if( startMobileTest(sender, settings))
            {
               // if( startRouterTest(sender, settings))
                //{
                    Log.i(TAG, "Successfully told the router to start all tests");
                    Message msg = obtain();
                    msg.what = TestMsg.START_TEST;
                    msg.obj = settings;
                    sender.sendMessage(msg);
                //}
            }

        }
    }

    //This is the helper method that reaches out to the server and gets login information
    //It is run on the Communication Thread
    private void loginToServer(Service sender)
    {
        Log.i(TAG, "Login to server");

        HttpClient client = this.createHttpClient();
        HttpPost post = new HttpPost(data.getHostname()+LoginURL);

        try {

            post.setHeader("Content-type", "application/x-www-form-urlencoded");
            post.setEntity(new StringEntity("password=" + data.getPassword() + "&email=" + data.getEmail(), "UTF-8"));

            HttpResponse response = client.execute(post);
            HttpEntity entity = response.getEntity();

            InputStream inputStream = null;

            try {
                inputStream = entity.getContent();
                String result;
                // json is UTF-8 by default
                BufferedReader reader = new BufferedReader(new InputStreamReader(inputStream, "UTF-8"), 8);
                StringBuilder sb = new StringBuilder();

                String line;
                while ((line = reader.readLine()) != null) {
                    String addline = line + "\n";
                    sb.append(addline);
                }
                result = sb.toString();

                //JSON Parser
                JSONObject json = new JSONObject(result);
                if (json.getString("status").equals("success")) {
                    data.setSessionId(json.getString("user_token"));
                } else {
                    data.setSessionId(null);
                }

                Log.i(TAG, "Received Login: " + result);
            } catch (Exception e) {
                Log.e(TAG, "Error parsing json", e);
                data.setSessionId("DOMAIN");
            } finally {
                try {
                    if (inputStream != null) inputStream.close();
                } catch (Exception s) {
                    Log.e(TAG, "Could not close stream");
                }
            }
        } catch (Exception e) {
            Log.e(TAG, "Error creating Post", e);
            data.setSessionId("ERROR");
        }

        Intent intent = new Intent();
        intent.setAction("LoginComplete");

        ((Communication)sender).sendBroadcast(intent);

    }

    //Allows us to self sign our own certificates. Should be removed when we get real certs
    private HttpClient createHttpClient() {
        try {
            KeyStore trustStore = KeyStore.getInstance(KeyStore.getDefaultType());
            trustStore.load(null, null);

            SSLSocketFactory sf = new HermesSocketFactory(trustStore);
            sf.setHostnameVerifier(SSLSocketFactory.ALLOW_ALL_HOSTNAME_VERIFIER);

            HttpParams params = new BasicHttpParams();
            HttpProtocolParams.setVersion(params, HttpVersion.HTTP_1_1);
            HttpProtocolParams.setContentCharset(params, HTTP.DEFAULT_CONTENT_CHARSET);
            HttpProtocolParams.setUseExpectContinue(params, true);

            SchemeRegistry schReg = new SchemeRegistry();
            schReg.register(new Scheme("http", PlainSocketFactory.getSocketFactory(), 80));
            schReg.register(new Scheme("https", sf, 443));
            ClientConnectionManager conMgr = new ThreadSafeClientConnManager(params, schReg);

            return new DefaultHttpClient(conMgr, params);
        } catch (Exception e) {
            Log.e(TAG, "Error with Setting Key", e);
        }
        return null;
    }

    private Boolean startRouterTest(Handler sender, TestSettings settings)
    {
        Log.i(TAG, "startRouterTest");
        //Ask the server for a config and then give it back to the tester
        //We are going to notify directly because the Tester service will add a async message to its
        //subsystem
        HttpClient client = this.createHttpClient();
        HttpPost post = new HttpPost(data.getHostname()+StartRouterURL);

        try {
            post.setHeader("Content-type", "application/x-www-form-urlencoded");

            post.setHeader("Content-type", "application/x-www-form-urlencoded");
            String postString = new String();
            postString += "user_token=" + URLEncoder.encode(data.getSessionId() , "UTF-8");
            postString += "&";
            postString += "set_id=" + URLEncoder.encode(Integer.toString(settings.getSetId()), "UTF-8");
            post.setEntity(new StringEntity( postString));

            HttpResponse response = client.execute(post);

            HttpEntity entity = response.getEntity();

            InputStream inputStream = null;
            try {
                inputStream = entity.getContent();
                String result;

                // json is UTF-8 by default
                BufferedReader reader = new BufferedReader(new InputStreamReader(inputStream, "UTF-8"), 8);
                StringBuilder sb = new StringBuilder();

                String line;
                while ((line = reader.readLine()) != null) {
                    String addline = line + "\n";
                    sb.append(addline);
                }
                result = sb.toString();

                Log.i(TAG, "JSON from starting a router test = " + result);
                //JSON Parser
                JSONObject json = new JSONObject(result);

                //TODO:Create the settings object from the json
                Message msg = obtain();
                msg.what = TestMsg.START_TEST;
                msg.obj = null;


                if((json.getString("status").equals("success"))) {
                    Log.i(TAG, "Successful got router");

                    //TODO: Set the settings values
                    return true;
                }
                //Send the settings to the tester
                sender.sendMessage(msg);
            } catch (Exception e) {
                Log.e(TAG, "Error parsing Router json", e);
            } finally {
                try {
                    if (inputStream != null) inputStream.close();
                } catch (Exception s) {
                    Log.e(TAG, "Could not close stream");
                }
            }
        } catch (Exception e) {
            Log.e(TAG, "Error creating Post", e);
            data.setSessionId("ERROR");
        }
        return false;
    }

    private Boolean startMobileTest(Handler sender, TestSettings settings)
    {
        Log.i(TAG, "startMobileTest");
        //Ask the server for a config and then give it back to the tester
        //We are going to notify directly because the Tester service will add a async message to its
        //subsystem
        HttpClient client = this.createHttpClient();
        HttpPost post = new HttpPost(data.getHostname()+StartMobileURL);

        try {
            post.setHeader("Content-type", "application/x-www-form-urlencoded");
            String postString = new String();
            postString += "user_token=" + URLEncoder.encode(data.getSessionId() , "UTF-8");
            postString += "&";
            postString += "set_id=" + URLEncoder.encode(Integer.toString(settings.getSetId()), "UTF-8");

            post.setEntity(new StringEntity( postString));
            HttpResponse response = client.execute(post);

            HttpEntity entity = response.getEntity();

            InputStream inputStream = null;
            try {
                inputStream = entity.getContent();
                String result;

                // json is UTF-8 by default
                BufferedReader reader = new BufferedReader(new InputStreamReader(inputStream, "UTF-8"), 8);
                StringBuilder sb = new StringBuilder();

                String line;
                while ((line = reader.readLine()) != null) {
                    String addline = line + "\n";
                    sb.append(addline);
                }
                result = sb.toString();

                //JSON Parser
                JSONObject json = new JSONObject(result);
                Log.i(TAG, "JSON = " + result);
                if((json.getString("status").equals("success"))) {
                    Log.i(TAG, "Successful get settings");

                    //Get Invalid Data
                    JSONObject dns_config = json.getJSONObject("config").getJSONObject("dns_config");

                    JSONArray jArray = dns_config.getJSONArray("invalid_names");
                    if (jArray != null) {
                        for (int i=0;i<jArray.length();i++){
                            settings.addInvalidDomain(jArray.get(i).toString());
                        }
                    }

                    //Get Valid Names
                    jArray = dns_config.getJSONArray("valid_names");
                    if (jArray != null) {
                        for (int i=0;i<jArray.length();i++){
                            settings.addValidDomain(jArray.get(i).toString());
                        }
                    }

                    //Set Resolver
                    settings.setDNSServer(dns_config.getString("dns_server"));

                    //Set the timeout
                    settings.setTimeout(dns_config.getInt("timeout"));

                    return true;
                }
                else
                {
                    //Send an error
                    Message msg = obtain();
                    msg.what = TestMsg.START_TEST;
                    msg.obj = null;
                    sender.sendMessage(msg);
                }
                //Send the settings to the tester

            } catch (Exception e) {
                Log.e(TAG, "Error parsing json", e);
            } finally {
                try {
                    if (inputStream != null) inputStream.close();
                } catch (Exception s) {
                    Log.e(TAG, "Could not close stream");
                }
            }
        } catch (Exception e) {
            Log.e(TAG, "Error creating Post", e);
            data.setSessionId("ERROR");
        }
        return false;
    }

    private int getSetId(Handler sender)
    {
        Log.i(TAG, "getSetId");
        //Ask the server for a config and then give it back to the tester
        //We are going to notify directly because the Tester service will add a async message to its
        //subsystem
        HttpClient client = this.createHttpClient();
        HttpPost post = new HttpPost(data.getHostname()+StartTestURL);

        try {
            post.setHeader("Content-type", "application/x-www-form-urlencoded");
            post.setEntity(new StringEntity( "user_token=" + URLEncoder.encode(data.getSessionId() , "UTF-8"), "UTF-8"));
            HttpResponse response = client.execute(post);

            HttpEntity entity = response.getEntity();

            InputStream inputStream = null;
            try {
                inputStream = entity.getContent();
                String result;

                // json is UTF-8 by default
                BufferedReader reader = new BufferedReader(new InputStreamReader(inputStream, "UTF-8"), 8);
                StringBuilder sb = new StringBuilder();

                String line;
                while ((line = reader.readLine()) != null) {
                    String addline = line + "\n";
                    sb.append(addline);
                }
                result = sb.toString();

                //JSON Parser
                JSONObject json = new JSONObject(result);

                //TODO:Create the settings object from the json
                Message msg = obtain();
                msg.what = TestMsg.START_TEST;
                msg.obj = null;

                Log.i(TAG, "JSON = " + result);
                if((json.getString("status").equals("success"))) {
                    Log.i(TAG, "Successful start test");
                    return json.getInt("set_id"); //Return the set ID
                }
                //Send the settings to the tester
                sender.sendMessage(msg);
            } catch (Exception e) {
                Log.e(TAG, "Error parsing json", e);
            } finally {
                try {
                    if (inputStream != null) inputStream.close();
                } catch (Exception s) {
                    Log.e(TAG, "Could not close stream");
                }
            }
        } catch (Exception e) {
            Log.e(TAG, "Error creating Post", e);
            data.setSessionId("ERROR");
        }
        return -1;
    }
}
