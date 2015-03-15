package main.Views;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.view.View;

import com.oplabs.hermes.R;

import main.helpers.HermesActivity;
import tester.TestService;
import tester.TestState;

public class ResultsActivity extends HermesActivity {

    private final String TAG = "HermesResultsActivity";
    private boolean testBound;
    private TestService testService;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "OnCreate");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_results);
        if (savedInstanceState == null) {
            getFragmentManager().beginTransaction()
                    .add(R.id.ResultsFrame, new AnimationFragment())
                    .commit();
        }
        testBound = false;
        testService = null;
    }

    //Pretty much only called on an app start up
    public void checkStatus()
    {
        //Start Testing Process if IDLE
        if(testService.getState() == TestState.State.IDLE)
        {
            //Tell Tester that we have prepared
            testService.startTest();
        }
        else if( testService.getState() == TestState.State.PREPARING)
        {
            //We are preparing to Run a Test
        }
        else if(testService.getState() == TestState.State.COMPLETED)
        {
            //Get Results and move to results fragment
        }
        else
        {
            //We are in a testing state and we should update the view to show animation
        }
    }

    //On start and on Stop methods
    @Override
    protected void onStart() {
        super.onStart();
        Log.i(TAG, "OnStart");

        /*NOTE: The Hermes Activity will check to see that we are still logged in*/
        startService(new Intent(this, TestService.class));
        Intent intent = new Intent(this, TestService.class);
        bindService(intent, testConnection, Context.BIND_AUTO_CREATE);
    }

    @Override
    protected void onStop() {
        super.onStop();
        Log.i(TAG, "OnStop");

        if(!(testService == null)) {
            unbindService(testConnection);
            if (testService.getState() == TestState.State.IDLE  ||
                    testService.getState() == TestState.State.COMPLETED) {
                stopService(new Intent(this, TestService.class));
            }
        }
    }

    //View Methods
    public void goToLogin(View view) {
        //Temporary until we get a login page
        goToSettings(view);
    }

    public void goToSettings(View view) {
        Intent intent = new Intent(this, SettingsActivity.class);
        intent.putExtra(SettingsActivity.EXTRA_SHOW_FRAGMENT, SettingsActivity.GeneralPrefs.class.getName());
        intent.putExtra(SettingsActivity.EXTRA_NO_HEADERS, true);
        startActivity(intent);
    }

    //Methods Used to interact with the test service
    protected ServiceConnection testConnection = new ServiceConnection() {
        public void onServiceConnected(ComponentName className, IBinder service) {
            TestService.MyLocalBinder binder = (TestService.MyLocalBinder) service;
            testService = binder.getService();
            testBound = true;
            testService.setCommunicator(commService);
            checkStatus();
        }
        public void onServiceDisconnected(ComponentName arg0) {
            testBound = false;
            testService = null;
        }
    };
}
