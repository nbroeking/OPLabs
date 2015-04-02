package communication;

import android.os.HandlerThread;
import android.os.Looper;
import android.util.Log;

import communication.Helpers.CommMessageHandler;

//This class is the Handler thread that runs the Communciation subsystem
public class CommThread extends HandlerThread {
	
	private static final String TAG = "Hermes Comm Thread";
	private String mName;
	
	public CommMessageHandler mHandler;

    //Set everything to null. Member variables must be created after the thread is started
	public CommThread(String name) {
		super(name);
		mName = name;

		mHandler = null;
		Log.i(TAG, "CommThread constructed");
	}

    //The main run loop for the communication subsystem
	@Override
	public void run() {
        Log.i(TAG, "Running Thread");
		Looper.prepare();

		mHandler = new CommMessageHandler();
		Looper.loop();
	}

	synchronized public String name(){
		return mName;
	}

}
