package main.Views;

import android.app.ProgressDialog;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import com.oplabs.hermes.R;

import main.helpers.HermesActivity;

/**
 * Created by sfeller on 4/21/15.
 */

//This activity displays information about how tests work
public class AboutTestsActivity extends HermesActivity {

    private final String TAG = "HermesMainActivity";

    ProgressDialog dialog;

    //When we create the activity we load the view
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_about_tests);
        Log.i(TAG, "OnCreate");
    }

    //If we need to go to the login page we call go to settings
    @Override
    public void goToLogin(View view) {
        goToSettings(view);
    }

    //Called when the user tries to go to the settings page from the about page
    @Override
    public void goToSettings(View view) {
        Intent intent = new Intent(this, SettingsActivity.class);
        intent.putExtra(SettingsActivity.EXTRA_SHOW_FRAGMENT, SettingsActivity.GeneralPrefs.class.getName());
        intent.putExtra(SettingsActivity.EXTRA_NO_HEADERS, true);
        startActivity(intent);
    }

    //Go to the about activity
    @Override
    public void goToAbout(View view) {
        Intent intent = new Intent(this, AboutTestsActivity.class);
        startActivity(intent);
    }
}
