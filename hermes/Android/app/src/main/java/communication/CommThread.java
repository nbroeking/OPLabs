package communication;

import android.os.HandlerThread;
import android.os.Looper;
import android.util.Log;

public class CommThread extends HandlerThread {
	
	private static final String TAG = "Hermes Comm Thread";
	private String mName;
	
	public CommMessageHandler mHandler;

	public CommThread(String name) {
		super(name);
		mName = name;

		mHandler = null;
		Log.i(TAG, "CommThread constructed");
	}

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
