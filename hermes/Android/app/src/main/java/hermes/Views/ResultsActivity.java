package hermes.Views;

import android.app.Activity;
import android.app.ActionBar;
import android.app.Fragment;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.os.Build;

import com.oplabs.hermes.R;

public class ResultsActivity extends Activity {

    private final String TAG = "HermesResultsActivity";

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
    }   
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_results, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    @Override
    protected void onDestroy()
    {
        super.onDestroy();
        Log.i(TAG, "OnDestroy");
    }
    @Override
    protected void onStart()
    {
        super.onStart();
        Log.i(TAG, "OnStart");
    }
    @Override
    protected void onRestart()
    {
        super.onRestart();
        Log.i(TAG, "OnRestart");
    }
    @Override
    protected void onResume()
    {
        super.onResume();
        Log.i(TAG, "OnResume");
    }
    @Override
    protected void onPause()
    {
        super.onPause();
        Log.i(TAG, "OnPause");
    }
    @Override
    protected void onStop()
    {
        super.onStop();
        Log.i(TAG, "OnStop");
    }
    /**
     * A placeholder fragment containing a simple view.
     */
    public static class AnimationFragment extends Fragment {

        public AnimationFragment() {
        }

        @Override
        public View onCreateView(LayoutInflater inflater, ViewGroup container,
                                 Bundle savedInstanceState) {
            View rootView = inflater.inflate(R.layout.fragment_results, container, false);
            return rootView;
        }
    }
}
