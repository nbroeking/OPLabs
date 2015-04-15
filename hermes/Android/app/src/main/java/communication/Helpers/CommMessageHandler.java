package communication.Helpers;

import android.app.Service;
import android.content.Intent;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.util.Pair;

import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.HttpVersion;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
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
import org.apache.http.params.HttpConnectionParams;
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
import tester.TestResults;
import tester.TestService;
import tester.TestState;
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
    private final String ReportResultURL = "/api/test_result/%d/edit";
    private final String RouterResultsURL = "/api/test_result/%d";

    private SessionData data;

    private Boolean ShouldCheck;

    public CommMessageHandler()
    {
        data = SessionData.getInstance();
        ShouldCheck = false;
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
                break;

            case CommMsg.REPORT_TEST:
                Pair<Object, Object> obj = ((Pair <Object, Object> )msg.obj); //Tuple
                reportTest((TestResults)obj.first);

                TestResults results = (TestResults)obj.first;

                //If we had a router
                if( results.getRouter_id() >= 0) {
                    //After we report we want to request results from router
                    Message msgRequest = obtain();
                    msgRequest.what = CommMsg.REQUEST_ROUTER_RESULTS;
                    msgRequest.obj = msg.obj;

                    //Set the check to true so we keep trying
                    setShouldCheck(Boolean.TRUE);
                    sendMessage(msgRequest);
                }
                else{
                    Log.d(TAG, "No valid router to request results from");
                }
                break;

            case CommMsg.REQUEST_ROUTER_RESULTS:
                Log.i(TAG, "REQUESTING_ROUTER_RESULTS");
                requestTestResults((Pair<Object, Object>)msg.obj);
                break;

            case CommMsg.CLEAR:
                setShouldCheck(Boolean.FALSE);
                break;
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
            //Get the actual settings
            TestSettings settings = new TestSettings();
            settings.setSetId(id);

            if( startMobileTest(sender, settings))
            {
                //If there is an error with Start Router Test then we have null as a routerid
                if(startRouterTest(sender, settings)){
                    Log.i(TAG, "Successfully told the router to start all tests");
                }
                else{
                    Log.w(TAG, "Router returned an error. We probably don't have a compatible router.");
                }

                Message msg = obtain();
                msg.what = TestMsg.START_TEST;
                msg.obj = settings;
                sender.sendMessage(msg);
            }
        }
    }

    //This method gets a report from the server
    private void requestTestResults(Pair<Object, Object> pairResults){

        TestResults results = (TestResults) pairResults.first;
        Log.i(TAG, "Get Router Results");

        HttpPost post = new HttpPost(String.format(data.getHostname()+RouterResultsURL, results.getRouter_id()));
        try {

            post.setHeader("Content-type", "application/x-www-form-urlencoded");
            post.setEntity(new StringEntity("user_token=" + URLEncoder.encode(data.getSessionId(), "UTF-8")));

            //JSON Parser

            JSONObject json = sendPost(post);

            Log.d(TAG, "Response = " + json );
            if (json.getString("status").equals("success")) {
                Log.d(TAG, "Received Status success");
                switch (json.getString("state")){
                    case "finished":
                        Log.d(TAG, "We have router results");
                        TestResults routerResults = new TestResults(json);
                        routerResults.setValid();
                        TestState.getInstance().setRouterResults(routerResults);
                        Intent intent = new Intent();
                        intent.setAction("ReportRouter");
                        intent.putExtra(("Results"), routerResults);
                        ((Communication) pairResults.second).sendBroadcast(intent);
                        Log.d(TAG, "We sent the router results to whoever needs it");
                        break;
                    default:
                        Log.d(TAG, "Will try again in 15 seconds");
                        Message msg = Message.obtain();
                        msg.what = CommMsg.REQUEST_ROUTER_RESULTS;
                        msg.obj = pairResults;

                        if (shouldCheck()) {
                            sendMessageDelayed(msg, 5000);
                        }

                        break;
                }
            } else {
                throw new Exception("Status failed reporting Results");
            }

            Log.i(TAG, "Reported Results");
        } catch (Exception e) {
            Log.e(TAG, "Error getting router results", e);
        }
    }

    //This method reports our test to the controller
    private void reportTest(TestResults results){
        HttpPost post = new HttpPost(String.format(data.getHostname()+ReportResultURL, results.getId()));
        try {

            post.setHeader("Content-type", "application/x-www-form-urlencoded");
            post.setEntity(new StringEntity("user_token=" + URLEncoder.encode(data.getSessionId(), "UTF-8") +"&"+ results.getPost()));

            //JSON Parser
            JSONObject json = sendPost(post);
            if (json.getString("status").equals("success")) {
                Log.d(TAG, "Received Status success");
            } else {
                throw new Exception("Status failed reporting Results");
            }

            Log.i(TAG, "Reported Results");
        } catch (Exception e) {
            Log.e(TAG, "Error reporting results", e);
        }
    }
    //This is the helper method that reaches out to the server and gets login information
    //It is run on the Communication Thread
    private void loginToServer(Service sender)
    {
        Log.d(TAG, "Login to server");

        HttpPost post = new HttpPost(data.getHostname()+LoginURL);

        try {
            post.setHeader("Content-type", "application/x-www-form-urlencoded");
            post.setEntity(new StringEntity("password=" + data.getPassword() + "&email=" + data.getEmail(), "UTF-8"));

            //JSON Parser
            JSONObject json = sendPost(post);
            if (json.getString("status").equals("success")) {
                data.setSessionId(json.getString("user_token"));
            } else {
                data.setSessionId(null);
            }
        } catch (Exception e) {
            Log.e(TAG, "Error creating Post", e);
            data.setSessionId("ERROR");
        }
        Intent intent = new Intent();
        intent.setAction("LoginComplete");

        ((Communication)sender).sendBroadcast(intent);
    }

    //This method tells the controller that we want to start a router test
    //The controller will return the result id of the router set so we can fetch those results
    //when we need them
    private Boolean startRouterTest(Handler sender, TestSettings settings)
    {
        Log.d(TAG, "startRouterTest");
        HttpPost post = new HttpPost(data.getHostname()+StartRouterURL);

        try {
            //Create the post
            post.setHeader("Content-type", "application/x-www-form-urlencoded");
            String postString = new String();
            postString += "user_token=" + URLEncoder.encode(data.getSessionId() , "UTF-8");
            postString += "&";
            postString += "set_id=" + URLEncoder.encode(Integer.toString(settings.getSetId()), "UTF-8");
            post.setEntity(new StringEntity( postString));

            //Execute the post
            JSONObject json = sendPost(post);

            if((json.getString("status").equals("success"))) {
                Log.i(TAG, "Successful got router");
                settings.setRouterResultsID(json.getInt("result_id"));
                return true;
            }
            else{
                settings.setRouterResultsID(-1);
            }
        } catch (Exception e) {
            Log.e(TAG, "Error creating Post", e);
        }
        return false;
    }

    //This method will reach out to the controller and request that we start the mobile test
    //This is to let the controller know the state of our system
    private Boolean startMobileTest(Handler sender, TestSettings settings)
    {
        Log.d(TAG, "startMobileTest");
        HttpPost post = new HttpPost(data.getHostname()+StartMobileURL);
        try {
            //Create the post
            post.setHeader("Content-type", "application/x-www-form-urlencoded");
            String postString = new String();
            postString += "user_token=" + URLEncoder.encode(data.getSessionId() , "UTF-8");
            postString += "&";
            postString += "set_id=" + URLEncoder.encode(Integer.toString(settings.getSetId()), "UTF-8");
            post.setEntity(new StringEntity( postString));

            JSONObject json = sendPost(post);

            if((json.getString("status").equals("success"))) {
                Log.i(TAG, "Successful get settings");


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

                //Set the result ID
                settings.setResultID(json.getInt("result_id"));

                return true;
            }
            else {
                //Send an error
                Message msg = obtain();
                msg.what = TestMsg.START_TEST;
                msg.obj = null;
                sender.sendMessage(msg);
            }
        } catch (Exception e) {
            Log.e(TAG, "Error creating Post", e);
        }
        return false;
    }

    //Returns the Set_id from the server so we know what set our results go with
    private int getSetId(Handler sender)
    {
        Log.d(TAG, "getSetId");
        HttpPost post = new HttpPost(data.getHostname()+StartTestURL);

        try {
            post.setHeader("Content-type", "application/x-www-form-urlencoded");
            post.setEntity(new StringEntity( "user_token=" + URLEncoder.encode(data.getSessionId() , "UTF-8"), "UTF-8"));

            JSONObject json = sendPost(post);

            if((json.getString("status").equals("success"))) {
                Log.i(TAG, "Successful start test");
                return json.getInt("set_id"); //Return the set ID
            }

            //If an error we send the error to the sender
            Message msg = obtain();
            msg.what = TestMsg.START_TEST;
            msg.obj = null;

            //Send the settings to the tester
            sender.sendMessage(msg);

        } catch (Exception e) {
            Log.e(TAG, "Error creating Post", e);
        }
        return -1;
    }

    //This method will send a http get request and return the json object that is returned from the get request
    private JSONObject sendGet(HttpGet get){
        HttpClient client = this.createHttpClient();
        try {
            get.setHeader("Content-type", "application/x-www-form-urlencoded");
            HttpResponse response = client.execute(get);

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
                return json;

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
            Log.e(TAG, "Error creating Get Request", e);
        }
        return null;
    }

    //This method will send a http post and return the json object that is returned from that post
    private JSONObject sendPost(HttpPost post){
        HttpClient client = this.createHttpClient();
        try {
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
                return json;

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
        }
        return null;
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
            HttpConnectionParams.setConnectionTimeout(params, 8000);

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

    public Boolean shouldCheck() {
        synchronized (this) {
            return ShouldCheck;
        }
    }

    public void setShouldCheck(Boolean shouldCheck) {
        synchronized (this) {
            ShouldCheck = shouldCheck;
        }
    }
}
