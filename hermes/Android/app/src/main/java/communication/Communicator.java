/**
 * 
 */
package communication;

import android.os.HandlerThread;
import android.os.Looper;
import android.util.Log;

/**
 * @author NBroeking
 *
 */
public class Communicator extends HandlerThread {
	
	private static final String TAG = "HermesComm";
	private String mName;
	
	public CommMessageHandler mHandler;
	
	/**
	 * 
	 */
	public Communicator(String name) {
		super(name);
		mName = name;

		mHandler = null;
		Log.i(TAG, "Communicator constructed");
	}

	@Override
	public void run() {
		//Set up looper
		Looper.prepare();

		mHandler = new CommMessageHandler();
		Looper.loop();
	}

	synchronized public String name(){
		return mName;
	}

}
