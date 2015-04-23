package main.Application;

import android.util.*;
import android.app.Application;
import android.content.res.Configuration;
import android.view.ViewConfiguration;

import java.lang.reflect.Field;

//This class allows us to control application level events.
//Currently only used to debug application behavior
public class HermesApplication extends Application 
{
	private final String TAG = "HermesApp";

	public HermesApplication(){
		super();
		Log.i(TAG, "Application object was constructed");
	}
	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);
	}
 
	@Override
	public void onCreate() {
        super.onCreate();
        Log.i(TAG, "Application onCreate");
        //make sure the action overflow icon is shown no matter the device
        forceShowActionOverflow();
    }

    private void forceShowActionOverflow() {
        //hack for the app to behave more similarly between different devices
        //devices with hardware menu button (e.g. Samsung Note) don't show action overflow menu
        try {
            ViewConfiguration config = ViewConfiguration.get(this);
            Field menuKeyField = ViewConfiguration.class.getDeclaredField("sHasPermanentMenuKey");
            if (menuKeyField != null) {
                menuKeyField.setAccessible(true);
                menuKeyField.setBoolean(config, false);
            }
        } catch (Exception e) {
            Log.d(TAG, e.getLocalizedMessage());
        }
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
	}
}
