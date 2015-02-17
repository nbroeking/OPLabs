package hermes.Views;

import com.oplabs.hermes.R;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;

public class MainActivity extends Activity {

    private final String TAG = "HermesMainActivity";
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

        Log.i(TAG, "OnCreate");
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

    public void runTest(View view)
    {
        Log.i(TAG, "RunTest");
        Intent myIntent = new Intent(MainActivity.this, ResultsActivity.class);
        MainActivity.this.startActivity(myIntent);
    }
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
			return true;
		}
		return super.onOptionsItemSelected(item);
	}
}
