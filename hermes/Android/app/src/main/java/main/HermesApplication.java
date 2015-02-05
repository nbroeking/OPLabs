package main;

import android.util.*;
import communication.*;
import tester.*;
import android.app.Application;
import android.content.res.Configuration;

public class HermesApplication extends Application 
{
	private final String TAG = "HermesApp";
	private Communicator communicator;
	
	private Tester tester;
	
	public HermesApplication(){
		super();
		Log.i(TAG, "Application object was constructed");
		tester = null;
		communicator = null;
	}
	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);
	}
 
	@Override
	public void onCreate() {
		super.onCreate();
		Log.i(TAG, "Application onCreate");
		
		//Create some threads	
		tester = new Tester("Tester");
		tester.start();
		
		communicator = new Communicator("Comm");
		communicator.start();
		
	}
	
	@Override
	public void onLowMemory() {
		super.onLowMemory();
		Log.w(TAG, "Application has low memory");
	}
 
	@Override
	public void onTerminate() {
		super.onTerminate();
		Log.i(TAG, "Application will Terminate");
		
		try {
			tester.join();
			communicator.join();
		} catch (InterruptedException e) {
			Log.i(TAG, "Application could not join with tester");
			e.printStackTrace();
		}
	
	}
}
