package main;

import android.content.Context;
import android.content.SharedPreferences;
import android.nfc.Tag;
import android.preference.PreferenceManager;
import android.util.Log;

import com.oplabs.hermes.R;

/**
 * Created by NBroeking on 2/18/15.
 */
public class SessionData {
    //Singleton Members
    private static SessionData ourInstance = new SessionData();

    public static SessionData getInstance() {
        return ourInstance;
    }

    public static SessionData getOurInstance() {
        return ourInstance;
    }

    //Tag
    private final String TAG = "Session Data";

    //Constructor

    private SessionData() {
        sessionId = null;
        email = null;
        password = null;
        hostname = null;
    }

    //Member Variables
    private String sessionId;
    private String email;
    private String password;
    private String hostname;

    public void save()
    {
        Log.i(TAG, "Save");

    }
    public void sync(Context context)
    {
        Log.i(TAG, "Sync");
        SharedPreferences sharedPref = PreferenceManager.getDefaultSharedPreferences(context);
        String e = sharedPref.getString("email", context.getResources().getString(R.string.defaultemail));
        String pass = sharedPref.getString("password", context.getResources().getString(R.string.defaultpassword));
        String host = sharedPref.getString("hostname", context.getResources().getString(R.string.defaulthostname));

        setEmail(e);
        setPassword(pass);
        setHostname(host);
    }
    //Getters and Setters
    public String getEmail() {
        synchronized (this) {
            return email;
        }
    }

    public void setEmail(String email) {
        synchronized (this) {
            this.email = email;
        }
    }

    public String getSessionId() {
        synchronized (this) {
            return sessionId;
        }
    }

    public void setSessionId(String sessionId) {
        synchronized (this) {
            this.sessionId = sessionId;
        }
    }

    public String getPassword() {
        synchronized (this) {
            return password;
        }
    }

    public void setPassword(String password) {
        synchronized (this) {
            this.password = password;
        }
    }

    public String getHostname() {
        synchronized (this) {
            return hostname;
        }
    }

    public void setHostname(String hostname) {
        synchronized (this) {
            this.hostname = hostname;
        }
    }
}
