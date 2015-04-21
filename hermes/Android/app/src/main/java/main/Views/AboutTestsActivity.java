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
public class AboutTestsActivity extends HermesActivity {

    private final String TAG = "HermesMainActivity";

    ProgressDialog dialog;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_about_tests);
        Log.i(TAG, "OnCreate");
    }

    //@Override
    //protected void onStart() {
    //    Log.i(TAG, "On Start");
    //    super.onStart();
    //}

    @Override
    public void goToLogin(View view) {
        goToSettings(view);
    }

    @Override
    public void goToSettings(View view) {
        Intent intent = new Intent(this, SettingsActivity.class);
        intent.putExtra(SettingsActivity.EXTRA_SHOW_FRAGMENT, SettingsActivity.GeneralPrefs.class.getName());
        intent.putExtra(SettingsActivity.EXTRA_NO_HEADERS, true);
        startActivity(intent);
    }

    @Override
    public void goToAbout(View view) {
        Intent intent = new Intent(this, AboutTestsActivity.class);
        startActivity(intent);
    }
}
