package main.Views;

import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
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

    // When created we init everything to a empty state
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

    //Called when the activity starts.
    //We check what state we are in and edit the fragments accordingly
    public void checkStatus()
    {
        TestState stateMachine = TestState.getInstance();
        //Start Testing Process if IDLE
        if(stateMachine.getState() == TestState.State.IDLE)
        {
            //Tell Tester that we have prepared
            testService.startTest();
        }
        else if( stateMachine.getState() == TestState.State.PREPARING)
        {
            //We are preparing to Run a Test
        }
        else if(stateMachine.getState() == TestState.State.COMPLETED)
        {
            //Get Results and move to results fragment
        }
        else
        {
            //We are in a testing state and we should update the view to show animation
        }
    }

    //On start we need to check our state and subscribe to testing broadcasts
    @Override
    protected void onStart() {
        super.onStart();
        Log.i(TAG, "OnStart");

        /*NOTE: The Hermes Activity will check to see that we are still logged in*/
        startService(new Intent(this, TestService.class));
        Intent intent = new Intent(this, TestService.class);
        bindService(intent, testConnection, Context.BIND_AUTO_CREATE);

        //Set up broadcast receiver

        IntentFilter filter = new IntentFilter("TestingCompleted");
        registerReceiver(receiver, filter);
    }

    //On stop we clean up our broadcast requests and our states
    @Override
    protected void onStop() {
        super.onStop();
        Log.i(TAG, "OnStop");

        if(!(testService == null)) {
            unbindService(testConnection);
            if (TestState.getInstance().getState() == TestState.State.IDLE  ||
                    TestState.getInstance().getState() == TestState.State.COMPLETED) {
                stopService(new Intent(this, TestService.class));
            }
        }

        //Stop listening for broadcasts
        unregisterReceiver(receiver);
    }

    //We want to go straight to settings if this is ever called in this view.
    //The only thing that could cause this method to get called is an error
    public void goToLogin(View view) {
        //Temporary until we get a login page
        goToSettings(view);
    }

    //We transfer control to the settings activity. Can be called either on error or on button press
    public void goToSettings(View view) {
        Intent intent = new Intent(this, SettingsActivity.class);
        intent.putExtra(SettingsActivity.EXTRA_SHOW_FRAGMENT, SettingsActivity.GeneralPrefs.class.getName());
        intent.putExtra(SettingsActivity.EXTRA_NO_HEADERS, true);
        startActivity(intent);
    }

    //Reports a bind and unbind from the service
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

    //Used to receive broadcasts from the testing subsystem
    private BroadcastReceiver receiver = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {
            // Implement code here to be performed when
            // broadcast is detected
            Log.i(TAG, "Received Intent");
        }
    };
}
