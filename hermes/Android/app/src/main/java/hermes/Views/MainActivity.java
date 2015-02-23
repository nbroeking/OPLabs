package hermes.Views;

import com.oplabs.hermes.R;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import general.HermesActivity;
import interfaces.CommunicationDelegate;

public class MainActivity extends HermesActivity implements CommunicationDelegate{

    private final String TAG = "HermesMainActivity";
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

        Log.i(TAG, "OnCreate");
	}

    @Override
    public void onResume()
    {
        super.onResume();
        data.sync(this);
        checkLogin();
    }
    //Methods for Login in
    //Allows the Communication thread to communicate back
    @Override
    public void notifyLogin()
    {
        Log.i(TAG, "Notify Login");
    }
    //Check login
    public void checkLogin()
    {
        Log.i(TAG, "Check Login");
        if( data.getEmail().equals("")|| data.getPassword().equals("") || data.getHostname().equals(""))
        {
            new AlertDialog.Builder(this)
                    .setTitle("Welcome")
                    .setMessage("Welcome to hermes. Please set up your login information.")
                    .setPositiveButton(android.R.string.yes, new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int which) {
                            Log.i("ALERT CLICKED", "ALERT CLICKED");
                            // continue with delete
                            goToLogin(null);
                        }
                    })
                    .setIcon(R.drawable.ic_launcher)
                    .show();
        }
    }

    //Marks Login
    public void startLogin()
    {

    }
    public void endLogin()
    {

    }

    //Methods for changing views
    //Run test
    public void runTest(View view)
    {
        Log.i(TAG, "RunTest");
        Intent myIntent = new Intent(MainActivity.this, ResultsActivity.class);
        MainActivity.this.startActivity(myIntent);
    }
}
