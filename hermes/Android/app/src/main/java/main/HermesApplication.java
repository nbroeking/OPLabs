package main;

import android.os.Message;
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

        //There is a bug here where if we try and send a message to the threads too soon it
        //will never get there, however I am betting on the fact that noone fingers are fast enough
        //to make this a problem

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
        Log.i(TAG, "Application will Terminate");
		super.onTerminate();
        Log.i(TAG, "Killing the worker threads");

        Message msg = Message.obtain();
        msg.what = MainMsg.QUIT;
        msg.obj = null;

        tester.mHandler.sendMessage(msg);
        communicator.mHandler.sendMessage(msg);

        Log.i(TAG, "Waiting for the workers to DIE");
		try {
			tester.join();
			communicator.join();
		} catch (InterruptedException e) {
			Log.i(TAG, "Application could not join with tester");
			e.printStackTrace();
		}
	
	}
}
