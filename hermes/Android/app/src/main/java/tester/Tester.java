/**
 * 
 */
package tester;

import android.os.HandlerThread;
import android.os.Looper;
import android.util.Log;

/**
 * @author NBroeking
 *
 */
public class Tester extends HandlerThread {
	
	private static final String TAG = "HermesTester";

	private String mName;
	
	public TestMessageHandler mHandler;
	
	/**
	 * 
	 */
	public Tester(String name) {
		super(name);
		mName = name;

		mHandler = null;
		Log.i(TAG, "Tester constructed");
	}
	
	@Override
	public void run() {
		//Set up looper
		Looper.prepare();
		
		mHandler = new TestMessageHandler();		
		Looper.loop();

	}
	synchronized public String name(){
		return mName;
	}

}
