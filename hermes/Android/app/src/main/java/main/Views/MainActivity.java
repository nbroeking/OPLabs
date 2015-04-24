package main.Views;

import android.app.ProgressDialog;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import com.oplabs.hermes.R;

import main.helpers.HermesActivity;
import tester.TestState;

public class MainActivity extends HermesActivity {

    private final String TAG = "HermesMainActivity";

    ProgressDialog dialog;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Log.i(TAG, "OnCreate");
    }

    //When we start we check the state and move to the appropriate activity
    @Override
    protected void onStart()
    {
        Log.i(TAG, "On Start");
        super.onStart();
        if(TestState.getInstance().getState() == TestState.State.IDLE)
        {
            Button button = (Button)findViewById(R.id.command);
            button.setText("Run Test");

            TestState.getInstance().setPhoneResults(null);
        }
        else
        {
            Button button = (Button)findViewById(R.id.command);
            button.setText("Get Results");
        }
    }

    //Used to do animations for the login screen
    @Override
    public void startLogin()
    {
        Log.e(TAG, "Start Login: Showing spinner");
        dialog = new ProgressDialog(this);
        dialog.setProgressStyle(ProgressDialog.STYLE_SPINNER);
        dialog.setMessage("Logging On ...");
        dialog.setIndeterminate(true);
        dialog.setCanceledOnTouchOutside(false);
        dialog.show();
    }

    //Used to stop the login animation
    @Override
    public void endLogin()
    {
        Handler handler = new Handler();
        handler.postDelayed(new Runnable() {
            public void run() {
                if( dialog != null) {
                    dialog.dismiss();
                }
            }}, 1000);

        Log.e(TAG, "Trying to dismiss");
    }
    //Called by button press to initiate a test
    public void runTest(View view) {
        Log.i(TAG, "RunTest");
        Intent myIntent = new Intent(MainActivity.this, ResultsActivity.class);
        MainActivity.this.startActivity(myIntent);
    }

    //Called by button press to go to the login page
    public void goToLogin(View view) {
        Intent myIntent = new Intent(MainActivity.this, LoginActivity.class);
        MainActivity.this.startActivityForResult(myIntent, 0);
    }

    //Called by button press to go to the settings page
    public void goToSettings(View view) {
        Intent intent = new Intent(this, SettingsActivity.class);
        intent.putExtra(SettingsActivity.EXTRA_SHOW_FRAGMENT, SettingsActivity.GeneralPrefs.class.getName());
        intent.putExtra(SettingsActivity.EXTRA_NO_HEADERS, true);
        startActivity(intent);
    }

    //Goes to the about view activity to display information
    @Override
    public void goToAbout(View view) {
        Intent intent = new Intent(this, AboutTestsActivity.class);
        startActivity(intent);
    }
}
