package hermes.Views;

import android.app.Fragment;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.oplabs.hermes.R;

import general.HermesActivity;

public class ResultsActivity extends HermesActivity {

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
            return inflater.inflate(R.layout.fragment_results, container, false);
        }
    }
}
