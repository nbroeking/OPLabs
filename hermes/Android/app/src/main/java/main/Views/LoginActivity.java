package main.Views;

import android.app.Activity;
import android.content.Intent;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.EditText;

import com.oplabs.hermes.R;

import main.Application.SessionData;

//This activity has the required info to allow a user to login and perform a test
public class LoginActivity extends Activity {

    private final static String TAG = "Login Activity";

    private SessionData data;

    //On create we lookup the current values and fill in our gui with them
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);
        data = SessionData.getInstance();

        //Set Default Values
        EditText email = (EditText)findViewById(R.id.email);
        EditText password = (EditText)findViewById(R.id.password);

        email.setText(data.getEmail());
        password.setText(data.getPassword());
    }

    //When the user presses login we want to start the login process.
    //We also save the newly entered data
    public void login(View view)
    {
        EditText email = (EditText)findViewById(R.id.email);
        EditText password = (EditText)findViewById(R.id.password);
        data.setEmail(email.getText().toString());
        data.setPassword(password.getText().toString());
        data.setSessionId("");
        SharedPreferences sharedPref = PreferenceManager.getDefaultSharedPreferences(this);
        SharedPreferences.Editor editor = sharedPref.edit();
        editor.putString("email", data.getEmail() );
        editor.putString("password", data.getPassword());
        editor.commit();

        finish();
        Log.i(TAG, "Login Activity Completed");
    }

    //Methods to allow for a menu
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_login, menu);
        return true;
    }

    //When the settings button is clicked we must honor the request and go to settings
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            Intent intent = new Intent(this, SettingsActivity.class);
            intent.putExtra(SettingsActivity.EXTRA_SHOW_FRAGMENT, SettingsActivity.GeneralPrefs.class.getName());
            intent.putExtra(SettingsActivity.EXTRA_NO_HEADERS, true);
            startActivity(intent);
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

}
