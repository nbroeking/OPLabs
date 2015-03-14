package tester.helpers;

import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.util.Log;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.ReentrantLock;

public class TestThread extends HandlerThread {

    private static final String TAG = "Hermes Test Thread";
    private String mName;

    private TestMessageHandler mHandler;

    //Mutex and conditions
    private final ReentrantLock lock = new ReentrantLock();
    private final Condition notReady = lock.newCondition();

    public TestThread(String name) {
        super(name);
        mName = name;
        mHandler = null;
        Log.i(TAG, "TestThread constructed");
    }

    public Handler getHandler()
    {
        lock.lock();
        try {
            while (mHandler == null){
                notReady.await();
            }
            return mHandler;
        }
        catch (Exception e)
        {
            Log.e(TAG,"Could not handle condition variable", e);
        }
        finally {
            lock.unlock();
        }
        return null;
    }
    @Override
    public void run() {
        lock.lock();
        Log.i(TAG, "Running Thread");
        Looper.prepare();
        mHandler = new TestMessageHandler();
        notReady.signalAll();
        lock.unlock();
        Looper.loop();
    }

    synchronized public String name(){
        return mName;
    }

}