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
	
	private static final String TAG = "HermesTester";
	private Boolean started;
	private String mName;
	
	public CommMessageHandler mHandler;
	
	/**
	 * 
	 */
	public Communicator(String name) {
		super(name);
		mName = name;
		started = false;

		mHandler = null;
		Log.i(TAG, "Tester constructed");
	}
	
	@Override
	public void run() {
		//Set up looper
		Looper.prepare();
		
		mHandler = new CommMessageHandler();		
		Looper.loop();

	}
	
	synchronized public Boolean isRunning(){
		return started;
	}
	synchronized public String name(){
		return mName;
	}

}
