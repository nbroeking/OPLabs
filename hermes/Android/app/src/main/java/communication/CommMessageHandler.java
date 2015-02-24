package communication;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;

import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.params.BasicHttpParams;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;

import main.SessionData;

public class CommMessageHandler extends Handler {

	final private int QUIT = 0;
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
                loginToServer();
                break;
        }
	}

    public void loginToServer()
    {
        Log.i(TAG, "Login to server");

        DefaultHttpClient client = new DefaultHttpClient(new BasicHttpParams());
        HttpPost post = new HttpPost(data.getHostname()+LoginURL);

        post.setHeader("Content-type", "application/json");

        InputStream inputStream = null;
        String result = null;

        try {
            HttpResponse response = client.execute(post);
            HttpEntity entity = response.getEntity();

            inputStream = entity.getContent();

            // json is UTF-8 by default
            BufferedReader reader = new BufferedReader(new InputStreamReader(inputStream, "UTF-8"), 8);
            StringBuilder sb = new StringBuilder();

            String line = null;
            while ((line = reader.readLine()) != null)
            {
                sb.append(line + "\n");
            }
            result = sb.toString();
        } catch (Exception e) {
            Log.e(TAG, "Error parsing json");
        }
        finally {
            try{if(inputStream != null)inputStream.close();}catch(Exception s){ Log.e(TAG, "Could not close stream");}
        }

        Log.i(TAG, "Received Login" + result);
    }
}
