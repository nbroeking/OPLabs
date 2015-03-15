package communication.Helpers;

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
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.security.KeyStore;

import main.helpers.HermesActivity;
import interfaces.CommunicationDelegate;
import main.helpers.MainMsg;
import main.Application.SessionData;
import tester.TestService;
import tester.helpers.TestMsg;

import static android.os.Message.obtain;

//This handler is running on the Comm Thread and will handle all communication related events
public class CommMessageHandler extends Handler {

    private final String TAG = "CommMessageHandler";
    private final String LoginURL ="/api/auth/login";

    private SessionData data;

    public CommMessageHandler()
    {
        data = SessionData.getInstance();
    }

	@Override
	public void handleMessage(Message msg) {
        switch (msg.what) {
            // things that this thread should do
        
            case CommMsg.QUIT:
                Looper.myLooper().quit();
                break;

            case CommMsg.LOGIN:
                loginToServer( (CommunicationDelegate) msg.obj);
                break;

            case CommMsg.REQUEST_TEST:
                serverStartTest((CommunicationDelegate) msg.obj);
        }
	}

    public void serverStartTest(CommunicationDelegate sender)
    {
       //Ask the server for a config and then give it back to the tester

        //We are going to notify directly because the Tester service will add a async message to its
        //subsystem
        Message msg = obtain();
        msg.what = TestMsg.START_TEST;
        msg.obj = null; //Config
        ((TestService)sender).getHandler().sendMessage(msg);
    }
    public void loginToServer(CommunicationDelegate sender)
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
                    data.setSessionId(json.getString("auth_token"));
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


        Message msg = obtain();
        msg.what = MainMsg.NOTIFYLOGIN;
        msg.obj = null;
        ((HermesActivity) sender).mhandler.sendMessage(msg);
    }

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
}
