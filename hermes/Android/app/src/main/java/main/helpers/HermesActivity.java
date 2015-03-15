package main.helpers;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.IBinder;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import com.oplabs.hermes.R;
import communication.Communication;
import main.Application.SessionData;

public abstract class HermesActivity extends Activity implements interfaces.CommunicationDelegate{

    //Message Handler
    public ActivityMessageHandler mhandler;

    //Tag
    protected final String TAG = "Hermes Activity";

    //Services
    protected boolean isBound;
    protected Communication commService;

    //Automatic Reference Counting Before we Destroy Service
    protected static int ActivitesThatNeedService = 0;

    //Data
    protected SessionData data;

    @Override
    protected void onStart()
    {
        super.onStart();
        isBound = false;
        commService = null;
        mhandler = new ActivityMessageHandler(this);

        data = SessionData.getInstance();
        data.sync(this);
        //Start the Services if not already started
        if( ActivitesThatNeedService == 0 )
        {
            Log.i("Hermes Activity", "Trying to start the service");
            startService(new Intent(this, Communication.class));
        }
        ActivitesThatNeedService+=1;

        //Bind
        Intent intent = new Intent(this, Communication.class);
        Log.i(TAG, "Binding an activiy");
        bindService(intent, myConnection, Context.BIND_AUTO_CREATE);
    }

    @Override
    protected void onStop()
    {
        super.onStop();

        //Unbind the Services
        unbindService(myConnection);
        ActivitesThatNeedService -= 1;

        if( ActivitesThatNeedService == 0)
        {
            Log.i("Hermes Activity", "Trying to stop the service");
            stopService(new Intent(this, Communication.class));
            //logout
            data.setSessionId(null);
        }
    }

    @Override
    public void notifyComm()
    {
        if (data.getSessionId() != null)
        {
            if(data.getSessionId().equals("ERROR"))
            {
                new AlertDialog.Builder(this)
                        .setTitle("Login Error")
                        .setMessage("There was an error connecting to the server.")
                        .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int which) {

                            }
                        })
                        .setIcon(R.drawable.ic_launcher)
                        .show();
                data.setSessionId(null);
            }
            else if(data.getSessionId().equals("DOMAIN"))
            {
                new AlertDialog.Builder(this)
                        .setTitle("Login Error")
                        .setMessage("There was an error finding your server. Are you sure you have the right hostname? Please check the app settings for the correct domain!")
                        .setPositiveButton("Ok Ill check!", new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int which) {
                                goToSettings(null);
                            }
                        })
                        .setIcon(R.drawable.ic_launcher)
                        .show();
            }
            else
            {
                Log.i(TAG, "Successful Login");
            }
        }
        else
        {
            new AlertDialog.Builder(this)
                    .setTitle("Login Error")
                    .setMessage("You have invalid credentials.")
                    .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int which) {
                            // login
                            goToLogin(null);
                        }
                    })
                    .setIcon(R.drawable.ic_launcher)
                    .show();
        }
        //End Login
        endLogin();
    }

    //Check login
    public boolean checkLogin()
    {
        Log.i(TAG, "Check Login");

        if( commService == null)
        {
            Log.e(TAG, "Comm Service is NULL");
        }

        if( !(data.getSessionId() == null) && !(data.getSessionId().equals(""))) {
            return false;
        }

        if( (data.getEmail().equals("")&&data.getPassword().equals("")))
        {
            new AlertDialog.Builder(this)
                    .setTitle("Welcome")
                    .setMessage("Welcome to hermes. Please set up your login information.")
                    .setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int which) {
                            // login
                            goToLogin(null);
                        }
                    })
                    .setIcon(R.drawable.ic_launcher)
                    .show();
            return false;
        }
        else if (data.getHostname().equals("") )
        {
            new AlertDialog.Builder(this)
                    .setTitle("Error")
                    .setMessage("Invalid Hostname configured.")
                    .setPositiveButton(android.R.string.yes, new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int which) {
                            // login
                            goToSettings(null);
                        }
                    })
                    .setIcon(R.drawable.ic_launcher)
                    .show();
            return false;
        }

        return true;
    }

    //Marks Login
    public void startLogin()
    {
        Log.e(TAG, "Start Login: Should Not be called by HERMES ACTIVITY. This method should be overloaded");
    }
    public void endLogin()
    {
        Log.e(TAG, "End Login: Should Not be called by HERMES ACTIVITY. This method should be overloaded");
    }

    //Service Connection
    protected ServiceConnection myConnection = new ServiceConnection() {
        public void onServiceConnected(ComponentName className, IBinder service) {
            Communication.MyLocalBinder binder = (Communication.MyLocalBinder) service;
            commService = binder.getService();
            isBound = true;

            Log.i(TAG, "OnServiceConnected checklogin");

            if( commService == null)
            {
                Log.e(TAG, "Comm Service is NULL");
            }
            //Check the login when the service is bound

            if( checkLogin())
            {
                commService.login(HermesActivity.this);
                startLogin();
            }
        }
        public void onServiceDisconnected(ComponentName arg0) {
            isBound = false;
            commService = null;
        }

        public void completedLogin()
        {
            notifyComm();
        }

    };

    //Methods to see menu
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        if (id == R.id.action_settings) {
            goToSettings(null);
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    //Required Overloads

    public abstract void goToLogin(View view);
    public abstract void goToSettings(View view);

}
