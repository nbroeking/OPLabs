package tester;

import android.os.HandlerThread;
import android.os.Looper;
import android.util.Log;


public class TestThread extends HandlerThread {

    private static final String TAG = "Hermes Test Thread";
    private String mName;

    public TestMessageHandler mHandler;

    public TestThread(String name) {
        super(name);
        mName = name;

        mHandler = null;
        Log.i(TAG, "TestThread constructed");
    }

    @Override
    public void run() {
        Log.i(TAG, "Running Thread");
        Looper.prepare();

        mHandler = new TestMessageHandler();
        Looper.loop();
    }

    synchronized public String name(){
        return mName;
    }

}